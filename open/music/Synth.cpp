#include "Synth.h"

FluidSynth::FluidSynth(std::string soundfont)
{
	settings_ = new_fluid_settings();
#ifdef WIN32
	fluid_settings_setstr(settings_, "audio.driver", "dsound");
#elif UNIX
	fluid_settings_setstr(settings_, "audio.driver", "oss");
#endif
	synth_ = new_fluid_synth(settings_);
	sfid_ = fluid_synth_sfload(synth_, soundfont.c_str(), 1);
	driver_ = new_fluid_audio_driver(settings_, synth_);

	for (int i = 0; i < NUM_CHANNELS; i++) {
		channels_[i] = true;
	}
}

FluidSynth::~FluidSynth()
{
	delete_fluid_audio_driver(driver_);
	fluid_synth_sfunload(synth_, sfid_, 1);
	delete_fluid_synth(synth_);
	delete_fluid_settings(settings_);
}

FluidSynthTrack* FluidSynth::new_track()
{
	for (int i = 0; i < NUM_CHANNELS; i++) {
		if (channels_[i]) {
			channels_[i] = false;
			return new FluidSynthTrack(this, i);
		}
	}
	return 0;
}


FluidSynthTrack::FluidSynthTrack(FluidSynth* synth, int channel)
	: synth_(synth), channel_(channel)
{
	set_patch(0, 0);
}

FluidSynthTrack::~FluidSynthTrack()
{
	synth_->channels_[channel_] = true;
}

Synth* FluidSynthTrack::get_synth() const 
{
	return synth_;
}

void FluidSynthTrack::set_patch(int bank, int patch)
{
	fluid_synth_bank_select(synth_->synth_, channel_, bank);
	fluid_synth_program_change(synth_->synth_, channel_, patch);
}

void FluidSynthTrack::note_on(int noteid, int velocity)
{
	fluid_synth_noteon(synth_->synth_, channel_, noteid, velocity);
}

void FluidSynthTrack::note_off(int noteid)
{
	fluid_synth_noteoff(synth_->synth_, channel_, noteid);
}
