// Fill out your copyright notice in the Description page of Project Settings.

#include "DualSenseTriggerEffectGenerator.h"

namespace
{
	// Byte values the firmware inspects to select an adaptive trigger effect.
	// Only Off/Feedback/Weapon/Vibration are officially documented; Bow/Galloping/Machine
	// are unofficial-but-present leftovers that Sony could remove in a future update.
	enum class EDualSenseTriggerEffectOpcode : uint8
	{
		Off       = 0x05,
		Feedback  = 0x21,
		Bow       = 0x22,
		Galloping = 0x23,
		Weapon    = 0x25,
		Vibration = 0x26,
		Machine   = 0x27,
	};
}

void FDualSenseTriggerEffectGenerator::Clear(uint8 Out[11])
{
	FMemory::Memzero(Out, 11);
}

void FDualSenseTriggerEffectGenerator::Off(uint8 Out[11])
{
	Clear(Out);
	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Off);
}

bool FDualSenseTriggerEffectGenerator::Feedback(uint8 Out[11], uint8 Position, uint8 Strength)
{
	Position = FMath::Clamp<uint8>(Position, 0, 9);
	Strength = FMath::Clamp<uint8>(Strength, 0, 8);

	if (Strength == 0)
	{
		Off(Out);
		return true;
	}
	
	Clear(Out);

	// Every zone from Position to the end of the trigger's range is activated at the
	// same 3-bit force value, matching the official Feedback encoding.
	const uint8 ForceValue = (Strength - 1) & 0x07;
	uint32 ForceZones = 0;
	uint16 ActiveZones = 0;

	for (int32 Zone = Position; Zone < 10; ++Zone)
	{
		ForceZones |= static_cast<uint32>(ForceValue) << (3 * Zone);
		ActiveZones |= static_cast<uint16>(1 << Zone);
	}

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Feedback);
	Out[1] = static_cast<uint8>((ActiveZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((ActiveZones >> 8) & 0xFF);
	Out[3] = static_cast<uint8>((ForceZones >> 0) & 0xFF);
	Out[4] = static_cast<uint8>((ForceZones >> 8) & 0xFF);
	Out[5] = static_cast<uint8>((ForceZones >> 16) & 0xFF);
	Out[6] = static_cast<uint8>((ForceZones >> 24) & 0xFF);
	
	return true;
}

bool FDualSenseTriggerEffectGenerator::Weapon(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength)
{
	StartPosition = FMath::Clamp<uint8>(StartPosition, 2, 7);
	EndPosition = FMath::Clamp<uint8>(EndPosition, StartPosition + 1, 8);

	Strength = FMath::Clamp<uint8>(Strength, 0, 8);
	if (Strength == 0)
	{
		Off(Out);
		return true;
	}

	Clear(Out);

	const uint16 StartAndStopZones = static_cast<uint16>((1 << StartPosition) | (1 << EndPosition));

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Weapon);
	Out[1] = static_cast<uint8>((StartAndStopZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((StartAndStopZones >> 8) & 0xFF);
	// Only one zone pair is ever active here, so the 3-bit force value fits in a full byte
	// with no packing required.
	Out[3] = static_cast<uint8>(Strength - 1);

	return true;
}

bool FDualSenseTriggerEffectGenerator::Vibration(uint8 Out[11], uint8 Position, uint8 Amplitude, uint8 Frequency)
{
	Position = FMath::Clamp<uint8>(Position, 0, 9);
	Amplitude = FMath::Clamp<uint8>(Amplitude, 0, 8);

	if (Amplitude == 0 || Frequency == 0)
	{
		Off(Out);
		return true;
	}

	Clear(Out);

	const uint8 StrengthValue = (Amplitude - 1) & 0x07;
	uint32 AmplitudeZones = 0;
	uint16 ActiveZones = 0;

	for (int32 Zone = Position; Zone < 10; ++Zone)
	{
		AmplitudeZones |= static_cast<uint32>(StrengthValue) << (3 * Zone);
		ActiveZones |= static_cast<uint16>(1 << Zone);
	}

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Vibration);
	Out[1] = static_cast<uint8>((ActiveZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((ActiveZones >> 8) & 0xFF);
	Out[3] = static_cast<uint8>((AmplitudeZones >> 0) & 0xFF);
	Out[4] = static_cast<uint8>((AmplitudeZones >> 8) & 0xFF);
	Out[5] = static_cast<uint8>((AmplitudeZones >> 16) & 0xFF);
	Out[6] = static_cast<uint8>((AmplitudeZones >> 24) & 0xFF);
	Out[9] = Frequency;

	return true;
}

bool FDualSenseTriggerEffectGenerator::Bow(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce)
{
	// The reference permits StartPosition 0-8, but EndPosition must be greater and
	// cannot exceed 8. Seven is therefore the highest effective starting zone.
	StartPosition = FMath::Clamp<uint8>(StartPosition, 0, 7);
	EndPosition = FMath::Clamp<uint8>(EndPosition, StartPosition + 1, 8);

	Strength = FMath::Clamp<uint8>(Strength, 0, 8);
	SnapForce = FMath::Clamp<uint8>(SnapForce, 0, 8);
	
	if (Strength == 0 || SnapForce == 0)
	{
		Off(Out);
		return true;
	}

	Clear(Out);

	const uint16 StartAndStopZones = static_cast<uint16>((1 << StartPosition) | (1 << EndPosition));
	const uint32 ForcePair = static_cast<uint32>((((Strength - 1) & 0x07) << (3 * 0))
	                                            | (((SnapForce - 1) & 0x07) << (3 * 1)));

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Bow);
	Out[1] = static_cast<uint8>((StartAndStopZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((StartAndStopZones >> 8) & 0xFF);
	Out[3] = static_cast<uint8>((ForcePair >> 0) & 0xFF);
	Out[4] = static_cast<uint8>((ForcePair >> 8) & 0xFF);

	return true;
}

bool FDualSenseTriggerEffectGenerator::Galloping(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency)
{
	StartPosition = FMath::Clamp<uint8>(StartPosition, 0, 8);
	EndPosition = FMath::Clamp<uint8>(EndPosition, StartPosition + 1, 9);

	FirstFoot = FMath::Clamp<uint8>(FirstFoot, 0, 6);
	SecondFoot = FMath::Clamp<uint8>(SecondFoot, FirstFoot + 1, 7);

	if (Frequency == 0)
	{
		Off(Out);
		return true;
	}

	Clear(Out);

	const uint16 StartAndStopZones = static_cast<uint16>((1 << StartPosition) | (1 << EndPosition));
	const uint32 TimeAndRatio = static_cast<uint32>(((SecondFoot & 0x07) << (3 * 0))
	                                               | ((FirstFoot & 0x07) << (3 * 1)));

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Galloping);
	Out[1] = static_cast<uint8>((StartAndStopZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((StartAndStopZones >> 8) & 0xFF);
	Out[3] = static_cast<uint8>((TimeAndRatio >> 0) & 0xFF);
	// Only one time/ratio pair is ever active, so frequency fits in a full byte with no
	// packing required.
	Out[4] = Frequency;

	return true;
}

bool FDualSenseTriggerEffectGenerator::Machine(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period)
{
	StartPosition = FMath::Clamp<uint8>(StartPosition, 0, 8);
	EndPosition = FMath::Clamp<uint8>(EndPosition, StartPosition + 1, 9);

	AmplitudeA = FMath::Clamp<uint8>(AmplitudeA, 0, 7);
	AmplitudeB = FMath::Clamp<uint8>(AmplitudeB, 0, 7);
	
	if (Frequency == 0)
	{
		Off(Out);
		return true;
	}

	Clear(Out);

	const uint16 StartAndStopZones = static_cast<uint16>((1 << StartPosition) | (1 << EndPosition));
	const uint32 StrengthPair = static_cast<uint32>(((AmplitudeA & 0x07) << (3 * 0))
	                                               | ((AmplitudeB & 0x07) << (3 * 1)));

	Out[0] = static_cast<uint8>(EDualSenseTriggerEffectOpcode::Machine);
	Out[1] = static_cast<uint8>((StartAndStopZones >> 0) & 0xFF);
	Out[2] = static_cast<uint8>((StartAndStopZones >> 8) & 0xFF);
	Out[3] = static_cast<uint8>((StrengthPair >> 0) & 0xFF);
	Out[4] = Frequency;
	Out[5] = Period;

	return true;
}
