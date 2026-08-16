// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IndependentInputManagerTypes.h"
#include "DeviceInputMappings.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class INDEPENDENTINPUTMANAGEREDITOR_API UDeviceInputMappingBase : public UObject
{
	GENERATED_BODY()

public:

	void InitializeValidationContext(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping);
	bool BuildValidatedDeviceKeyMapping(
		FJoystickDeviceIdentifier& OutDeviceIdentifier,
		FJoystickDeviceKeyMapping& OutDeviceKeyMapping,
		FText& OutValidationError) const;
	bool ValidateMapping(FText& OutValidationError) const;
	bool RequiresRestartToApplyMapping() const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const PURE_VIRTUAL(
		UDeviceInputMappingBase::ApplyEditedMapping);

private:

	UPROPERTY(Transient)
	FJoystickDeviceIdentifier DeviceIdentifier;

	UPROPERTY(Transient)
	FJoystickDeviceKeyMapping DeviceKeyMappingSnapshot;

	FText ValidationError;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UButtonInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()
	
public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		int32 InButtonIndex,
		const FJoystickButtonKeyMapping& InButtonMapping);

	UPROPERTY(EditAnywhere, Category = ButtonMapping, meta = (ShowOnlyInnerProperties))
	FIndependentInputKey Key;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	int32 ButtonIndex = INDEX_NONE;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UAxisInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()

public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		int32 InAxisIndex,
		const FJoystickAxisKeyMapping& InAxisMapping);

	UPROPERTY(EditAnywhere, Category = AxisMapping, meta = (ShowOnlyInnerProperties))
	FJoystickAxisKeyMapping AxisMapping;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	int32 AxisIndex = INDEX_NONE;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UHatInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()

public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		int32 InHatIndex,
		const FJoystickHatKeyMapping& InHatMapping);

	UPROPERTY(EditAnywhere, Category = HatMapping, meta = (ShowOnlyInnerProperties))
	FJoystickHatKeyMapping HatMapping;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	int32 HatIndex = INDEX_NONE;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UBallInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()

public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		int32 InBallIndex,
		const FJoystickBallKeyMapping& InBallMapping);

	UPROPERTY(EditAnywhere, Category = BallMapping, meta = (ShowOnlyInnerProperties))
	FJoystickBallKeyMapping BallMapping;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	int32 BallIndex = INDEX_NONE;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UTouchpadInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()

public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		int32 InTouchpadIndex,
		const FJoystickTouchpadKeyMapping& InTouchpadKeyMapping);

	UPROPERTY(EditAnywhere, Category = TouchpadKeyMapping, meta = (ShowOnlyInnerProperties))
	FJoystickTouchpadKeyMapping TouchpadKeyMapping;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	int32 TouchpadIndex = INDEX_NONE;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API USensorInputMapping : public UDeviceInputMappingBase
{
	GENERATED_BODY()

public:

	void Initialize(
		const FJoystickDeviceIdentifier& InDeviceIdentifier,
		const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
		EDeviceSensorType InSensorType,
		const FJoystickSensorKeyMapping& InSensorKeyMapping);

	UPROPERTY(EditAnywhere, Category = TouchpadKeyMapping, meta = (ShowOnlyInnerProperties))
	FJoystickSensorKeyMapping SensorKeyMapping;

protected:

	virtual void ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const override;

private:

	EDeviceSensorType SensorType = EDeviceSensorType::None;

};
