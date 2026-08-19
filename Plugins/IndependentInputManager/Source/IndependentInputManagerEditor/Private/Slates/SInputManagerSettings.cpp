// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SInputManagerSettings.h"

#include "Styling/AppStyle.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "IndependentInputManagerSettings.h"
#include "IndependentInputSubsystem.h"

#define LOCTEXT_NAMESPACE "SInputManagerSettings"


void SInputManagerSettings::Construct(const FArguments& InArgs)
{
	const FMargin SectionsPadding(12.0f, 0.0f, 12.0f, 5.0f);

	ChildSlot
		[
			SNew(SVerticalBox)

			// Device Filtering Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(SectionsPadding)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					CreateDeviceFilteringSection()
				]
			]

			// Device Settings Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(SectionsPadding)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				[
					CreateDeviceSettingsSection()
				]
			]
		];
}

TSharedRef<SWidget> SInputManagerSettings::CreateDeviceFilteringSection()
{
	return SNew(SVerticalBox)

		// Section Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DeviceFiltering", "Device Filtering"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Ignore XInput devices section
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("IgnoreXInputDevices", "Ignore XInput Devices:"))
				.ToolTipText(LOCTEXT("IgnoreXInputDevicesTooltip",
					"Keeps Windows XInput devices visible for diagnostics while allowing "
					"Unreal's XInput interface to own their gameplay input.\nTo let Independent "
					"Input Manager own these devices instead, disable this option and Unreal's "
					"XInput Device plugin from plugin list to avoid duplicate input."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(IgnoreXInputDevicesCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetIgnoreXInputDevices)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnIgnoreXInputDevicesChange)
			]
		]

		// Ignore SteamInput devices section
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("IgnoreSteamInputDevices", "Ignore Steam Input Devices:"))
				.ToolTipText(LOCTEXT("IgnoreSteamInputDevicesTooltip",
					"Keeps Valve input devices visible for diagnostics while allowing Unreal's "
					"Steam Controller interface to own their gameplay input. Valve controllers "
					"have not yet been tested with Independent Input Manager. To let this plugin "
					"own them, disable this option and Unreal's Steam Controller plugin from plugin list "
					"to avoid duplicate inputs."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(IgnoreSteamInputDevicesCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetIgnoreSteamInputDevices)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnIgnoreSteamInputDevicesChange)
			]
		];
}

TSharedRef<SWidget> SInputManagerSettings::CreateDeviceSettingsSection()
{
	return SNew(SVerticalBox)

		// Section Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DeviceSettings", "Device Settings"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Force all devices as single user
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ForceAllDevicesAsSingleUser", "Force All Devices for Single User:"))
				.ToolTipText(LOCTEXT("ForceAllDevicesAsSingleUserTooltip",
					"Maps every plugin-owned input device to Unreal's primary platform user. Disable this if your game is splitscreen multiplayer."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(ForceAllDevicesForSingleUserCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetForceAllDevicesForSingleUser)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnForceAllDevicesForSingleUserChange)
			]
		]

		// Enable Accelerometer Sensor Section
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("EnableAccelerometerSensor", "Enable Accelerometer Sensor:"))
				.ToolTipText(LOCTEXT("EnableAccelerometerSensorTooltip", "Whether device(s) Accelerometer sensor should be enabled or not."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(AccelerometerSensorCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetAccelerometerSensorEnabled)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnAccelerometerSensorEnabledChange)
			]
		]

		// Enable Gyroscope Sensor Section
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("EnableGyroscopeSensor", "Enable Gyroscope Sensor:"))
				.ToolTipText(LOCTEXT("EnableGyroscopeSensorTooltip", "Whether device(s) Gyroscope sensor should be enabled or not."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(GyroscopeSensorCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetGyroscopeSensorEnabled)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnGyroscopeSensorEnabledChange)
			]
		]

		// Use Device Name As Hardware Device Identifier Section
		+ SVerticalBox::Slot()
		.Padding(0.0f, 0.0f, 0.0f, 4.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("UseDeviceNameAsHardwareDeviceIdentifier", "Use Device Name as Hardware Device Identifier:"))
				.ToolTipText(LOCTEXT("UseDeviceNameAsHardwareDeviceIdentifierTooltip", 
					"Use the device name as the hardware device identifier. "
					"Useful if you need specific hardware identification(ie. for icons)."))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(UseDeviceNameAsHardwareDeviceIdentifierCheckBox, SCheckBox)
				.IsChecked(this, &SInputManagerSettings::GetUseDeviceNameAsHardwareDeviceIdentifier)
				.OnCheckStateChanged(this, &SInputManagerSettings::OnUseDeviceNameAsHardwareDeviceIdentifierChange)
			]
		];
}

ECheckBoxState SInputManagerSettings::GetIgnoreXInputDevices() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetIgnoreXInputDevices() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnIgnoreXInputDevicesChange(ECheckBoxState NewState)
{
	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
		InputManagerSettings->SetIgnoreXInputDevices(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetIgnoreSteamInputDevices() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetIgnoreSteamInputDevices() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnIgnoreSteamInputDevicesChange(ECheckBoxState NewState)
{
	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
		InputManagerSettings->SetIgnoreSteamInputDevices(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetIgnoreVirtualInputDevices() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetIgnoreVirtualDevices() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnIgnoreVirtualInputDevicesChange(ECheckBoxState NewState)
{
	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
		InputManagerSettings->SetIgnoreVirtualInputDevices(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetForceAllDevicesForSingleUser() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetForceDevicesForSingleUser() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnForceAllDevicesForSingleUserChange(ECheckBoxState NewState)
{
	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
		InputManagerSettings->SetForceAllDevicesForSingleUser(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetAccelerometerSensorEnabled() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetAccelerometerSensorEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnAccelerometerSensorEnabledChange(ECheckBoxState NewState)
{
	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
		InputSubsystem->SetAccelerometerSensorEnable(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetGyroscopeSensorEnabled() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetGyroscopeSensorEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnGyroscopeSensorEnabledChange(ECheckBoxState NewState)
{
	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
		InputSubsystem->SetGyroscopeSensorEnable(NewState == ECheckBoxState::Checked ? true : false);
}

ECheckBoxState SInputManagerSettings::GetUseDeviceNameAsHardwareDeviceIdentifier() const
{
	if (const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get())
		return InputManagerSettings->GetUseDeviceNameAsHardwareDeviceIdentifier() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	return ECheckBoxState::Unchecked;
}

void SInputManagerSettings::OnUseDeviceNameAsHardwareDeviceIdentifierChange(ECheckBoxState NewState)
{
	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
		InputManagerSettings->SetUseDeviceNameAsHardwareDeviceIdentifier(NewState == ECheckBoxState::Checked ? true : false);
}

#undef LOCTEXT_NAMESPACE
