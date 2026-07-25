// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

template <typename OptionType>
class SComboBox;
class SVerticalBox;

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SConnectedDeviceInfo : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SConnectedDeviceInfo)
		{
		}
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);
	void DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo);
	void DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo);
	void DeviceInfoUpdated(const FJoystickDeviceInfo& InDeviceInfo);

private:

	void RefreshOptions() const;
	void UpdateList(const FInputDeviceInstanceId& PreferedSelection);
	void SelectDevice(const TSharedPtr<FInputDeviceInstanceId>& NewSelection);
	TSharedRef<SWidget> CreatePropertyRow(const FText& Label, TAttribute<FText> Value);

	TSharedRef<SWidget> CreateDeviceSelectionSection();
	TSharedRef<SWidget> CreateDeviceInformationSection();
	TSharedRef<SWidget> CreateHardwareInformationSection();
	TSharedRef<SWidget> CreateDeviceStatusSection();
	void RefreshTouchpads();

	FJoystickDeviceInfo DeviceInfo;
	TSharedPtr<SComboBox<TSharedPtr<FInputDeviceInstanceId>>> DeviceComboBox;
	TArray<TSharedPtr<FInputDeviceInstanceId>> DevicesId;
	TSharedPtr<FInputDeviceInstanceId> SelectedDeviceId;
	TSharedPtr<SVerticalBox> TouchpadsContainer;

};
