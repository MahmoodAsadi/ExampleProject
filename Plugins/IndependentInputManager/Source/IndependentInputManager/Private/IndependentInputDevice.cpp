// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputDevice.h"

#include "DualSenseState.h"
#if PLATFORM_WINDOWS
#include "DualSenseWindows.h"
#endif
#include "IndependentInputManager.h"
#include "IndependentInputManagerSettings.h"

#define LOCTEXT_NAMESPACE "FIndependentInputDevice"

static FName IndependentInputInterfaceName = FName("IndependentInputDevice");


FIndependentInputDevice::FIndependentInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
{
	InitialButtonRepeatDelay = 0.2f;
	ButtonRepeatDelay = 0.1f;
	
	GConfig->GetFloat(TEXT("/Script/Engine.InputSettings"), TEXT("InitialButtonRepeatDelay"), InitialButtonRepeatDelay, GInputIni);
	GConfig->GetFloat(TEXT("/Script/Engine.InputSettings"), TEXT("ButtonRepeatDelay"), ButtonRepeatDelay, GInputIni);
}

void FIndependentInputDevice::SendControllerEvents()
{
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& DeviceInfo : DeviceInfos)
	{
		FInputDeviceScope InputDeviceScope(this, IndependentInputInterfaceName, DeviceInfo.Key.GetId(), GetDeviceHardwareDeviceIdentifier(DeviceInfo.Value));
		FJoystickDeviceState& DeviceState = DeviceStates[DeviceInfo.Key];
		
		// Handle Buttons
		for (TPair<int32, FButtonState>& ButtonState : DeviceState.Buttons)
		{
			HandleButtonState(ButtonState.Value, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
		}

		// Handle Axis
		for (TPair<int32, FAxisState>& AxisState : DeviceState.Axes)
		{
			HandleAxisState(AxisState.Value, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
		}

		// Handle Balls
		for (TPair<int32, FBallState>& BallState : DeviceState.Balls)
		{
			HandleBallState(BallState.Value, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
		}

		// Handle Hats
		for (TPair<int32, FHatState>& HatPair : DeviceState.Hats)
		{
			HandleHatState(HatPair.Value, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
		}

		for (TPair<int32, FTouchpadState>& TouchpadPair : DeviceState.Touchpads)
		{
			for (FTouchFingerState& Finger : TouchpadPair.Value.FingersState)
			{
				HandleTouchFingerState(Finger, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
			}
		}

		HandleSensorState(DeviceState.Sensors, DeviceState.PlatformUserId, DeviceState.InputDeviceId);
		HandleForceFeedback(DeviceState.ForceFeedback, DeviceInfo.Value.InstanceId);
	}
}

void FIndependentInputDevice::SetChannelValue(int ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	FPlatformUserId UserId = PLATFORMUSERID_NONE;
	FInputDeviceId DeviceId = INPUTDEVICEID_NONE;
	DeviceMapper.RemapControllerIdToPlatformUserAndDevice(ControllerId, UserId, DeviceId);
	if (!UserId.IsValid())
		return;

	Value = FMath::Clamp(Value, 0.0f, 1.0f);
	for (TPair<FInputDeviceInstanceId, FJoystickDeviceState>& DeviceStatePair : DeviceStates)
	{
		FJoystickDeviceState& DeviceState = DeviceStatePair.Value;
		if (DeviceMapper.GetUserForInputDevice(DeviceState.InputDeviceId) != UserId)
			continue;

		switch (ChannelType)
		{
		case FForceFeedbackChannelType::LEFT_LARGE:
			DeviceState.ForceFeedback.LeftLarge = Value;
			break;

		case FForceFeedbackChannelType::LEFT_SMALL:
			DeviceState.ForceFeedback.LeftSmall = Value;
			break;

		case FForceFeedbackChannelType::RIGHT_LARGE:
			DeviceState.ForceFeedback.RightLarge = Value;
			break;

		case FForceFeedbackChannelType::RIGHT_SMALL:
			DeviceState.ForceFeedback.RightSmall = Value;
			break;
		}

		DeviceState.ForceFeedback.bDirty = true;
	}
}

void FIndependentInputDevice::SetChannelValues(int ControllerId, const FForceFeedbackValues& Values)
{
	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	FPlatformUserId UserId = PLATFORMUSERID_NONE;
	FInputDeviceId DeviceId = INPUTDEVICEID_NONE;
	DeviceMapper.RemapControllerIdToPlatformUserAndDevice(ControllerId, UserId, DeviceId);
	if (!UserId.IsValid())
		return;

	for (TPair<FInputDeviceInstanceId, FJoystickDeviceState>& DeviceStatePair : DeviceStates)
	{
		FJoystickDeviceState& DeviceState = DeviceStatePair.Value;
		if (DeviceMapper.GetUserForInputDevice(DeviceState.InputDeviceId) != UserId)
			continue;

		FForceFeedbackState& ForceFeedback = DeviceState.ForceFeedback;
		ForceFeedback.LeftLarge = FMath::Clamp(Values.LeftLarge, 0.0f, 1.0f);
		ForceFeedback.LeftSmall = FMath::Clamp(Values.LeftSmall, 0.0f, 1.0f);
		ForceFeedback.RightLarge = FMath::Clamp(Values.RightLarge, 0.0f, 1.0f);
		ForceFeedback.RightSmall = FMath::Clamp(Values.RightSmall, 0.0f, 1.0f);

		ForceFeedback.bDirty = true;
	}
}

void FIndependentInputDevice::DevicePluggedIn(FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice, const FJoystickDeviceKeyMapping& DeviceMapping)
{
	if (!DeviceInfo.IsValid())
		return;

	if (!DeviceMapping.bUseIndependentInputAPI)
		return;

	FJoystickDeviceState NewState = CreateDeviceState(DeviceInfo, DeviceMapping);
	DeviceInfos.Add(DeviceInfo.InstanceId, DeviceInfo);
	SDLDevices.Add(DeviceInfo.InstanceId, SDLDevice);
	DeviceMappings.Add(DeviceInfo.InstanceId, DeviceMapping);

#if PLATFORM_WINDOWS
	if (SDLDevice.bIsDualSense)
	{
		NewState.DualSense = MakeShared<FDualSenseWindows>();

		const bool bWasSuccess = NewState.DualSense->Open(
			SDL_GetJoystickVendor(SDLDevice.Joystick),
			SDL_GetJoystickProduct(SDLDevice.Joystick),
			DeviceInfo.SerialNumber);

		if (bWasSuccess)
			NewState.DualSense->SetTriggerEffectsEnable();
	}
#endif

	DeviceStates.Add(DeviceInfo.InstanceId, NewState);

	const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get();
	for (const TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : DeviceMapping.SensorMappings)
	{
		if (SensorMapping.Key == EDeviceSensorType::None)
			continue;

		bool bEnabled = false;
		switch (SensorMapping.Key)
		{
			case EDeviceSensorType::Accelerometer:
			case EDeviceSensorType::LeftAccelerometer:
			case EDeviceSensorType::RightAccelerometer:
				if (InputManagerSettings)
					bEnabled = InputManagerSettings->GetAccelerometerSensorEnabled();
			break;
		
			case EDeviceSensorType::Gyroscope:
			case EDeviceSensorType::LeftGyroscope:
			case EDeviceSensorType::RightGyroscope:
				if (InputManagerSettings)
					bEnabled = InputManagerSettings->GetGyroscopeSensorEnabled();
			break;
		}

		SDL_SetGamepadSensorEnabled(SDLDevice.Gamepad, FSDLInputUtils::ConvertSensorType(SensorMapping.Key), bEnabled);
	}
}

void FIndependentInputDevice::DeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo)
{
	if (!DeviceInfos.Contains(DeviceInfo.InstanceId))
		return;

	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	const FInputDeviceId& InputDeviceId = DeviceStates[DeviceInfo.InstanceId].InputDeviceId;
	const FPlatformUserId NewUserToAssign = DeviceMapper.GetUserForUnpairedInputDevices();
	DeviceMapper.Internal_MapInputDeviceToUser(InputDeviceId, NewUserToAssign, EInputDeviceConnectionState::Disconnected);

	DeviceInfos.Remove(DeviceInfo.InstanceId);
	DeviceMappings.Remove(DeviceInfo.InstanceId);
	DeviceStates.Remove(DeviceInfo.InstanceId);
	SDLDevices.Remove(DeviceInfo.InstanceId);
}

void FIndependentInputDevice::HandleButtonEvent(const FInputDeviceInstanceId& DeviceId, const int32 Button, const bool bPressed)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FButtonState* ButtonState = DeviceState->Buttons.Find(Button);
	if (!ButtonState)
		return;

	ButtonState->Update(bPressed);
}

void FIndependentInputDevice::HandleAxisEvent(const FInputDeviceInstanceId& DeviceId, const int32 Axis, const float Value)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FAxisState* AxisState = DeviceState->Axes.Find(Axis);
	if (!AxisState)
		return;

	AxisState->Update(Value);
	UpdateVirtualButtons(AxisState);
}

void FIndependentInputDevice::HandleHatEvent(const FInputDeviceInstanceId& DeviceId, const int32 Hat, const uint8 Value)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FHatState* HatState = DeviceState->Hats.Find(Hat);
	if (!HatState)
		return;

	HatState->Update(Value);
}

void FIndependentInputDevice::HandleBallEvent(const FInputDeviceInstanceId& DeviceId, const int32 Ball, const float XRel, const float YRel)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FBallState* BallState = DeviceState->Balls.Find(Ball);
	if (!BallState)
		return;

	BallState->Accumulate(XRel, YRel);
}

void FIndependentInputDevice::HandleTouchpadEvent(const FInputDeviceInstanceId& DeviceId, int32 Touchpad, int32 Finger, bool bTouched, float X, float Y, float Pressure)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FTouchpadState* TouchpadState = DeviceState->Touchpads.Find(Touchpad);
	if (!TouchpadState)
		return;

	if (!TouchpadState->FingersState.IsValidIndex(Finger))
		return;

	TouchpadState->FingersState[Finger].Update(bTouched, X, Y, Pressure);
}

void FIndependentInputDevice::HandleSensorEvent(const FInputDeviceInstanceId& DeviceId, const EDeviceSensorType SensorType, const FVector& Value)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FSensorState* SensorState = DeviceState->Sensors.Find(SensorType);
	if (!SensorState)
		return;

	SensorState->Update(Value);
}

bool FIndependentInputDevice::SetAdaptiveTriggerResistance(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 Force)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;
	
	return DeviceState->DualSense->SetAdaptiveTriggerResistance(Trigger, StartPosition, Force);
#else
	return false;
#endif
}

bool FIndependentInputDevice::SetAdaptiveTriggerWeapon(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;

	return DeviceState->DualSense->SetAdaptiveTriggerWeapon(Trigger, StartPosition, EndPosition, Strength);
#else
	return false;
#endif
}

bool FIndependentInputDevice::SetAdaptiveTriggerVibration(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;

	return DeviceState->DualSense->SetAdaptiveTriggerVibration(Trigger, Position, Amplitude, Frequency);
#else
	return false;
#endif
}

bool FIndependentInputDevice::SetAdaptiveTriggerBow(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;

	return DeviceState->DualSense->SetAdaptiveTriggerBow(Trigger, StartPosition, EndPosition, Strength, SnapForce);
#else
	return false;
#endif
}

bool FIndependentInputDevice::SetAdaptiveTriggerGalloping(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;

	return DeviceState->DualSense->SetAdaptiveTriggerGalloping(Trigger, StartPosition, EndPosition, FirstFoot, SecondFoot, Frequency);
#else
	return false;
#endif
}

bool FIndependentInputDevice::SetAdaptiveTriggerMachine(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	if (!DeviceState->AdaptiveTriggerEffect.IsEnabled())
		return false;

	return DeviceState->DualSense->SetAdaptiveTriggerMachine(Trigger, StartPosition, EndPosition, AmplitudeA, AmplitudeB, Frequency, Period);
#else
	return false;
#endif
}

bool FIndependentInputDevice::ClearAdaptiveTriggerEffect(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger)
{
#if PLATFORM_WINDOWS
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return false;

	if (!DeviceState->DualSense)
		return false;

	return DeviceState->DualSense->ClearAdaptiveTriggerEffect(Trigger);
#else
	return false;
#endif
}

bool FIndependentInputDevice::GetButtonState(const FInputDeviceInstanceId& DeviceId, const int32 Button) const
{
	if (!DeviceStates.Contains(DeviceId))
		return false;

	if (!DeviceStates[DeviceId].Buttons.Contains(Button))
		return false;

	return DeviceStates[DeviceId].Buttons[Button].GetValue();
}

float FIndependentInputDevice::GetAxisState(const FInputDeviceInstanceId& DeviceId, const int32 Axis) const
{
	if (!DeviceStates.Contains(DeviceId))
		return 0.0f;

	if (!DeviceStates[DeviceId].Axes.Contains(Axis))
		return 0.0f;

	return DeviceStates[DeviceId].Axes[Axis].GetValue();
}

float FIndependentInputDevice::GetAxisRawState(const FInputDeviceInstanceId& DeviceId, const int32 Axis) const
{
	if (!DeviceStates.Contains(DeviceId))
		return 0.0f;

	if (!DeviceStates[DeviceId].Axes.Contains(Axis))
		return 0.0f;

	return DeviceStates[DeviceId].Axes[Axis].RawValue;
}

bool FIndependentInputDevice::GetAxisVirtualButtonState(const FInputDeviceInstanceId& DeviceId, const int32 Axis, const int32 VirtualButton) const
{
	if (!DeviceStates.Contains(DeviceId))
		return false;

	if (!DeviceStates[DeviceId].Axes.Contains(Axis))
		return false;

	if (!DeviceStates[DeviceId].Axes[Axis].VirtualButtons.IsValidIndex(VirtualButton))
		return false;

	return DeviceStates[DeviceId].Axes[Axis].VirtualButtons[VirtualButton].ButtonState.GetValue();
}

uint8 FIndependentInputDevice::GetHatState(const FInputDeviceInstanceId& DeviceId, const int32 Hat) const
{
	if (!DeviceStates.Contains(DeviceId))
		return 0;

	if (!DeviceStates[DeviceId].Hats.Contains(Hat))
		return 0;

	return DeviceStates[DeviceId].Hats[Hat].Value;
}

FVector2D FIndependentInputDevice::GetBallState(const FInputDeviceInstanceId& DeviceId, const int32 Ball) const
{
	if (!DeviceStates.Contains(DeviceId))
		return FVector2D::ZeroVector;

	if (!DeviceStates[DeviceId].Balls.Contains(Ball))
		return FVector2D::ZeroVector;

	return DeviceStates[DeviceId].Balls[Ball].GetOutputValue();
}

FTouchFingerState FIndependentInputDevice::GetTouchpadFingerState(const FInputDeviceInstanceId& DeviceId, const int32 TouchpadIndex, const int32 FingerIndex) const
{
	if (!DeviceStates.Contains(DeviceId))
		return FTouchFingerState();

	if (!DeviceStates[DeviceId].Touchpads.Contains(TouchpadIndex))
		return FTouchFingerState();

	if (!DeviceStates[DeviceId].Touchpads[TouchpadIndex].FingersState.IsValidIndex(FingerIndex))
		return FTouchFingerState();

	return DeviceStates[DeviceId].Touchpads[TouchpadIndex].FingersState[FingerIndex];
}

FSensorState FIndependentInputDevice::GetSensorState(const FInputDeviceInstanceId& DeviceId, const EDeviceSensorType SensorType) const
{
	if (!DeviceStates.Contains(DeviceId))
		return FSensorState();

	if (!DeviceStates[DeviceId].Sensors.Contains(SensorType))
		return FSensorState();

	return DeviceStates[DeviceId].Sensors[SensorType];
}

void FIndependentInputDevice::SetAccelerometerSensorEnable(bool bEnable)
{
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& DeviceInfo : DeviceInfos)
	{
		const FJoystickDeviceKeyMapping* DeviceMapping = DeviceMappings.Find(DeviceInfo.Key);
		if (!DeviceMapping)
			continue;

		FSDLJoystickDevice* SDLDevice = SDLDevices.Find(DeviceInfo.Key);
		if (!SDLDevice || !SDLDevice->Gamepad)
			continue;

		for (const TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : DeviceMapping->SensorMappings)
		{
			switch (SensorMapping.Key)
			{
				case EDeviceSensorType::Accelerometer:
				case EDeviceSensorType::LeftAccelerometer:
				case EDeviceSensorType::RightAccelerometer:
					SDL_SetGamepadSensorEnabled(SDLDevice->Gamepad, FSDLInputUtils::ConvertSensorType(SensorMapping.Key), bEnable);
				break;
			}
		}
	}

	if (bEnable)
		return;

	// Reset sensor states in case of disabling.
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceState>& DeviceState : DeviceStates)
	{
		for (const TPair<EDeviceSensorType, FSensorState>& SensorState : DeviceState.Value.Sensors)
		{
			switch (SensorState.Key)
			{
				case EDeviceSensorType::Accelerometer:
				case EDeviceSensorType::LeftAccelerometer:
				case EDeviceSensorType::RightAccelerometer:
					ResetSensorState(DeviceState.Key, SensorState.Key);
				break;
			}
		}
	}
}

void FIndependentInputDevice::SetGyroscopeSensorEnable(bool bEnable)
{
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& DeviceInfo : DeviceInfos)
	{
		const FJoystickDeviceKeyMapping* DeviceMapping = DeviceMappings.Find(DeviceInfo.Key);
		if (!DeviceMapping)
			continue;

		FSDLJoystickDevice* SDLDevice = SDLDevices.Find(DeviceInfo.Key);
		if (!SDLDevice || !SDLDevice->Gamepad)
			continue;

		for (const TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : DeviceMapping->SensorMappings)
		{
			switch (SensorMapping.Key)
			{
				case EDeviceSensorType::Gyroscope:
				case EDeviceSensorType::LeftGyroscope:
				case EDeviceSensorType::RightGyroscope:
					SDL_SetGamepadSensorEnabled(SDLDevice->Gamepad, FSDLInputUtils::ConvertSensorType(SensorMapping.Key), bEnable);
				break;
			}
		}
	}

	if (bEnable)
		return;

	// Reset sensor states in case of disabling.
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceState>& DeviceState : DeviceStates)
	{
		for (const TPair<EDeviceSensorType, FSensorState>& SensorState : DeviceState.Value.Sensors)
		{
			switch (SensorState.Key)
			{
				case EDeviceSensorType::Gyroscope:
				case EDeviceSensorType::LeftGyroscope:
				case EDeviceSensorType::RightGyroscope:
					ResetSensorState(DeviceState.Key, SensorState.Key);
				break;
			}
		}
	}
}

FString FIndependentInputDevice::GetDeviceHardwareDeviceIdentifier(const FJoystickDeviceInfo& DeviceInfo) const
{
	EJoystickDeviceType DeviceType = DeviceInfo.Type;
	const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get();
	bool bUseDeviceName = InputManagerSettings->GetUseDeviceNameAsHardwareDeviceIdentifier();

	switch (DeviceType)
	{
	case EJoystickDeviceType::Gamepad:
		return FSDLInputUtils::GetDeviceHardwareIdentifierFromGamepadType(DeviceInfo.GamepadType, DeviceInfo.DeviceName);

	case EJoystickDeviceType::Wheel:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("SteeringWheel");
		
	case EJoystickDeviceType::ArcadeStick:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("ArcadeStick");

	case EJoystickDeviceType::FlightStick:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("FlightStick");

	case EJoystickDeviceType::DancePad:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("DancePad");

	case EJoystickDeviceType::Guitar:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("Guitar");

	case EJoystickDeviceType::DrumKit:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("DrumKit");

	case EJoystickDeviceType::ArcadePad:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("ArcadePad");

	case EJoystickDeviceType::Throttle:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("Pedal");

	case EJoystickDeviceType::Max:
	case EJoystickDeviceType::Unknown:
	default:
		return bUseDeviceName ? DeviceInfo.DeviceName : FString("GenericJoystick");
	}
}

FJoystickDeviceState FIndependentInputDevice::CreateDeviceState(FJoystickDeviceInfo& DeviceInfo, const FJoystickDeviceKeyMapping& InKeyMapping)
{
	const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get();
	FJoystickDeviceState State;
	IPlatformInputDeviceMapper& DeviceMapper = IPlatformInputDeviceMapper::Get();
	DeviceInfo.InputDeviceId = InternalDeviceIdMappings.GetOrCreateDeviceId(DeviceInfo.InstanceId);
	DeviceInfo.PlatformUserId = InputManagerSettings->GetForceDevicesForSingleUser()
		? DeviceMapper.GetPrimaryPlatformUser()
		: DeviceMapper.GetPlatformUserForNewlyConnectedDevice();

	State.InputDeviceId = DeviceInfo.InputDeviceId;
	State.PlatformUserId = DeviceInfo.PlatformUserId;
	DeviceMapper.Internal_MapInputDeviceToUser(State.InputDeviceId, State.PlatformUserId, EInputDeviceConnectionState::Connected);
	State.Buttons.Reserve(InKeyMapping.ButtonMappings.Num());
	State.Axes.Reserve(InKeyMapping.AxisMappings.Num());
	State.Balls.Reserve(InKeyMapping.BallMappings.Num());
	State.Hats.Reserve(InKeyMapping.HatMappings.Num());
	State.Touchpads.Reserve(InKeyMapping.TouchpadMappings.Num());
	State.Rumble = InKeyMapping.Rumble;
	State.TriggerRumble = InKeyMapping.TriggerRumble;
	State.AdaptiveTriggerEffect = InKeyMapping.AdaptiveTriggerEffect;

	for (const TPair<int32, FJoystickButtonKeyMapping>& ButtonMapping : InKeyMapping.ButtonMappings)
	{
		State.Buttons.Add(ButtonMapping.Key, FButtonState(ButtonMapping.Value.Key.GetKey()));
	}
	
	for (const TPair<int32, FJoystickAxisKeyMapping>& AxisKeyMapping : InKeyMapping.AxisMappings)
	{
		const FJoystickAxisKeyMapping& AxisMapping = AxisKeyMapping.Value;
		FAxisState AxisState(AxisMapping.Key.GetKey(), AxisMapping.DeadZone, AxisMapping.DeadZoneCenter, AxisMapping.bRemap, AxisMapping.InputRange, AxisMapping.OutputRange);
		for (const FAxisVirtualButtonKeyMapping& VirtualButtonMapping : AxisKeyMapping.Value.VirtualButtons)
		{
			AxisState.VirtualButtons.Add(FAxisVirtualButtonState(VirtualButtonMapping));
		}

		State.Axes.Add(AxisKeyMapping.Key, MoveTemp(AxisState));
	}

	for (const TPair<int32, FJoystickHatKeyMapping>& HatKeyMapping : InKeyMapping.HatMappings)
	{
		const FJoystickHatKeyMapping& Mapping = HatKeyMapping.Value;

		FHatState HatState;
		HatState.Up = FButtonState(Mapping.Up.GetKey());
		HatState.Down = FButtonState(Mapping.Down.GetKey());
		HatState.Left = FButtonState(Mapping.Left.GetKey());
		HatState.Right = FButtonState(Mapping.Right.GetKey());

		State.Hats.Add(HatKeyMapping.Key, MoveTemp(HatState));
	}

	for (const TPair<int32, FJoystickBallKeyMapping>& BallKeyMapping : InKeyMapping.BallMappings)
	{
		State.Balls.Add(BallKeyMapping.Key, FBallState(BallKeyMapping.Value));
	}

	for (const TPair<int32, FJoystickTouchpadKeyMapping>& TouchpadPair : InKeyMapping.TouchpadMappings)
	{
		FTouchpadState TouchpadState;

		for (const FJoystickTouchpadFingerKeyMapping& FingerMapping : TouchpadPair.Value.Fingers)
		{
			TouchpadState.FingersState.Emplace(
				FingerMapping.Touch.GetKey(),
				FingerMapping.PositionX.GetKey(),
				FingerMapping.PositionY.GetKey(),
				FingerMapping.Pressure.GetKey());
		}

		State.Touchpads.Add(TouchpadPair.Key, MoveTemp(TouchpadState));
	}

	for (const TPair<EDeviceSensorType, FJoystickSensorKeyMapping>& SensorMapping : InKeyMapping.SensorMappings)
	{
		EDeviceSensorType Type = SensorMapping.Key;
		switch (Type)
		{
		case EDeviceSensorType::Accelerometer:
		case EDeviceSensorType::Gyroscope:
			State.Sensors.Add(SensorMapping.Key, FSensorState());
			break;

		case EDeviceSensorType::LeftAccelerometer:
		case EDeviceSensorType::LeftGyroscope:
		case EDeviceSensorType::RightAccelerometer:
		case EDeviceSensorType::RightGyroscope:
			State.Sensors.Add(SensorMapping.Key, FSensorState(SensorMapping.Value.X.GetKey(), SensorMapping.Value.Y.GetKey(), SensorMapping.Value.Z.GetKey()));
			break;
		}
	}
	
	return State;
}

void FIndependentInputDevice::UpdateVirtualButtons(FAxisState* AxisState)
{
	for (FAxisVirtualButtonState& VirtualButton : AxisState->VirtualButtons)
	{
		const bool bPressed = VirtualButton.EvalutateIsPressed(AxisState->Value);
		VirtualButton.ButtonState.Update(bPressed);
	}
}

void FIndependentInputDevice::HandleButtonState(FButtonState& ButtonState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	if (!ButtonState.Key.IsValid())
	{
		ButtonState.Commit();
		return;
	}

	const double CurrentTime = FPlatformTime::Seconds();
	const FName KeyName = ButtonState.Key.GetFName();
	bool bPressed = false;

	while (ButtonState.PopPendingTransition(bPressed))
	{
		if (bPressed)
		{
			// Button Pressed.
			MessageHandler->OnControllerButtonPressed(KeyName, PlatformUser, DeviceId, false);
			ButtonState.SetNextRepeatTime(CurrentTime + FMath::Max(0.0, InitialButtonRepeatDelay));
		}
		else
		{
			// Button Released.
			MessageHandler->OnControllerButtonReleased(KeyName, PlatformUser, DeviceId, false);
		}
	}

	if (ButtonState.GetValue() && CurrentTime >= ButtonState.GetNextRepeatTime())
	{
		// Button Repeating.
		MessageHandler->OnControllerButtonPressed(KeyName, PlatformUser, DeviceId, true);
		ButtonState.SetNextRepeatTime(CurrentTime + FMath::Max(0.0, ButtonRepeatDelay));
	}
}

void FIndependentInputDevice::HandleAxisState(FAxisState& AxisState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	const bool bAxisChanged = AxisState.HasChanged();
	if (AxisState.Key.IsValid() && bAxisChanged)
	{
		MessageHandler->OnControllerAnalog(AxisState.Key.GetFName(), PlatformUser, DeviceId, AxisState.GetValue());
	}
	
	// Process virtual buttons generated from this axis.
	for (FAxisVirtualButtonState& VirtualButton : AxisState.VirtualButtons)
	{
		HandleButtonState(VirtualButton.ButtonState, PlatformUser, DeviceId);
	}

	if (bAxisChanged)
		AxisState.Commit();
}

void FIndependentInputDevice::HandleHatState(FHatState& HatState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	HandleButtonState(HatState.Up, PlatformUser, DeviceId);
	HandleButtonState(HatState.Down, PlatformUser, DeviceId);
	HandleButtonState(HatState.Left, PlatformUser, DeviceId);
	HandleButtonState(HatState.Right, PlatformUser, DeviceId);

	HatState.Commit();
}

void FIndependentInputDevice::HandleBallState(FBallState& BallState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	const bool bHasPendingInput = BallState.HasPendingInput();
	const FVector2D OutputValue = BallState.ConsumeOutputValue();
	if (bHasPendingInput)
	{
		if (BallState.X.Key.IsValid())
		{
			MessageHandler->OnControllerAnalog(BallState.X.Key.GetFName(), PlatformUser, DeviceId, OutputValue.X);
		}

		if (BallState.Y.Key.IsValid())
		{
			MessageHandler->OnControllerAnalog(BallState.Y.Key.GetFName(), PlatformUser, DeviceId, OutputValue.Y);
		}
	}
}

void FIndependentInputDevice::HandleTouchFingerState(FTouchFingerState& FingerState, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	HandleButtonState(FingerState.Touch, PlatformUser, DeviceId);
	HandleAxisState(FingerState.X, PlatformUser, DeviceId);
	HandleAxisState(FingerState.Y, PlatformUser, DeviceId);
	HandleAxisState(FingerState.Pressure, PlatformUser, DeviceId);
}

void FIndependentInputDevice::HandleSensorState(TMap<EDeviceSensorType, FSensorState>& Sensors, const FPlatformUserId& PlatformUser, const FInputDeviceId& DeviceId)
{
	if (Sensors.IsEmpty())
		return;

	FVector Accelerometer = FVector::ZeroVector;
	FVector Gyroscope = FVector::ZeroVector;

	for (TPair<EDeviceSensorType, FSensorState>& Sensor : Sensors)
	{
		switch (Sensor.Key)
		{
			case EDeviceSensorType::Accelerometer:
			{
				if (Sensor.Value.HasChanged())
				{
					Accelerometer = Sensor.Value.Value;
					if (Sensors.Contains(EDeviceSensorType::Gyroscope))
					{
						Gyroscope = Sensors[EDeviceSensorType::Gyroscope].Value;
						Sensors[EDeviceSensorType::Gyroscope].Commit();
					}

					MessageHandler->OnMotionDetected(FVector::ZeroVector, Gyroscope, FVector::ZeroVector, Accelerometer, PlatformUser, DeviceId);
					Sensor.Value.Commit();
				}
				break;
			}

			case EDeviceSensorType::Gyroscope:
			{
				if (Sensor.Value.HasChanged())
				{
					Gyroscope = Sensor.Value.Value;
					if (Sensors.Contains(EDeviceSensorType::Accelerometer))
					{
						Accelerometer = Sensors[EDeviceSensorType::Accelerometer].Value;
						Sensors[EDeviceSensorType::Accelerometer].Commit();
					}

					MessageHandler->OnMotionDetected(FVector::ZeroVector, Gyroscope, FVector::ZeroVector, Accelerometer, PlatformUser, DeviceId);
					Sensor.Value.Commit();
				}
				break;
			}

			case EDeviceSensorType::LeftAccelerometer:
			case EDeviceSensorType::LeftGyroscope:
			case EDeviceSensorType::RightAccelerometer:
			case EDeviceSensorType::RightGyroscope:
			{
				HandleAxisState(Sensor.Value.X, PlatformUser, DeviceId);
				HandleAxisState(Sensor.Value.Y, PlatformUser, DeviceId);
				HandleAxisState(Sensor.Value.Z, PlatformUser, DeviceId);
				break;
			}
		}
	}
}

void FIndependentInputDevice::HandleForceFeedback(FForceFeedbackState& ForceFeedbackState, const FInputDeviceInstanceId& DeviceId)
{
	if (!ForceFeedbackState.bDirty)
		return;

	if (const FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId))
	{
		if (!DeviceState->Rumble.IsEnabled())
			return;
	}

	const float Low = FMath::Max(ForceFeedbackState.LeftLarge, ForceFeedbackState.LeftSmall);
	const float High = FMath::Max(ForceFeedbackState.RightLarge, ForceFeedbackState.RightSmall);

	if (FMath::IsNearlyEqual(Low, ForceFeedbackState.LastLowFrequency) &&
		FMath::IsNearlyEqual(High, ForceFeedbackState.LastHighFrequency))
	{
		ForceFeedbackState.bDirty = false;
		return;
	}

	FSDLJoystickDevice SDLDevice;

	if (!SDLDevices.Contains(DeviceId))
		return;

	SDLDevice = SDLDevices[DeviceId];

	const Uint16 LowMotor = static_cast<Uint16>(Low * 65535.0f);
	const Uint16 HighMotor = static_cast<Uint16>(High * 65535.0f);

	if (SDLDevice.bIsGamepad)
		SDL_RumbleGamepad(SDLDevice.Gamepad, LowMotor, HighMotor, 100);
	else if (SDLDevice.Joystick)
		SDL_RumbleJoystick(SDLDevice.Joystick, LowMotor, HighMotor, 100);

	ForceFeedbackState.LastLowFrequency = Low;
	ForceFeedbackState.LastHighFrequency = High;
	ForceFeedbackState.bDirty = false;
}

void FIndependentInputDevice::ResetSensorState(const FInputDeviceInstanceId& DeviceId, EDeviceSensorType SensorType)
{
	FJoystickDeviceState* DeviceState = DeviceStates.Find(DeviceId);
	if (!DeviceState)
		return;

	FSensorState* SensorState = DeviceState->Sensors.Find(SensorType);
	if (!SensorState)
		return;

	SensorState->Update(FVector::ZeroVector);
}

#undef LOCTEXT_NAMESPACE
