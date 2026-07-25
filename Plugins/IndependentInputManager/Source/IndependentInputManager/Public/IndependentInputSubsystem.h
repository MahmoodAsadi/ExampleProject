// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "IndependentInputManagerTypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "IndependentInputSubsystem.generated.h"

class FIndependentInputDevice;

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMANAGER_API UIndependentInputSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	static UIndependentInputSubsystem* Get();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void InitializeInputDevice(const TSharedPtr<FIndependentInputDevice>& InputDevice);
	FKey CreateDeviceKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& Key);
	FKey CreateDevicePairedKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& KeyX, const FIndependentInputKey& KeyY);
	FIndependentInputDevice* GetInputDevice() const;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintAssignable)
	FOnDevicePluggedIn OnDevicePluggedInFirstTime;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintAssignable)
	FOnDevicePluggedIn OnDevicePluggedIn;

	UPROPERTY(BlueprintAssignable)
	FOnDeviceUnplugged OnDeviceUnplugged;

	UPROPERTY(BlueprintAssignable)
	FOnDeviceInfoUpdate OnDeviceInfoUpdated;

	const TMap<FInputDeviceInstanceId, FJoystickDeviceInfo>& GetConnectedDevices() const { return ConnectedDevices; }

	// Find DeviceInfo for DeviceId
	const FJoystickDeviceInfo* GetDeviceInfo(const FInputDeviceInstanceId& DeviceId) const;

	// Find DevinceInfo for DeviceId
	const FSDLJoystickDevice* GetSDLDeviceInfo(const FInputDeviceInstanceId& DeviceId) const;

	// Find first connected DeviceInfo for DeviceIdentifier
	const FJoystickDeviceInfo* FindDeviceInfoByIdentifier(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool IsDeviceConnected(FInputDeviceInstanceId DeviceId) const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	int32 GetConnectedDeviceCount() const { return ConnectedDevices.Num(); }
	
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	static FRotator CalculateOrientationFromAccelerometer(const FVector& Accelerometer);

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	TArray<FJoystickDeviceInfo> GetConnectedDevicesInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool PlayRumble(const FInputDeviceInstanceId& DeviceId, float LowFrequency, float HighFrequency, float Duration = 0.05f);

	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool PlayTriggerRumble(const FInputDeviceInstanceId& DeviceId, float LeftTrigger, float RightTrigger, float Duration = 0.05f);

	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SupportsTriggerRumble(const FInputDeviceInstanceId& DeviceId);

	/**
	 * The trigger resists movement beyond Position
	 * with a constant force. Formerly (mis-)implemented as "Resistance" using the deprecated
	 * Simple_Feedback opcode (0x01).
	 * @param Position  Zone the resistance begins at. 0-9.
	 * @param Strength  Resistance force. 0-8 (0 disables the effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerResistance(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 Force);

	/**
	 * Resistance builds from StartPosition to
	 * EndPosition, then releases entirely, like pulling the trigger of a gun.
	 * @param StartPosition  2-7.
	 * @param EndPosition    StartPosition+1 to 8.
	 * @param Strength       0-8 (0 disables the effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerWeapon(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength);

	/**
	 * The trigger vibrates at Frequency once
	 * pulled beyond Position.
	 * @param Position   Zone the vibration begins at. 0-9.
	 * @param Amplitude  Strength of the vibration. 0-8 (0 disables the effect).
	 * @param Frequency  Frequency in Hz (0 disables the effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerVibration(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency);

	/**
	 * Resembles Weapon, but with an added spring-like snap-back force.
	 * @param StartPosition  0-8.
	 * @param EndPosition    StartPosition+1 to 8.
	 * @param Strength       0-8.
	 * @param SnapForce      Force of the snap-back. 0-8.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerBow(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);

	/**
	 * Rhythmic two-beat cycling between two sub-positions; only clearly perceptible at low
	 * frequencies.
	 * @param StartPosition  0-8.
	 * @param EndPosition    StartPosition+1 to 9.
	 * @param FirstFoot      Position of the first "foot" in the cycle. 0-6.
	 * @param SecondFoot     Position of the second "foot" in the cycle. FirstFoot+1 to 7.
	 * @param Frequency      Frequency in Hz (0 disables the effect).
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerGalloping(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);

	/**
	 * Resembles Vibration, but oscillates between two amplitudes.
	 * @param StartPosition  1-8.
	 * @param EndPosition    StartPosition+1 to 9.
	 * @param AmplitudeA     Primary vibration strength. 0-7.
	 * @param AmplitudeB     Secondary vibration strength. 0-7.
	 * @param Frequency      Frequency in Hz (0 disables the effect).
	 * @param Period         Period of the oscillation between AmplitudeA/B, in tenths of a second.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerMachine(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);

	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool ClearAdaptiveTriggerEffect(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger);

	/** Reconnects all connected devices matching the identifier to rebuild their runtime state using the latest device mapping. */
	void ReconnectDevice(const FJoystickDeviceIdentifier& DeviceIdentifier);

private:

	bool bInitialized = false;
	TSharedPtr<FIndependentInputDevice> IndependentInputDevice;
	FTSTicker::FDelegateHandle TickHandle;
	TMap<FInputDeviceInstanceId, FJoystickDeviceInfo> ConnectedDevices;
	TMap<FInputDeviceInstanceId, FSDLJoystickDevice> SDLDevices;
	TMap<FInputDeviceInstanceId, FJoystickDeviceKeyMapping> ConnectedDevicesMappings;
	TArray<FName> RegisteredKeyCategories;

	bool Tick(float DeltaTime);
	void InitSDL();
	void PumpEvents();
	void ShutdownSDL();
	void CloseSDLDevice(FSDLJoystickDevice& Device, const FJoystickDeviceInfo& DeviceInfo, bool bFadeOutLED);
	void FadeOutJoystickLED(SDL_Joystick* Joystick);
	
	bool RegisterDevice(SDL_JoystickID InstanceId);
	void UnregisterDevice(SDL_JoystickID InstanceId, bool bFadeOutLED = false);
	void HandleBatteryUpdated(SDL_JoystickID InstanceId, EDeviceBatteryState InState, int32 InPercent);
	void CreateKeyMappingIfMissing(FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice, bool& bCreatedNewDeviceMapping);
	void CheckShouldUseInputAPI(const FJoystickDeviceInfo& DeviceInfo, FJoystickDeviceKeyMapping& DeviceKeyMapping);

};
