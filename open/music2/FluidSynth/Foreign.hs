{-# INCLUDE <fluidsynth.h> #-}
{-# LANGUAGE ForeignFunctionInterface #-}

module FluidSynth.Foreign where

import Foreign
import Foreign.C

----------------
-- settings.h --
----------------

data FluidSettingsT

newtype Settings = Settings (Ptr FluidSettingsT)

foreign import ccall new_fluid_settings
    :: IO (Ptr FluidSettingsT)

foreign import ccall delete_fluid_settings
    :: Ptr FluidSettingsT -> IO ()

foreign import ccall fluid_settings_setstr
    :: Ptr FluidSettingsT -> CString -> CString -> IO CInt

foreign import ccall fluid_settings_getstr
    :: Ptr FluidSettingsT -> CString -> Ptr CString -> IO CInt

foreign import ccall fluid_settings_getstr_default
    :: Ptr FluidSettingsT -> CString -> IO CString

foreign import ccall fluid_settings_setnum
    :: Ptr FluidSettingsT -> CString -> CDouble -> IO CInt

foreign import ccall fluid_settings_getnum
    :: Ptr FluidSettingsT -> CString -> Ptr CDouble -> IO CInt

foreign import ccall fluid_settings_getnum_default
    :: Ptr FluidSettingsT -> CString -> IO CDouble

foreign import ccall fluid_settings_getnum_range
    :: Ptr FluidSettingsT -> CString -> Ptr CDouble -> Ptr CDouble -> IO ()

foreign import ccall fluid_settings_setint
    :: Ptr FluidSettingsT -> CString -> CInt -> IO CInt

foreign import ccall fluid_settings_getint
    :: Ptr FluidSettingsT -> CString -> Ptr CInt -> IO CInt

foreign import ccall fluid_settings_getint_default
    :: Ptr FluidSettingsT -> CString -> IO CInt

foreign import ccall fluid_settings_getint_range
    :: Ptr FluidSettingsT -> CString -> Ptr CInt -> Ptr CInt -> IO ()

type FluidSettingsForeachOptionT = Ptr () -> CString -> CString -> IO ()

foreign import ccall fluid_settings_foreach_option
    :: Ptr FluidSettingsT -> CString -> Ptr () -> FunPtr FluidSettingsForeachOptionT -> IO ()

foreign import ccall "wrapper" mkFluidSettingsForeachOptionT
    :: FluidSettingsForeachOptionT -> IO (FunPtr FluidSettingsForeachOptionT)

type FluidSettingsForeachT = Ptr () -> CString -> CInt -> IO ()

foreign import ccall fluid_settings_foreach
    :: Ptr FluidSettingsT -> Ptr () -> FunPtr FluidSettingsForeachT -> IO ()

foreign import ccall "wrapper" mkFluidSettingsForeachT
    :: FluidSettingsForeachT -> IO (FunPtr FluidSettingsForeachT)

-------------
-- synth.h --
-------------

data FluidSynthT

newtype Synth = Synth (FluidSynthT)

foreign import ccall new_fluid_synth 
    :: Ptr FluidSettingsT -> IO (Ptr FluidSynthT)

foreign import ccall delete_fluid_synth
    :: Ptr FluidSynthT -> IO CInt

foreign import ccall fluid_synth_get_settings
    :: Ptr FluidSynthT -> IO (Ptr FluidSettingsT)

foreign import ccall fluid_synth_noteon
    :: Ptr FluidSynthT -> CInt -> CInt -> CInt -> IO CInt -- what is the return value for?

foreign import ccall fluid_synth_noteoff
    :: Ptr FluidSynthT -> CInt -> CInt -> IO CInt

foreign import ccall fluid_synth_cc
    :: Ptr FluidSynthT -> CInt -> CInt -> CInt -> IO CInt

foreign import ccall fluid_synth_get_cc
    :: Ptr FluidSynthT -> CInt -> CInt -> Ptr CInt -> IO CInt

foreign import ccall fluid_synth_pitch_bend
    :: Ptr FluidSynthT -> CInt -> CInt -> IO CInt

foreign import ccall fluid_synth_get_pitch_bend
    :: Ptr FluidSynthT -> CInt -> Ptr CInt -> IO CInt

foreign import ccall fluid_synth_pitch_wheel_sens
    :: Ptr FluidSynthT -> CInt -> CInt -> IO CInt

foreign import ccall fluid_synth_program_change
    :: Ptr FluidSynthT -> CInt -> CInt -> IO CInt

foreign import ccall fluid_synth_bank_select
    :: Ptr FluidSynthT -> CInt -> CUInt -> IO CInt

foreign import ccall fluid_synth_sfont_select
    :: Ptr FluidSynthT -> CInt -> CUInt -> IO CInt

foreign import ccall fluid_synth_program_select
    :: Ptr FluidSynthT -> CInt -> CUInt -> CUInt -> CUInt -> IO CInt

foreign import ccall fluid_synth_get_program
    :: Ptr FluidSynthT -> CInt -> Ptr CUInt -> Ptr CUInt -> Ptr CUInt -> IO CInt

foreign import ccall fluid_synth_program_reset
    :: Ptr FluidSynthT -> IO CInt

foreign import ccall fluid_synth_system_reset
    :: Ptr FluidSynthT -> IO CInt

foreign import ccall fluid_synth_sfload
    :: Ptr FluidSynthT -> CString -> CInt -> IO CInt

foreign import ccall fluid_synth_sfreload
    :: Ptr FluidSynthT -> CUInt -> IO CInt

foreign import ccall fluid_synth_sfunload
    :: Ptr FluidSynthT -> CUInt -> CInt -> IO CInt

foreign import ccall fluid_synth_sfcount
    :: Ptr FluidSynthT -> IO CInt
