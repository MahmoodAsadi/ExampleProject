// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "IndependentInputEditorSubsystem.generated.h"

class SIndependentInputDeviceManager;

static inline FName InputManagerTabId("IndependentInputDeviceManager");

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMANAGEREDITOR_API UIndependentInputEditorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	static UIndependentInputEditorSubsystem* Get();
	static TSharedPtr<SIndependentInputDeviceManager> FindInputManagerTab();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:

	UFUNCTION()
	void OnDevicePluggedInFirstTime(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice);

	UFUNCTION()
	void OnDevicePluggedIn(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice);

	UFUNCTION()
	void OnDeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice);

	UFUNCTION()
	void OnDeviceInfoUpdated(const FJoystickDeviceInfo& DeviceInfo);

	//TSharedPtr<SIndependentInputDeviceManager> InputDeviceManager;

};
