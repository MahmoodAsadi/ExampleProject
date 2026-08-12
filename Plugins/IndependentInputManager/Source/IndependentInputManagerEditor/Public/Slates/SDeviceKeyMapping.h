// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"
#include "SDeviceKeyMapping.generated.h"

template <typename OptionType>
class SComboBox;
class SVerticalBox;
class SWindow;
class UDeviceInputMappingBase;

USTRUCT()
struct FKeyMappingDeviceIdentifier
{
	GENERATED_BODY()

public:

	FKeyMappingDeviceIdentifier() {}
	FKeyMappingDeviceIdentifier(const FJoystickDeviceIdentifier& InDeviceIdentifier)
		: DeviceIdentifier(InDeviceIdentifier)
	{
	}

	UPROPERTY()
	FJoystickDeviceIdentifier DeviceIdentifier;

	friend uint32 GetTypeHash(const FKeyMappingDeviceIdentifier& Other)
	{
		uint32 Hash = GetTypeHash(Other.DeviceIdentifier.VendorId);
		Hash = HashCombine(Hash, GetTypeHash(Other.DeviceIdentifier.ProductId));
		
		return Hash;
	}

};

struct FKeyMappingPreviewDevice
{
	FKeyMappingPreviewDevice() {}
	FKeyMappingPreviewDevice(const FJoystickDeviceInfo& InDeviceInfo)
		: DeviceInfo(InDeviceInfo)
	{
	}

	FJoystickDeviceInfo DeviceInfo;

};

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

	TSharedPtr<SComboBox<TSharedPtr<FKeyMappingDeviceIdentifier>>> MappingComboBox;
	TSharedPtr<SComboBox<TSharedPtr<FKeyMappingPreviewDevice>>> PreviewDeviceComboBox;
	TArray<TSharedPtr<FKeyMappingDeviceIdentifier>> DeviceMappings;
	TArray<TSharedPtr<FKeyMappingPreviewDevice>> PreviewDevices;
	TSharedPtr<FKeyMappingDeviceIdentifier> SelectedDeviceIdentifier;
	TSharedPtr<FKeyMappingPreviewDevice> SelectedPreviewDevice;
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
