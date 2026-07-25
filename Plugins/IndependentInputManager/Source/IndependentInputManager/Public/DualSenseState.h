// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#pragma pack(push,1)

struct FDS5State
{
	uint8 LeftStickX;
	uint8 LeftStickY;
	uint8 RightStickX;
	uint8 RightStickY;

	uint8 LeftTrigger;
	uint8 RightTrigger;

	uint8 Counter;

	uint8 Buttons[4];

	uint32 Sequence;

	int16 GyroX;
	int16 GyroY;
	int16 GyroZ;

	int16 AccelX;
	int16 AccelY;
	int16 AccelZ;

	uint32 SensorTimestamp;

	uint8 SensorTemp;

	uint8 Touch1Counter;
	uint8 Touch1Data[3];

	uint8 Touch2Counter;
	uint8 Touch2Data[3];

	uint8 Unknown[8];

	uint32 Timer;

	uint8 Battery;

	uint8 ConnectionState;
};

#pragma pack(pop)

static_assert(sizeof(FDS5State) == 54);