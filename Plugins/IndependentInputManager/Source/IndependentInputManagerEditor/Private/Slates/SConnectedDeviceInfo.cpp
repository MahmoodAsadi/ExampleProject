// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SConnectedDeviceInfo.h"

#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "IndependentInputSubsystem.h"

#define LOCTEXT_NAMESPACE "SConnectedDeviceInfo"


void SConnectedDeviceInfo::Construct(const FArguments& InArgs)
{
	const FMargin SectionsPadding(12.0f, 0.0f, 12.0f, 5.0f);

	ChildSlot
		[
			SNew(SVerticalBox)

			// Device Selection Section
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
					CreateDeviceSelectionSection()
				]
			]

			// Device Info Section
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
					CreateDeviceInformationSection()
				]
			]

			// Device Hardware Section
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
					CreateHardwareInformationSection()
				]
			]

			// Device Status Section
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
					CreateDeviceStatusSection()
				]
			]
		];

	UpdateList(FInputDeviceInstanceId());
}

void SConnectedDeviceInfo::DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo)
{
	UpdateList(InDeviceInfo.InstanceId);
}

void SConnectedDeviceInfo::DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo)
{
	FInputDeviceInstanceId PreferredDeviceId;
	if (SelectedDeviceId.IsValid() && !SelectedDeviceId->Equals(InDeviceInfo.InstanceId))
		PreferredDeviceId = *SelectedDeviceId;

	if (DeviceInfo.InstanceId.Equals(InDeviceInfo.InstanceId))
		DeviceInfo = FJoystickDeviceInfo();

	UpdateList(PreferredDeviceId);
}

void SConnectedDeviceInfo::DeviceInfoUpdated(const FJoystickDeviceInfo& InDeviceInfo)
{
	if (!SelectedDeviceId.IsValid())
		return;

	if (!SelectedDeviceId->Equals(InDeviceInfo.InstanceId))
		return;

	DeviceInfo = InDeviceInfo;
	RefreshTouchpads();
	Invalidate(EInvalidateWidgetReason::Layout);
}

void SConnectedDeviceInfo::RefreshOptions() const
{
	if (DeviceComboBox)
		DeviceComboBox->RefreshOptions();
}

void SConnectedDeviceInfo::UpdateList(const FInputDeviceInstanceId& PreferedDeviceId)
{
	TArray<FInputDeviceInstanceId> InstanceIds;
	if (const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
		InputSubsystem->GetConnectedDevices().GenerateKeyArray(InstanceIds);

	InstanceIds.Sort([](const FInputDeviceInstanceId& A, const FInputDeviceInstanceId& B)
		{
			return A.GetId() < B.GetId();
		});

	DevicesId.Empty();

	if (InstanceIds.Num() == 0)
	{
		SelectDevice(nullptr);
		if (DeviceComboBox)
			DeviceComboBox->SetSelectedItem(nullptr);
		RefreshOptions();
		return;
	}

	SelectedDeviceId.Reset();
	for (const FInputDeviceInstanceId& Instance : InstanceIds)
	{
		TSharedPtr<FInputDeviceInstanceId> NewDeviceInstance = MakeShared<FInputDeviceInstanceId>(Instance);
		DevicesId.Add(NewDeviceInstance);

		if (PreferedDeviceId.IsValid() && PreferedDeviceId.Equals(Instance))
			SelectedDeviceId = NewDeviceInstance;
	}

	if (!SelectedDeviceId.IsValid())
		SelectedDeviceId = DevicesId[0];

	SelectDevice(SelectedDeviceId);
	if (DeviceComboBox)
		DeviceComboBox->SetSelectedItem(SelectedDeviceId);
	RefreshOptions();
}

void SConnectedDeviceInfo::SelectDevice(const TSharedPtr<FInputDeviceInstanceId>& NewSelection)
{
	SelectedDeviceId = NewSelection;
	DeviceInfo = FJoystickDeviceInfo();

	if (NewSelection.IsValid())
	{
		if (const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
		{
			if (const FJoystickDeviceInfo* FoundInfo = InputSubsystem->GetDeviceInfo(*NewSelection))
				DeviceInfo = *FoundInfo;
		}
	}

	RefreshTouchpads();
	Invalidate(EInvalidateWidgetReason::Layout);
}

TSharedRef<SWidget> SConnectedDeviceInfo::CreatePropertyRow(const FText& Label, TAttribute<FText> Value)
{
	return SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text(Label)
			.TextStyle(FAppStyle::Get(), "NormalText")
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(Value)
			.TextStyle(FAppStyle::Get(), "NormalText")
		];
}

TSharedRef<SWidget> SConnectedDeviceInfo::CreateDeviceSelectionSection()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Device"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 8.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Device:"))
				.TextStyle(FAppStyle::Get(), "NormalText")
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.MaxWidth(400.0f)
			[
				SAssignNew(DeviceComboBox, SComboBox<TSharedPtr<FInputDeviceInstanceId>>)
				.OptionsSource(&DevicesId)
				.OnGenerateWidget_Lambda([](const TSharedPtr<FInputDeviceInstanceId>& InItem)
					{
						if (InItem.IsValid())
						{
							if (const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
							{
								if (const FJoystickDeviceInfo* FoundInfo = InputSubsystem->GetDeviceInfo(*InItem))
								{
									return SNew(STextBlock)
										.Text(FText::FromString(FoundInfo->DeviceName))
										.TextStyle(FAppStyle::Get(), "NormalText");
								}
							}
						}

						return SNew(STextBlock)
							.Text(FText::FromString("ERROR"))
							.TextStyle(FAppStyle::Get(), "NormalText");
					})
				.OnSelectionChanged_Lambda([this](const TSharedPtr<FInputDeviceInstanceId>& NewSelection, ESelectInfo::Type)
					{
						SelectDevice(NewSelection);
					})
				.InitiallySelectedItem(SelectedDeviceId)
				[
					SNew(STextBlock)
					.Text_Lambda([this]() -> FText
						{
							if (!SelectedDeviceId.IsValid())
								return FText::FromString("None");

							return FText::FromString(*DeviceInfo.DeviceName);
						})
					.TextStyle(FAppStyle::Get(), "NormalText")
				]
			]
		];
}

TSharedRef<SWidget> SConnectedDeviceInfo::CreateDeviceInformationSection()
{
	FNumberFormattingOptions NumberFormat;
	NumberFormat.UseGrouping = false;

	return SNew(SHorizontalBox)

		// Device Information
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(0.0f, 0.0f, 12.0f, 0.0f)
		[
			SNew(SVerticalBox)

			// Device Information Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 8.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Device Information"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
				.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
			]

			// Device Information Section
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SVerticalBox)

				// Device Name
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("NameLabel", "Name:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(DeviceInfo.DeviceName)
									: LOCTEXT("NA", "N/A");
							}))
				]

				// Devive Type
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("TypeLabel", "Type:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? UEnum::GetDisplayValueAsText(DeviceInfo.Type)
									: LOCTEXT("NA", "N/A");
							}))
				]

				// Gamepad Type
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					SNew(SHorizontalBox)
					.Visibility_Lambda([&]()
						{
							if (DeviceInfo.Type == EJoystickDeviceType::Gamepad && DeviceInfo.GamepadType != EGamepadType::Unknown)
								return EVisibility::HitTestInvisible;

							return EVisibility::Collapsed;
						})

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(0.0f, 0.0f, 8.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Gamepad Type:"))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text_Lambda([this]() -> FText
							{
								if (!DeviceInfo.IsValid())
									return FText::FromString("N/A");

								return UEnum::GetDisplayValueAsText(DeviceInfo.GamepadType);
							})
						.TextStyle(FAppStyle::Get(), "NormalText")
					]
				]

				// VendorId
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("VendorIDLabel", "Vendor ID:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(FString::Printf(TEXT("%04X"), DeviceInfo.Identifier.VendorId))
									: LOCTEXT("NA", "N/A");
							}))
				]

				// ProductId
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("ProductIDLabel", "Product ID:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(FString::Printf(TEXT("%04X"), DeviceInfo.Identifier.ProductId))
									: LOCTEXT("NA", "N/A");
							}))
				]

				// Serial Number
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("SerialNumberLabel", "Serial Number:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(DeviceInfo.SerialNumber)
									: LOCTEXT("NA", "N/A");
							}))
				]

				// Firmware Version
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("FirmwareVersionLabel", "Firmware Version:"),
						TAttribute<FText>::CreateLambda([this, NumberFormat]
							{
								return DeviceInfo.IsValid()
									? FText::AsNumber(DeviceInfo.FirmwareVersion, &NumberFormat)
									: LOCTEXT("NA", "N/A");
							}))
				]

				// Product Version
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("ProductVersionLabel", "Product Version:"),
						TAttribute<FText>::CreateLambda([this, NumberFormat]
							{
								return DeviceInfo.IsValid()
									? FText::AsNumber(DeviceInfo.ProductVersion, &NumberFormat)
									: LOCTEXT("NA", "N/A");
							}))
				]
			]
		]

	// Vertical Separator
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(12.0f, 0.0f)
		[
			SNew(SSeparator)
				.Orientation(Orient_Vertical)
		]

		// Device Supported Features
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SVerticalBox)

				// Supported Features Title
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(STextBlock)
						.Text(FText::FromString("Supported Features"))
						.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
						.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
				]

				// Supported Features Section
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SVerticalBox)

						// Mono LED
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("MonoLEDLabel", "Mono LED:"),
								TAttribute<FText>::CreateLambda([this]
									{
										return DeviceInfo.IsValid()
											? FText::FromString(HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::MonoLED)
												? TEXT("Yes") : TEXT("No"))
											: LOCTEXT("NA", "N/A");
									}))
						]

					// Player LED
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("PlayerLEDLabel", "Player LED:"),
								TAttribute<FText>::CreateLambda([this]
									{
										return DeviceInfo.IsValid()
											? FText::FromString(HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::PlayerLED)
												? TEXT("Yes") : TEXT("No"))
											: LOCTEXT("NA", "N/A");
									}))
						]

					// RGB LED
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("RGBLEDLabel", "RGB LED:"),
								TAttribute<FText>::CreateLambda([this]
									{
										return DeviceInfo.IsValid()
											? FText::FromString(HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::RGBLED)
												? TEXT("Yes") : TEXT("No"))
											: LOCTEXT("NA", "N/A");
									}))
						]

					// Rumble
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("RumbleLabel", "Rumble:"),
								TAttribute<FText>::CreateLambda([this]
									{
										return DeviceInfo.IsValid()
											? FText::FromString(HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::Rumble)
												? TEXT("Yes") : TEXT("No"))
											: LOCTEXT("NA", "N/A");
									}))
						]

					// Rumble Trigger
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("TriggerRumbleLabel", "Trigger Rumble:"),
								TAttribute<FText>::CreateLambda([this]
									{
										return DeviceInfo.IsValid()
											? FText::FromString(HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::TriggerRumble)
												? TEXT("Yes") : TEXT("No"))
											: LOCTEXT("NA", "N/A");
									}))
						]

					// Adaptive Trigger
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0.0f, 2.0f)
						[
							CreatePropertyRow(LOCTEXT("AdaptiveTriggerLabel", "Adaptive Trigger:"),
								TAttribute<FText>::CreateLambda([this]
									{
										if (!DeviceInfo.IsValid())
											return FText::FromString("N/A");

										UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
										if (!InputSubsystem)
											return FText::FromString("No");

										const FSDLJoystickDevice* SDLInfo = InputSubsystem->GetSDLDeviceInfo(DeviceInfo.InstanceId);
										if (!SDLInfo)
											return FText::FromString("No");

										return FText::FromString(SDLInfo->bIsDualSense ? TEXT("Yes") : TEXT("No"));
									}))
						]
				]
		]

	// Vertical Separator
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(12.0f, 0.0f)
		[
			SNew(SSeparator)
				.Orientation(Orient_Vertical)
		]

		// Device Supported Sensors
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SVerticalBox)

				// Supported Sensors Title
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(STextBlock)
						.Text(FText::FromString("Supported Sensors"))
						.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
						.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
				]

				// Accelerometer
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("AccelerometerLabel", "Accelerometer:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Accelerometer)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]

			// Gyroscope
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("GyroscopeLabel", "Gyroscope:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Gyroscope)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]

			// Left Accelerometer
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("LeftAccelerometerLabel", "Left Accelerometer:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftAccelerometer)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]

			// Right Accelerometer
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("RightAccelerometerLabel", "Right Accelerometer:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightAccelerometer)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]

			// Left Gyroscope
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("LeftGyroscopeLabel", "Left Gyroscope:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftGyroscope)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]

			// Right Gyroscope
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f)
				[
					CreatePropertyRow(LOCTEXT("RightGyroscopeLabel", "Right Gyroscope:"),
						TAttribute<FText>::CreateLambda([this]
							{
								return DeviceInfo.IsValid()
									? FText::FromString(
										HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightGyroscope)
										? TEXT("Yes") : TEXT("No"))
									: LOCTEXT("NA", "N/A");
							}))
				]
		];
}

TSharedRef<SWidget> SConnectedDeviceInfo::CreateHardwareInformationSection()
{
	FNumberFormattingOptions NumberFormat;
	NumberFormat.UseGrouping = false;

	return SNew(SVerticalBox)

		// Device Hardware Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Device Hardware Information"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Device Hardware Section Container
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SVerticalBox)

			// Buttons
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("NumberOfButtonsLabel", "Number Of Buttons:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::AsNumber(DeviceInfo.NumberOfButtons, &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Axis
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("NumberOfAxisLabel", "Number Of Axis:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::AsNumber(DeviceInfo.NumberOfAxis, &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Hats
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("NumberOfHatsLabel", "Number Of Hats:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::AsNumber(DeviceInfo.NumberOfHats, &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Balls
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("NumberOfBallsLabel", "Number Of Balls:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::AsNumber(DeviceInfo.NumberOfBalls, &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Touchpads
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SVerticalBox)
				.Visibility_Lambda([&]()
					{
						return DeviceInfo.Touchpads.Num() > 0 ?
							EVisibility::HitTestInvisible
							: EVisibility::Collapsed;
					})

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 8.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TouchpadsLabel", "Touchpads:"))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(TouchpadsContainer, SVerticalBox)
				]
			]
		];
}

TSharedRef<SWidget> SConnectedDeviceInfo::CreateDeviceStatusSection()
{
	FNumberFormattingOptions NumberFormat;
	NumberFormat.UseGrouping = false;

	return SNew(SVerticalBox)

		// Device Status Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Device Status"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Device Status Containers
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SVerticalBox)

			// SDL Instance ID
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("InstanceIDLabel", "SDL Instance ID:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.InstanceId.IsValid()
								? FText::AsNumber(DeviceInfo.InstanceId.GetId(), &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Unreal Input Device ID
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("InputDeviceIdLabel", "Unreal Input Device ID:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.InputDeviceId.IsValid()
								? FText::AsNumber(DeviceInfo.InputDeviceId.GetId(), &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Unreal Platform User ID
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("PlatformUserIdLabel", "Unreal Platform User ID:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.PlatformUserId.IsValid()
								? FText::AsNumber(DeviceInfo.PlatformUserId.GetInternalId(), &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Key Mapping ID
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("KeyMappingIDLabel", "Key Mapping ID:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::FromString(DeviceInfo.MappingId.ToString())
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Connection Type
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("ConnectionTypeLabel", "Connection Type:"),
					TAttribute<FText>::CreateLambda([this]
						{
							return DeviceInfo.IsValid()
								? UEnum::GetDisplayValueAsText(DeviceInfo.ConnectionType)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Battery State
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("BatteryStateLabel", "Battery State:"),
					TAttribute<FText>::CreateLambda([this]
						{
							return DeviceInfo.IsValid()
								? UEnum::GetDisplayValueAsText(DeviceInfo.BatteryState)
								: LOCTEXT("NA", "N/A");
						}))
			]

			// Battery Percent
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("BatteryPercentLabel", "Battery Percent:"),
					TAttribute<FText>::CreateLambda([this, NumberFormat]
						{
							return DeviceInfo.IsValid()
								? FText::AsNumber(DeviceInfo.BatteryPercent, &NumberFormat)
								: LOCTEXT("NA", "N/A");
						}))
			]
		];
}

void SConnectedDeviceInfo::RefreshTouchpads()
{
	if (!TouchpadsContainer.IsValid())
		return;

	TouchpadsContainer->ClearChildren();
	const TArray<FTouchpadInfo>& Touchpads = DeviceInfo.Touchpads;

	for (int32 Index = 0; Index < Touchpads.Num(); ++Index)
	{
		TouchpadsContainer->AddSlot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::Format(
						LOCTEXT("TouchpadSupportedFingersFormat", "\tTouchpad {0} Supported Fingers:"),
						FText::AsNumber(Index + 1)))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]

				+ SHorizontalBox::Slot()
				.Padding(8.0f, 0.0f, 0.0f, 0.0f)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::AsNumber(Touchpads[Index].NumOfFingers))
					.TextStyle(FAppStyle::Get(), "NormalText")
				]
			];
	}

	Invalidate(EInvalidateWidgetReason::Layout);
}

#undef LOCTEXT_NAMESPACE
