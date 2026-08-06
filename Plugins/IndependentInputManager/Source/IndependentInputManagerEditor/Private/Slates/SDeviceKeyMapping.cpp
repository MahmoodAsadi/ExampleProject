// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SDeviceKeyMapping.h"

#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

#include "DeviceInputMappings.h"
#include "IndependentInputDevice.h"
#include "IndependentInputManagerSettings.h"
#include "IndependentInputSubsystem.h"
#include "Slates/SAxisInputView.h"
#include "Slates/SBallInputView.h"
#include "Slates/SButtonInputView.h"
#include "Slates/SHatInputView.h"
#include "Slates/SInputMappingEditor.h"
#include "Slates/SSensorInputView.h"
#include "Slates/STouchpadInputView.h"

#define LOCTEXT_NAMESPACE "SDeviceKeyMapping"


SDeviceKeyMapping::SDeviceKeyMapping()
{
	NoShadowScrollStyle = FAppStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox");
	NoShadowScrollStyle.TopShadowBrush = FSlateNoResource();
	NoShadowScrollStyle.BottomShadowBrush = FSlateNoResource();
}

SDeviceKeyMapping::~SDeviceKeyMapping()
{
	CloseInputMappingEditor();
}

void SDeviceKeyMapping::Construct(const FArguments& InArgs)
{
	const FMargin SectionsPadding(12.0f, 0.0f, 12.0f, 5.0f);
	UpdateList(FJoystickDeviceIdentifier());

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
							CreateProfileSelectionSection()
						]
				]

			// Profile Info Section
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
							CreateProfileInfoSection()
						]
				]

			// Device Key Mapping Section
			+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(SectionsPadding)
				[
					SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(12.0f)
						.VAlign(VAlign_Top)
						.HAlign(HAlign_Fill)
						[
							SNew(SScrollBox)
								.Style(&NoShadowScrollStyle)
								.ScrollBarPadding(FMargin(6.0f, 2.0f, 2.0f, 2.0f))

								+ SScrollBox::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Top)
								[
									CreateDeviceKeyMappingSection()
								]
						]
				]
		];

	RefreshDeviceKeyMappingContainer();
}

void SDeviceKeyMapping::DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo)
{
	UpdateList(InDeviceInfo.Identifier);
}

void SDeviceKeyMapping::DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo)
{
	const FJoystickDeviceIdentifier PreferredSelection = SelectedDeviceIdentifier.IsValid()
		? SelectedDeviceIdentifier->DeviceIdentifier
		: FJoystickDeviceIdentifier();

	UpdateList(PreferredSelection);
}

void SDeviceKeyMapping::UpdateList(const FJoystickDeviceIdentifier& PreferedSelection)
{
	DeviceMappings.Empty();
	const UIndependentInputManagerSettings* InputSettings = UIndependentInputManagerSettings::Get();
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	if (!IsValid(InputSettings))
	{
		SelectedDeviceIdentifier = nullptr;
		DeviceKeyMapping = FJoystickDeviceKeyMapping();
		if (MappingComboBox)
			MappingComboBox->SetSelectedItem(nullptr);
		RefreshOptions();
		RefreshDeviceKeyMappingContainer();
		return;
	}

	const TMap<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& KeyMappings = InputSettings->GetDevicesKeyMapping();
	if (KeyMappings.Num() == 0)
	{
		SelectedDeviceIdentifier = nullptr;
		DeviceKeyMapping = FJoystickDeviceKeyMapping();
		if (MappingComboBox)
			MappingComboBox->SetSelectedItem(nullptr);
		RefreshOptions();
		RefreshDeviceKeyMappingContainer();
		return;
	}

	SelectedDeviceIdentifier.Reset();
	for (const TPair<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping>& KeyMapping : KeyMappings)
	{
		TSharedPtr<FKeyMappingDeviceIdentifier> NewMapping = MakeShared<FKeyMappingDeviceIdentifier>(KeyMapping.Key);
		if (InputSubsystem)
		{
			if (const FJoystickDeviceInfo* DeviceInfo = InputSubsystem->FindDeviceInfoByIdentifier(KeyMapping.Key))
				NewMapping->DeviceInstanceId = DeviceInfo->InstanceId;
		}

		DeviceMappings.Add(NewMapping);

		if (PreferedSelection.IsValid() && KeyMapping.Key == PreferedSelection)
			SelectedDeviceIdentifier = NewMapping;
	}

	DeviceMappings.Sort([](const TSharedPtr<FKeyMappingDeviceIdentifier>& A, const TSharedPtr<FKeyMappingDeviceIdentifier>& B)
		{
			if (A->DeviceIdentifier.VendorId == B->DeviceIdentifier.VendorId)
				return A->DeviceIdentifier.ProductId < B->DeviceIdentifier.ProductId;

			return A->DeviceIdentifier.VendorId < B->DeviceIdentifier.VendorId;
		});

	if (!SelectedDeviceIdentifier.IsValid())
		SelectedDeviceIdentifier = DeviceMappings[0];

	if (MappingComboBox)
		MappingComboBox->SetSelectedItem(SelectedDeviceIdentifier);

	DeviceKeyMapping = FJoystickDeviceKeyMapping();
	if (SelectedDeviceIdentifier.IsValid())
	{
		if (const FJoystickDeviceKeyMapping* FoundMapping = InputSettings->FindDeviceKeyMappings(SelectedDeviceIdentifier->DeviceIdentifier))
		{
			DeviceKeyMapping = *FoundMapping;
		}
	}

	RefreshOptions();
	RefreshDeviceKeyMappingContainer();
}

void SDeviceKeyMapping::RefreshOptions() const
{
	if (MappingComboBox)
		MappingComboBox->RefreshOptions();
}

TSharedRef<SWidget> SDeviceKeyMapping::CreateProfileSelectionSection()
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
					SAssignNew(MappingComboBox, SComboBox<TSharedPtr<FKeyMappingDeviceIdentifier>>)
						.OptionsSource(&DeviceMappings)
						.OnGenerateWidget_Lambda([this](const TSharedPtr<FKeyMappingDeviceIdentifier>& InItem)
							{
								const UIndependentInputManagerSettings* InputSettings = UIndependentInputManagerSettings::Get();
								if (!IsValid(InputSettings))
								{
									return SNew(STextBlock)
										.Text(FText::FromString("ERROR: Invalid Settings"))
										.TextStyle(FAppStyle::Get(), "NormalText");
								}

								if (const FJoystickDeviceKeyMapping* KeyMapping = InputSettings->FindDeviceKeyMappings(InItem->DeviceIdentifier))
								{
									FText DeviceName = FText::Format(LOCTEXT("DeviceNameLabel", "{0}{1}"),
										FText::FromString(KeyMapping->DeviceName),
										InItem->DeviceInstanceId.IsValid() ? LOCTEXT("DeviceLabel", ": Connected") : FText::GetEmpty());

									return SNew(STextBlock)
										.Text(DeviceName)
										.TextStyle(FAppStyle::Get(), "NormalText");
								}

								return SNew(STextBlock)
									.Text(FText::FromString("ERROR: Invalid Key Mapping"))
									.TextStyle(FAppStyle::Get(), "NormalText");
							})
						.OnSelectionChanged_Lambda([this](const TSharedPtr<FKeyMappingDeviceIdentifier>& NewSelection, ESelectInfo::Type)
							{
								SelectedDeviceIdentifier = NewSelection;
								DeviceKeyMapping = FJoystickDeviceKeyMapping();
								if (SelectedDeviceIdentifier.IsValid())
								{
									if (const UIndependentInputManagerSettings* InputSettings = UIndependentInputManagerSettings::Get())
									{
										if (const FJoystickDeviceKeyMapping* FoundMapping = InputSettings->FindDeviceKeyMappings(SelectedDeviceIdentifier->DeviceIdentifier))
										{
											DeviceKeyMapping = *FoundMapping;
										}
									}
								}

								RefreshDeviceKeyMappingContainer();
							})
						.InitiallySelectedItem(SelectedDeviceIdentifier)
						[
							SNew(STextBlock)
								.Text_Lambda([this]() -> FText
									{
										if (!SelectedDeviceIdentifier.IsValid())
											return FText::FromString("None");

										const UIndependentInputManagerSettings* InputSettings = UIndependentInputManagerSettings::Get();
										if (!IsValid(InputSettings))
										{
											return FText::FromString("ERROR: Invalid Settings");
										}

										if (const FJoystickDeviceKeyMapping* FoundMapping = InputSettings->FindDeviceKeyMappings(SelectedDeviceIdentifier->DeviceIdentifier))
										{
											const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
											FText DeviceName = FText::FromString(*FoundMapping->DeviceName);

											if (InputSubsystem && InputSubsystem->FindDeviceInfoByIdentifier(SelectedDeviceIdentifier->DeviceIdentifier))
											{
												DeviceName = FText::Format(LOCTEXT("DeviceNameLabel", "{0}{1}"),
													DeviceName, LOCTEXT("DeviceLabel", ": Connected"));
											}

											return DeviceName;
										}

										return FText::FromString("None");
									})
								.TextStyle(FAppStyle::Get(), "NormalText")
						]
				]
		];
}

TSharedRef<SWidget> SDeviceKeyMapping::CreateProfileInfoSection()
{
	return SNew(SVerticalBox)

		// Mapping Info Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
				.Text(FText::FromString("Mapping Info"))
				.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
				.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Mapping Info Containers
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SVerticalBox)

			// Device Name
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("DeviceNameLabel", "Device Name:"),
					TAttribute<FText>::CreateLambda([this]
						{
							return FText::FromString(DeviceKeyMapping.DeviceName);
						}))
			]

			// Mapping Id
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("MappingIDLabel", "Mapping ID:"),
					TAttribute<FText>::CreateLambda([this]
						{
							return FText::FromString(DeviceKeyMapping.MappingId.ToString());
						}))
			]

			// Uses Gamepad API
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				CreatePropertyRow(LOCTEXT("UsesGamepadAPILabel", "Uses Gamepad API:"),
					TAttribute<FText>::CreateLambda([this]
						{
							return FText::FromString(DeviceKeyMapping.bUseGamepadAPI ? "Yes" : "No");
						}))
			]
		];
}

TSharedRef<SWidget> SDeviceKeyMapping::CreateDeviceKeyMappingSection()
{
	return SNew(SVerticalBox)

		// Key Mapping Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Device Key Mapping"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		]

		// Key Mapping Containers
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SVerticalBox)

			// Button Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(ButtonSectionContainer, SVerticalBox)
				]
			]

			// Axis Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(AxisSectionContainer, SVerticalBox)
				]
			]

			// Hat Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(HatSectionContainer, SVerticalBox)
				]
			]

			// Ball Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(BallSectionContainer, SVerticalBox)
				]
			]

			// Touchpad Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(TouchpadSectionContainer, SVerticalBox)
				]
			]

			// Sensor Mapping Container
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(12.0f)
				.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
				[
					SAssignNew(SensorSectionContainer, SVerticalBox)
				]
			]
		];
}

void SDeviceKeyMapping::ApplyDeviceKeyMapping()
{
	if (!SelectedDeviceIdentifier.IsValid())
		return;

	if (!SelectedDeviceIdentifier->DeviceIdentifier.IsValid())
		return;

	if (UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable())
	{
		InputManagerSettings->AddOrUpdateDeviceKeyMapping(SelectedDeviceIdentifier->DeviceIdentifier, DeviceKeyMapping);
		RefreshDeviceKeyMappingContainer();

		if (UIndependentInputSubsystem* Subsystem = UIndependentInputSubsystem::Get())
			Subsystem->ReconnectDevice(SelectedDeviceIdentifier->DeviceIdentifier);
	}
}

void SDeviceKeyMapping::RefreshDeviceKeyMappingContainer()
{
	RefreshButtonsContainer();
	RefreshAxisContainer();
	RefreshHatsContainer();
	RefreshBallsContainer();
	RefreshTouchpadContainer();
	RefreshSensorContainer();
	Invalidate(EInvalidateWidgetReason::Layout);
}

void SDeviceKeyMapping::RefreshButtonsContainer()
{
	if (!ButtonSectionContainer.IsValid())
		return;

	ButtonSectionContainer->ClearChildren();
	TSharedRef<SWrapBox> ButtonContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);
	
	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;

	for (const TPair<int32, FJoystickButtonKeyMapping>& ButtonMapping : DeviceKeyMapping.ButtonMappings)
	{
		ButtonContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SBox)
				.WidthOverride(280.0f)
				.MinDesiredHeight(90.0f)
				[
					SNew(SButtonInputView)
					.ButtonIndex(ButtonMapping.Key)

					// This assumes FIndependentInputKey has GetKey().
					// Replace this with your actual FIndependentInputKey -> FKey accessor.
					.Key(ButtonMapping.Value.Key)

					// Connect this to your live device button state later.
					.IsPressed_Lambda([WeakDeviceMapping, ButtonMapping, InputDevice]()
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return false;

							if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
								return InputDevice->GetButtonState(Self->SelectedDeviceIdentifier->DeviceInstanceId, ButtonMapping.Key);

							return false;
						})

					.OnClicked_Lambda([WeakDeviceMapping, ButtonMapping]()
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FReply::Handled();

							if (Self->InputMappingEditor.IsValid())
								Self->CloseInputMappingEditor();

							UButtonInputMapping* ButtonMappingObject = NewObject<UButtonInputMapping>();
							ButtonMappingObject->Initialize(
								Self->SelectedDeviceIdentifier->DeviceIdentifier,
								Self->DeviceKeyMapping,
								ButtonMapping.Key,
								ButtonMapping.Value);

							Self->InputMappingEditor =
								SNew(SWindow)
								.Title(LOCTEXT("DeviceDetailsWindowTitle", "Button Input Mapping"))
								.ClientSize(FVector2D(500.f, 300.f))
								.SupportsMinimize(false)
								.SupportsMaximize(true)
								[
									SNew(SInputMappingEditor)
										.InputMappingObject(ButtonMappingObject)
										.OnCancel_Lambda([WeakDeviceMapping]
											{
												if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
													Self->CloseInputMappingEditor();

												return FReply::Handled();
											})
										.OnSave_Lambda([WeakDeviceMapping, ButtonMapping](const UObject* ModifiedObject)
											{
												const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
												if (!Self)
													return;

												if (const UButtonInputMapping* ModifiedMapping = Cast<UButtonInputMapping>(ModifiedObject))
													Self->DeviceKeyMapping.ButtonMappings[ButtonMapping.Key].Key = ModifiedMapping->Key;

												Self->CloseInputMappingEditor();
												Self->ApplyDeviceKeyMapping();
											})
								];

							FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
							return FReply::Handled();
						})
				]
			];
	}

	ButtonSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ButtonsSectionTitle", "Buttons"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	ButtonSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.ButtonMappings.Num() > 0
				? ButtonContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoButtonMappings", "This device has no Button mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::RefreshAxisContainer()
{
	if (!AxisSectionContainer.IsValid())
		return;

	AxisSectionContainer->ClearChildren();

	TSharedRef<SWrapBox> AxisContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);

	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;

	for (const TPair<int32, FJoystickAxisKeyMapping>& AxisMapping : DeviceKeyMapping.AxisMappings)
	{
		TArray<TAttribute<bool>> VirtualButtonStates;
		VirtualButtonStates.Reserve(AxisMapping.Value.VirtualButtons.Num());

		for (int32 VirtualButtonIndex = 0; VirtualButtonIndex < AxisMapping.Value.VirtualButtons.Num(); ++VirtualButtonIndex)
		{
			VirtualButtonStates.Add(TAttribute<bool>::CreateLambda([WeakDeviceMapping, AxisMapping, VirtualButtonIndex, InputDevice]
				{
					const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
					if (!Self)
						return false;

					if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
						return InputDevice->GetAxisVirtualButtonState(Self->SelectedDeviceIdentifier->DeviceInstanceId, AxisMapping.Key, VirtualButtonIndex);

					return false;
				}));
		}


		AxisContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SBox)
				.WidthOverride(400.0f)
				.MinDesiredHeight(90.0f)
				[
					SNew(SAxisInputView)
					.AxisMapping(AxisMapping.Value)
					.RawInputValue_Lambda([WeakDeviceMapping, AxisMapping, InputDevice]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return 0.0f;

							if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
								return InputDevice->GetAxisRawState(Self->SelectedDeviceIdentifier->DeviceInstanceId, AxisMapping.Key);

							return 0.0f;
						})
					.OutputInputValue_Lambda([WeakDeviceMapping, AxisMapping, InputDevice]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return 0.0f;

							if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
								return InputDevice->GetAxisState(Self->SelectedDeviceIdentifier->DeviceInstanceId, AxisMapping.Key);

							return 0.0f;
						})
					.VirtualButtonStates(VirtualButtonStates)
					.OnClicked_Lambda([WeakDeviceMapping, AxisMapping]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FReply::Handled();

							if (Self->InputMappingEditor.IsValid())
								Self->CloseInputMappingEditor();

							UAxisInputMapping* AxisMappingObject = NewObject<UAxisInputMapping>();
							AxisMappingObject->Initialize(
								Self->SelectedDeviceIdentifier->DeviceIdentifier,
								Self->DeviceKeyMapping,
								AxisMapping.Key,
								AxisMapping.Value);

							Self->InputMappingEditor =
								SNew(SWindow)
								.Title(LOCTEXT("DeviceDetailsWindowTitle", "Axis Input Mapping"))
								.ClientSize(FVector2D(500.f, 700.f))
								.SupportsMinimize(false)
								.SupportsMaximize(true)
								[
									SNew(SInputMappingEditor)
									.InputMappingObject(AxisMappingObject)
									.OnCancel_Lambda([WeakDeviceMapping]
										{
											if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
												Self->CloseInputMappingEditor();

											return FReply::Handled();
										})
									.OnSave_Lambda([WeakDeviceMapping, AxisMapping](const UObject* ModifiedObject)
										{
											const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
											if (!Self)
												return;

											if (const UAxisInputMapping* ModifiedMapping = Cast<UAxisInputMapping>(ModifiedObject))
												Self->DeviceKeyMapping.AxisMappings[AxisMapping.Key] = ModifiedMapping->AxisMapping;

											Self->CloseInputMappingEditor();
											Self->ApplyDeviceKeyMapping();
										})
								];

							FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
							return FReply::Handled();
						})
				]
			];
	}

	AxisSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AxisSectionTitle", "Axis"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	AxisSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.AxisMappings.Num() > 0
				? AxisContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoAxisMappings", "This device has no Axis mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::RefreshHatsContainer()
{
	if (!HatSectionContainer.IsValid())
		return;

	HatSectionContainer->ClearChildren();
	TSharedRef<SWrapBox> HatsContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);

	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;

	for (const TPair<int32, FJoystickHatKeyMapping>& HatMapping : DeviceKeyMapping.HatMappings)
	{
		HatsContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SHatInputView)
				.HatMapping(HatMapping.Value)
				.Direction(TAttribute<uint8>::CreateLambda([WeakDeviceMapping, HatMapping, InputDevice]
					{
						const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
						if (!Self)
							return uint8(0);

						if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
							return InputDevice->GetHatState(Self->SelectedDeviceIdentifier->DeviceInstanceId, HatMapping.Key);

						return uint8(0);
					}))
				.OnClicked_Lambda([WeakDeviceMapping, HatMapping]
					{
						const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
						if (!Self)
							return FReply::Handled();

						if (Self->InputMappingEditor.IsValid())
							Self->CloseInputMappingEditor();

						UHatInputMapping* HatMappingObject = NewObject<UHatInputMapping>();
						HatMappingObject->Initialize(
							Self->SelectedDeviceIdentifier->DeviceIdentifier,
							Self->DeviceKeyMapping,
							HatMapping.Key,
							HatMapping.Value);

						Self->InputMappingEditor =
							SNew(SWindow)
							.Title(LOCTEXT("DeviceDetailsWindowTitle", "Hat Input Mapping"))
							.ClientSize(FVector2D(500.f, 700.f))
							.SupportsMinimize(false)
							.SupportsMaximize(true)
							[
								SNew(SInputMappingEditor)
								.InputMappingObject(HatMappingObject)
								.OnCancel_Lambda([WeakDeviceMapping]
									{
										if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
											Self->CloseInputMappingEditor();

										return FReply::Handled();
									})
								.OnSave_Lambda([WeakDeviceMapping, HatMapping](const UObject* ModifiedObject)
									{
										const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
										if (!Self)
											return;

										if (const UHatInputMapping* ModifiedMapping = Cast<UHatInputMapping>(ModifiedObject))
										{
											Self->DeviceKeyMapping.HatMappings[HatMapping.Key] = ModifiedMapping->HatMapping;
										}

										Self->CloseInputMappingEditor();
										Self->ApplyDeviceKeyMapping();
									})
							];

						FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
						return FReply::Handled();
					})
			];
	}

	HatSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("HatSectionTitle", "Hats"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	HatSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.HatMappings.Num() > 0
				? HatsContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoHatMappings", "This device has no Hat Mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::RefreshBallsContainer()
{
	if (!BallSectionContainer.IsValid())
		return;

	BallSectionContainer->ClearChildren();
	TSharedRef<SWrapBox> BallsContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);

	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;

	for (const TPair<int32, FJoystickBallKeyMapping>& BallMapping : DeviceKeyMapping.BallMappings)
	{
		BallsContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SBallInputView)
					.BallMapping(BallMapping.Value)
					.BallState(TAttribute<FVector2D>::CreateLambda([WeakDeviceMapping, BallMapping, InputDevice]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FVector2D::ZeroVector;

							if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
								return InputDevice->GetBallState(Self->SelectedDeviceIdentifier->DeviceInstanceId, BallMapping.Key);

							return FVector2D::ZeroVector;
						}))
					.OnClicked_Lambda([WeakDeviceMapping, BallMapping]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FReply::Handled();

							if (Self->InputMappingEditor.IsValid())
								Self->CloseInputMappingEditor();

							UBallInputMapping* BallMappingObject = NewObject<UBallInputMapping>();
							BallMappingObject->Initialize(
								Self->SelectedDeviceIdentifier->DeviceIdentifier,
								Self->DeviceKeyMapping,
								BallMapping.Key,
								BallMapping.Value);

							Self->InputMappingEditor =
								SNew(SWindow)
								.Title(LOCTEXT("DeviceDetailsWindowTitle", "Ball Input Mapping"))
								.ClientSize(FVector2D(500.f, 700.f))
								.SupportsMinimize(false)
								.SupportsMaximize(true)
								[
									SNew(SInputMappingEditor)
										.InputMappingObject(BallMappingObject)
										.OnCancel_Lambda([WeakDeviceMapping]
											{
												if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
													Self->CloseInputMappingEditor();

												return FReply::Handled();
											})
										.OnSave_Lambda([WeakDeviceMapping, BallMapping](const UObject* ModifiedObject)
											{
												const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
												if (!Self)
													return;

												if (const UBallInputMapping* ModifiedMapping = Cast<UBallInputMapping>(ModifiedObject))
												{
													Self->DeviceKeyMapping.BallMappings[BallMapping.Key] = ModifiedMapping->BallMapping;
												}

												Self->CloseInputMappingEditor();
												Self->ApplyDeviceKeyMapping();
											})
								];

							FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
							return FReply::Handled();
						})
			];
	}

	BallSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BallsSectionTitle", "Balls"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	BallSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.BallMappings.Num() > 0
				? BallsContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoBallMappings", "This device has no Ball Mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::RefreshTouchpadContainer()
{
	if (!TouchpadSectionContainer.IsValid())
		return;

	TouchpadSectionContainer->ClearChildren();

	TSharedRef<SWrapBox> TouchpadContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);

	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;

	for (const TPair<int32, FJoystickTouchpadKeyMapping>& TouchpadMapping : DeviceKeyMapping.TouchpadMappings)
	{
		TArray<TAttribute<FTouchFingerState>> TouchpadFingerStates;
		TouchpadFingerStates.Reserve(TouchpadMapping.Value.Fingers.Num());

		for (int32 FingerIndex = 0; FingerIndex < TouchpadMapping.Value.Fingers.Num(); ++FingerIndex)
		{
			TouchpadFingerStates.Add(TAttribute<FTouchFingerState>::CreateLambda([WeakDeviceMapping, TouchpadMapping, FingerIndex, InputDevice]
				{
					const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
					if (!Self)
						return FTouchFingerState();

					if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
						return InputDevice->GetTouchpadFingerState(Self->SelectedDeviceIdentifier->DeviceInstanceId, TouchpadMapping.Key, FingerIndex);

					return FTouchFingerState();
				}));
		}

		TouchpadContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SBox)
				.WidthOverride(400.0f)
				.MinDesiredHeight(90.0f)
				[
					SNew(STouchpadInputView)
					.TouchpadMapping(TouchpadMapping.Value)
					.FingersState(TouchpadFingerStates)
					.OnClicked_Lambda(
						[WeakDeviceMapping, TouchpadMapping]()
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FReply::Handled();

							if (Self->InputMappingEditor.IsValid())
								Self->CloseInputMappingEditor();

							UTouchpadInputMapping* TouchpadMappingObject = NewObject<UTouchpadInputMapping>();
							TouchpadMappingObject->Initialize(
								Self->SelectedDeviceIdentifier->DeviceIdentifier,
								Self->DeviceKeyMapping,
								TouchpadMapping.Key,
								TouchpadMapping.Value);

							Self->InputMappingEditor =
								SNew(SWindow)
								.Title(LOCTEXT("DeviceDetailsWindowTitle", "Touchpad Input Mapping"))
								.ClientSize(FVector2D(500.f, 700.f))
								.SupportsMinimize(false)
								.SupportsMaximize(true)
								[
									SNew(SInputMappingEditor)
									.InputMappingObject(TouchpadMappingObject)
									.OnCancel_Lambda([WeakDeviceMapping]
										{
											if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
												Self->CloseInputMappingEditor();

											return FReply::Handled();
										})
									.OnSave_Lambda([WeakDeviceMapping, TouchpadMapping](const UObject* ModifiedObject)
										{
											const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
											if (!Self)
												return;

											if (const UTouchpadInputMapping* ModifiedMapping = Cast<UTouchpadInputMapping>(ModifiedObject))
												Self->DeviceKeyMapping.TouchpadMappings[TouchpadMapping.Key] = ModifiedMapping->TouchpadKeyMapping;

											Self->CloseInputMappingEditor();
											Self->ApplyDeviceKeyMapping();
										})
								];

							FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
							return FReply::Handled();
						})
				]
			];
	}

	TouchpadSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AxisSectionTitle", "Touchpads"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	TouchpadSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.TouchpadMappings.Num() > 0
				? TouchpadContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoTouchpadMappings", "This device has no Touchpad mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::RefreshSensorContainer()
{
	if (!SensorSectionContainer.IsValid())
		return;

	SensorSectionContainer->ClearChildren();
	TSharedRef<SWrapBox> SensorsContainer = SNew(SWrapBox)

		.PreferredSize_Lambda([this]()
			{
				return GetCachedGeometry().GetLocalSize().X - 80.0f;
			})
		.InnerSlotPadding(FVector2D(8.0f, 8.0f))
		.Orientation(Orient_Horizontal);

	TWeakPtr<SDeviceKeyMapping> WeakDeviceMapping = SharedThis(this);
	const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get();
	FIndependentInputDevice* InputDevice = InputSubsystem ? InputSubsystem->GetInputDevice() : nullptr;
	
	for (const TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : DeviceKeyMapping.SensorMappings)
	{
		SensorsContainer->AddSlot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SBox)
				.MinDesiredHeight(90.0f)
				[
					SNew(SSensorInputView)
					.SensorType(SensorMapping.Key)
					.SensorMapping(SensorMapping.Value)
					.SensorState(TAttribute<FSensorState>::CreateLambda([WeakDeviceMapping, SensorMapping, InputDevice]
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FSensorState();

							if (Self->SelectedDeviceIdentifier->DeviceInstanceId.IsValid() && InputDevice)
								return InputDevice->GetSensorState(Self->SelectedDeviceIdentifier->DeviceInstanceId, SensorMapping.Key);

							return FSensorState();
						}))
					.OnClicked_Lambda([WeakDeviceMapping, SensorMapping]()
						{
							const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
							if (!Self)
								return FReply::Handled();

							if (Self->InputMappingEditor.IsValid())
								Self->CloseInputMappingEditor();

							USensorInputMapping* SensorMappingObject = NewObject<USensorInputMapping>();
							SensorMappingObject->Initialize(
								Self->SelectedDeviceIdentifier->DeviceIdentifier,
								Self->DeviceKeyMapping,
								SensorMapping.Key,
								SensorMapping.Value);

							Self->InputMappingEditor =
								SNew(SWindow)
								.Title(LOCTEXT("DeviceDetailsWindowTitle", "Sensor Input Mapping"))
								.ClientSize(FVector2D(500.f, 700.f))
								.SupportsMinimize(false)
								.SupportsMaximize(true)
								[
									SNew(SInputMappingEditor)
									.InputMappingObject(SensorMappingObject)
									.OnCancel_Lambda([WeakDeviceMapping]
										{
											if (const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin())
												Self->CloseInputMappingEditor();
											
											return FReply::Handled();
										})
									.OnSave_Lambda([WeakDeviceMapping, SensorMapping](const UObject* ModifiedObject)
										{
											const TSharedPtr<SDeviceKeyMapping> Self = WeakDeviceMapping.Pin();
											if (!Self)
												return;

											if (const USensorInputMapping* ModifiedMapping = Cast<USensorInputMapping>(ModifiedObject))
												Self->DeviceKeyMapping.SensorMappings[SensorMapping.Key] = ModifiedMapping->SensorKeyMapping;

											Self->CloseInputMappingEditor();
											Self->ApplyDeviceKeyMapping();
										})
								];

							FSlateApplication::Get().AddWindow(Self->InputMappingEditor.ToSharedRef());
							return FReply::Handled();
						})
				]
			];
	}

	SensorSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SensorSectionTitle", "Sensors"))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.Font(FAppStyle::GetFontStyle("PropertyWindow.BoldFont"))
		];

	SensorSectionContainer->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			DeviceKeyMapping.SensorMappings.Num() > 0
				? SensorsContainer
				: StaticCastSharedRef<SWidget>(
					SNew(SBox)
					.MinDesiredHeight(50.0f)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoSensorMappings", "This device has no Sensor Mappings."))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					])
		];
}

void SDeviceKeyMapping::CloseInputMappingEditor()
{
	if (InputMappingEditor)
	{
		InputMappingEditor->RequestDestroyWindow();
		InputMappingEditor.Reset();
	}
}

TSharedRef<SWidget> SDeviceKeyMapping::CreatePropertyRow(const FText& Label, TAttribute<FText> Value)
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

#undef LOCTEXT_NAMESPACE
