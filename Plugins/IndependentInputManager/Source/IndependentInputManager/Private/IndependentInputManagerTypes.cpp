// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManagerTypes.h"

#include "IndependentInputManagerSettings.h"
#include "IndependentInputSubsystem.h"

#define LOCTEXT_NAMESPACE "IndependentInputManagerTypes"

const FKey FIndependentInputKeys::Gamepad_Guide("Gamepad_Guide");
const FKey FIndependentInputKeys::Gamepad_Misc1("Gamepad_Misc1");
const FKey FIndependentInputKeys::Gamepad_Misc2("Gamepad_Misc2");
const FKey FIndependentInputKeys::Gamepad_Misc3("Gamepad_Misc3");
const FKey FIndependentInputKeys::Gamepad_Misc4("Gamepad_Misc4");
const FKey FIndependentInputKeys::Gamepad_Misc5("Gamepad_Misc5");
const FKey FIndependentInputKeys::Gamepad_Misc6("Gamepad_Misc6");
const FKey FIndependentInputKeys::Gamepad_RightPaddle1("Gamepad_RightPaddle1");
const FKey FIndependentInputKeys::Gamepad_RightPaddle2("Gamepad_RightPaddle2");
const FKey FIndependentInputKeys::Gamepad_LeftPaddle1("Gamepad_LeftPaddle1");
const FKey FIndependentInputKeys::Gamepad_LeftPaddle2("Gamepad_LeftPaddle2");

const FKey FIndependentInputKeys::Gamepad_Touchpad("Gamepad_Touchpad");
const FKey FIndependentInputKeys::Gamepad_Touchpad2("Gamepad_Touchpad2");

const FKey FIndependentInputKeys::Gamepad_LeftAccelerometer_X("Gamepad_LeftAccelerometer_X");
const FKey FIndependentInputKeys::Gamepad_LeftAccelerometer_Y("Gamepad_LeftAccelerometer_Y");
const FKey FIndependentInputKeys::Gamepad_LeftAccelerometer_Z("Gamepad_LeftAccelerometer_Z");

const FKey FIndependentInputKeys::Gamepad_RightAccelerometer_X("Gamepad_RightAccelerometer_X");
const FKey FIndependentInputKeys::Gamepad_RightAccelerometer_Y("Gamepad_RightAccelerometer_Y");
const FKey FIndependentInputKeys::Gamepad_RightAccelerometer_Z("Gamepad_RightAccelerometer_Z");

const FKey FIndependentInputKeys::Gamepad_LeftGyroscope_X("Gamepad_LeftGyroscope_X");
const FKey FIndependentInputKeys::Gamepad_LeftGyroscope_Y("Gamepad_LeftGyroscope_Y");
const FKey FIndependentInputKeys::Gamepad_LeftGyroscope_Z("Gamepad_LeftGyroscope_Z");

const FKey FIndependentInputKeys::Gamepad_RightGyroscope_X("Gamepad_RightGyroscope_X");
const FKey FIndependentInputKeys::Gamepad_RightGyroscope_Y("Gamepad_RightGyroscope_Y");
const FKey FIndependentInputKeys::Gamepad_RightGyroscope_Z("Gamepad_RightGyroscope_Z");

const FName FIndependentGamepadKeyNames::Guide("Gamepad_Guide");
const FName FIndependentGamepadKeyNames::Misc1("Gamepad_Misc1");
const FName FIndependentGamepadKeyNames::Misc2("Gamepad_Misc2");
const FName FIndependentGamepadKeyNames::Misc3("Gamepad_Misc3");
const FName FIndependentGamepadKeyNames::Misc4("Gamepad_Misc4");
const FName FIndependentGamepadKeyNames::Misc5("Gamepad_Misc5");
const FName FIndependentGamepadKeyNames::Misc6("Gamepad_Misc6");
const FName FIndependentGamepadKeyNames::RightPaddle1("Gamepad_RightPaddle1");
const FName FIndependentGamepadKeyNames::RightPaddle2("Gamepad_RightPaddle2");
const FName FIndependentGamepadKeyNames::LeftPaddle1("Gamepad_LeftPaddle1");
const FName FIndependentGamepadKeyNames::LeftPaddle2("Gamepad_LeftPaddle2");

const FName FIndependentGamepadKeyNames::Touchpad("Gamepad_Touchpad");
const FName FIndependentGamepadKeyNames::Touchpad2("Gamepad_Touchpad2");

const FName FIndependentGamepadKeyNames::LeftAccelerometer_X("Gamepad_LeftAccelerometer_X");
const FName FIndependentGamepadKeyNames::LeftAccelerometer_Y("Gamepad_LeftAccelerometer_Y");
const FName FIndependentGamepadKeyNames::LeftAccelerometer_Z("Gamepad_LeftAccelerometer_Z");

const FName FIndependentGamepadKeyNames::RightAccelerometer_X("Gamepad_RightAccelerometer_X");
const FName FIndependentGamepadKeyNames::RightAccelerometer_Y("Gamepad_RightAccelerometer_Y");
const FName FIndependentGamepadKeyNames::RightAccelerometer_Z("Gamepad_RightAccelerometer_Z");

const FName FIndependentGamepadKeyNames::LeftGyroscope_X("Gamepad_LeftGyroscope_X");
const FName FIndependentGamepadKeyNames::LeftGyroscope_Y("Gamepad_LeftGyroscope_Y");
const FName FIndependentGamepadKeyNames::LeftGyroscope_Z("Gamepad_LeftGyroscope_Z");

const FName FIndependentGamepadKeyNames::RightGyroscope_X("Gamepad_RightGyroscope_X");
const FName FIndependentGamepadKeyNames::RightGyroscope_Y("Gamepad_RightGyroscope_Y");
const FName FIndependentGamepadKeyNames::RightGyroscope_Z("Gamepad_RightGyroscope_Z");


void FIndependentInputKey::GenerateRuntimeKeyIfNeeded(const FJoystickDeviceKeyMapping& DeviceKeyMapping)
{
	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
	{
		if (bCustomKey)
			RuntimeKey = InputSubsystem->CreateDeviceKey(DeviceKeyMapping, *this);
	}
}

void FSDLInputUtils::RegisterDefaultKeys()
{
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Guide, LOCTEXT("Gamepad_Guide", "Gamepad Guide"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc1, LOCTEXT("Gamepad_Misc1", "Gamepad Misc 1"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc2, LOCTEXT("Gamepad_Misc2", "Gamepad Misc 2"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc3, LOCTEXT("Gamepad_Misc3", "Gamepad Misc 3"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc4, LOCTEXT("Gamepad_Misc4", "Gamepad Misc 4"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc5, LOCTEXT("Gamepad_Misc5", "Gamepad Misc 5"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Misc6, LOCTEXT("Gamepad_Misc6", "Gamepad Misc 6"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightPaddle1, LOCTEXT("Gamepad_RightPaddle1", "Gamepad Right Paddle 1"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightPaddle2, LOCTEXT("Gamepad_RightPaddle2", "Gamepad Right Paddle 2"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftPaddle1, LOCTEXT("Gamepad_LeftPaddle1", "Gamepad Left Paddle 1"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftPaddle2, LOCTEXT("Gamepad_LeftPaddle2", "Gamepad Left Paddle 2"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Touchpad, LOCTEXT("Gamepad_Touchpad", "Gamepad Touchpad Button"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_Touchpad2, LOCTEXT("Gamepad_Touchpad2", "Gamepad Touchpad (2) Button"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftAccelerometer_X, LOCTEXT("Gamepad_LeftAccelerometer_X", "Gamepad Accelerometer (L) X-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftAccelerometer_Y, LOCTEXT("Gamepad_LeftAccelerometer_Y", "Gamepad Accelerometer (L) Y-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftAccelerometer_Z, LOCTEXT("Gamepad_LeftAccelerometer_Z", "Gamepad Accelerometer (L) Z-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightAccelerometer_X, LOCTEXT("Gamepad_RightAccelerometer_X", "Gamepad Accelerometer (R) X-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightAccelerometer_Y, LOCTEXT("Gamepad_RightAccelerometer_Y", "Gamepad Accelerometer (R) Y-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightAccelerometer_Z, LOCTEXT("Gamepad_RightAccelerometer_Z", "Gamepad Accelerometer (R) Z-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));

	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftGyroscope_X, LOCTEXT("Gamepad_LeftGyroscope_X", "Gamepad Gyroscope (L) X-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftGyroscope_Y, LOCTEXT("Gamepad_LeftGyroscope_Y", "Gamepad Gyroscope (L) Y-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_LeftGyroscope_Z, LOCTEXT("Gamepad_LeftGyroscope_Z", "Gamepad Gyroscope (L) Z-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));

	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightGyroscope_X, LOCTEXT("Gamepad_RightGyroscope_X", "Gamepad Gyroscope (R) X-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightGyroscope_Y, LOCTEXT("Gamepad_RightGyroscope_Y", "Gamepad Gyroscope (R) Y-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	EKeys::AddKey(FKeyDetails(FIndependentInputKeys::Gamepad_RightGyroscope_Z, LOCTEXT("Gamepad_RightGyroscope_Z", "Gamepad Gyroscope (R) Z-Axis"), FKeyDetails::GamepadKey | FKeyDetails::Axis1D));
	//EKeys::AddMenuCategoryDisplayInfo("GamepadTouch", LOCTEXT("TouchSubCateogry", "Gamepad Touch"), TEXT("GraphEditor.TouchEvent_16x"));

}

EJoystickDeviceType FSDLInputUtils::ConvertJoystickType(SDL_JoystickType InType)
{
	switch (InType)
	{
		case SDL_JOYSTICK_TYPE_UNKNOWN: return EJoystickDeviceType::Unknown;
		case SDL_JOYSTICK_TYPE_GAMEPAD: return EJoystickDeviceType::Gamepad;
		case SDL_JOYSTICK_TYPE_WHEEL: return EJoystickDeviceType::Wheel;
		case SDL_JOYSTICK_TYPE_ARCADE_STICK: return EJoystickDeviceType::ArcadeStick;
		case SDL_JOYSTICK_TYPE_FLIGHT_STICK: return EJoystickDeviceType::FlightStick;
		case SDL_JOYSTICK_TYPE_DANCE_PAD: return EJoystickDeviceType::DancePad;
		case SDL_JOYSTICK_TYPE_GUITAR: return EJoystickDeviceType::Guitar;
		case SDL_JOYSTICK_TYPE_DRUM_KIT: return EJoystickDeviceType::DrumKit;
		case SDL_JOYSTICK_TYPE_ARCADE_PAD: return EJoystickDeviceType::ArcadePad;
		case SDL_JOYSTICK_TYPE_THROTTLE: return EJoystickDeviceType::Throttle;
		case SDL_JOYSTICK_TYPE_COUNT: return EJoystickDeviceType::Max;
		default: return EJoystickDeviceType::Unknown;
	}
}

EGamepadType FSDLInputUtils::ConvertGamepadType(SDL_GamepadType InType)
{
	switch (InType)
	{
		case SDL_GAMEPAD_TYPE_UNKNOWN: return EGamepadType::Unknown;
		case SDL_GAMEPAD_TYPE_STANDARD: return EGamepadType::Standard;
		case SDL_GAMEPAD_TYPE_XBOX360: return EGamepadType::Xbox360;
		case SDL_GAMEPAD_TYPE_XBOXONE: return EGamepadType::XboxOne;
		case SDL_GAMEPAD_TYPE_PS3: return EGamepadType::PS3;
		case SDL_GAMEPAD_TYPE_PS4: return EGamepadType::PS4;
		case SDL_GAMEPAD_TYPE_PS5: return EGamepadType::PS5;
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: return EGamepadType::SwitchPro;
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT: return EGamepadType::SwitchJoyconLeft;
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT: return EGamepadType::SwitchJoyconRight;
		case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return EGamepadType::SwitchJoyconPair;
		case SDL_GAMEPAD_TYPE_GAMECUBE: return EGamepadType::GameCube;
		case SDL_GAMEPAD_TYPE_COUNT: return EGamepadType::Max;
		default: return EGamepadType::Unknown;
	}
}

EDeviceConnectionType FSDLInputUtils::ConvertConnectionType(SDL_JoystickConnectionState InState)
{
	switch (InState)
	{
		case SDL_JOYSTICK_CONNECTION_INVALID: return EDeviceConnectionType::Unknown;
		case SDL_JOYSTICK_CONNECTION_UNKNOWN: return EDeviceConnectionType::Unknown;
		case SDL_JOYSTICK_CONNECTION_WIRED: return EDeviceConnectionType::Wired;
		case SDL_JOYSTICK_CONNECTION_WIRELESS: return EDeviceConnectionType::Wireless;
		default: return EDeviceConnectionType::Unknown;
	}
}

FString FSDLInputUtils::GetDeviceHardwareIdentifierFromGamepadType(const EGamepadType GamepadType, const FString& InDeviceName)
{
	const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get();
	bool bUseDeviceName = InputManagerSettings->GetUseDeviceNameAsHardwareDeviceIdentifier();

	switch (GamepadType)
	{
	case EGamepadType::Xbox360:
	case EGamepadType::XboxOne:
		return "XInputController";

	case EGamepadType::PS3:
		return "DualShock3";

	case EGamepadType::PS4:
		return "DualShock4";

	case EGamepadType::PS5:
		return "DualSense";

	case EGamepadType::SwitchPro:
		return "SwitchPro";

	case EGamepadType::SwitchJoyconLeft:
	case EGamepadType::SwitchJoyconRight:
	case EGamepadType::SwitchJoyconPair:
		return "JoyCon";

	case EGamepadType::GameCube:
		return "GameCube";

	case EGamepadType::Standard:
	case EGamepadType::Max:
	default:
		return bUseDeviceName ? InDeviceName : "GenericGamepad";
	}
}

FString FSDLInputUtils::GetDeviceShortNameFromGamepadType(const EGamepadType GamepadType, FString DeviceName)
{
	switch (GamepadType)
	{
	case EGamepadType::Xbox360:
		return "Xbox360";

	case EGamepadType::XboxOne:
		return "XboxOne";

	case EGamepadType::PS3:
		return "DualShock3";

	case EGamepadType::PS4:
		return "DualShock4";

	case EGamepadType::PS5:
		return "DualSense";

	case EGamepadType::SwitchPro:
		return "SwitchPro";

	case EGamepadType::SwitchJoyconLeft:
		return "JoyCon Left";

	case EGamepadType::SwitchJoyconRight:
		return "JoyCon Right";

	case EGamepadType::SwitchJoyconPair:
		return "JoyCon Paired";

	case EGamepadType::GameCube:
		return "GameCube";

	case EGamepadType::Standard:

	case EGamepadType::Max:
	default:
		DeviceName.ReplaceInline(TEXT(" Controller"), TEXT(""));
		DeviceName.ReplaceInline(TEXT("Controller"), TEXT(""));
		DeviceName.ReplaceInline(TEXT(" Wireless"), TEXT(""));
		DeviceName.ReplaceInline(TEXT("Wireless"), TEXT(""));
		return DeviceName;
	}
}

FString FSDLInputUtils::GetDeviceShortNameFromDeviceName(FString DeviceName)
{
	DeviceName.ReplaceInline(TEXT(" Controller"), TEXT(""));
	DeviceName.ReplaceInline(TEXT("Controller"), TEXT(""));
	DeviceName.ReplaceInline(TEXT(" Wireless"), TEXT(""));
	DeviceName.ReplaceInline(TEXT("Wireless"), TEXT(""));
	return DeviceName;
}

EDeviceBatteryState FSDLInputUtils::ConvertBatteryState(SDL_PowerState InState)
{
	switch (InState)
	{
		case SDL_POWERSTATE_ERROR: return EDeviceBatteryState::Error;
		case SDL_POWERSTATE_UNKNOWN: return EDeviceBatteryState::Unknown;
		case SDL_POWERSTATE_ON_BATTERY: return EDeviceBatteryState::OnBattery;
		case SDL_POWERSTATE_NO_BATTERY: return EDeviceBatteryState::NoBattery;
		case SDL_POWERSTATE_CHARGING: return EDeviceBatteryState::Charging;
		case SDL_POWERSTATE_CHARGED: return EDeviceBatteryState::Charged;
		default: return EDeviceBatteryState::Unknown;
	}
}

FKey FSDLInputUtils::ConvertSDLButtonToKey(SDL_GamepadButton InButton)
{
	switch (InButton)
	{
		case SDL_GAMEPAD_BUTTON_INVALID: return FKey();
		case SDL_GAMEPAD_BUTTON_SOUTH: return FKey(EKeys::Gamepad_FaceButton_Bottom);
		case SDL_GAMEPAD_BUTTON_EAST: return FKey(EKeys::Gamepad_FaceButton_Right);
		case SDL_GAMEPAD_BUTTON_WEST: return FKey(EKeys::Gamepad_FaceButton_Left);
		case SDL_GAMEPAD_BUTTON_NORTH: return FKey(EKeys::Gamepad_FaceButton_Top);
		case SDL_GAMEPAD_BUTTON_BACK: return FKey(EKeys::Gamepad_Special_Left);
		case SDL_GAMEPAD_BUTTON_GUIDE: return FKey(FIndependentInputKeys::Gamepad_Guide);
		case SDL_GAMEPAD_BUTTON_START: return FKey(EKeys::Gamepad_Special_Right);
		case SDL_GAMEPAD_BUTTON_LEFT_STICK: return FKey(EKeys::Gamepad_LeftThumbstick);
		case SDL_GAMEPAD_BUTTON_RIGHT_STICK: return FKey(EKeys::Gamepad_RightThumbstick);
		case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER: return FKey(EKeys::Gamepad_LeftShoulder);
		case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: return FKey(EKeys::Gamepad_RightShoulder);
		case SDL_GAMEPAD_BUTTON_DPAD_UP: return FKey(EKeys::Gamepad_DPad_Up);
		case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return FKey(EKeys::Gamepad_DPad_Down);
		case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return FKey(EKeys::Gamepad_DPad_Left);
		case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return FKey(EKeys::Gamepad_DPad_Right);
		case SDL_GAMEPAD_BUTTON_MISC1: return FKey(FIndependentInputKeys::Gamepad_Misc1);
		case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1: return FKey(FIndependentInputKeys::Gamepad_RightPaddle1);
		case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1: return FKey(FIndependentInputKeys::Gamepad_LeftPaddle1);
		case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2: return FKey(FIndependentInputKeys::Gamepad_RightPaddle2);
		case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2: return FKey(FIndependentInputKeys::Gamepad_LeftPaddle2);
		case SDL_GAMEPAD_BUTTON_TOUCHPAD: return FKey(FIndependentInputKeys::Gamepad_Touchpad);
		case SDL_GAMEPAD_BUTTON_MISC2: return FKey(FIndependentInputKeys::Gamepad_Misc2);
		case SDL_GAMEPAD_BUTTON_MISC3: return FKey(FIndependentInputKeys::Gamepad_Misc3);
		case SDL_GAMEPAD_BUTTON_MISC4: return FKey(FIndependentInputKeys::Gamepad_Misc4);
		case SDL_GAMEPAD_BUTTON_MISC5: return FKey(FIndependentInputKeys::Gamepad_Misc5);
		case SDL_GAMEPAD_BUTTON_MISC6: return FKey(FIndependentInputKeys::Gamepad_Misc6);
		case SDL_GAMEPAD_BUTTON_COUNT: return FKey();
		default: return FKey();
	}
}

FKey FSDLInputUtils::ConvertSDLAxisToKey(SDL_GamepadAxis InAxis)
{
	switch (InAxis)
	{
		case SDL_GAMEPAD_AXIS_INVALID: return FKey();
		case SDL_GAMEPAD_AXIS_LEFTX: return FKey(EKeys::Gamepad_LeftX);
		case SDL_GAMEPAD_AXIS_LEFTY: return FKey(EKeys::Gamepad_LeftY);
		case SDL_GAMEPAD_AXIS_RIGHTX: return FKey(EKeys::Gamepad_RightX);
		case SDL_GAMEPAD_AXIS_RIGHTY: return FKey(EKeys::Gamepad_RightY);
		case SDL_GAMEPAD_AXIS_LEFT_TRIGGER: return FKey(EKeys::Gamepad_LeftTriggerAxis);
		case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER: return FKey(EKeys::Gamepad_RightTriggerAxis);
		case SDL_GAMEPAD_AXIS_COUNT: return FKey();
		default: return FKey();
	}
}

void FSDLInputUtils::PopulateSupportedSensors(SDL_Gamepad* Gamepad, FJoystickDeviceInfo& DeviceInfo)
{
	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_ACCEL))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Accelerometer);
	}

	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_GYRO))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Gyroscope);
	}

	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_ACCEL_L))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftAccelerometer);
	}

	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_GYRO_L))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftGyroscope);
	}

	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_ACCEL_R))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightAccelerometer);
	}

	if (SDL_GamepadHasSensor(Gamepad, SDL_SENSOR_GYRO_R))
	{
		AddFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightGyroscope);
	}
}

void FSDLInputUtils::PopulateSupportedFeatures(SDL_Joystick* Joystick, FJoystickDeviceInfo& DeviceInfo)
{
	if (SDL_PropertiesID Properties = SDL_GetJoystickProperties(Joystick))
	{
		if (SDL_GetBooleanProperty(Properties, SDL_PROP_JOYSTICK_CAP_MONO_LED_BOOLEAN, false))
		{
			AddFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::MonoLED);
		}

		if (SDL_GetBooleanProperty(Properties, SDL_PROP_JOYSTICK_CAP_RGB_LED_BOOLEAN, false))
		{
			AddFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::RGBLED);
		}

		if (SDL_GetBooleanProperty(Properties, SDL_PROP_JOYSTICK_CAP_PLAYER_LED_BOOLEAN, false))
		{
			AddFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::PlayerLED);
		}

		if (SDL_GetBooleanProperty(Properties, SDL_PROP_JOYSTICK_CAP_RUMBLE_BOOLEAN, false))
		{
			AddFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::Rumble);
		}

		if (SDL_GetBooleanProperty(Properties, SDL_PROP_JOYSTICK_CAP_TRIGGER_RUMBLE_BOOLEAN, false))
		{
			AddFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::TriggerRumble);
		}
	}
}

EDeviceSensorType FSDLInputUtils::ConvertSensorType(Sint32 InType)
{
	switch (InType)
	{
	case SDL_SENSOR_ACCEL:
		return EDeviceSensorType::Accelerometer;
		
	case SDL_SENSOR_GYRO:
		return EDeviceSensorType::Gyroscope;

	case SDL_SENSOR_ACCEL_L:
		return EDeviceSensorType::LeftAccelerometer;
		
	case SDL_SENSOR_GYRO_L:
		return EDeviceSensorType::LeftGyroscope;
		
	case SDL_SENSOR_ACCEL_R:
		return EDeviceSensorType::RightAccelerometer;

	case SDL_SENSOR_GYRO_R:
		return EDeviceSensorType::RightGyroscope;
		
	case SDL_SENSOR_INVALID:
	case SDL_SENSOR_UNKNOWN:
	case SDL_SENSOR_COUNT:
	default:
		return EDeviceSensorType::None;
	}
}

SDL_SensorType FSDLInputUtils::ConvertSensorType(EDeviceSensorType InType)
{
	switch (InType)
	{
	case EDeviceSensorType::None: return SDL_SENSOR_UNKNOWN;
	case EDeviceSensorType::Accelerometer: return SDL_SENSOR_ACCEL;
	case EDeviceSensorType::Gyroscope: return SDL_SENSOR_GYRO;
	case EDeviceSensorType::LeftAccelerometer: return SDL_SENSOR_ACCEL_L;
	case EDeviceSensorType::LeftGyroscope: return SDL_SENSOR_GYRO_L;
	case EDeviceSensorType::RightAccelerometer: return SDL_SENSOR_ACCEL_R;
	case EDeviceSensorType::RightGyroscope: return SDL_SENSOR_GYRO_R;
	default: return SDL_SENSOR_INVALID;
	}
}

FName FSDLInputUtils::SanitizeDisplayName(const FString& DisplayName)
{
	FString DummyName = DisplayName;
	DummyName.ReplaceInline(TEXT("X-Axis"), TEXT("X"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT("Y-Axis"), TEXT("Y"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT(" : "), TEXT("_"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT(": "), TEXT("_"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT(" :"), TEXT("_"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT(":"), TEXT("_"), ESearchCase::IgnoreCase);
	DummyName.ReplaceInline(TEXT(" "), TEXT("_"), ESearchCase::IgnoreCase);
	return *DummyName;
}

bool FSDLInputUtils::IsDualSense(SDL_Joystick* Joystick)
{
	if (!Joystick)
	{
		return false;
	}

	constexpr uint16 SonyVendorId = 0x054C;
	const uint16 Vendor = SDL_GetJoystickVendor(Joystick);

	if (Vendor != SonyVendorId)
	{
		return false;
	}

	const uint16 Product = SDL_GetJoystickProduct(Joystick);

	switch (Product)
	{
	case 0x0CE6: // DualSense
	case 0x0DF2: // DualSense Edge
		return true;

	default:
		return false;
	}
}

#undef LOCTEXT_NAMESPACE