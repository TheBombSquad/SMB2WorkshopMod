
{-# LANGUAGE LambdaCase #-}

module ParseHelpers where

import Control.Applicative
import Control.Monad
import Data.Char
import Data.Word
import Text.Read

import ParseMonad


localLabelParser = liftA2 (:) (token '.') nameParser

functionLabelParser = liftA2 (:) (token '$') nameParser

offsetLabelParser = liftA2 (:) (token '@') nameParser

anyLabelParser = localLabelParser <|> functionLabelParser <|> offsetLabelParser

nameParser :: Parse String String
nameParser = liftA2 (:) (spot isAlpha) (list $ spot isAlphaNum)

readParser :: Read a => Parse String String -> Parse String a
readParser = (=<<) $ flip (.) readMaybe $ \case
  Nothing -> none
  Just res -> return res

numberStringParser = (list1 $ spot isDigit) <|> (liftA2 (++) (tokens "0x") (list1 $ spot isHexDigit))

numberParser :: Read a => Parse String a
numberParser = readParser $ numberStringParser

decNumberParser :: Read a => Parse String a
decNumberParser = readParser $ list1 $ spot isDigit

signedNumberParser :: Read a => Parse String a
signedNumberParser = readParser $ numberStringParser <|> liftA2 (:) (token '-') numberStringParser
