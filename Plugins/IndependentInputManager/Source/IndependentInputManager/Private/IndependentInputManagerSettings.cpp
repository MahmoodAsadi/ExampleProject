// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManagerSettings.h"

#include "IndependentInputSubsystem.h"


void UIndependentInputManagerSettings::DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo)
{
	if (!InDeviceInfo.IsValid())
		return;

	if (!ConnectedDevices.Contains(InDeviceInfo))
		ConnectedDevices.Add(InDeviceInfo);
}

void UIndependentInputManagerSettings::DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo)
{
	if (!InDeviceInfo.IsValid())
		return;

	ConnectedDevices.Remove(InDeviceInfo);
}

void UIndependentInputManagerSettings::UpdatePluggedDeviceInfo(const FJoystickDeviceInfo& InDeviceInfo)
{
	if (!InDeviceInfo.IsValid())
		return;

	if (int32 FoundIndex = ConnectedDevices.IndexOfByKey(InDeviceInfo); FoundIndex != INDEX_NONE)
		ConnectedDevices[FoundIndex] = InDeviceInfo;
}

TArray<FJoystickDeviceKeyMapping> UIndependentInputManagerSettings::GetDevicesKeyMappings() const
{
	TArray<FJoystickDeviceKeyMapping> Mappings;
	DevicesKeyMapping.GenerateValueArray(Mappings);
	return Mappings;
}

bool UIndependentInputManagerSettings::K2_FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier, FJoystickDeviceKeyMapping& OutDeviceKeyMapping)
{
	OutDeviceKeyMapping = FJoystickDeviceKeyMapping();
	FJoystickDeviceKeyMapping* DeviceKeyMapping = FindDeviceKeyMappings(DeviceIdentifier);
	if (DeviceKeyMapping)
	{
		OutDeviceKeyMapping = *DeviceKeyMapping;
		return true;
	}
	return false;
}

FJoystickDeviceKeyMapping* UIndependentInputManagerSettings::FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier)
{
	return DevicesKeyMapping.Find(DeviceIdentifier);
}

const FJoystickDeviceKeyMapping* UIndependentInputManagerSettings::FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	return DevicesKeyMapping.Find(DeviceIdentifier);
}

void UIndependentInputManagerSettings::AddOrUpdateDeviceKeyMapping(const FJoystickDeviceIdentifier& DeviceIdentifier, const FJoystickDeviceKeyMapping& DeviceKeyMapping)
{
	FJoystickDeviceKeyMapping NewDeviceKeyMapping = DeviceKeyMapping;
	GenerateRuntimeKeysForDeviceMapping(NewDeviceKeyMapping);
	DevicesKeyMapping.Add(DeviceIdentifier, NewDeviceKeyMapping);
	TryUpdateDefaultConfigFile();
}

void UIndependentInputManagerSettings::GenerateDevicesRuntimeKeys()
{
	for (TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& DeviceMapping : DevicesKeyMapping)
	{
		GenerateRuntimeKeysForDeviceMapping(DeviceMapping.Value);
	}
}

void UIndependentInputManagerSettings::GenerateRuntimeKeysForDeviceMapping(FJoystickDeviceKeyMapping& DeviceKeyMapping)
{
	UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();

	// Register button input keys.
	for (TPair<int32, FJoystickButtonKeyMapping>& ButtonMapping : DeviceKeyMapping.ButtonMappings)
	{
		if (ButtonMapping.Value.Key.bCustomKey)
			ButtonMapping.Value.Key.bIsAxisKey = false;

		ButtonMapping.Value.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
	}

	// Register axis input keys.
	for (TPair<int32, FJoystickAxisKeyMapping>& AxisMapping : DeviceKeyMapping.AxisMappings)
	{
		if (AxisMapping.Value.Key.bCustomKey)
			AxisMapping.Value.Key.bIsAxisKey = true;

		AxisMapping.Value.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);

		for (FAxisVirtualButtonKeyMapping& VirtualButtonMapping : AxisMapping.Value.VirtualButtons)
		{
			if (VirtualButtonMapping.Key.bCustomKey)
				VirtualButtonMapping.Key.bIsAxisKey = false;

			VirtualButtonMapping.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		}
	}

	// Register hat input keys.
	for (TPair<int32, FJoystickHatKeyMapping>& HatMapping : DeviceKeyMapping.HatMappings)
	{
		if (HatMapping.Value.Up.bCustomKey)
			HatMapping.Value.Up.bIsAxisKey = false;

		if (HatMapping.Value.Down.bCustomKey)
			HatMapping.Value.Down.bIsAxisKey = false;

		if (HatMapping.Value.Left.bCustomKey)
			HatMapping.Value.Left.bIsAxisKey = false;

		if (HatMapping.Value.Right.bCustomKey)
			HatMapping.Value.Right.bIsAxisKey = false;

		HatMapping.Value.Up.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		HatMapping.Value.Down.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		HatMapping.Value.Left.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		HatMapping.Value.Right.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
	}

	// Register relative ball input keys.
	for (TPair<int32, FJoystickBallKeyMapping>& BallMapping : DeviceKeyMapping.BallMappings)
	{
		if (BallMapping.Value.X.Key.bCustomKey)
			BallMapping.Value.X.Key.bIsAxisKey = true;

		if (BallMapping.Value.Y.Key.bCustomKey)
			BallMapping.Value.Y.Key.bIsAxisKey = true;

		BallMapping.Value.X.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping, true);
		BallMapping.Value.Y.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping, true);

		for (FAxisVirtualButtonKeyMapping& VirtualButtonMapping : BallMapping.Value.X.VirtualButtons)
		{
			if (VirtualButtonMapping.Key.bCustomKey)
				VirtualButtonMapping.Key.bIsAxisKey = false;

			VirtualButtonMapping.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		}

		for (FAxisVirtualButtonKeyMapping& VirtualButtonMapping : BallMapping.Value.Y.VirtualButtons)
		{
			if (VirtualButtonMapping.Key.bCustomKey)
				VirtualButtonMapping.Key.bIsAxisKey = false;

			VirtualButtonMapping.Key.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		}

		if (InputSubsystem)
			InputSubsystem->CreateDevicePairedKey(DeviceKeyMapping, BallMapping.Value.X.Key, BallMapping.Value.Y.Key, true);
	}

	// Register touchpad input keys.
	for (TPair<int32, FJoystickTouchpadKeyMapping>& TouchpadMapping : DeviceKeyMapping.TouchpadMappings)
	{
		for (FJoystickTouchpadFingerKeyMapping& Finger : TouchpadMapping.Value.Fingers)
		{
			if (Finger.Touch.bCustomKey)
				Finger.Touch.bIsAxisKey = false;

			if (Finger.PositionX.bCustomKey)
				Finger.PositionX.bIsAxisKey = true;

			if (Finger.PositionY.bCustomKey)
				Finger.PositionY.bIsAxisKey = true;

			if (Finger.Pressure.bCustomKey)
				Finger.Pressure.bIsAxisKey = true;

			Finger.Touch.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
			Finger.PositionX.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
			Finger.PositionY.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);

			if (InputSubsystem)
				InputSubsystem->CreateDevicePairedKey(DeviceKeyMapping, Finger.PositionX, Finger.PositionY);

			Finger.Pressure.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		}
	}

	// Register sensor input keys.
	for (TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : DeviceKeyMapping.SensorMappings)
	{
		if (SensorMapping.Value.X.bCustomKey)
			SensorMapping.Value.X.bIsAxisKey = true;

		if (SensorMapping.Value.Y.bCustomKey)
			SensorMapping.Value.Y.bIsAxisKey = true;

		if (SensorMapping.Value.Z.bCustomKey)
			SensorMapping.Value.Z.bIsAxisKey = true;

		SensorMapping.Value.X.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		SensorMapping.Value.Y.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
		SensorMapping.Value.Z.GenerateRuntimeKeyIfNeeded(DeviceKeyMapping);
	}

}
