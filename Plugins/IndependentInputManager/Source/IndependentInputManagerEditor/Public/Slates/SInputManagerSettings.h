// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

class SCheckBox;

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SInputManagerSettings : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SInputManagerSettings)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	TSharedPtr<SCheckBox> IgnoreXInputDevicesCheckBox;
	TSharedPtr<SCheckBox> IgnoreSteamInputDevicesCheckBox;
	TSharedPtr<SCheckBox> ForceAllDevicesForSingleUserCheckBox;
	TSharedPtr<SCheckBox> AccelerometerSensorCheckBox;
	TSharedPtr<SCheckBox> GyroscopeSensorCheckBox;
	TSharedPtr<SCheckBox> UseDeviceNameAsHardwareDeviceIdentifierCheckBox;

	TSharedRef<SWidget> CreateDeviceFilteringSection();
	TSharedRef<SWidget> CreateDeviceSettingsSection();

	ECheckBoxState GetIgnoreXInputDevices() const;
	void OnIgnoreXInputDevicesChange(ECheckBoxState NewState);

	ECheckBoxState GetIgnoreSteamInputDevices() const;
	void OnIgnoreSteamInputDevicesChange(ECheckBoxState NewState);

	ECheckBoxState GetForceAllDevicesForSingleUser() const;
	void OnForceAllDevicesForSingleUserChange(ECheckBoxState NewState);

	ECheckBoxState GetAccelerometerSensorEnabled() const;
	void OnAccelerometerSensorEnabledChange(ECheckBoxState NewState);

	ECheckBoxState GetGyroscopeSensorEnabled() const;
	void OnGyroscopeSensorEnabledChange(ECheckBoxState NewState);

	ECheckBoxState GetUseDeviceNameAsHardwareDeviceIdentifier() const;
	void OnUseDeviceNameAsHardwareDeviceIdentifierChange(ECheckBoxState NewState);

};
