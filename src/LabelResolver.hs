
{-# LANGUAGE LambdaCase #-}

module LabelResolver where

import Control.Comonad.Cofree
import Control.Lens
import Control.Monad
import Control.Monad.State
import qualified Data.DList as DList
import Data.Either
import Data.Foldable
import Data.Functor.Extend
import Data.List
import Data.Maybe
import Data.Tuple
import Data.Word
-- import qualified Flow as F
import System.Exit
import System.IO

import Helpers
import Types


type LabelData = ((Label,Word32),Word32)
type RoseTree = Cofree []

-- This takes a function table and finds out where they should be put
-- This checks whether there isn't a loop in the graph of label referral
-- This also checks if an after-offset label points to something that exists
-- Semantic errors for after-offsets are found and handled here
resolveLabelsInFunctionTable :: FunctionTable -> IO LabelTable
resolveLabelsInFunctionTable fnTbl = do

  -- Beginning let statement
  let
    resFnToLblDataPerhaps f = (f ^? fnOffset . _Address) 
      & maybe (Left f) (\addr -> Right ((f^.fnLabel,addr),sizeOf (f^.fnInstructions)))
    (initUnresolvedFns,initResolvedLblData) = partitionEithers $ map resFnToLblDataPerhaps fnTbl
    initResolvedLblTbl = map fst initResolvedLblData

  -- Check if two functions have same id
  let 
    sameIDArrow (lbl:lbls) = if elem lbl lbls then Just lbl else Nothing
    sameIDList = nub $ catMaybes $ extended sameIDArrow (fnTbl^..traverse.fnLabel)
  when (not $ null sameIDList) $ do
    hPutStrLn stderr "These labels have been defined twice:"
    mapM_ (hPutStrLn stderr) sameIDList
    exitFailure
    
  
  -- Check if two initially resolved functions would be placed in the same position
  let
    samePosArrow ((lbl,offset):rest) = if elem offset $ map snd rest then Just lbl else Nothing
    samePosList = nub $ catMaybes $ extended samePosArrow initResolvedLblTbl
  when (not $ null samePosList) $ do
    hPutStrLn stderr "These functions would have been placed in the same place as another function:"
    mapM_ (hPutStrLn stderr) samePosList
    exitFailure

  let
    -- Resolution
    -- par stands for parent, as there is a parent child relationship induced by the concept of after labels
    -- That is, if function with label c has an after offset with label p, function p is the parent of function c
    -- Also, this algorithm does resolution by building a tree induced by this parent-child relationship then collapsing it
    hasRightAfterOffset :: Label -> Function -> Bool
    hasRightAfterOffset parLbl fn = anyOf (fnOffset . _After) (==parLbl) fn 

    partitionByAfterOffset :: Label -> State [Function] [Function]
    partitionByAfterOffset parLbl = state $ partition (hasRightAfterOffset parLbl)

    resolveOneOffset :: Function -> State LabelData LabelData
    resolveOneOffset fn = state $ \((_,parOffset),parSize) -> let result = ((fn^.fnLabel,parOffset+parSize),sizeOf (fn^.fnInstructions)) in (result,result)

    resolveListOfOffsets :: LabelData -> [Function] -> [LabelData]
    resolveListOfOffsets parLblDatum fns = evalState (mapM resolveOneOffset fns) parLblDatum

    resolveRelevantOffsets :: LabelData -> State [Function] [LabelData]
    resolveRelevantOffsets parLblDatum = partitionByAfterOffset (fst . fst $ parLblDatum) >|= resolveListOfOffsets parLblDatum

    resolveTreeFromParent :: LabelData -> State [Function] (RoseTree LabelData)
    resolveTreeFromParent = coiterM resolveRelevantOffsets

    (treesOfResolvedOffsets,unresolvedFns) = runState (mapM resolveTreeFromParent initResolvedLblData) initUnresolvedFns
    resolvedLblData = concatMap toList treesOfResolvedOffsets

  -- Check if there is an after label that doesn't exist
  when (not $ null unresolvedFns) $ do
    hPutStrLn stderr $ "There is at least one function whose after-offset doesn\'t resolve. Here they are:"
    forM_ unresolvedFns $ \(Function lbl (After afterLabel) _ _ _) -> 
      hPutStrLn stderr $ "  Function \"" ++ lbl ++ "\" placed after function \"" ++ afterLabel ++ "\""
    exitFailure

  -- Done! :3
  return $ map fst resolvedLblData

    

------ TEST STUFF ------

nops = DList.replicate 4 $ Iori 0 0 0
  
ft1 = [fn1,fn2,fn3,fn4,fn5]
  where
    fn1 = Function "fn1" (Address 0xa32cc) Nothing nops []
    fn2 = Function "fn2" (After "fn1") Nothing nops []
    fn3 = Function "fn3" (After "fn2") Nothing nops []
    fn4 = Function "fn4" (Address 0xa4208) Nothing nops []
    fn5 = Function "fn5" (After "fn4") Nothing nops []
ft2 = [fnF1,fnF2]
  where
    fnF1 = Function "fnF1" (After "fnF2") Nothing nops []
    fnF2 = Function "fnF2" (After "fnF1") Nothing nops []
ft3 = fnF3:[]
  where
    fnF3 = Function "fnF3" (After "fnF3") Nothing nops []
ft4 = fn1:fn2:fn3:[]
  where
    fn1 = Function "fn1" (Address 0xa32cc) Nothing nops []
    fn2 = Function "fn2" (After "fn1") Nothing nops []
    fn3 = Function "fn3" (After "fn1") Nothing nops []
ft5 = fn1:fn2:[]
  where
    fn1 = Function "fn1" (Address 0xa32cc) Nothing nops []
    fn2 = Function "fn1" (Address 0xa4208) Nothing nops []
ft6 = fn1:fn2:[]
  where
    fn1 = Function "fn1" (After "bagel") Nothing nops []
    fn2 = Function "fn2" (After "bagel") Nothing nops []
ft7 = fn1:fn2:[]
  where
    fn1 = Function "fn1" (Address 0xa32cc) Nothing nops []
    fn2 = Function "fn2" (Address 0xa32cc) Nothing nops []
