/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

/*
* Based on the Reverse Engineering work of Christophe Fontanel,
* maintainer of the Dungeon Master Encyclopaedia (http://dmweb.free.fr/)
*/

#ifndef DM_SOUND_H
#define DM_SOUND_H

#include "dm/dm.h"

namespace DM {

enum SoundMode {
	kDMSoundModeDoNotPlaySound = -1, // @ CM1_MODE_DO_NOT_PLAY_SOUND
	kDMSoundModePlayImmediately = 0, // @ C00_MODE_PLAY_IMMEDIATELY
	kDMSoundModePlayIfPrioritized = 1, // @ C01_MODE_PLAY_IF_PRIORITIZED
	kDMSoundModePlayOneTickLater = 2 // @ C02_MODE_PLAY_ONE_TICK_LATER
};

#define k34_D13_soundCount 34 // @ D13_SOUND_COUNT

class SoundData {
public:
	uint32 _byteCount;
	byte *_firstSample;
	uint32 _sampleCount;
	SoundData() : _byteCount(0), _firstSample(nullptr), _sampleCount(0) {}
}; // @ SOUND_DATA

class Sound {
public:
	int16 _graphicIndex;
	byte _period;
	byte _priority;
	byte _loudDistance;
	byte _softDistance;
	Sound(int16 index, byte period, byte priority, byte loudDist, byte softDist) :
		_graphicIndex(index), _period(period), _priority(priority), _loudDistance(loudDist), _softDistance(softDist) {}
	Sound() : _graphicIndex(0), _period(0), _priority(0), _loudDistance(0), _softDistance(0) {}
}; // @ Sound

class PendingSound {
public:
	uint8 _leftVolume;
	uint8 _rightVolume;
	int16 _soundIndex;
	PendingSound(uint8 leftVolume, uint8 rightVolume, int16 soundIndex) :
		_leftVolume(leftVolume), _rightVolume(rightVolume), _soundIndex(soundIndex) {}
};

class SoundMan {
	DMEngine *_vm;

protected:
	SoundMan(DMEngine *vm);
public:
	virtual ~SoundMan();

	static SoundMan *getSoundMan(DMEngine *vm, const DMADGameDescription *gameVersion);

	SoundData _soundData[k34_D13_soundCount]; // @ K0024_as_SoundData
	Common::Queue<PendingSound> _pendingSounds;

	virtual void loadSounds(); // @ F0503_SOUND_LoadAll
	virtual void requestPlay(uint16 soundIndex, int16 mapX, int16 mapY, SoundMode soundMode); // @ F0064_SOUND_RequestPlay_CPSD
	virtual void play(uint16 soundIndex, uint16 period, uint8 leftVolume, uint8 rightVolume); // @ F0060_SOUND_Play
	void playPendingSound(); // @ F0065_SOUND_PlayPendingSound_CPSD
	bool soundGetVolume(int16 mapX, int16 mapY, uint8 *leftVolume, uint8 *rightVolume); // @ F0505_SOUND_GetVolume

	Sound _sounds[k34_D13_soundCount];
	void initConstants();
};

class SoundMan_Atari: public SoundMan {
	friend class SoundMan;

	SoundMan_Atari(DMEngine *vm): SoundMan(vm) {};
public:
	void loadSounds() override {} // @ F0503_SOUND_LoadAll
	void requestPlay(uint16 soundIndex, int16 mapX, int16 mapY, SoundMode soundMode) override {} // @ F0064_SOUND_RequestPlay_CPSD
	void play(uint16 soundIndex, uint16 period, uint8 leftVolume, uint8 rightVolume) override {} // @ F0060_SOUND_Play
};

}

#endif
