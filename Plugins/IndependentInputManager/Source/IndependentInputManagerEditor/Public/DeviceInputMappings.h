// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IndependentInputManagerTypes.h"
#include "DeviceInputMappings.generated.h"

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UButtonInputMapping : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = ButtonMapping, meta = (ShowOnlyInnerProperties))
	FIndependentInputKey Key;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UAxisInputMapping : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = AxisMapping, meta = (ShowOnlyInnerProperties))
	FJoystickAxisKeyMapping AxisMapping;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UHatInputMapping : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = AxisMapping, meta = (ShowOnlyInnerProperties))
	FJoystickHatKeyMapping HatMapping;

};


/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UTouchpadInputMapping : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = TouchpadKeyMapping, meta = (ShowOnlyInnerProperties))
	FJoystickTouchpadKeyMapping TouchpadKeyMapping;

};

/**
 *
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API USensorInputMapping : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = TouchpadKeyMapping, meta = (ShowOnlyInnerProperties))
	FJoystickSensorKeyMapping SensorKeyMapping;

};