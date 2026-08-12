// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

template <typename OptionType>
class SComboBox;
class SVerticalBox;
class SWindow;
class UDeviceInputMappingBase;

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SDeviceKeyMapping : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDeviceKeyMapping)
		{
		}
	SLATE_END_ARGS()

	SDeviceKeyMapping();
	~SDeviceKeyMapping();

public:
	
	void Construct(const FArguments& InArgs);
	void DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo);
	void DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo);

private:

	void UpdateList(const FJoystickDeviceIdentifier& PreferedSelection, const FInputDeviceInstanceId& PreferedPreviewDeviceId = FInputDeviceInstanceId());
	void UpdatePreviewDevices(const FInputDeviceInstanceId& PreferedPreviewDeviceId);
	void RefreshOptions() const;
	TSharedRef<SWidget> CreateProfileSelectionSection();
	TSharedRef<SWidget> CreatePreviewDeviceSelector();
	TSharedRef<SWidget> CreateProfileInfoSection();
	TSharedRef<SWidget> CreateDeviceKeyMappingSection();
	void ApplyDeviceKeyMapping(const UDeviceInputMappingBase& InputMapping);
	void RefreshDeviceKeyMappingContainer();
	void RefreshButtonsContainer();
	void RefreshAxisContainer();
	void RefreshHatsContainer();
	void RefreshBallsContainer();
	void RefreshTouchpadContainer();
	void RefreshSensorContainer();
	void CloseInputMappingEditor();

	FInputDeviceInstanceId GetSelectedPreviewDeviceId() const;
	FText GetPreviewDeviceDisplayText(const FJoystickDeviceInfo& DeviceInfo) const;
	TSharedRef<SWidget> CreatePropertyRow(const FText& Label, TAttribute<FText> Value);

	TSharedPtr<SComboBox<TSharedPtr<FJoystickDeviceIdentifier>>> MappingComboBox;
	TSharedPtr<SComboBox<TSharedPtr<FJoystickDeviceInfo>>> PreviewDeviceComboBox;
	TArray<TSharedPtr<FJoystickDeviceIdentifier>> DeviceMappings;
	TArray<TSharedPtr<FJoystickDeviceInfo>> PreviewDevices;
	TSharedPtr<FJoystickDeviceIdentifier> SelectedDeviceIdentifier;
	TSharedPtr<FJoystickDeviceInfo> SelectedPreviewDevice;
	FInputDeviceInstanceId SelectedPreviewDeviceId;
	FJoystickDeviceKeyMapping DeviceKeyMapping;
	TSharedPtr<SVerticalBox> ButtonSectionContainer;
	TSharedPtr<SVerticalBox> AxisSectionContainer;
	TSharedPtr<SVerticalBox> HatSectionContainer;
	TSharedPtr<SVerticalBox> BallSectionContainer;
	TSharedPtr<SVerticalBox> TouchpadSectionContainer;
	TSharedPtr<SVerticalBox> SensorSectionContainer;
	TSharedPtr<SWindow> InputMappingEditor;
	FScrollBoxStyle NoShadowScrollStyle;

};
