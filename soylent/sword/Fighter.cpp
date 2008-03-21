#include "Fighter.h"

void Fighter::StartRecording()
{
	if (recording_.is_open())
	{
		recording_.close();
	}
	recording_.open("recording.txt");
}
void Fighter::StopRecording()
{
	if (recording_.is_open())
	{
		recording_.close();
	}
}
bool Fighter::IsRecording()
{
	return recording_.is_open();
}

void Fighter::Replay()
{
	if (playback_.is_open())
		playback_.close();

	playback_.open("recording.txt");
	loop_playback_ = false;
}
void Fighter::LoopPlayback(const char *file)
{
	playbackfile_ = file;
	if (playback_.is_open())
	{
		playback_.close();
	}

	playback_.open(file);
	loop_playback_ = true;
}
void Fighter::StopPlayback()
{
	playback_.close();
	loop_playback_ = false;
}
bool Fighter::IsReplaying()
{
	return playback_.is_open();
}
