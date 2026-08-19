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

	/** Returns the connected device metadata for DeviceId, or nullptr if it is unavailable. */
	const FJoystickDeviceInfo* GetDeviceInfo(const FInputDeviceInstanceId& DeviceId) const;

	/** Returns mutable connected-device metadata for DeviceId, or nullptr if it is unavailable. */
	FJoystickDeviceInfo* GetMutableDeviceInfo(const FInputDeviceInstanceId& DeviceId);

	/** Returns the SDL device handles for DeviceId, or nullptr if they are unavailable. */
	const FSDLJoystickDevice* GetSDLDeviceInfo(const FInputDeviceInstanceId& DeviceId) const;

	/** Returns the first connected device matching DeviceIdentifier, or nullptr if none matches. */
	const FJoystickDeviceInfo* FindDeviceInfoByIdentifier(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool IsDeviceConnected(FInputDeviceInstanceId DeviceId) const;

	/** Returns whether the connected device is currently owned by Independent Input Manager. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool IsDeviceOwnedByIndependentInputManager(FInputDeviceInstanceId DeviceId) const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	int32 GetConnectedDeviceCount() const { return ConnectedDevices.Num(); }
	
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	TArray<FJoystickDeviceInfo> GetConnectedDevicesInfo() const;

	/** Finds the connected SDL instance ID assigned to InputDeviceId. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool FindDeviceInstanceIdForInputDevice(const FInputDeviceId InputDeviceId, FInputDeviceInstanceId& OutInstanceId) const;

	/**
	 * Finds all connected SDL instance IDs assigned to PlatformUserId.
	 */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool FindDeviceInstanceIdsForPlatformUser(const FPlatformUserId PlatformUserId, TArray<FInputDeviceInstanceId>& OutInstanceIds) const;


	/** Returns whether the connected device supports rumble. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool SupportsRumble(const FInputDeviceInstanceId& DeviceId) const;

	/** Returns whether rumble is enabled in the connected device's profile. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool GetRumbleEnabled(const FInputDeviceInstanceId& DeviceId) const;

	/**
	 * Enables or disables rumble in the connected device's profile.
	 * @return False if the device is unavailable, or if enabling was requested
	 * for a device that does not support rumble.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetRumbleEnable(const FInputDeviceInstanceId& DeviceId, bool bEnable);

	/** Enables or disables rumble for every device profile. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	void SetRumbleEnableForAllDevices(bool bEnable);

	/** Plays Rumble for given device */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool PlayRumble(const FInputDeviceInstanceId& DeviceId, float LowFrequency, float HighFrequency, float Duration = 0.05f);

	/** Returns whether the connected device supports trigger rumble. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool SupportsTriggerRumble(const FInputDeviceInstanceId& DeviceId) const;

	/** Returns whether trigger rumble is enabled in the connected device's profile. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool GetTriggerRumbleEnabled(const FInputDeviceInstanceId& DeviceId) const;

	/**
	 * Enables or disables trigger rumble in the connected device's profile.
	 * @return False if the device is unavailable, or if enabling was requested
	 * for a device that does not support trigger rumble.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetTriggerRumbleEnable(const FInputDeviceInstanceId& DeviceId, bool bEnable);

	/** Enables or disables trigger rumble for every device profile. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	void SetTriggerRumbleEnableForAllDevices(bool bEnable);

	/** Plays Trigger Rumble for given device */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool PlayTriggerRumble(const FInputDeviceInstanceId& DeviceId, float LeftTrigger, float RightTrigger, float Duration = 0.05f);


	/**
	 * Returns whether the connected device supports adaptive-trigger effects.
	 * This currently requires a DualSense controller on Windows.
	 */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Force Feedback")
	bool SupportsAdaptiveTriggerEffects(const FInputDeviceInstanceId& DeviceId) const;

	/**
	 * Enables or disables adaptive-trigger effects in the connected device's profile.
	 * @return False if the device is unavailable, or if enabling was requested
	 * for a device that does not support adaptive-trigger effects.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	bool SetAdaptiveTriggerEffectsEnable(const FInputDeviceInstanceId& DeviceId, bool bEnable);

	/** Enables or disables adaptive-trigger effects for every device profile. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem|Force Feedback")
	void SetAdaptiveTriggerEffectsEnableForAllDevices(bool bEnable);



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
	 * @param StartPosition  Starting zone. Clamped to 1-8.
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


	/** Returns the effective mapping for DeviceId, or a default mapping if none exists. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	FJoystickDeviceKeyMapping GetJoystickDeviceKeyMapping(const FInputDeviceInstanceId& DeviceId) const;

	/** Finds a button mapping by its hardware button index. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	bool GetDeviceButtonMapping(const FInputDeviceInstanceId& DeviceId, int32 ButtonIndex, FJoystickButtonKeyMapping& OutButtonMapping) const;

	/** Finds an axis mapping by its hardware axis index. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	bool GetDeviceAxisMapping(const FInputDeviceInstanceId& DeviceId, int32 AxisIndex, FJoystickAxisKeyMapping& OutAxisMapping) const;

	/** Finds a hat mapping by its hardware hat index. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	bool GetDeviceHatMapping(const FInputDeviceInstanceId& DeviceId, int32 HatIndex, FJoystickHatKeyMapping& OutHatMapping) const;

	/** Finds a ball mapping by its hardware ball index. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	bool GetDeviceBallMapping(const FInputDeviceInstanceId& DeviceId, int32 BallIndex, FJoystickBallKeyMapping& OutBallMapping) const;

	/** Finds a touchpad mapping by its hardware touchpad index. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem|Key Mapping")
	bool GetDeviceTouchpadMapping(const FInputDeviceInstanceId& DeviceId, int32 TouchpadIndex, FJoystickTouchpadKeyMapping& OutTouchpadMapping) const;

	

	/** Returns every sensor reported by the connected device. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	TArray<EDeviceSensorType> GetSupportedSensors(const FInputDeviceInstanceId& DeviceId);

	/** Returns every optional SDL feature reported by the connected device. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	TArray<EJoystickProperties> GetSupportedFeatures(const FInputDeviceInstanceId& DeviceId);

	/** Returns whether Accelerometer Sensor(s) (Accelerometer, LeftAccelerometer, RightAccelerometer) is enabled. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool GetAccelerometerSensorEnabled() const;

	/** Returns whether Gyroscope Sensor(s) (Gyroscope, LeftGyroscope, RightGyroscope) is enabled. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Subsystem")
	bool GetGyroscopeSensorEnabled() const;

	/** Enables or disables Accelerometer sensor(s) (Accelerometer, LeftAccelerometer, RightAccelerometer). */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem")
	void SetAccelerometerSensorEnable(bool bEnable);

	/** Enables or disables Gyroscope sensor(s) (Gyroscope, LeftGyroscope, RightGyroscope). */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem")
	void SetGyroscopeSensorEnable(bool bEnable);

	/**
	 * Reopens all connected devices matching DeviceIdentifier and rebuilds their
	 * runtime state from the latest profile. This internal reconnect keeps the
	 * SDL instance ID assigned to the current physical connection.
	 */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Subsystem")
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
