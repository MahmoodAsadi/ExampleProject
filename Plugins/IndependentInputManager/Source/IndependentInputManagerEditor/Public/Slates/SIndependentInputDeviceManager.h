// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

class SButton;
class SConnectedDeviceInfo;
class SDeviceKeyMapping;
class SWidgetSwitcher;

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SIndependentInputDeviceManager : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SIndependentInputDeviceManager)
		{
		}

	SLATE_END_ARGS()

	SIndependentInputDeviceManager();
	static TSharedPtr<SIndependentInputDeviceManager> Open(const FJoystickDeviceInfo& DeviceInfo);

	void Construct(const FArguments& InArgs);
	void DevicePluggedIn(const FJoystickDeviceInfo& DeviceInfo);
	void DeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo);
	void DeviceUpdated(const FJoystickDeviceInfo& DeviceInfo);

private:

	TMap<FInputDeviceInstanceId, FJoystickDeviceInfo> DeviceInfos;
	TMap<FInputDeviceInstanceId, FSDLJoystickDevice> SDLDevices;

	FInputDeviceInstanceId SelectedDeviceId;

	FReply ConnectedDevicesButtonClicked();
	FReply KeyMappingButtonClicked();

	void RefreshWindow();
	void UpdateTabButtons();

	TSharedPtr<SButton> ConnectedDevicesButton;
	TSharedPtr<SButton> KeyMappingButton;
	TSharedPtr<SWidgetSwitcher> ContentWidgetSwitcher;
	TSharedPtr<SConnectedDeviceInfo> ConnectedDeviceTab;
	TSharedPtr<SDeviceKeyMapping> KeyMappingTab;
	int32 SelectedTabIndex = INDEX_NONE;

	FButtonStyle DefaultStyle;
	FButtonStyle SelectedStyle;

};
