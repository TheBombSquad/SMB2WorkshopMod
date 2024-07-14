
{-# LANGUAGE TemplateHaskell #-}

module FileParser (parseFile) where

import Control.Applicative
import Control.Lens
import Control.Monad
import Control.Monad.Fix
import qualified Data.DList as DList
import Data.Word
import System.Exit
import System.IO

import Helpers
import HexStuff
import InstructionParser
import ParseHelpers
import ParseMonad
import Types


------ TODO: Make ParseFileRecord keep track of line number


data ParseFileRecord = PFR { 
                              _pfrFnTbl :: [Function] , -- The functions parsed so far
                              _pfrCurFn :: Maybe Function , -- The function we're in
                              _pfrOffTbl :: LabelTable -- Table of offsets (NOT FUNCTION OFFSETS!!!!!!1!!!! yes i know the terminology is phenomenal)
                            }
makeLenses ''ParseFileRecord

data ParserLine = BeginFunctionLine (Label,UnresolvedOffset,(Maybe Word32)) 
                  | LabelLine String 
                  | InstructionLine Instruction
                  | OffsetLine String Word32
                  | Comment

-- Note that the local labels will have a offset relative to the beginning of its function
-- This will be fixed on write-out
parseFile :: String -> IO (FunctionTable,LabelTable)
parseFile inFileName = do
  lineNumsAndLns <- zip [1..] . lines <$> readFile inFileName 
  let 
    initRecord = PFR { _pfrFnTbl = [] , _pfrCurFn = Nothing , _pfrOffTbl = [] }
    
    maxSizeCheck fn = 
      when (anyOf _Just (\maxSize -> maxSize < sizeOf (fn^.fnInstructions)) (fn^.fnMaxSize)) $ 
        die $ "The size of function \"" ++ (fn^.fnLabel) 
          ++ "\" exceeds the max size that you specified, 0x" ++ (show $ Hex $ sum $ fn^.fnMaxSize) ++ " bytes"
    updateFunctionTable curFunctionTable curFn 
      = maybe (return curFunctionTable) (\fn -> maxSizeCheck fn >> return (fn:curFunctionTable)) curFn
    duplicateFnLabelCheck curFunctionTable lbl =
      when (anyOf (folded . fnLabel) (==lbl) curFunctionTable) $
        die $ "Function \"" ++ lbl ++ "\" has already been defined"
    startFunction curFunctionTable (lbl,off,maxSizeMaybe) = 
      duplicateFnLabelCheck curFunctionTable lbl >> return (Function lbl off maxSizeMaybe DList.empty [])
    noCurFnError lineType lineNum = die $ "At line " ++ (show lineNum) ++ "you have a " ++ lineType ++ "line that isn\'t part of a function"
    duplicateLblInLblTblCheck lblType lblTbl lbl = 
      when (anyOf _Just (const True) (lookup lbl lblTbl)) $
        die $ "You defined " ++ lblType ++ " \"" ++ lbl ++ "\" twice"

    parseLine rec (lineNum,ln) = case (parse lineParser ln) of 
      Left _ -> die $ "In file " ++ inFileName ++ ", line " ++ (show lineNum) ++ ", this line couldn\'t be parsed"
      Right Comment -> return rec
      Right (BeginFunctionLine newFnData) -> do
        newFunctionTable <- updateFunctionTable (rec^.pfrFnTbl) (rec^.pfrCurFn)
        newFn <- startFunction (rec^.pfrFnTbl) newFnData
        return $ pfrFnTbl .~ newFunctionTable $ pfrCurFn .~ (Just newFn) $ rec
      Right (OffsetLine lbl off) -> do
        newFunctionTable <- updateFunctionTable (rec^.pfrFnTbl) (rec^.pfrCurFn)
        duplicateLblInLblTblCheck "offset" (rec^.pfrOffTbl) lbl
        return $ pfrCurFn .~ Nothing $ pfrFnTbl .~ newFunctionTable $ pfrOffTbl %~ ((lbl,off):) $ rec
      Right (LabelLine lbl) -> 
        runMaybe (rec^.pfrCurFn) (noCurFnError "label" lineNum) $ \curFn -> do
          duplicateLblInLblTblCheck "label" (curFn^.fnLocalLabelTable) lbl 
          return $ (pfrCurFn . _Just . fnLocalLabelTable) %~ ((lbl,sizeOf (curFn^.fnInstructions)):) $ rec
      Right (InstructionLine inst) -> 
        runMaybe (rec^.pfrCurFn) (noCurFnError "instruction" lineNum) $ \_ -> 
          return $ (pfrCurFn . _Just . fnInstructions) %~ (`DList.snoc` inst) $ rec
  finalRecord <- foldM parseLine initRecord lineNumsAndLns
  finalFunctionTable <- updateFunctionTable (finalRecord^.pfrFnTbl) (finalRecord^.pfrCurFn)
  return (finalFunctionTable,finalRecord^.pfrOffTbl)
        
      

  


lineParser :: Parse String ParserLine
lineParser = comment <|> beginFunctionParser <|> localLabelLineParser <|> offsetParser <|> instructionParser

comment = do
  ws 
  return () <|> (token '%' >> list item >> return ()) 
  return Comment

beginFunctionParser = do
  ws
  tokens "#function"
  ws1
  lbl <- functionLabelParser
  ws1
  off <- 
    (numberParser >>= (return . Address)) 
    <|> (tokens "after" >> ws1 >> fmap After functionLabelParser)
  maxSizeMaybe <- 
    return Nothing <|> (ws1 >> tokens "maxSize" >> ws1 >> fmap Just numberParser) 
  comment
  return $ BeginFunctionLine (lbl,off,maxSizeMaybe)

localLabelLineParser = ws >> fmap LabelLine localLabelParser

offsetParser = do
  ws
  tokens "#offset"
  ws1
  lbl <- offsetLabelParser
  ws1
  off <- numberParser
  comment
  return $ OffsetLine lbl off

instructionParser = do
  ws
  inst <- realInstructionParser -- Defined in ParseInstruction.hs
  comment
  return $ InstructionLine inst

