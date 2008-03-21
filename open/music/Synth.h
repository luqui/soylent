#ifndef __SYNTH_H__
#define __SYNTH_H__

#include <string>
#include <fluidsynth.h>

class Synth;

class SynthTrack
{
public:
	virtual ~SynthTrack() { }

	virtual Synth* get_synth() const = 0;

	virtual void set_patch(int bank, int patch) = 0;
	virtual void note_on(int noteid, int vel = 100) = 0;
	virtual void note_off(int noteid) = 0;
};

class Synth
{
public:
	virtual ~Synth() { }

	virtual SynthTrack* new_track() = 0;
};

class FluidSynth;

class FluidSynthTrack : public SynthTrack
{
	friend class FluidSynth;
public:
	~FluidSynthTrack();

	Synth* get_synth() const;

	void set_patch(int bank, int patch);
	void note_on(int noteid, int vel = 100);
	void note_off(int noteid);
private:
	FluidSynthTrack(FluidSynth* synth, int channel);

	int channel_;
	FluidSynth* synth_;
};

class FluidSynth : public Synth
{
	friend class FluidSynthTrack;
public:
	FluidSynth(std::string soundfont);
	~FluidSynth();
	FluidSynthTrack* new_track();
private:
	enum { NUM_CHANNELS = 16 };
	
	fluid_settings_t* settings_;
	fluid_synth_t* synth_;
	fluid_audio_driver_t* driver_;
	int sfid_;
	bool channels_[NUM_CHANNELS];
};

#endif
