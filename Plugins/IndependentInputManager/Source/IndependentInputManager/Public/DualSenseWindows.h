// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IndependentInputManagerTypes.h"

#if PLATFORM_WINDOWS

#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include "DualSenseEffects.h"
#include "DualSenseState.h"


/**
 * 
 */
class FDualSenseWindows
{
public:
	FDualSenseWindows();
	~FDualSenseWindows();

	bool ReadInputReport();

	const FDS5State& GetState() const { return State; }

	bool Open(uint16 VendorId, uint16 ProductId, const FString& SDLSerial);
	void Close();
	bool IsOpen() const;
	void ResetEffects();

	/*
	* NOTE: Position/Strength/Amplitude/Force parameters below are 0-8 (0-9 for zone
	*	positions where noted), matching the DualSense firmware's 10-zone resolution -
	*	NOT free 0-255 bytes. Out-of-range values are clamped to the nearest supported
	*	values before the effect is applied.
	*/
	bool SetAdaptiveTriggerResistance(EDualSenseTrigger Trigger, uint8 Position, uint8 Strength);
	bool SetAdaptiveTriggerWeapon(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength);
	bool SetAdaptiveTriggerVibration(EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency);
	bool SetAdaptiveTriggerBow(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);
	bool SetAdaptiveTriggerGalloping(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);
	bool SetAdaptiveTriggerMachine(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);
	bool ClearAdaptiveTriggerEffect(EDualSenseTrigger Trigger);
	void SetTriggerEffectsEnable(bool bEnable = true);

private:

	HANDLE DeviceHandle;
	FDS5EffectsState Effects;
	void LogDeviceCapabilities(HANDLE Handle, const TCHAR* DevicePath);

	bool ReadState(FDS5State& OutState);
	bool ApplyTriggerEffect(EDualSenseTrigger Trigger, const uint8 Effect[11]);
	bool WriteEffects();
	void ResetTriggerEffect(EDualSenseTrigger Trigger);
	uint8* GetTriggerBuffer(EDualSenseTrigger Trigger);

	FDS5State State;
	EDeviceConnectionType ConnectionType = EDeviceConnectionType::Unknown;
	uint16 InputReportLength = 0;
	uint16 OutputReportLength = 0;
	uint16 FeatureReportLength = 0;
	uint8 BluetoothSequence = 0;

};

#endif // PLATFORM_WINDOWS
