
module FluidSynth.Settings 
    ( Settings
    , new, delete
    , setStr, getStr, getStrDefault
    , setNum, getNum, getNumDefault, getNumRange
    , setInt, getInt, getIntDefault, getIntRange
    , listOptions
    , listSettings
    )
where

import Foreign hiding (new)
import Foreign.C
import Control.Monad
import Data.IORef
import FluidSynth.Foreign

new :: IO Settings
new = fmap Settings new_fluid_settings

delete :: Settings -> IO ()
delete (Settings p) = delete_fluid_settings p

setStr :: Settings -> String -> String -> IO ()
setStr (Settings p) k v = do
    withCString k $ \k' -> withCString v $ \v' -> do
        fluid_settings_setstr p k' v' -- XXX error checking
        return ()

getStr :: Settings -> String -> IO (Maybe String)
getStr (Settings p) k = do
    withCString k $ \k' -> alloca $ \retval -> do
        success <- fluid_settings_getstr p k' retval
        case success of
             1 -> fmap Just $ peekCString =<< peek retval
             0 -> return Nothing
             _ -> fail "Unknown return value from fluid_settings_getstr"

getStrDefault :: Settings -> String -> IO (Maybe String)
getStrDefault (Settings p) k = do
    withCString k $ \k' -> do
        cs <- fluid_settings_getstr_default p k'
        if cs == nullPtr
           then return Nothing
           else fmap Just $ peekCString cs

setNum :: Settings -> String -> Double -> IO ()
setNum (Settings p) k v = do
    withCString k $ \k' -> do
        fluid_settings_setnum p k' (realToFrac v)  -- XXX error checking
        return ()

getNum :: Settings -> String -> IO (Maybe Double)
getNum (Settings p) k = do
    withCString k $ \k' -> alloca $ \dptr -> do
        success <- fluid_settings_getnum p k' dptr
        case success of
             1 -> fmap (Just . realToFrac) $ peek dptr
             0 -> return Nothing
             _ -> fail "Unknown return value from fluid_settings_getnum"

getNumDefault :: Settings -> String -> IO Double
getNumDefault (Settings p) k = do
    withCString k $ \k' -> do
        fmap realToFrac $ fluid_settings_getnum_default p k'

getNumRange :: Settings -> String -> IO (Double,Double)
getNumRange (Settings p) k = do
    withCString k $ \k' -> alloca $ \ptrLo -> alloca $ \ptrHi -> do
        fluid_settings_getnum_range p k' ptrLo ptrHi
        liftM2 (,) (fmap realToFrac $ peek ptrLo) (fmap realToFrac $ peek ptrHi)

setInt :: Settings -> String -> Int -> IO ()
setInt (Settings p) k v = do
    withCString k $ \k' -> do
        fluid_settings_setint p k' (fromIntegral v) -- XXX error checking
        return ()

getInt :: Settings -> String -> IO (Maybe Int)
getInt (Settings p) k = do
    withCString k $ \k' -> alloca $ \ptr -> do
        success <- fluid_settings_getint p k' ptr
        case success of
             1 -> fmap (Just . fromIntegral) $ peek ptr
             0 -> return Nothing
             _ -> fail "Unknown return value from fluid_settings_getint"

getIntDefault :: Settings -> String -> IO Int
getIntDefault (Settings p) k = do
    withCString k $ \k' -> do
        fmap fromIntegral $ fluid_settings_getint_default p k'

getIntRange :: Settings -> String -> IO (Int,Int)
getIntRange (Settings p) k = do
    withCString k $ \k' -> alloca $ \ptrLo -> alloca $ \ptrHi -> do
        fluid_settings_getint_range p k' ptrLo ptrHi
        liftM2 (,) (fmap fromIntegral $ peek ptrLo) (fmap fromIntegral $ peek ptrHi)

listOptions :: Settings -> String -> IO [String]
listOptions (Settings p) k = do
    withCString k $ \k' -> do
        ret <- newIORef []
        let iter _ _ opt = do
                opt' <- peekCString opt
                modifyIORef ret (opt':)
        iter' <- mkFluidSettingsForeachOptionT iter
        fluid_settings_foreach_option p k' nullPtr iter'
        readIORef ret

data SettingType = NumType | IntType | StrType
    deriving (Enum,Show)

listSettings :: Settings -> IO [(String, SettingType)]
listSettings (Settings p) = do
    ret <- newIORef []
    let iter _ name typ = do
        name' <- peekCString name
        case typ of
             0 -> modifyIORef ret ((name',NumType):)
             1 -> modifyIORef ret ((name',IntType):)
             2 -> modifyIORef ret ((name',StrType):)
             _ -> return ()
    iter' <- mkFluidSettingsForeachT iter
    fluid_settings_foreach p nullPtr iter'
    readIORef ret
