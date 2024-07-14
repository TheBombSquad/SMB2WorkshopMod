
{-# LANGUAGE FlexibleContexts, NoMonomorphismRestriction, TemplateHaskell #-}

module Types where

import Control.Lens
import Data.DList (DList)
import Data.Word

type Label = String

type LabelTable = [(Label,Word32)]

data Instruction =
  Ib { bLK :: Bool , bAddrLabel :: Label } -- Address is the address where you want to go to
  | Ibc { bcLK :: Bool , bcCondOp :: CondOp , bcAddrLabel :: Label }
  | Ibclr { bcLK :: Bool , bcCondOp :: CondOp }
  | Icmpz { cmpzUnsigned :: Bool , cmpzCrfD :: Word8 , cmpzL :: Bool , cmpzRA :: Word8 , cmpzRB :: Word8 } -- cmp / cmpl
  | Icmpzi { cmpziUnsigned :: Bool , cmpziCrfD :: Word8 , cmpziL :: Bool , cmpziRA :: Word8 , cmpziSIMM :: Word16 } -- cmpl / cmpli
  | Iadd { addOE :: Bool , addRc :: Bool , addRD :: Word8 , addRA :: Word8 , addRB :: Word8 }
  | Imullw { mullwOE :: Bool , mullwRc :: Bool , mullwRD :: Word8 , mullwRA :: Word8 , mullwRB :: Word8 }
  | Isubf { subfOE :: Bool , subfRc :: Bool , subfRD :: Word8 , subfRA :: Word8 , subfRB :: Word8 }
  | Iand { andRc :: Bool , andRA :: Word8 , andRS :: Word8 , andRB :: Word8 }
  | Ior { orRc :: Bool , orRA :: Word8 , orRS :: Word8 , orRB :: Word8 }
  | Ixor { xorRc :: Bool , xorRA :: Word8 , xorRS :: Word8 , xorRB :: Word8 }
  | Inand { nandRc :: Bool , nandRA :: Word8 , nandRS :: Word8 , nandRB :: Word8 }
  | Inor { norRc :: Bool , norRA :: Word8 , norRS :: Word8 , norRB :: Word8 }
  | Islw { slwRc :: Bool , slwRA :: Word8 , slwRS :: Word8 , slwRB :: Word8 }
  | Israw { srawRc :: Bool , srawRA :: Word8 , srawRS :: Word8 , srawRB :: Word8 }
  | Isrw { srwRc :: Bool , srwRA :: Word8 , srwRS :: Word8 , srwRB :: Word8 }
  | Iextsb { extsbRc :: Bool , extsbRA :: Word8 , extsbRS :: Word8 }
  | Iextsh { extshRc :: Bool , extshRA :: Word8 , extshRS :: Word8 }
  | Iaddi { addiRD :: Word8 , addiRA :: Word8 , addiSIMM :: Word16 }
  | Iaddis { addisRD :: Word8 , addisRA :: Word8 , addisSIMM :: Word16 }
  | Imulli { mulliRD :: Word8 , mulliRA :: Word8 , mulliSIMM :: Word16 }
  | Iandi { andiRA :: Word8 , andiRS :: Word8 , andiUIMM :: Word16 }
  | Iandis { andisRA :: Word8 , andisRS :: Word8 , andisUIMM :: Word16 }
  | Iori { oriRA :: Word8 , oriRS :: Word8 , oriUIMM :: Word16 }
  | Ioris { orisRA :: Word8 , orisRS :: Word8 , orisUIMM :: Word16 }
  | Ixori { xoriRA :: Word8 , xoriRS :: Word8 , xoriUIMM :: Word16 }
  | Ixoris { xorisRA :: Word8 , xorisRS :: Word8 , xorisUIMM :: Word16 }
  | Irlwinm { rlwinmRc :: Bool , rlwinmRA :: Word8 , rlwinmRS :: Word8 , rlwinmSH :: Word8 , rlwinmMB :: Word8 , rlwinmME :: Word8 }
  | Ilbz { lbzRD :: Word8 , lbzRA :: Word8 , lbzD :: Word16 }
  | Ilhz { lhzRD :: Word8 , lhzRA :: Word8 , lhzD :: Word16 }
  | Ilha { lhaRD :: Word8 , lhaRA :: Word8 , lhaD :: Word16 }
  | Ilwz { lwzRD :: Word8 , lwzRA :: Word8 , lwzD :: Word16 }
  | Istb { stbRS :: Word8 , stbRA :: Word8 , stbD :: Word16 }
  | Isth { sthRS :: Word8 , sthRA :: Word8 , sthD :: Word16 }
  | Istw { stwRS :: Word8 , stwRA :: Word8 , stwD :: Word16 }
  | Istwu { stwuRS :: Word8 , stwuRA :: Word8 , stwuD :: Word16 }
  | Ilwzx { lwzxRD :: Word8 , lwzxRA :: Word8 , lwzxRB :: Word8 }
  | Ilfs { lfsFRT :: Word8 , lfsRA :: Word8 , lfsD :: Word16 }
  | Ilfd { lfdFRT :: Word8 , lfdRA :: Word8 , lfdD :: Word16 }
  | Istfs { stfsFRS :: Word8 , stfsRA :: Word8 , stfsD :: Word16 }
  | Ifmr { fmrRc :: Bool , fmrFRT :: Word8 , fmrFRB :: Word8 }
  | Imtspr { mtsprSPR :: SpecialRegister , mtsprRS :: Word8 }
  | Imfspr { mfsprRS :: Word8 , mfsprSPR :: SpecialRegister }
  | Iother Word32
  deriving Show


data CondOp = LessThan | GreaterThan | LessThanEqual | GreaterThanEqual | Equal | NotEqual | Always | Other Word8 Word8
  deriving Show
-- Other represents other BO, BI values for bc

data SpecialRegister = XER | LR | CTR
  deriving Show

data UnresolvedOffset = After Label | Address Word32
  deriving Eq
type ResolvedOffset = Word32
makePrisms ''UnresolvedOffset

data Function = Function { _fnLabel :: Label , _fnOffset :: UnresolvedOffset , _fnMaxSize :: Maybe Word32 , _fnInstructions :: DList Instruction , _fnLocalLabelTable :: LabelTable }
type FunctionTable = [Function]
makeLenses ''Function
{-
getLabel = view fnLabel
getOffset = view fnOffset
getMaxSize = view fnMaxSize
-- getInstructions = view fnInstructions
getLocalLabelTable = view fnLocalLabelTable
-}

sizeOf :: (Foldable t, Num a) => t Instruction -> a
sizeOf = (4*) . fromIntegral . length

