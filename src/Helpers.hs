
{-# LANGUAGE LambdaCase #-}

module Helpers where

import Control.Comonad.Cofree
import Control.Monad
import Data.Function


enumToNum :: (Enum a, Num b) => a -> b
enumToNum = fromIntegral . fromEnum


brutalLookup :: (Eq a) => a -> [(a,b)] -> b
brutalLookup ele store = lookup ele store & \case
  Just b -> b
  Nothing -> error "brutalLookup failed as it couldn\'t find an element in a map. You shouldn\'t be seeing this; it\'s TwixNinja\'s fault. He sucks."

coiterM :: (Monad m, Traversable f) => (a -> m (f a)) -> a -> m (Cofree f a)
coiterM mf = unfoldM $ (liftM2.liftM2) (,) return mf

infixl 1 >|=
(>|=) :: Functor f => f a -> (a -> b) -> f b
(>|=) = flip fmap

-- maybe with a different order of arguments
runMaybe :: Maybe a -> b -> (a -> b) -> b
runMaybe m d f = maybe d f m
