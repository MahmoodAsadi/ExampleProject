// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnhancedInputSubsystemInterface.h"
#include "InputMappingContext.h"

#include "IndependentInputMappingTypes.generated.h"

class UIndependentInputMappingContext;
class UInputAction;
class UInputKeyVisualCollection;
class UInputModifier;
class UInputMappingContext;
class UInputTrigger;


template<typename TEnum>
FORCEINLINE bool HasFlag(const int32 Flags, TEnum Flag)
{
	return Flags & static_cast<int32>(Flag);
}


UENUM(BlueprintType)
enum class EIndependentInputDeviceType : uint8
{
	KeyboardMouse	UMETA(DisplayName = "Keyboard & Mouse"),
	Controller
};

/** Bitmask of input-device sections supported by a binding definition. */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EIndependentInputBindingDeviceMask : uint8
{
	None = 0					UMETA(Hidden),
	KeyboardMouse = 1 << 0		UMETA(DisplayName = "Keyboard & Mouse"),
	Controller = 1 << 1
};
ENUM_CLASS_FLAGS(EIndependentInputBindingDeviceMask);


/** Type of physical input accepted by a binding row. */
UENUM(BlueprintType)
enum class EIndependentInputBindingCaptureType : uint8
{
	Button,
	Axis1D,
	Axis2D
};


/** Public binding slots exposed by the in-game settings UI. */
UENUM(BlueprintType)
enum class EIndependentInputBindingSlot : uint8
{
	Primary,
	Secondary
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputCaptureInfo
{
	GENERATED_BODY()

public:

	FIndependentInputCaptureInfo() = default;

	FIndependentInputCaptureInfo(int32 InSupportedCaptureInputDevice, 
		EIndependentInputBindingCaptureType InBindingCaptureType, 
		bool bInAllowModifierKeys, 
		bool bInAllowTouchKeys, 
		TArray<FKey> InEscapeKeys) 
		: SupportedCaptureInputDevice(InSupportedCaptureInputDevice)
		, BindingCaptureType(InBindingCaptureType)
		, bAllowModifierKeys(bInAllowModifierKeys)
		, bAllowTouchKeys(bInAllowTouchKeys)
		, EscapeKeys(InEscapeKeys)
	{
	}

	/** Bitmask of input-device supported by a binding definition. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (Bitmask, BitmaskEnum = "/Script/IndependentInputMapper.EIndependentInputBindingDeviceMask"))
	int32 SupportedCaptureInputDevice = 0;

	/** Type of physical input accepted by a binding row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping")
	EIndependentInputBindingCaptureType BindingCaptureType = EIndependentInputBindingCaptureType::Button;

	/** Whether allow modifier keys. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (EditCondition = "SupportedCaptureInputDevice & '/Script/IndependentInputMapper.EIndependentInputBindingDeviceMask::KeyboardMouse'"))
	bool bAllowModifierKeys = false;

	/** Whether allow mouse button or axis keys. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (EditCondition = "SupportedCaptureInputDevice & '/Script/IndependentInputMapper.EIndependentInputBindingDeviceMask::KeyboardMouse'"))
	bool bAllowMouseKeys = true;

	/** Whether allow touch keys. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping")
	bool bAllowTouchKeys = false;

	/** Threshold for axis value to be considered a valid input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (EditCondition = "BindingCaptureType != EIndependentInputBindingCaptureType::Button"))
	float AxisThreshold = 0.5f;

	/** Threshold for mouse axis delta to be considered a valid input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (EditCondition = "SupportedCaptureInputDevice & '/Script/IndependentInputMapper.EIndependentInputBindingDeviceMask::KeyboardMouse' && BindingCaptureType != EIndependentInputBindingCaptureType::Button && bAllowMouseKeys"))
	float MouseAxisDeltaThreshold = 50.0f;

	/** Threshold for touch axis delta to be considered a valid input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping", meta = (EditCondition = "BindingCaptureType != EIndependentInputBindingCaptureType::Button && bAllowTouchKeys"))
	float TouchDeltaThreshold = 0.1f;

	/** Keys that will cancel the input capture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping")
	TArray<FKey> EscapeKeys = { FKey(EKeys::Escape), FKey(EKeys::Gamepad_Special_Right) };

	/** When true, the input capture will clear the current key chord on start of capture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping")
	bool bClearOnStartCapture = true;

	bool IsKeyCompatibleForCapture(FKey InKey) const;
	bool IsEscapeKey(FKey InKey) const;

};


/** Authored default mapping and semantic template for one device section/slot. */
USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputDefaultMapping
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	EIndependentInputBindingSlot Slot = EIndependentInputBindingSlot::Primary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	FKey Key;

	/** Mapping triggers duplicated into generated runtime contexts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = InputMapping)
	TArray<TObjectPtr<UInputTrigger>> Triggers;

	/** Mapping modifiers duplicated into generated runtime contexts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = InputMapping)
	TArray<TObjectPtr<UInputModifier>> Modifiers;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputMappingDefinition
{
	GENERATED_BODY()

public:

	FIndependentInputMappingDefinition()
	{
		PrimaryMapping.Slot = EIndependentInputBindingSlot::Primary;
		SecondaryMapping.Slot = EIndependentInputBindingSlot::Secondary;
	}

	/** If false, Can not modify this binding for this input mapping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	bool bCanBeModified = true;

	/** The input device which this mapping is for. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	EIndependentInputDeviceType DeviceType = EIndependentInputDeviceType::KeyboardMouse;

	/** The input capture info for this mapping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	FIndependentInputCaptureInfo InputCaptureInfo;

	UPROPERTY()
	bool bCanEditMappingId = true;

	/** Mapping Id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping, meta = (EditCondition = "bCanEditMappingId", HideEditConditionToggle))
	FName MappingId;

	/** The input action this mapping relate to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	TSoftObjectPtr<UInputAction> InputAction;

	/** The display name for this input mapping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	FText DisplayName;

	/** The category this mapping belongs to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	FText DisplayCategory;

	/** Whether this mapping has secondary slot with another input key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	bool bSupportsSecondarySlot = true;

	/** The default input key(s) for this mapping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	FIndependentInputDefaultMapping PrimaryMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping, meta = (EditCondition = bSupportsSecondarySlot))
	FIndependentInputDefaultMapping SecondaryMapping;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = InputMapping)
	TObjectPtr<UIndependentInputMappingContext> OwningMappingContext = nullptr;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputMappingDefinitionContainer
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputMapping)
	TArray<FIndependentInputMappingDefinition> MappingDefinitions;

};


/** Per-mapping user overrides for the two public binding slots. */
USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputBindingOverride
{
	GENERATED_BODY()

public:

	bool operator==(const FIndependentInputBindingOverride& Other) const
	{
		return PrimaryKey == Other.PrimaryKey
			&& SecondaryKey == Other.SecondaryKey;
	}

	bool operator!=(const FIndependentInputBindingOverride& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = InputMapping)
	FKey PrimaryKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = InputMapping)
	FKey SecondaryKey;

};


/** All gameplay-binding overrides for one device section or controller profile. */
USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputBindingSet
{
	GENERATED_BODY()

public:

	bool operator==(const FIndependentInputBindingSet& Other) const
	{
		return Bindings.OrderIndependentCompareEqual(Other.Bindings);
	}

	bool operator!=(const FIndependentInputBindingSet& Other) const
	{
		return !(*this == Other);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Input Mapping")
	TMap<FName, FIndependentInputBindingOverride> Bindings;
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FIndependentInputMappingContextsInfo
{
	GENERATED_BODY()

public:

	FIndependentInputMappingContextsInfo() {}
	FIndependentInputMappingContextsInfo(
		TObjectPtr<UInputMappingContext> InMappingContext,
		int32 InPriority,
		FModifyContextOptions InModifyOptions)
		: MappingContext(InMappingContext)
		, Priority(InPriority)
		, ModifyOptions(InModifyOptions)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BindingSet)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BindingSet)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BindingSet)
	FModifyContextOptions ModifyOptions;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FInputKeyVisualInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Info")
	TSoftObjectPtr<UTexture2D> ActionImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Info")
	FText ActionKeyDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Info")
	bool bShowBorder = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Info", meta = (InlineEditConditionToggle))
	bool bOverrideBorderBrush = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Info", meta = (EditCondition = "bOverrideBorderBrush"))
	FSlateBrush OverrideBorderBrush;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMAPPER_API FInputDeviceVisualCollection
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Visual Collection")
	TMap<FName, TSoftObjectPtr<UInputKeyVisualCollection>> HardwareDevicesVisualCollection;

};
