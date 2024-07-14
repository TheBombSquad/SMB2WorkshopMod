
module FileStuff where

import Control.Monad
import System.IO

copyBytes :: (Num a, Enum a) => Handle -> Handle -> a -> a -> IO ()
copyBytes inFile outFile a b = forM_ [a..b] $ const $ hGetChar inFile >>= hPutChar outFile

copyUntilEnd :: Handle -> Handle -> IO ()
copyUntilEnd inFile outFile = do
  ineof <- hIsEOF inFile
  if ineof
    then return ()
    else do
      hGetChar inFile >>= hPutChar outFile
      copyUntilEnd inFile outFile

doFileIO :: String -> String -> (Handle -> Handle -> IO ()) -> IO ()
doFileIO inFileName outFileName k = do
  inFile <- openFile inFileName ReadMode
  outFile <- openFile outFileName WriteMode
  hSetBinaryMode inFile True
  hSetBinaryMode outFile True
  hSetBuffering outFile (BlockBuffering Nothing)

  k inFile outFile

  hClose inFile
  hClose outFile
