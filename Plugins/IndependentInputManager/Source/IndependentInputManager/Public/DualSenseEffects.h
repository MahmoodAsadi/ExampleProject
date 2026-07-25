// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#pragma pack(push, 1)

struct FDS5EffectsState
{
	// 0
	uint8 EnableBits1 = 0;

	// 1
	uint8 EnableBits2 = 0;

	// 2
	uint8 RumbleRight = 0;

	// 3
	uint8 RumbleLeft = 0;

	// 4
	uint8 HeadphoneVolume = 0;

	// 5
	uint8 SpeakerVolume = 0;

	// 6
	uint8 MicrophoneVolume = 0;

	// 7
	uint8 AudioEnableBits = 0;

	// 8
	uint8 MicLightMode = 0;

	// 9
	uint8 AudioMuteBits = 0;

	// 10
	uint8 RightTriggerEffect[11] = {};

	// 21
	uint8 LeftTriggerEffect[11] = {};

	// 32
	uint8 Unknown1[6] = {};

	// 38
	uint8 EnableBits3 = 0;

	// 39
	uint8 Unknown2[2] = {};

	// 41
	uint8 LedAnimation = 0;

	// 42
	uint8 LedBrightness = 0;

	// 43
	uint8 PlayerLED = 0;

	// 44
	uint8 LedRed = 0;

	// 45
	uint8 LedGreen = 0;

	// 46
	uint8 LedBlue = 0;
};

#pragma pack(pop)

static_assert(sizeof(FDS5EffectsState) == 47);