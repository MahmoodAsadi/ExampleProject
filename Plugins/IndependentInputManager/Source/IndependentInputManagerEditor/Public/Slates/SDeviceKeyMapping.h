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

	FKeyMappingDeviceIdentifier(const FJoystickDeviceIdentifier& InDeviceIdentifier, const FInputDeviceInstanceId& InDeviceInstanceId)
		: DeviceIdentifier(InDeviceIdentifier)
		, DeviceInstanceId(InDeviceInstanceId)
	{
	}

	UPROPERTY()
	FJoystickDeviceIdentifier DeviceIdentifier;

	UPROPERTY()
	FInputDeviceInstanceId DeviceInstanceId;

	friend uint32 GetTypeHash(const FKeyMappingDeviceIdentifier& Other)
	{
		uint32 Hash = GetTypeHash(Other.DeviceIdentifier.VendorId);
		Hash = HashCombine(Hash, GetTypeHash(Other.DeviceIdentifier.ProductId));

		if (Other.DeviceInstanceId.IsValid())
			Hash = HashCombine(Hash, GetTypeHash(Other.DeviceInstanceId.GetId()));
		
		return Hash;
	}

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

	void UpdateList(const FJoystickDeviceIdentifier& PreferedSelection);
	void RefreshOptions() const;
	TSharedRef<SWidget> CreateProfileSelectionSection();
	TSharedRef<SWidget> CreateProfileInfoSection();
	TSharedRef<SWidget> CreateDeviceKeyMappingSection();
	void ApplyDeviceKeyMapping();
	void RefreshDeviceKeyMappingContainer();
	void RefreshButtonsContainer();
	void RefreshAxisContainer();
	void RefreshHatContainer();
	void RefreshTouchpadContainer();
	void RefreshSensorContainer();
	void CloseInputMappingEditor();

	TSharedRef<SWidget> CreatePropertyRow(const FText& Label, TAttribute<FText> Value);

	TSharedPtr<SComboBox<TSharedPtr<FKeyMappingDeviceIdentifier>>> MappingComboBox;
	TArray<TSharedPtr<FKeyMappingDeviceIdentifier>> DeviceMappings;
	TSharedPtr<FKeyMappingDeviceIdentifier> SelectedDeviceIdentifier;
	FJoystickDeviceKeyMapping DeviceKeyMapping;
	TSharedPtr<SVerticalBox> ButtonSectionContainer;
	TSharedPtr<SVerticalBox> AxisSectionContainer;
	TSharedPtr<SVerticalBox> HatSectionContainer;
	TSharedPtr<SVerticalBox> TouchpadSectionContainer;
	TSharedPtr<SVerticalBox> SensorSectionContainer;
	TSharedPtr<SWindow> InputMappingEditor;
	FScrollBoxStyle NoShadowScrollStyle;

};