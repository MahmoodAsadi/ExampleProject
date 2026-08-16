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

bool UIndependentInputManagerSettings::GetSensorEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier, EDeviceSensorType Sensor) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	const FJoystickSensorKeyMapping* SensorMapping = DeviceKeyMapping->SensorMappings.Find(Sensor);
	if (!SensorMapping)
		return false;

	return SensorMapping->bEnabled;
}

bool UIndependentInputManagerSettings::SetSensorEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, EDeviceSensorType Sensor, bool bEnable)
{
	FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	FJoystickSensorKeyMapping* SensorMapping = DeviceKeyMapping->SensorMappings.Find(Sensor);
	if (!SensorMapping)
		return false;

	if (SensorMapping->bEnabled == bEnable)
		return true;

	SensorMapping->bEnabled = bEnable;
	TryUpdateDefaultConfigFile();

	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
		InputSubsystem->ReconnectDevice(DeviceIdentifier);

	return true;
}

void UIndependentInputManagerSettings::SetSensorEnableForAllDevices(EDeviceSensorType Sensor, bool bEnable)
{
	UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	bool bUpdatedAny = false;

	for (TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& DeviceMapping : DevicesKeyMapping)
	{
		if (FJoystickSensorKeyMapping* SensorKeyMapping = DeviceMapping.Value.SensorMappings.Find(Sensor))
		{
			if (SensorKeyMapping->bEnabled != bEnable)
			{
				bUpdatedAny = true;
				SensorKeyMapping->bEnabled = bEnable;
				if (IsValid(InputSubsystem))
					InputSubsystem->ReconnectDevice(DeviceMapping.Key);
			}
		}
	}

	if (bUpdatedAny)
		TryUpdateDefaultConfigFile();
}

bool UIndependentInputManagerSettings::GetRumbleSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->Rumble.bSupports;
}

bool UIndependentInputManagerSettings::GetRumbleEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->Rumble.IsEnabled();
}

bool UIndependentInputManagerSettings::GetTriggerRumbleSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->TriggerRumble.bSupports;
}

bool UIndependentInputManagerSettings::GetTriggerRumbleEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->TriggerRumble.IsEnabled();
}

bool UIndependentInputManagerSettings::GetAdaptiveTriggerEffectsSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->AdaptiveTriggerEffect.bSupports;
}

bool UIndependentInputManagerSettings::GetAdaptiveTriggerEffectsEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	const FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	return DeviceKeyMapping->AdaptiveTriggerEffect.IsEnabled();
}

bool UIndependentInputManagerSettings::SetRumbleEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable)
{
	FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	FJoystickFeatureConfig& Rumble = DeviceKeyMapping->Rumble;
	if (bEnable && !Rumble.bSupports)
		return false;

	if (Rumble.IsEnabledRequested() == bEnable)
		return true;

	const bool bWasEnabled = Rumble.IsEnabled();
	Rumble.SetEnabled(bEnable);
	TryUpdateDefaultConfigFile();

	if (bWasEnabled != Rumble.IsEnabled())
	{
		if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
			InputSubsystem->ReconnectDevice(DeviceIdentifier);
	}

	return true;
}

bool UIndependentInputManagerSettings::SetTriggerRumbleEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable)
{
	FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	FJoystickFeatureConfig& TriggerRumble = DeviceKeyMapping->TriggerRumble;
	if (bEnable && !TriggerRumble.bSupports)
		return false;

	if (TriggerRumble.IsEnabledRequested() == bEnable)
		return true;

	const bool bWasEnabled = TriggerRumble.IsEnabled();
	TriggerRumble.SetEnabled(bEnable);
	TryUpdateDefaultConfigFile();

	if (bWasEnabled != TriggerRumble.IsEnabled())
	{
		if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
			InputSubsystem->ReconnectDevice(DeviceIdentifier);
	}

	return true;
}

bool UIndependentInputManagerSettings::SetAdaptiveTriggerEffectsEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable)
{
	FJoystickDeviceKeyMapping* DeviceKeyMapping = DevicesKeyMapping.Find(DeviceIdentifier);
	if (!DeviceKeyMapping)
		return false;

	FJoystickFeatureConfig& AdaptiveTriggerEffect = DeviceKeyMapping->AdaptiveTriggerEffect;
	if (bEnable && !AdaptiveTriggerEffect.bSupports)
		return false;

	if (AdaptiveTriggerEffect.IsEnabledRequested() == bEnable)
		return true;

	const bool bWasEnabled = AdaptiveTriggerEffect.IsEnabled();
	if (bWasEnabled && !bEnable)
	{
		UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
		if (IsValid(InputSubsystem))
		{
			for (const FJoystickDeviceInfo& DeviceInfo : ConnectedDevices)
			{
				if (DeviceInfo.Identifier == DeviceIdentifier)
				{
					InputSubsystem->ClearAdaptiveTriggerEffect(DeviceInfo.InstanceId, EDualSenseTrigger::Left);
					InputSubsystem->ClearAdaptiveTriggerEffect(DeviceInfo.InstanceId, EDualSenseTrigger::Right);
				}
			}
		}
	}

	AdaptiveTriggerEffect.SetEnabled(bEnable);
	TryUpdateDefaultConfigFile();

	if (bWasEnabled != AdaptiveTriggerEffect.IsEnabled())
	{
		UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
		if (IsValid(InputSubsystem))
			InputSubsystem->ReconnectDevice(DeviceIdentifier);
	}

	return true;
}

void UIndependentInputManagerSettings::SetRumbleEnableForAllDevices(bool bEnable)
{
	UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	bool bUpdatedAny = false;

	for (TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& DeviceMapping : DevicesKeyMapping)
	{
		FJoystickFeatureConfig& Rumble = DeviceMapping.Value.Rumble;
		if (Rumble.IsEnabledRequested() != bEnable)
		{
			bUpdatedAny = true;
			const bool bWasEnabled = Rumble.IsEnabled();
			Rumble.SetEnabled(bEnable);
			if (bWasEnabled != Rumble.IsEnabled() && IsValid(InputSubsystem))
				InputSubsystem->ReconnectDevice(DeviceMapping.Key);
		}
	}

	if (bUpdatedAny)
		TryUpdateDefaultConfigFile();
}

void UIndependentInputManagerSettings::SetTriggerRumbleEnableForAllDevices(bool bEnable)
{
	UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	bool bUpdatedAny = false;

	for (TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& DeviceMapping : DevicesKeyMapping)
	{
		FJoystickFeatureConfig& TriggerRumble = DeviceMapping.Value.TriggerRumble;
		if (TriggerRumble.IsEnabledRequested() != bEnable)
		{
			bUpdatedAny = true;
			const bool bWasEnabled = TriggerRumble.IsEnabled();
			TriggerRumble.SetEnabled(bEnable);
			if (bWasEnabled != TriggerRumble.IsEnabled() && IsValid(InputSubsystem))
				InputSubsystem->ReconnectDevice(DeviceMapping.Key);
		}
	}

	if (bUpdatedAny)
		TryUpdateDefaultConfigFile();
}

void UIndependentInputManagerSettings::SetAdaptiveTriggerEffectsEnableForAllDevices(bool bEnable)
{
	UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	bool bUpdatedAny = false;

	if (!bEnable && IsValid(InputSubsystem))
	{
		for (const FJoystickDeviceInfo& DeviceInfo : ConnectedDevices)
		{
			InputSubsystem->ClearAdaptiveTriggerEffect(DeviceInfo.InstanceId, EDualSenseTrigger::Left);
			InputSubsystem->ClearAdaptiveTriggerEffect(DeviceInfo.InstanceId, EDualSenseTrigger::Right);
		}
	}

	for (TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& DeviceMapping : DevicesKeyMapping)
	{
		FJoystickFeatureConfig& AdaptiveTriggerEffect = DeviceMapping.Value.AdaptiveTriggerEffect;
		if (AdaptiveTriggerEffect.IsEnabledRequested() != bEnable)
		{
			bUpdatedAny = true;
			const bool bWasEnabled = AdaptiveTriggerEffect.IsEnabled();
			AdaptiveTriggerEffect.SetEnabled(bEnable);
			if (bWasEnabled != AdaptiveTriggerEffect.IsEnabled() && IsValid(InputSubsystem))
				InputSubsystem->ReconnectDevice(DeviceMapping.Key);
		}
	}

	if (bUpdatedAny)
		TryUpdateDefaultConfigFile();
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
