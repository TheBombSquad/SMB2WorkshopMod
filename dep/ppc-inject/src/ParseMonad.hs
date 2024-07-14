
-- Adapted from ``Monadic parsing in Haskell'' by Hutton, Meijer
-- Naming conventions by Christopher Lynch's parsing tools

module ParseMonad where

import Control.Applicative
import Control.Monad
import Data.Char


newtype Pair a b = Pair { getPair :: (b,a) }

instance Functor (Pair a) where
  fmap f (Pair (x,y)) = Pair (f x , y)

instance (Show a,Show b) => Show (Pair a b) where
  show (Pair (a,b)) = "(" ++ show a ++ "," ++ show b ++ ")"

newtype Parse a b = Parse { getParser :: (a -> [(Pair a b)]) }

instance Functor (Parse a) where
  -- fmap :: (b -> c) -> Parse a b -> Parse a c
  -- This is equivalent to build
  fmap f (Parse parser) = Parse ((map (fmap f)) . parser)

instance Applicative (Parse a) where
  pure = return
  p1 <*> p2 = do
    f <- p1
    x <- p2
    return (f x)

instance Monad (Parse a) where
  return w = Parse $ \x -> [Pair (w,x)]
  p >>= f = Parse $ \inp -> concat $ do
    Pair (a,inp') <- getParser p inp
    return $ getParser (f a) inp'

instance Alternative (Parse a) where
  empty = Parse $ \_ -> []
  (<|>) (Parse f) (Parse g) = Parse $ liftA2 (++) f g

item :: Parse [a] a
item = Parse $ \inp -> do
  (guard . not . null) inp
  (return . Pair . ( (,) <$> head <*> tail)) inp

spot :: (a -> Bool) -> Parse [a] a
spot p = do
  c <- item
  guard (p c)
  return c

token :: Eq a => a -> Parse [a] a
token ch = spot (== ch)

tokens :: Eq a => [a] -> Parse [a] [a]
tokens [] = Parse $ \inp -> [Pair ([],inp)]
tokens (s:str) = do
  token s
  tokens str
  return (s:str)

alt :: Monoid a => Parse a b -> Parse a b -> Parse a b
-- alt (Parse f) (Parse g) = Parse $ liftA2 (++) f g
alt = (<|>)

list :: Monoid a => Parse a b -> Parse a [b]
list p = (return []) <|> list1 p

list1 :: Monoid a => Parse a b -> Parse a [b]
list1 p = do
  r <- p
  rs <- list p
  return (r:rs)

ws :: Parse String String
ws = (list (spot isSpace))

ws1 = (list1 (spot isSpace))

succeed :: b -> Parse a b
succeed = return

none :: Parse a b
none = Parse $ const $ []

parse :: Parse String b -> String -> Either String b
parse (Parse f) inp = do
  -- let parses = map fst (filter (null . snd) (f inp))
  let parses = do {
    Pair (val,leftover) <- f inp ;
    guard (null leftover) ;
    return val
  }
  -- guard ((not . null) parses)
  -- guard ((null . tail) parses)
  when (null parses) (Left "No parses")
  when ((not . null . tail) parses) (Left "Too much parses")
  (return . head) parses

parseNoWS p inp = parse p (filter (not . isSpace) inp)

brutalParse :: Parse String b -> String -> b
brutalParse p inp 
  = (\eb -> case eb of { Left msg -> error msg ; Right b -> b })
    $ (parse p inp)
