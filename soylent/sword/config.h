#ifndef __CONFIG_H__
#define __CONFIG_H__

#define SOUNDDIR "Media/Sounds/"

class Sound;
extern Sound SOUND;

enum DrawEye {
    DRAWEYE_BOTH,
    DRAWEYE_LEFT,
    DRAWEYE_RIGHT
};
extern DrawEye EYE;

#endif
