
module InstructionParser (realInstructionParser) where

import Control.Applicative
import Control.Monad
import Data.Bits
import Data.Word

import ParseHelpers
import ParseMonad
import Types

commaParser = ws >> token ',' >> ws >> return ()

crfDWithCommaParser = do
  tokens "cr" 
  n <- decNumberParser 
  commaParser
  return n

regWithoutCommaParser :: String -> Parse String Word8
regWithoutCommaParser prefix = tokens prefix >> decNumberParser

rWithoutCommaParser = regWithoutCommaParser "r"
fWithoutCommaParser = regWithoutCommaParser "f"

regWithCommaParser :: String -> Parse String Word8
regWithCommaParser prefix = regWithoutCommaParser prefix <* commaParser

rWithCommaParser = regWithCommaParser "r"
fWithCommaParser = regWithCommaParser "f"

oeParser :: Parse String Bool
oeParser = return False <|> (token 'o' >> return True)

rcParser :: Parse String Bool
rcParser = return False <|> (token '.' >> return True)

numberWithCommaParser = do
  n <- numberParser
  commaParser
  return n

loadStoreArgParserOld :: Parse String (Word8,Word8,Word16)
loadStoreArgParserOld = do
  rX <- rWithoutCommaParser
  commaParser
  d <- signedNumberParser
  ws
  token '('
  ws
  rA <- rWithoutCommaParser
  ws
  token ')'
  isRegister rX
  isRegister rA
  return (rX,rA,d)

loadStoreArgParser :: String -> (Word8 -> Word8 -> Word16 -> a) -> Parse String a
loadStoreArgParser prefix f = do 
  rX <- regWithoutCommaParser prefix
  commaParser
  d <- signedNumberParser
  ws
  token '('
  ws
  rA <- rWithoutCommaParser 
  ws
  token ')'
  isRegister rX
  isRegister rA
  return $ f rX rA d 

rLoadStoreArgParser = loadStoreArgParser "r"
fLoadStoreArgParser = loadStoreArgParser "f"

isRegister :: Word8 -> Parse String ()
isRegister rP = when ((/= 0) $ rP .&. 0xE0) $ none

isCrf :: Word8 -> Parse String ()
isCrf crfP = when ((/= 0) $ crfP .&. 0xFC) $ none


-- This is where the reading of the assembly instructions is done
-- Common mnemonics are handled here
realInstructionParser :: Parse String Instruction
realInstructionParser =
  -- b
  do 
    token 'b'
    ws1
    lbl <- anyLabelParser
    return $ Ib False lbl
  -- bl
  <|> do
    tokens "bl"
    ws1
    lbl <- anyLabelParser
    return $ Ib True lbl
  -- blr
  <|> do
    tokens "blr"
    return $ Ibclr False Always
  -- bc / bcl 
  <|> do
    _LK <- 
      (tokens "bc" >> return False) 
      <|> (tokens "bcl" >> return True) 
    ws1
    _BO <- numberWithCommaParser
    _BI <- numberWithCommaParser
    lbl <- anyLabelParser
    return $ Ibc _LK (Other _BO _BI) lbl

  -- beq
  <|> do
    tokens "beq"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False Equal lbl
  -- bne
  <|> do
    tokens "bne"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False NotEqual lbl
  -- blt
  <|> do
    tokens "blt"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False LessThan lbl
  -- ble
  <|> do
    tokens "ble"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False LessThanEqual lbl
  -- bgt
  <|> do
    tokens "bgt"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False GreaterThan lbl
  -- bge
  <|> do
    tokens "bge"
    ws1
    lbl <- anyLabelParser
    return $ Ibc False GreaterThanEqual lbl
  -- cmpw
  <|> do
    tokens "cmpw"
    ws1
    crfD <- return 0 <|> crfDWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isCrf crfD
    isRegister rA
    isRegister rB
    return $ Icmpz False crfD False rA rB
  -- cmplw
  <|> do
    tokens "cmplw"
    ws1
    crfD <- return 0 <|> crfDWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isCrf crfD
    isRegister rA
    isRegister rB
    return $ Icmpz True crfD False rA rB
  -- cmpwi
  <|> do
    tokens "cmpwi"
    ws1
    crfD <- return 0 <|> crfDWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isCrf crfD
    isRegister rA
    return $ Icmpzi False crfD False rA simm
  -- cmplwi
  <|> do
    tokens "cmplwi"
    ws1
    crfD <- return 0 <|> crfDWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isCrf crfD
    isRegister rA
    return $ Icmpzi True crfD False rA simm
  -- add
  <|> do
    tokens "add"
    _OE <- oeParser
    _Rc <- rcParser
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rD
    isRegister rA
    isRegister rB
    return $ Iadd _OE _Rc rD rA rB
  -- mullw
  <|> do
    tokens "mullw"
    _OE <- oeParser
    _Rc <- rcParser
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rD
    isRegister rA
    isRegister rB
    return $ Imullw _OE _Rc rD rA rB
  -- subf
  <|> do
    tokens "subf"
    _OE <- oeParser
    _Rc <- rcParser
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rD
    isRegister rA
    isRegister rB
    return $ Isubf _OE _Rc rD rA rB
  -- sub
  <|> do
    tokens "sub"
    _OE <- oeParser
    _Rc <- rcParser
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rD
    isRegister rA
    isRegister rB
    return $ Isubf _OE _Rc rD rB rA
  -- and
  <|> do
    tokens "and"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Iand _Rc rA rS rB
  -- or
  <|> do
    tokens "or"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Ior _Rc rA rS rB
  -- mr 
  <|> do
    tokens "mr"
    _Rc <- rcParser
    ws1
    rX <- rWithCommaParser
    rY <- rWithoutCommaParser
    isRegister rX
    isRegister rY
    return $ Ior _Rc rX rY rY
  -- xor
  <|> do
    tokens "xor"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Ixor _Rc rA rS rB
  -- nand
  <|> do
    tokens "nand"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Inand _Rc rA rS rB
  -- nor
  <|> do
    tokens "nor"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Inor _Rc rA rS rB
  -- not 
  <|> do
    tokens "not"
    _Rc <- rcParser
    ws1
    rX <- rWithCommaParser
    rY <- rWithoutCommaParser
    isRegister rX
    isRegister rY
    return $ Inor _Rc rX rY rY
  -- slw
  <|> do
    tokens "slw"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Islw _Rc rA rS rB
  -- sraw
  <|> do
    tokens "sraw"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Israw _Rc rA rS rB
  -- srw
  <|> do
    tokens "srw"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    isRegister rB
    return $ Isrw _Rc rA rS rB
  -- extsb
  <|> do
    tokens "extsb"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    return $ Iextsb _Rc rA rS 
  -- extsh
  <|> do
    tokens "extsh"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithoutCommaParser
    isRegister rA
    isRegister rS
    return $ Iextsh _Rc rA rS 
  -- addi
  <|> do
    tokens "addi"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rD
    isRegister rA
    return $ Iaddi rD rA simm
  -- addis
  <|> do
    tokens "addis"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rD
    isRegister rA
    return $ Iaddis rD rA simm
  -- li
  <|> do
    tokens "li"
    ws1
    rX <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rX
    return $ Iaddi rX 0 simm
  -- lis
  <|> do
    tokens "lis"
    ws1
    rX <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rX
    return $ Iaddis rX 0 simm
  -- subi
  <|> do
    tokens "subi"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rD
    isRegister rA
    return $ Iaddi rD rA $ negate simm
  -- subis
  <|> do
    tokens "subis"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rD
    isRegister rA
    return $ Iaddis rD rA $ negate simm
  -- mulli
  <|> do
    tokens "mulli"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    simm <- signedNumberParser
    isRegister rD
    isRegister rA
    return $ Imulli rD rA simm
  -- andi
  <|> do
    tokens "andi" <|> tokens "andi."
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Iandi rA rS uimm
  -- andis
  <|> do
    tokens "andis" <|> tokens "andis."
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Iandis rA rS uimm
  -- ori
  <|> do
    tokens "ori"
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Iori rA rS uimm
  -- oris
  <|> do
    tokens "oris"
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Ioris rA rS uimm
  -- xori
  <|> do
    tokens "xori"
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Ixori rA rS uimm
  -- xoris
  <|> do
    tokens "xoris"
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    uimm <- signedNumberParser
    isRegister rA
    isRegister rS
    return $ Ixoris rA rS uimm
  -- nop
  <|> do
    tokens "nop"
    return $ Iori 0 0 0
  -- rlwinm
  <|> do
    tokens "rlwinm"
    _Rc <- rcParser
    ws1
    rA <- rWithCommaParser
    rS <- rWithCommaParser
    sh <- numberParser
    commaParser
    mb <- numberParser
    commaParser
    me <- numberParser
    let malformed = (/= 5) $ length $ flip filter [rA,rS,sh,mb,me] $ flip elem [0..31] 
    when malformed $ none
    return $ Irlwinm _Rc rA rS sh mb me
  -- lbz
  <|> do
    tokens "lbz"
    ws1
    rLoadStoreArgParser Ilbz
  -- lhz
  <|> do
    tokens "lhz"
    ws1
    rLoadStoreArgParser Ilhz
  -- lha
  <|> do
    tokens "lha"
    ws1
    rLoadStoreArgParser Ilha
  -- lwz
  <|> do
    tokens "lwz"
    ws1
    rLoadStoreArgParser Ilwz
  -- stb
  <|> do
    tokens "stb"
    ws1
    rLoadStoreArgParser Istb
  -- sth
  <|> do
    tokens "sth"
    ws1
    rLoadStoreArgParser Isth
  -- stw
  <|> do
    tokens "stw"
    ws1
    rLoadStoreArgParser Istw
  -- stwu
  <|> do
    tokens "stwu"
    ws1
    rLoadStoreArgParser Istwu
  -- lwzx
  <|> do
    tokens "lwzx"
    ws1
    rD <- rWithCommaParser
    rA <- rWithCommaParser
    rB <- rWithoutCommaParser
    isRegister rD
    isRegister rA
    isRegister rB
    return $ Ilwzx rD rA rB
  -- lfs
  <|> do
    tokens "lfs"
    ws1
    fLoadStoreArgParser Ilfs
  -- lfd
  <|> do
    tokens "lfd"
    ws1
    fLoadStoreArgParser Ilfd
  -- stfs
  <|> do
    tokens "stfs"
    ws1
    fLoadStoreArgParser Istfs
  -- fmr
  <|> do
    tokens "fmr"
    _Rc <- rcParser
    ws1
    fX <- fWithCommaParser
    fY <- fWithoutCommaParser
    isRegister fX
    isRegister fY
    return $ Ifmr _Rc fX fY 
  -- mtlr
  <|> do
    tokens "mtlr"
    ws1
    rX <- rWithoutCommaParser
    isRegister rX
    return $ Imtspr LR rX
  -- mflr
  <|> do
    tokens "mflr"
    ws1
    rX <- rWithoutCommaParser
    isRegister rX
    return $ Imfspr rX LR 
  -- other
  <|> do
    inst <- numberParser
    return $ Iother inst
