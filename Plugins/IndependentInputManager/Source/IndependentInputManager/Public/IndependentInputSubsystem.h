// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "IndependentInputManagerTypes.h"
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
	FKey CreateDeviceKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& Key, bool bUpdateAxisWithoutSamples = false);
	FKey CreateDevicePairedKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& KeyX, const FIndependentInputKey& KeyY, bool bUpdateAxisWithoutSamples = false);
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

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool SupportsTriggerRumble(const FInputDeviceInstanceId& DeviceId);

	/**
	 * The trigger resists movement beyond StartPosition
	 * with a constant force. Formerly (mis-)implemented as "Resistance" using the deprecated
	 * Simple_Feedback opcode (0x01).
	 * @param DeviceId       Device that receives the effect.
	 * @param Trigger        Left or right adaptive trigger.
	 * @param StartPosition  Zone where resistance begins. Clamped to 0-9.
	 * @param Force          Resistance force. Clamped to 0-8; 0 disables the effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerResistance(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 Force);

	/**
	 * Resistance builds from StartPosition to
	 * EndPosition, then releases entirely, like pulling the trigger of a gun.
	 * @param DeviceId       Device that receives the effect.
	 * @param Trigger        Left or right adaptive trigger.
	 * @param StartPosition  Starting zone. Clamped to 2-7.
	 * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 8.
	 * @param Strength       Resistance force. Clamped to 0-8; 0 disables the effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerWeapon(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength);

	/**
	 * The trigger vibrates at Frequency once
	 * pulled beyond Position.
	 * @param DeviceId   Device that receives the effect.
	 * @param Trigger    Left or right adaptive trigger.
	 * @param Position   Zone where vibration begins. Clamped to 0-9.
	 * @param Amplitude  Strength of the vibration. Clamped to 0-8; 0 disables the effect.
	 * @param Frequency  Frequency in Hz. Full uint8 range; 0 disables the effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerVibration(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency);

	/**
	 * Resembles Weapon, but with an added spring-like snap-back force.
	 * @param DeviceId       Device that receives the effect.
	 * @param Trigger        Left or right adaptive trigger.
	 * @param StartPosition  Starting zone. Clamped to the effective range 0-7.
	 * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 8.
	 * @param Strength       Resistance force. Clamped to 0-8; 0 disables the effect.
	 * @param SnapForce      Snap-back force. Clamped to 0-8; 0 disables the effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerBow(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);

	/**
	 * Rhythmic two-beat cycling between two sub-positions; only clearly perceptible at low
	 * frequencies.
	 * @param DeviceId       Device that receives the effect.
	 * @param Trigger        Left or right adaptive trigger.
	 * @param StartPosition  Starting zone. Clamped to 0-8.
	 * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 9.
	 * @param FirstFoot      First cycle position. Clamped to 0-6.
	 * @param SecondFoot     Second cycle position. Clamped to FirstFoot+1 through 7.
	 * @param Frequency      Frequency in Hz. Full uint8 range; 0 disables the effect.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerGalloping(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);

	/**
	 * Resembles Vibration, but oscillates between two amplitudes.
	 * @param DeviceId       Device that receives the effect.
	 * @param Trigger        Left or right adaptive trigger.
	 * @param StartPosition  Starting zone. Clamped to 0-8.
	 * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 9.
	 * @param AmplitudeA     Primary cycling strength. Clamped to 0-7.
	 * @param AmplitudeB     Secondary cycling strength. Clamped to 0-7.
	 * @param Frequency      Frequency in Hz. Full uint8 range; 0 disables the effect.
	 * @param Period         Oscillation period in tenths of a second. Full uint8 range.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerMachine(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);

	/**
	 * Clears the active adaptive-trigger effect and returns the trigger stop to neutral.
	 * @param DeviceId  Device whose effect is cleared.
	 * @param Trigger   Left or right adaptive trigger.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool ClearAdaptiveTriggerEffect(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger);

	/** Reconnects all connected devices matching the identifier to rebuild their runtime state using the latest device mapping. */
	void ReconnectDevice(const FJoystickDeviceIdentifier& DeviceIdentifier);

private:

	bool bInitialized = false;
	TSharedPtr<FIndependentInputDevice> IndependentInputDevice;
	FTSTicker::FDelegateHandle TickHandle;
	TMap<FInputDeviceInstanceId, FJoystickDeviceInfo> ConnectedDevices;
	TMap<FInputDeviceInstanceId, FJoystickDeviceKeyMapping> ConnectedDevicesMappings;
	TMap<FInputDeviceInstanceId, FSDLJoystickDevice> SDLDevices;
	TSet<FInputDeviceInstanceId> IgnoredDeviceIds;
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
	void ApplyInputOwnershipPolicy(const FJoystickDeviceInfo& DeviceInfo, FJoystickDeviceKeyMapping& DeviceKeyMapping);

};
