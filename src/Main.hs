
{-# LANGUAGE LambdaCase #-}


module Main where

import Control.Lens
import Control.Monad
import qualified Data.ByteString.Builder as BB
import qualified Data.DList as DList
import Data.Functor.Extend
import Data.Function
import Data.List
import Data.Maybe
import System.Environment
import System.Exit
import System.IO


import Helpers
import FileParser
import FileStuff
import InstructionSet
import LabelResolver
import Types

main = do
  args <- getArgs
  when (length args < 3) $
    die $ "Usage: ./PPCInject [in file] [out file] [one or more assembly files]"
  let (inFileName:outFileName:asmFiles) = args

  let 
    handleFile (curFunctionTable,curOffsetTable) fileName = do
      hPutStrLn stderr $ "Reading file " ++ fileName
      (fnTbl,offTbl) <- parseFile fileName
      let 
        fnLblsAlreadyDefined = do
          fn1 <- fnTbl
          fn2 <- curFunctionTable
          guard (fn1^.fnLabel == fn2^.fnLabel)
          return $ fn1^.fnLabel
        offLblsAlreadyDefined = do
          (lbl1,_) <- offTbl
          (lbl2,_) <- curOffsetTable
          guard (lbl1 == lbl2)
          return lbl1
      when (not $ null fnLblsAlreadyDefined) $ do
        hPutStrLn stderr $ "These functions have already been defined in another file:"
        forM_ fnLblsAlreadyDefined $ putStrLn . ("  "++)
        exitFailure
      when (not $ null offLblsAlreadyDefined) $ do
        hPutStrLn stderr $ "These offsets have already been defined in another file:"
        forM_ offLblsAlreadyDefined $ putStrLn . ("  "++)
        exitFailure
      return $ (fnTbl ++ curFunctionTable, offTbl ++ curOffsetTable)
  -- Read all files and get all functions
  (functionTable,offsetTable) <- foldM handleFile ([],[]) asmFiles
  -- Resolve after-offsets
  globalLabelTable <- resolveLabelsInFunctionTable functionTable
  -- Make the local labels absolute
  fixedFunctionTable <- forM functionTable $ \fn -> do
    let unresolvedErr = die $ "The offset for function \"" ++ fn^.fnLabel ++ "\" has not been resolved"
    runMaybe (lookup (fn^.fnLabel) globalLabelTable) unresolvedErr $ \off ->
      return $ (fnLocalLabelTable . mapped . _2) %~ (+off) $ fn
  let 
    sortedLabelTable = sortBy (on compare snd) globalLabelTable
    finalFunctionTable = do
      (lbl,off) <- sortedLabelTable
      fn <- fixedFunctionTable
      guard (lbl == fn^.fnLabel)
      return (lbl, off, fn^.fnInstructions, fn^.fnLocalLabelTable)
    endBeginTableArrow ((lbl1,off1,insts1,_):(lbl2,off2,_,_):_) = Just $ ((lbl1, off1+sizeOf insts1), (lbl2, off2))
    endBeginTableArrow _ = Nothing
    -- Each entry in this table pairs the end offset of a function to the beginning of the next, along with the respective labels
    endBeginTable = catMaybes $ extended endBeginTableArrow finalFunctionTable
  
  -- Make sure that any two functions don't overlap
  forM_ endBeginTable $ \((lbl1,endOfFn1),(lbl2,beginOfFn2)) ->
    when (endOfFn1 > beginOfFn2) $
      die $ "Functions \"" ++ lbl1 ++ "\" and \"" ++ lbl2 ++ "\" overlap"

  -- Get machine code from instructions
  machineCodeTable <- 
    forM finalFunctionTable $ \(lbl,off,insts,localLabelTable) -> do
      let thisLabelTable = localLabelTable ++ globalLabelTable ++ offsetTable
      fmap ((,) off) $ forM (zip (DList.toList insts) [off,off+4..]) $ 
        \(inst,pc) -> instructionToWord thisLabelTable pc inst
      

  doFileIO inFileName outFileName $ \inFile outFile -> do
    let 
      cpBytes = copyBytes inFile outFile
      cpUntilEnd = copyUntilEnd inFile outFile
      writeInstruction instMc = BB.hPutBuilder outFile $ BB.word32BE instMc
      writeFunction (off,instsMc) = do
        hSeek outFile AbsoluteSeek (fromIntegral off) 
        mapM_ writeInstruction instsMc
    cpUntilEnd
    when (null finalFunctionTable) $ do
      hPutStrLn stderr $ "No functions to write. Copied input file"
      exitSuccess
    mapM_ writeFunction machineCodeTable
      
          


