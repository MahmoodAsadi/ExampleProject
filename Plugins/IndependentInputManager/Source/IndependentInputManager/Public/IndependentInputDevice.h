// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericInputDeviceMap.h"
#include "IInputDevice.h"

#include "IndependentInputManagerTypes.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGER_API FIndependentInputDevice final : public IInputDevice
{
public:

	friend class UIndependentInputSubsystem;
	friend class UIndependentInputManagerSettings;

	explicit FIndependentInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);

	virtual void Tick(float DeltaTime) override {}
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override { MessageHandler = InMessageHandler; }
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override { return false; }
	virtual void SetChannelValue(int ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int ControllerId, const FForceFeedbackValues& Values) override;
	virtual bool IsGamepadAttached() const override { return DeviceInfos.Num() > 0; }

	void DevicePluggedIn(FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice, const FJoystickDeviceKeyMapping& DeviceMapping);
	void DeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo);
	void HandleButtonEvent(const FInputDeviceInstanceId& DeviceId, const int32 Button, const bool bPressed);
	void HandleAxisEvent(const FInputDeviceInstanceId& DeviceId, const int32 Axis, const float Value);
	void HandleHatEvent(const FInputDeviceInstanceId& DeviceId, const int32 Hat, const uint8 Value);
	void HandleBallEvent(const FInputDeviceInstanceId& DeviceId, const int32 Ball, const float XRel, const float YRel);
	void HandleTouchpadEvent(const FInputDeviceInstanceId& DeviceId, int32 Touchpad, int32 Finger, bool bTouched, float X, float Y, float Pressure);
	void HandleSensorEvent(const FInputDeviceInstanceId& DeviceId, const EDeviceSensorType SensorType, const FVector& Value);
	
	bool SetAdaptiveTriggerResistance(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 Force);
	bool SetAdaptiveTriggerWeapon(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength);
	bool SetAdaptiveTriggerVibration(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency);
	bool SetAdaptiveTriggerBow(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);
	bool SetAdaptiveTriggerGalloping(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);
	bool SetAdaptiveTriggerMachine(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);
	bool ClearAdaptiveTriggerEffect(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger);

	bool GetButtonState(const FInputDeviceInstanceId& DeviceId, const int32 Button) const;
	float GetAxisState(const FInputDeviceInstanceId& DeviceId, const int32 Axis) const;
	float GetAxisRawState(const FInputDeviceInstanceId& DeviceId, const int32 Axis) const;
	bool GetAxisVirtualButtonState(const FInputDeviceInstanceId& DeviceId, const int32 Axis, const int32 VirtualButton) const;
	uint8 GetHatState(const FInputDeviceInstanceId& DeviceId, const int32 Hat) const;
	FVector2D GetBallState(const FInputDeviceInstanceId& DeviceId, const int32 Ball) const;
	FTouchFingerState GetTouchpadFingerState(const FInputDeviceInstanceId& DeviceId, const int32 TouchpadIndex, const int32 FingerIndex) const;
	FSensorState GetSensorState(const FInputDeviceInstanceId& DeviceId, const EDeviceSensorType SensorType) const;

private:

	void SetAccelerometerSensorEnable(bool bEnable);
	void SetGyroscopeSensorEnable(bool bEnable);
	FString GetDeviceHardwareDeviceIdentifier(const FJoystickDeviceInfo& DeviceInfo) const;
	FJoystickDeviceState CreateDeviceState(FJoystickDeviceInfo& DeviceInfo, const FJoystickDeviceKeyMapping& InKeyMapping);
	void UpdateVirtualButtons(FAxisState* AxisState);
	void HandleButtonState(FButtonState& ButtonState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleAxisState(FAxisState& AxisState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleHatState(FHatState& HatState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleBallState(FBallState& BallState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleTouchFingerState(FTouchFingerState& FingerState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleSensorState(TMap<EDeviceSensorType, FSensorState>& Sensors, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId);
	void HandleForceFeedback(FForceFeedbackState& ForceFeedbackState, const FInputDeviceInstanceId& DeviceId);

	void ResetSensorState(const FInputDeviceInstanceId& DeviceId, EDeviceSensorType SensorType);

	TInputDeviceMap<FInputDeviceInstanceId> InternalDeviceIdMappings;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	TMap<FInputDeviceInstanceId, FJoystickDeviceInfo> DeviceInfos;
	TMap<FInputDeviceInstanceId, FSDLJoystickDevice> SDLDevices;
	TMap<FInputDeviceInstanceId, FJoystickDeviceKeyMapping> DeviceMappings;
	TMap<FInputDeviceInstanceId, FJoystickDeviceState> DeviceStates;

	/** Delay before sending a repeat message after a button was first pressed */
	float InitialButtonRepeatDelay;

	/** Delay before sending a repeat message after a button has been pressed for a while */
	float ButtonRepeatDelay;
};
