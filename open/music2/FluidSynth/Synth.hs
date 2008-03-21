module FluidSynth.Synth 
    ( )
where

import FluidSynth.Foreign

type MIDIChannel

new :: Settings -> IO Synth
new (Settings p) = fmap Synth (new_fluid_synth p)

delete :: Synth -> IO ()
delete (Synth p) = do
    delete_fluid_synth p  -- XXX errors
    return ()

getSettings :: Synth -> IO Settings
getSettings (Synth p) = 
    fmap Settings (fluid_synth_get_settings p)

noteOn :: Synth -> MIDIChannel -> Int -> Int -> IO ()
noteOn (Synth p) chan note vel = do
    fluid_synth_noteon p (fromIntegral chan)
                         (fromIntegral note)
                         (fromIntegral vel)
    return ()

noteOff :: Synth -> MIDIChannel -> Int -> IO ()
noteOff (Synth p) chan note = do
    fluid_synth_noteoff p (fromIntegral chan) (fromIntegral note)
    return ()

controlChange :: Synth -> MIDIChannel -> Int -> Int -> IO ()
controlChange (Synth p) chan control value =
    fluid_synth_cc p (fromIntegral chan)
                     (fromIntegral control)
                     (fromIntegral value)
    return ()

getControl :: Synth -> MIDIChannel -> Int -> IO Int
getControl (Synth p) chan control =
    alloca $ \ptr -> do
        fluid_synth_get_cc (fromIntegral chan) (fromIntegral control) ptr
        fmap fromIntegral $ peek ptr

pitchBend :: Synth -> MIDIChannel -> Int -> IO ()
pitchBend (Synth p) chan bend = do
    fluid_synth_pitch_bend p (fromIntegral chan) (fromIntegral bend)
    return ()

getPitchBend :: Synth -> MIDIChannel -> IO Int
getPitchBend (Synth p) chan = 
    alloca $ \ptr -> do
        fluid_synth_get_pitch_bend p (fromIntegral chan) ptr
        fmap fromIntegral $ peek ptr

pitchWheelSensitivity :: Synth -> MIDIChannel -> Int -> IO ()
pitchWheelSensitivity (Synth p) chan sens = do
    fluid_synth_pitch_wheel_sens (fromIntegral chan) (fromIntegral sens)
    return ()

programChange :: Synth -> MIDIChannel -> Int -> IO ()
programChange (Synth p) chan prog = do
    fluid_synth_program_change p (fromIntegral chan) (fromIntegral prog)
    return ()

bankSelect :: Synth -> MIDIChannel -> Int -> IO ()
bankSelect (Synth p) chan bank = do
    fluid_synth_bank_select p (fromIntegral chan) (fromIntegral bank)
    return ()

soundfontSelect :: Synth -> MIDIChannel -> Int -> IO ()
soundfontSelect (Synth p) chan sfid = do
    fluid_synth_sfont_select p (fromIntegral chan) (fromIntegral sfid)
    return ()

programSelect :: Synth -> MIDIChannel -> Int -> Int -> Int -> IO ()
programSelect (Synth p) chan sfid bank preset = do
    fluid_synth_program_select p (fromIntegral chan) (fromIntegral sfid) (fromIntegral bank) (fromIntegral preset)
    return ()

getProgram :: Synth -> MIDIChannel -> IO (Int,Int,Int)
getProgram (Synth p) chan = do
    alloca $ \sfid -> alloca $ \bank -> alloca $ \preset -> do
        fluid_synth_get_program p (fromIntegral chan) sfid bank preset
        liftM3 (,,) (fromIntegral $ peek $ sfid)
                    (fromIntegral $ peek $ bank)
                    (fromIntegral $ peek $ preset)
