// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputSubsystem.h"

#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_hints.h"

#include "IndependentInputDevice.h"
#include "IndependentInputManager.h"
#include "IndependentInputManagerSettings.h"

#define LOCTEXT_NAMESPACE "UIndependentInputSubsystem"

namespace
{
	constexpr SDL_InitFlags IndependentInputSubsystemFlags =
		SDL_INIT_EVENTS
		| SDL_INIT_JOYSTICK
		| SDL_INIT_GAMEPAD
		| SDL_INIT_HAPTIC
		| SDL_INIT_SENSOR;
}

UIndependentInputSubsystem* UIndependentInputSubsystem::Get()
{
	if (GEngine)
		return GEngine->GetEngineSubsystem<UIndependentInputSubsystem>();

	return nullptr;
}

void UIndependentInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick));
}

void UIndependentInputSubsystem::Deinitialize()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}

	ShutdownSDL();
	UE_LOG(LogIndependentInput, Log, TEXT("IndependentInput Deinitialize"));

	Super::Deinitialize();
}

void UIndependentInputSubsystem::InitializeInputDevice(const TSharedPtr<FIndependentInputDevice>& InputDevice)
{
	IndependentInputDevice = InputDevice;
	UIndependentInputManagerSettings::GetMutable()->GenerateDevicesRuntimeKeys();

	InitSDL();
}

FKey UIndependentInputSubsystem::CreateDeviceKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& Key, bool bUpdateAxisWithoutSamples)
{
	if (!Key.bCustomKey)
		return Key.Key;

	const FString& DeviceName = DeviceMapping.DeviceName;
	const FString& MappingId = DeviceMapping.MappingId.ToString();

	FName CategoryName = FSDLInputUtils::SanitizeDisplayName(DeviceName);
	if (!RegisteredKeyCategories.Contains(CategoryName))
	{
		EKeys::AddMenuCategoryDisplayInfo(CategoryName, FText::FromString(DeviceName), TEXT("GraphEditor.PadEvent_16x"));
		RegisteredKeyCategories.Add(CategoryName);
	}

	FString KeyName = MappingId + Key.GetKeyName().ToString();
	const FKey NewKey = FKey(*KeyName);
	const TSharedPtr<FKeyDetails> ExistingKeyDetails = EKeys::GetKeyDetails(NewKey);
	if (!ExistingKeyDetails)
	{
		uint32 KeyFlags = FKeyDetails::GamepadKey;
		if (Key.bIsAxisKey)
			KeyFlags |= FKeyDetails::Axis1D;

		if (bUpdateAxisWithoutSamples)
			KeyFlags |= FKeyDetails::UpdateAxisWithoutSamples;
		
		FKeyDetails NewKeyDetails = FKeyDetails(NewKey, FText::FromString(Key.GetKeyDisplayName()), KeyFlags, CategoryName);
		EKeys::AddKey(NewKeyDetails);
	}
	
	return NewKey;
}

FKey UIndependentInputSubsystem::CreateDevicePairedKey(const FJoystickDeviceKeyMapping& DeviceMapping, const FIndependentInputKey& KeyX, const FIndependentInputKey& KeyY, bool bUpdateAxisWithoutSamples)
{
	const FString& DeviceName = DeviceMapping.DeviceName;
	const FString& MappingId = DeviceMapping.MappingId.ToString();
	FString PairedKeyDisplayName = KeyX.GetKeyDisplayName();
	FString PairedKeyName;
	
	if (PairedKeyDisplayName.EndsWith("X Delta"))
	{
		if (!PairedKeyDisplayName.RemoveFromEnd(" X Delta"))
			PairedKeyDisplayName.RemoveFromEnd("X Delta");

		PairedKeyDisplayName.Append(" Delta 2D");
		PairedKeyName = FSDLInputUtils::SanitizeDisplayName(PairedKeyDisplayName).ToString();
	}
	else if (PairedKeyDisplayName.EndsWith("X-Axis"))
	{
		if (!PairedKeyDisplayName.RemoveFromEnd(" X-Axis"))
			PairedKeyDisplayName.RemoveFromEnd("X-Axis");

		PairedKeyName = FSDLInputUtils::SanitizeDisplayName(PairedKeyDisplayName).ToString();
		PairedKeyName.Append("_2D");
		PairedKeyDisplayName.Append(" 2D-Axis");
	}
	else if (PairedKeyDisplayName.EndsWith("X"))
	{
		if (!PairedKeyDisplayName.RemoveFromEnd(" X"))
			PairedKeyDisplayName.RemoveFromEnd("X");

		PairedKeyDisplayName.Append(" 2D");
		PairedKeyName = FSDLInputUtils::SanitizeDisplayName(PairedKeyDisplayName).ToString();
	}
	else
	{
		PairedKeyDisplayName.Append(" 2D");
		PairedKeyName = FSDLInputUtils::SanitizeDisplayName(PairedKeyDisplayName).ToString();
	}
	
	FName CategoryName = FSDLInputUtils::SanitizeDisplayName(DeviceName);
	if (!RegisteredKeyCategories.Contains(CategoryName))
	{
		EKeys::AddMenuCategoryDisplayInfo(CategoryName, FText::FromString(DeviceName), TEXT("GraphEditor.PadEvent_16x"));
		RegisteredKeyCategories.Add(CategoryName);
	}

	FString KeyName = MappingId + PairedKeyName;
	const FKey NewKey = FKey(*KeyName);
	const TSharedPtr<FKeyDetails> ExistingKeyDetails = EKeys::GetKeyDetails(NewKey);
	if (!ExistingKeyDetails)
	{
		uint32 KeyFlags = FKeyDetails::GamepadKey | FKeyDetails::Axis2D;
		if (bUpdateAxisWithoutSamples)
			KeyFlags |= FKeyDetails::UpdateAxisWithoutSamples;

		FKeyDetails NewKeyDetails = FKeyDetails(NewKey, FText::FromString(PairedKeyDisplayName), KeyFlags, CategoryName);
		EKeys::AddPairedKey(NewKeyDetails, KeyX.GetKey(), KeyY.GetKey());
	}
	
	return NewKey;
}

FIndependentInputDevice* UIndependentInputSubsystem::GetInputDevice() const
{
	return IndependentInputDevice.Get();
}

const FJoystickDeviceInfo* UIndependentInputSubsystem::GetDeviceInfo(const FInputDeviceInstanceId& DeviceId) const
{
	return ConnectedDevices.Find(DeviceId);
}

const FSDLJoystickDevice* UIndependentInputSubsystem::GetSDLDeviceInfo(const FInputDeviceInstanceId& DeviceId) const
{
	return SDLDevices.Find(DeviceId);
}

const FJoystickDeviceInfo* UIndependentInputSubsystem::FindDeviceInfoByIdentifier(const FJoystickDeviceIdentifier& DeviceIdentifier) const
{
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& ConnectedDevice : ConnectedDevices)
	{
		if (ConnectedDevice.Value.Identifier == DeviceIdentifier)
		{
			return &ConnectedDevice.Value;
		}
	}
	return nullptr;
}

bool UIndependentInputSubsystem::IsDeviceConnected(FInputDeviceInstanceId DeviceId) const
{
	return ConnectedDevices.Contains(DeviceId);
}

FRotator UIndependentInputSubsystem::CalculateOrientationFromAccelerometer(const FVector& Accelerometer)
{
	if (Accelerometer.IsNearlyZero())
		return FRotator::ZeroRotator;

	const FVector Gravity = Accelerometer.GetSafeNormal();

	const float Roll =
		FMath::RadiansToDegrees(
			FMath::Atan2(
				Gravity.X,
				Gravity.Y));

	const float Pitch =
		FMath::RadiansToDegrees(
			FMath::Atan2(
				Gravity.Z,
				FMath::Sqrt(
					Gravity.X * Gravity.X +
					Gravity.Y * Gravity.Y)));

	return FRotator(Pitch, 0.f, Roll);
}

TArray<FJoystickDeviceInfo> UIndependentInputSubsystem::GetConnectedDevicesInfo() const
{
	TArray<FJoystickDeviceInfo> Infos;
	ConnectedDevices.GenerateValueArray(Infos);
	return Infos;
}

bool UIndependentInputSubsystem::PlayRumble(const FInputDeviceInstanceId& DeviceId, float LowFrequency, float HighFrequency, float Duration/* = 0.05f*/)
{
	if (Duration <= 0.0f)
		return false;

	FSDLJoystickDevice* SDLDevice = SDLDevices.Find(DeviceId);
	if (!SDLDevice)
		return false;

	// Clamp normalized values.
	LowFrequency = FMath::Clamp(LowFrequency, 0.0f, 1.0f);
	HighFrequency = FMath::Clamp(HighFrequency, 0.0f, 1.0f);

	const Uint16 LowMotor = static_cast<Uint16>(LowFrequency * static_cast<float>(UINT16_MAX));
	const Uint16 HighMotor = static_cast<Uint16>(HighFrequency * static_cast<float>(UINT16_MAX));
	const Uint32 DurationMS = static_cast<Uint32>(Duration * 1000.0f);

	bool bSuccess = false;
	if (SDLDevice->bIsGamepad)
	{
		bSuccess = SDL_RumbleGamepad(SDLDevice->Gamepad, LowMotor, HighMotor, DurationMS);
	}
	else if (SDLDevice->Joystick)
	{
		bSuccess = SDL_RumbleJoystick(SDLDevice->Joystick, LowMotor, HighMotor, DurationMS);
	}
	else
	{
		return false;
	}

	if (!bSuccess)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("Failed to start rumble for device %d: %s"), DeviceId.GetId(), UTF8_TO_TCHAR(SDL_GetError()));
		return false;
	}

	return true;
}

bool UIndependentInputSubsystem::PlayTriggerRumble(const FInputDeviceInstanceId& DeviceId, float LeftTrigger, float RightTrigger, float Duration)
{
	if (Duration <= 0.0f)
		return false;

	FSDLJoystickDevice* SDLDevice = SDLDevices.Find(DeviceId);
	if (!SDLDevice)
		return false;

	LeftTrigger = FMath::Clamp(LeftTrigger, 0.f, 1.f);
	RightTrigger = FMath::Clamp(RightTrigger, 0.f, 1.f);

	const Uint16 Left = static_cast<Uint16>(LeftTrigger * static_cast<float>(UINT16_MAX));
	const Uint16 Right = static_cast<Uint16>(RightTrigger * static_cast<float>(UINT16_MAX));
	const Uint32 DurationMS = static_cast<Uint32>(Duration * 1000.f);

	bool bSuccess = false;
	if (SDLDevice->bIsGamepad)
	{
		if (SDL_RumbleGamepadTriggers(SDLDevice->Gamepad, Left, Right, DurationMS))
			return true;
	}
	
	if (SDLDevice->Joystick)
	{
		bSuccess = SDL_RumbleJoystickTriggers(SDLDevice->Joystick, Left, Right, DurationMS);
	}
	else
	{
		return false;
	}

	if (!bSuccess)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("Failed to start rumble for device %d: %s"), DeviceId.GetId(), UTF8_TO_TCHAR(SDL_GetError()));
		return false;
	}

	return true;
}

bool UIndependentInputSubsystem::SupportsTriggerRumble(const FInputDeviceInstanceId& DeviceId)
{
	const FJoystickDeviceInfo* DeviceInfo = ConnectedDevices.Find(DeviceId);
	if (!DeviceInfo)
		return false;

	return HasFlag(DeviceInfo->SupportedFeatures, EJoystickProperties::TriggerRumble);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerResistance(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 Force)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerResistance(DeviceId, Trigger, StartPosition, Force);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerWeapon(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerWeapon(DeviceId, Trigger, StartPosition, EndPosition, Strength);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerVibration(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerVibration(DeviceId, Trigger, Position, Amplitude, Frequency);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerBow(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerBow(DeviceId, Trigger, StartPosition, EndPosition, Strength, SnapForce);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerGalloping(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerGalloping(DeviceId, Trigger, StartPosition, EndPosition, FirstFoot, SecondFoot, Frequency);
}

bool UIndependentInputSubsystem::SetAdaptiveTriggerMachine(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->SetAdaptiveTriggerMachine(DeviceId, Trigger, StartPosition, EndPosition, AmplitudeA, AmplitudeB, Frequency, Period);
}

bool UIndependentInputSubsystem::ClearAdaptiveTriggerEffect(const FInputDeviceInstanceId& DeviceId, EDualSenseTrigger Trigger)
{
	if (!IndependentInputDevice)
		return false;

	return IndependentInputDevice->ClearAdaptiveTriggerEffect(DeviceId, Trigger);
}

void UIndependentInputSubsystem::ReconnectDevice(const FJoystickDeviceIdentifier& DeviceIdentifier)
{
	TArray<FInputDeviceInstanceId> MatchingDeviceIds;
	for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& ConnectedDevice : ConnectedDevices)
	{
		if (ConnectedDevice.Value.Identifier == DeviceIdentifier)
			MatchingDeviceIds.Add(ConnectedDevice.Key);
	}

	for (const FInputDeviceInstanceId& DeviceId : MatchingDeviceIds)
	{
		UnregisterDevice(DeviceId);
		RegisterDevice(DeviceId);
	}
}

bool UIndependentInputSubsystem::Tick(float DeltaTime)
{
	if (!bInitialized)
		return true;

	PumpEvents();
	return true;
}

void UIndependentInputSubsystem::InitSDL()
{
	if (bInitialized)
		return;

	if (!SDL_InitSubSystem(IndependentInputSubsystemFlags))
	{
		UE_LOG(LogIndependentInput, Error, TEXT("SDL_InitSubSystem failed: %s"), UTF8_TO_TCHAR(SDL_GetError()));
		return;
	}

	bInitialized = true;
	UE_LOG(LogIndependentInput, Log, TEXT("IndependentInput Initialized"));
	UE_LOG(LogIndependentInput, Log, TEXT("SDL Version: %d.%d.%d"), SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
}

void UIndependentInputSubsystem::PumpEvents()
{
	if (!IndependentInputDevice)
		return;

	SDL_PumpEvents();

	SDL_Event Event;

	while (SDL_PeepEvents(
		&Event,
		1,
		SDL_GETEVENT,
		SDL_EVENT_JOYSTICK_AXIS_MOTION,
		SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED) > 0)
	{
		switch (Event.type)
		{
			case SDL_EVENT_JOYSTICK_ADDED:
			{
				RegisterDevice(Event.jdevice.which);
				break;
			}

			case SDL_EVENT_JOYSTICK_REMOVED:
			{
				UnregisterDevice(Event.jdevice.which);
				break;
			}

			case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			case SDL_EVENT_JOYSTICK_BUTTON_UP:
			{
				const FInputDeviceInstanceId DeviceId(Event.jbutton.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && !DeviceMapping->bUseGamepadAPI)
				{
					IndependentInputDevice->HandleButtonEvent(DeviceId, Event.jbutton.button, Event.jbutton.down);
				}
			
				break;
			}

			case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			{
				const FInputDeviceInstanceId DeviceId(Event.jaxis.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && !DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleAxisEvent(DeviceId, Event.jaxis.axis, Event.jaxis.value);
				}
			
				break;
			}

			case SDL_EVENT_JOYSTICK_HAT_MOTION:
			{
				const FInputDeviceInstanceId DeviceId(Event.jhat.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && !DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleHatEvent(DeviceId, Event.jhat.hat, Event.jhat.value);
				}

				break;
			}

			case SDL_EVENT_JOYSTICK_BALL_MOTION:
			{
				const FInputDeviceInstanceId DeviceId(Event.jball.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && IndependentInputDevice)
				{
					IndependentInputDevice->HandleBallEvent(
						DeviceId,
						Event.jball.ball,
						Event.jball.xrel,
						Event.jball.yrel);
				}

				break;
			}

			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
			{
				const FInputDeviceInstanceId DeviceId(Event.gbutton.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleButtonEvent(DeviceId, Event.gbutton.button, Event.gbutton.down);
				}

				break;
			}

			case SDL_EVENT_GAMEPAD_AXIS_MOTION:
			{
				const FInputDeviceInstanceId DeviceId(Event.gaxis.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleAxisEvent(DeviceId, Event.gaxis.axis, Event.gaxis.value);
				}
			
				break;
			}

			case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
			case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
			{
				const FInputDeviceInstanceId DeviceId(Event.gtouchpad.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleTouchpadEvent(
						DeviceId,
						Event.gtouchpad.touchpad,
						Event.gtouchpad.finger,
						true,
						Event.gtouchpad.x,
						Event.gtouchpad.y,
						Event.gtouchpad.pressure);
				}
				break;
			}

			case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
			{
				const FInputDeviceInstanceId DeviceId(Event.gtouchpad.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleTouchpadEvent(
						DeviceId,
						Event.gtouchpad.touchpad,
						Event.gtouchpad.finger,
						false,
						Event.gtouchpad.x,
						Event.gtouchpad.y,
						Event.gtouchpad.pressure);
				}
				break;
			}

			case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
			{
				const FInputDeviceInstanceId DeviceId(Event.gsensor.which);
				const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId);
				if (DeviceMapping && DeviceMapping->bUseGamepadAPI && IndependentInputDevice)
				{
					IndependentInputDevice->HandleSensorEvent(
						DeviceId,
						FSDLInputUtils::ConvertSensorType(Event.gsensor.sensor),
						FVector(Event.gsensor.data[0], Event.gsensor.data[1], Event.gsensor.data[2]));
				}

				break;
			}

			case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
			{
				HandleBatteryUpdated(Event.jdevice.which, FSDLInputUtils::ConvertBatteryState(Event.jbattery.state), Event.jbattery.percent);
				break;
			}

			default: break;
		}
	}
}

void UIndependentInputSubsystem::ShutdownSDL()
{
	if (!bInitialized)
		return;

	while (SDLDevices.Num() > 0)
	{
		UnregisterDevice(SDLDevices.CreateConstIterator()->Key, true);
	}

	IgnoredDeviceIds.Reset();
	SDL_QuitSubSystem(IndependentInputSubsystemFlags);
	bInitialized = false;
	UE_LOG(LogIndependentInput, Log, TEXT("IndependentInputManager Shutdown"));
}

void UIndependentInputSubsystem::CloseSDLDevice(FSDLJoystickDevice& SLDDevice, const FJoystickDeviceInfo& DeviceInfo, bool bFadeOutLED)
{
	if (SLDDevice.Haptic)
	{
		SDL_CloseHaptic(SLDDevice.Haptic);
		SLDDevice.Haptic = nullptr;
	}

	if (SLDDevice.Joystick)
	{
		bool bHasLED = HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::MonoLED)
			|| HasFlag(DeviceInfo.SupportedFeatures, EJoystickProperties::RGBLED);

		if (bFadeOutLED && bHasLED)
			FadeOutJoystickLED(SLDDevice.Joystick);

		SDL_RumbleJoystick(SLDDevice.Joystick, 0, 0, 0);
		SDL_SetJoystickPlayerIndex(SLDDevice.Joystick, -1);
	}

	if (SLDDevice.Gamepad)
	{
		SDL_RumbleGamepad(SLDDevice.Gamepad, 0, 0, 0);

		SDL_CloseGamepad(SLDDevice.Gamepad);
		SLDDevice.Gamepad = nullptr;
		SLDDevice.Joystick = nullptr;
	}
	else if (SLDDevice.Joystick)
	{
		SDL_CloseJoystick(SLDDevice.Joystick);
		SLDDevice.Joystick = nullptr;
	}
}

void UIndependentInputSubsystem::FadeOutJoystickLED(SDL_Joystick* Joystick)
{
	if (!Joystick)
		return;

	int32 LEDFadeSteps = 20;
	for (int32 Step = LEDFadeSteps; Step >= 0; --Step)
	{
		const float Alpha = static_cast<float>(Step) / LEDFadeSteps;

		SDL_SetJoystickLED(
			Joystick,
			static_cast<uint8>(32 * Alpha),
			static_cast<uint8>(32 * Alpha),
			static_cast<uint8>(32 * Alpha));

		SDL_Delay(10);
	}

	SDL_SetJoystickLED(Joystick, 0, 0, 0);
}

bool UIndependentInputSubsystem::RegisterDevice(SDL_JoystickID InstanceId)
{
	FInputDeviceInstanceId DeviceId(InstanceId);
	if (ConnectedDevices.Contains(DeviceId))
		return false;

	const bool bIsVirtualDevice = SDL_IsJoystickVirtual(InstanceId);
	if (UIndependentInputManagerSettings::Get()->GetIgnoreVirtualDevices() && bIsVirtualDevice)
	{
		IgnoredDeviceIds.Add(DeviceId);
		return false;
	}

	IgnoredDeviceIds.Remove(DeviceId);

	FSDLJoystickDevice SDLDevice;
	SDLDevice.bIsGamepad = SDL_IsGamepad(InstanceId);
	
	if (SDLDevice.bIsGamepad)
	{
		SDLDevice.Gamepad = SDL_OpenGamepad(InstanceId);
		if (SDLDevice.Gamepad)
		{
			SDLDevice.Joystick = SDL_GetGamepadJoystick(SDLDevice.Gamepad);
		}
		else
		{
		UE_LOG(LogIndependentInput, Warning,
				TEXT("SDL recognized '%d' as a gamepad but SDL_OpenGamepad failed: %s"),
				(int32)InstanceId, UTF8_TO_TCHAR(SDL_GetError()));

			SDLDevice.bIsGamepad = false;
			SDLDevice.Joystick = SDL_OpenJoystick(InstanceId);
		}
	}
	else
	{
		SDLDevice.Joystick = SDL_OpenJoystick(InstanceId);
	}

	if (!SDLDevice.Joystick)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("Failed to open joystick: %s"), UTF8_TO_TCHAR(SDL_GetError()));
		return false;
	}

	FJoystickDeviceInfo DeviceInfo;
	DeviceInfo.InstanceId = DeviceId;
	DeviceInfo.Identifier.VendorId = SDL_GetJoystickVendor(SDLDevice.Joystick);
	DeviceInfo.Identifier.ProductId = SDL_GetJoystickProduct(SDLDevice.Joystick);

	if (!DeviceInfo.Identifier.IsValid())
	{
		CloseSDLDevice(SDLDevice, DeviceInfo, false);
		return false;
	}

	SDLDevice.bIsDualSense = FSDLInputUtils::IsDualSense(SDLDevice.Joystick);
	SDLDevice.InstanceId = DeviceId;
	DeviceInfo.bIsVirtualDevice = bIsVirtualDevice;

	if (SDLDevice.bIsGamepad)
	{
		if (const char* Name = SDL_GetGamepadName(SDLDevice.Gamepad))
			DeviceInfo.DeviceName = UTF8_TO_TCHAR(Name);
		
		DeviceInfo.GamepadType = FSDLInputUtils::ConvertGamepadType(SDL_GetGamepadType(SDLDevice.Gamepad));
		DeviceInfo.ShortDeviceName = FSDLInputUtils::GetDeviceShortNameFromGamepadType(DeviceInfo.GamepadType, DeviceInfo.DeviceName);
		FSDLInputUtils::PopulateSupportedSensors(SDLDevice.Gamepad, DeviceInfo);

		int32 NumOfTouchpads = SDL_GetNumGamepadTouchpads(SDLDevice.Gamepad);
		for (int32 i = 0; i < NumOfTouchpads; i++)
		{
			int32 NumOfTouchpadFingers = SDL_GetNumGamepadTouchpadFingers(SDLDevice.Gamepad, i);
			DeviceInfo.Touchpads.Add(FTouchpadInfo(i, NumOfTouchpadFingers));
		}
	}
	else
	{
		if (const char* Name = SDL_GetJoystickName(SDLDevice.Joystick))
			DeviceInfo.DeviceName = UTF8_TO_TCHAR(Name);

		DeviceInfo.ShortDeviceName = FSDLInputUtils::GetDeviceShortNameFromDeviceName(DeviceInfo.DeviceName);
	}
	
	DeviceInfo.FirmwareVersion = SDL_GetJoystickFirmwareVersion(SDLDevice.Joystick);
	DeviceInfo.ProductVersion = SDL_GetJoystickProductVersion(SDLDevice.Joystick);
	DeviceInfo.UpdateProfileName();
	FSDLInputUtils::PopulateSupportedFeatures(SDLDevice.Joystick, DeviceInfo);

	if (const char* Serial = SDL_GetJoystickSerial(SDLDevice.Joystick))
		DeviceInfo.SerialNumber = UTF8_TO_TCHAR(Serial);

	DeviceInfo.Type = FSDLInputUtils::ConvertJoystickType(SDL_GetJoystickType(SDLDevice.Joystick));
	DeviceInfo.ConnectionType = FSDLInputUtils::ConvertConnectionType(SDL_GetJoystickConnectionState(SDLDevice.Joystick));
	
	int BatteryPercent = -1;
	if (SDLDevice.bIsGamepad)
		DeviceInfo.BatteryState = FSDLInputUtils::ConvertBatteryState(SDL_GetGamepadPowerInfo(SDLDevice.Gamepad, &BatteryPercent));
	else
		DeviceInfo.BatteryState = FSDLInputUtils::ConvertBatteryState(SDL_GetJoystickPowerInfo(SDLDevice.Joystick, &BatteryPercent));

	DeviceInfo.BatteryPercent = BatteryPercent;
	DeviceInfo.PlayerIndex = SDL_GetJoystickPlayerIndex(SDLDevice.Joystick);

	DeviceInfo.NumberOfButtons = SDL_GetNumJoystickButtons(SDLDevice.Joystick);
	DeviceInfo.NumberOfAxis = SDL_GetNumJoystickAxes(SDLDevice.Joystick);
	DeviceInfo.NumberOfHats = SDL_GetNumJoystickHats(SDLDevice.Joystick);
	DeviceInfo.NumberOfBalls = SDL_GetNumJoystickBalls(SDLDevice.Joystick);

	bool bCreatedNewDeviceMapping = false;
	CreateKeyMappingIfMissing(DeviceInfo, SDLDevice, bCreatedNewDeviceMapping);

	ConnectedDevices.Add(DeviceId, DeviceInfo);
	SDLDevices.Add(DeviceId, SDLDevice);

	UE_LOG(LogIndependentInput, Log, TEXT("Joystick Connected: %s"), *DeviceInfo.DeviceName);
	UE_LOG(LogIndependentInput, Log, TEXT("\tInstance Id: %d"), DeviceInfo.InstanceId.GetId());
	UE_LOG(LogIndependentInput, Log, TEXT("\tJoystick Type: %s"), *UEnum::GetDisplayValueAsText(DeviceInfo.Type).ToString());

	if (SDLDevice.bIsGamepad)
		UE_LOG(LogIndependentInput, Log, TEXT("\tGamepad Type: %s"), *UEnum::GetDisplayValueAsText(DeviceInfo.GamepadType).ToString());

	UE_LOG(LogIndependentInput, Log, TEXT("\tVendor Id: %d"), DeviceInfo.Identifier.VendorId);
	UE_LOG(LogIndependentInput, Log, TEXT("\tProduct Id: %d"), DeviceInfo.Identifier.ProductId);
	UE_LOG(LogIndependentInput, Log, TEXT("\tProfile: %s"), *DeviceInfo.MappingId.ToString());
	UE_LOG(LogIndependentInput, Log, TEXT("\tProduct Version: %d"), DeviceInfo.ProductVersion);
	UE_LOG(LogIndependentInput, Log, TEXT("\tFirmware Version: %d"), DeviceInfo.FirmwareVersion);
	UE_LOG(LogIndependentInput, Log, TEXT("\tSerial Number: %s"), *DeviceInfo.SerialNumber);
	UE_LOG(LogIndependentInput, Log, TEXT("\tNumber of Buttons: %d"), DeviceInfo.NumberOfButtons);
	UE_LOG(LogIndependentInput, Log, TEXT("\tNumber of Axis: %d"), DeviceInfo.NumberOfAxis);
	UE_LOG(LogIndependentInput, Log, TEXT("\tNumber of Hats: %d"), DeviceInfo.NumberOfHats);
	UE_LOG(LogIndependentInput, Log, TEXT("\tNumber of Balls: %d"), DeviceInfo.NumberOfBalls);
	UE_LOG(LogIndependentInput, Log, TEXT("\tPlayer Index: %d"), DeviceInfo.PlayerIndex);
	UE_LOG(LogIndependentInput, Log, TEXT("\tConnection Type: %s"), *UEnum::GetDisplayValueAsText(DeviceInfo.ConnectionType).ToString());
	UE_LOG(LogIndependentInput, Log, TEXT("\tBattery State: %s"), *UEnum::GetDisplayValueAsText(DeviceInfo.BatteryState).ToString());

	UIndependentInputManagerSettings::GetMutable()->DevicePluggedIn(DeviceInfo);
	if (const FJoystickDeviceKeyMapping* DeviceMapping = ConnectedDevicesMappings.Find(DeviceId))
		IndependentInputDevice->DevicePluggedIn(DeviceInfo, SDLDevice, *DeviceMapping);

	OnDevicePluggedIn.Broadcast(DeviceInfo, SDLDevice);

#if WITH_EDITORONLY_DATA
	if (bCreatedNewDeviceMapping)
		OnDevicePluggedInFirstTime.Broadcast(DeviceInfo, SDLDevice);
#endif // WITH_EDITORONLY_DATA

	return true;
}

void UIndependentInputSubsystem::UnregisterDevice(SDL_JoystickID InstanceId, bool bFadeOutLED)
{
	FInputDeviceInstanceId DeviceId(InstanceId);

	if (IgnoredDeviceIds.Remove(DeviceId) > 0)
		return;

	const FSDLJoystickDevice* FoundSDLDevice = SDLDevices.Find(DeviceId);
	if (!FoundSDLDevice)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("Failed to unregister device. SLDDevice not found: %d"), (int32)DeviceId);
		return;
	}

	const FJoystickDeviceInfo* FoundDeviceInfo = ConnectedDevices.Find(DeviceId);
	if (!FoundDeviceInfo)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("Failed to unregister device. DeviceInfo not found: %d"), (int32)DeviceId);
		return;
	}

	FSDLJoystickDevice SDLDevice = *FoundSDLDevice;
	FJoystickDeviceInfo DeviceInfo;
	DeviceInfo = *FoundDeviceInfo;
	UIndependentInputManagerSettings::GetMutable()->DeviceUnplugged(DeviceInfo);
	IndependentInputDevice->DeviceUnplugged(DeviceInfo);

	CloseSDLDevice(SDLDevice, DeviceInfo, bFadeOutLED);
	SDLDevices.Remove(DeviceId);
	ConnectedDevices.Remove(DeviceId);
	ConnectedDevicesMappings.Remove(DeviceId);
	OnDeviceUnplugged.Broadcast(DeviceInfo, SDLDevice);

	UE_LOG(LogIndependentInput, Log, TEXT("Joystick Removed: %s (%d)"), *DeviceInfo.DeviceName, (int32)DeviceId);
}

void UIndependentInputSubsystem::HandleBatteryUpdated(SDL_JoystickID InstanceId, EDeviceBatteryState InState, int32 InPercent)
{
	FInputDeviceInstanceId DeviceId(InstanceId);
	if (FJoystickDeviceInfo* DeviceInfo = ConnectedDevices.Find(DeviceId))
	{
		DeviceInfo->BatteryState = InState;
		DeviceInfo->BatteryPercent = InPercent;
		UIndependentInputManagerSettings::GetMutable()->UpdatePluggedDeviceInfo(*DeviceInfo);
		OnDeviceInfoUpdated.Broadcast(*DeviceInfo);
	}
}

void UIndependentInputSubsystem::CreateKeyMappingIfMissing(FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice, bool& bCreatedNewDeviceMapping)
{
	bCreatedNewDeviceMapping = false;
	if (!DeviceInfo.IsValid())
		return;

	if (DeviceInfo.MappingId.IsNone())
		return;

	UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::GetMutable();
	if (InputManagerSettings->HasDeviceKeyMapping(DeviceInfo.Identifier))
	{
		FJoystickDeviceKeyMapping DeviceMapping = *InputManagerSettings->FindDeviceKeyMappings(DeviceInfo.Identifier);
		ApplyInputOwnershipPolicy(DeviceInfo, DeviceMapping);

		if (DeviceMapping.bUseIndependentInputAPI)
			ConnectedDevicesMappings.Add(DeviceInfo.InstanceId, MoveTemp(DeviceMapping));

		return;
	}

	FJoystickDeviceKeyMapping DeviceKeyMapping(DeviceInfo.MappingId, DeviceInfo.DeviceName);
	ApplyInputOwnershipPolicy(DeviceInfo, DeviceKeyMapping);
	if (!DeviceKeyMapping.bUseIndependentInputAPI)
		return;
	
	
	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Accelerometer))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::Accelerometer, FJoystickSensorKeyMapping(EDeviceSensorType::Accelerometer, false));
	}

	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::Gyroscope))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::Gyroscope, FJoystickSensorKeyMapping(EDeviceSensorType::Gyroscope, false));
	}

	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftAccelerometer))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::LeftAccelerometer, FJoystickSensorKeyMapping(EDeviceSensorType::LeftAccelerometer, false));
	}

	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightAccelerometer))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::RightAccelerometer, FJoystickSensorKeyMapping(EDeviceSensorType::RightAccelerometer, false));
	}

	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::LeftGyroscope))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::LeftGyroscope, FJoystickSensorKeyMapping(EDeviceSensorType::LeftGyroscope, false));
	}

	if (HasFlag(DeviceInfo.SupportedSensors, EDeviceSensorType::RightGyroscope))
	{
		DeviceKeyMapping.SensorMappings.Add(EDeviceSensorType::RightGyroscope, FJoystickSensorKeyMapping(EDeviceSensorType::RightGyroscope, false));
	}

	for (int32 BallIndex = 0; BallIndex < DeviceInfo.NumberOfBalls; ++BallIndex)
	{
		FJoystickBallKeyMapping BallMapping;
		BallMapping.BallIndex = BallIndex;

		const FString KeyPrefix = FString::Printf(
			TEXT("%s: Ball %d"),
			*DeviceInfo.ShortDeviceName,
			BallIndex + 1);

		BallMapping.X.Key = FIndependentInputKey(KeyPrefix + TEXT(" X Delta"), DeviceInfo.MappingId.ToString(), true);
		BallMapping.Y.Key = FIndependentInputKey(KeyPrefix + TEXT(" Y Delta"), DeviceInfo.MappingId.ToString(), true);
		DeviceKeyMapping.BallMappings.Add(BallIndex, MoveTemp(BallMapping));
	}

	/*--------------------------------------------------------------------------
	*
	* Generate a default key mapping using SDL's Gamepad API.
	*
	* The Gamepad API provides a standardized layout, allowing known buttons
	* and axes to be mapped directly to Unreal Engine gamepad keys.
	*--------------------------------------------------------------------------*/
	if (SDLDevice.bIsGamepad)
	{
		int Count = 0;
		SDL_GamepadBinding** GamepadBindings = SDL_GetGamepadBindings(SDLDevice.Gamepad, &Count);

		if (GamepadBindings)
		{
			DeviceKeyMapping.bUseGamepadAPI = true;
			for (int i = 0; i < Count; i++)
			{
				if (SDL_GamepadBinding* Binding = GamepadBindings[i])
				{
					switch (Binding->output_type)
					{
						case SDL_GAMEPAD_BINDTYPE_BUTTON:
						{
							FJoystickButtonKeyMapping NewButtonMapping;
							NewButtonMapping.ButtonIndex = Binding->output.button;
							FKey ButtonKey = FSDLInputUtils::ConvertSDLButtonToKey(Binding->output.button);

							// If SDL does not expose a standardized Unreal key,
							// create a custom key based on the physical button index.
							NewButtonMapping.Key = ButtonKey.IsValid() ? FIndependentInputKey(ButtonKey)
								: FIndependentInputKey(FString::Printf(TEXT("%s: Button %d"), *DeviceInfo.ShortDeviceName, Binding->input.button), DeviceInfo.MappingId.ToString(), false);

							DeviceKeyMapping.ButtonMappings.Add(Binding->output.button, NewButtonMapping);
							break;
						}

						case SDL_GAMEPAD_BINDTYPE_AXIS:
						{
							FJoystickAxisKeyMapping NewAxisMapping;
							NewAxisMapping.AxisIndex = Binding->output.axis.axis;
							FKey AxisKey = FSDLInputUtils::ConvertSDLAxisToKey(Binding->output.axis.axis);

							// If SDL does not expose a standardized Unreal key,
							// create a custom key based on the physical axis index.
							NewAxisMapping.Key = AxisKey.IsValid() ? FIndependentInputKey(AxisKey)
								: FIndependentInputKey(FString::Printf(TEXT("%s: Axis %d"), *DeviceInfo.ShortDeviceName, Binding->input.axis.axis), DeviceInfo.MappingId.ToString(), true);

							NewAxisMapping.bRemap = true;
							NewAxisMapping.InputRange.Min = Binding->output.axis.axis_min;
							NewAxisMapping.InputRange.Max = Binding->output.axis.axis_max;
							NewAxisMapping.OutputRange.Min = float(NewAxisMapping.InputRange.Min) / float(SDL_JOYSTICK_AXIS_MIN) * FMath::Sign(NewAxisMapping.InputRange.Min);
							NewAxisMapping.OutputRange.Max = float(NewAxisMapping.InputRange.Max) / float(SDL_JOYSTICK_AXIS_MAX);

							if (!NewAxisMapping.Key.bCustomKey && NewAxisMapping.Key.Key.IsValid())
							{
								if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_LeftX)
								{
									FAxisVirtualButtonKeyMapping VirtualButtonLeft;
									VirtualButtonLeft.Condition = EAxisThresholdCondition::LessThan;
									VirtualButtonLeft.Threshold = -0.5f;
									VirtualButtonLeft.Key = FIndependentInputKey(FKey(EKeys::Gamepad_LeftStick_Left));

									FAxisVirtualButtonKeyMapping VirtualButtonRight;
									VirtualButtonRight.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButtonRight.Threshold = 0.5f;
									VirtualButtonRight.Key = FIndependentInputKey(FKey(EKeys::Gamepad_LeftStick_Right));

									NewAxisMapping.VirtualButtons.Add(VirtualButtonLeft);
									NewAxisMapping.VirtualButtons.Add(VirtualButtonRight);
								}
								else if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_LeftY)
								{
									FAxisVirtualButtonKeyMapping VirtualButtonDown;
									VirtualButtonDown.Condition = EAxisThresholdCondition::LessThan;
									VirtualButtonDown.Threshold = -0.5f;
									VirtualButtonDown.Key = FIndependentInputKey(FKey(EKeys::Gamepad_LeftStick_Down));

									FAxisVirtualButtonKeyMapping VirtualButtonUp;
									VirtualButtonUp.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButtonUp.Threshold = 0.5f;
									VirtualButtonUp.Key = FIndependentInputKey(FKey(EKeys::Gamepad_LeftStick_Up));

									NewAxisMapping.VirtualButtons.Add(VirtualButtonDown);
									NewAxisMapping.VirtualButtons.Add(VirtualButtonUp);

									// Swap Min/Max for Thumbstick Y-Axis
									Swap(NewAxisMapping.OutputRange.Min, NewAxisMapping.OutputRange.Max);
								}
								else if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_RightX)
								{
									FAxisVirtualButtonKeyMapping VirtualButtonLeft;
									VirtualButtonLeft.Condition = EAxisThresholdCondition::LessThan;
									VirtualButtonLeft.Threshold = -0.5f;
									VirtualButtonLeft.Key = FIndependentInputKey(FKey(EKeys::Gamepad_RightStick_Left));

									FAxisVirtualButtonKeyMapping VirtualButtonRight;
									VirtualButtonRight.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButtonRight.Threshold = 0.5f;
									VirtualButtonRight.Key = FIndependentInputKey(FKey(EKeys::Gamepad_RightStick_Right));

									NewAxisMapping.VirtualButtons.Add(VirtualButtonLeft);
									NewAxisMapping.VirtualButtons.Add(VirtualButtonRight);
								}
								else if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_RightY)
								{
									FAxisVirtualButtonKeyMapping VirtualButtonDown;
									VirtualButtonDown.Condition = EAxisThresholdCondition::LessThan;
									VirtualButtonDown.Threshold = -0.5f;
									VirtualButtonDown.Key = FIndependentInputKey(FKey(EKeys::Gamepad_RightStick_Down));

									FAxisVirtualButtonKeyMapping VirtualButtonUp;
									VirtualButtonUp.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButtonUp.Threshold = 0.5f;
									VirtualButtonUp.Key = FIndependentInputKey(FKey(EKeys::Gamepad_RightStick_Up));

									NewAxisMapping.VirtualButtons.Add(VirtualButtonDown);
									NewAxisMapping.VirtualButtons.Add(VirtualButtonUp);

									// Swap Min/Max for Thumbstick Y-Axis
									Swap(NewAxisMapping.OutputRange.Min, NewAxisMapping.OutputRange.Max);
								}
								else if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_LeftTriggerAxis)
								{
									FAxisVirtualButtonKeyMapping VirtualButton;
									VirtualButton.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButton.Threshold = 0.5f;
									VirtualButton.Key = FIndependentInputKey(FKey(EKeys::Gamepad_LeftTrigger));

									NewAxisMapping.VirtualButtons.Add(VirtualButton);
								}
								else if (NewAxisMapping.Key.Key.GetFName() == EKeys::Gamepad_RightTriggerAxis)
								{
									FAxisVirtualButtonKeyMapping VirtualButton;
									VirtualButton.Condition = EAxisThresholdCondition::GreaterThan;
									VirtualButton.Threshold = 0.5f;
									VirtualButton.Key = FIndependentInputKey(FKey(EKeys::Gamepad_RightTrigger));

									NewAxisMapping.VirtualButtons.Add(VirtualButton);
								}
							}

							DeviceKeyMapping.AxisMappings.Add(Binding->output.axis.axis, NewAxisMapping);
							break;
						}
					}
				}
			}

			for (const FTouchpadInfo& TouchpadInfo : DeviceInfo.Touchpads)
			{
				FJoystickTouchpadKeyMapping TouchpadMapping;
				TouchpadMapping.TouchpadIndex = TouchpadInfo.TouchpadIndex;
				
				for (int32 FingerIndex = 0; FingerIndex < TouchpadInfo.NumOfFingers; ++FingerIndex)
				{
					FJoystickTouchpadFingerKeyMapping FingerMapping;
					FingerMapping.FingerIndex = FingerIndex;
					
					const FString Prefix = FString::Printf(
						TEXT("%s: Touchpad %d Finger %d"),
						*DeviceInfo.ShortDeviceName,
						TouchpadInfo.TouchpadIndex + 1,
						FingerIndex + 1);

					FingerMapping.Touch = FIndependentInputKey(Prefix + TEXT(" Touch"), DeviceInfo.MappingId.ToString(), false);
					FingerMapping.PositionX = FIndependentInputKey(Prefix + TEXT(" X-Axis"), DeviceInfo.MappingId.ToString(), true);
					FingerMapping.PositionY = FIndependentInputKey(Prefix + TEXT(" Y-Axis"), DeviceInfo.MappingId.ToString(), true);
					FingerMapping.Pressure = FIndependentInputKey(Prefix + TEXT(" Pressure"), DeviceInfo.MappingId.ToString(), true);
					TouchpadMapping.Fingers.Add(MoveTemp(FingerMapping));
				}

				DeviceKeyMapping.TouchpadMappings.Add(TouchpadMapping.TouchpadIndex, MoveTemp(TouchpadMapping));
			}

			DeviceKeyMapping.ButtonMappings.KeySort([&](const int32& A, const int32& B)
				{
					return A < B;
				});

			DeviceKeyMapping.AxisMappings.KeySort([&](const int32& A, const int32& B)
				{
					return A < B;
				});

			SDL_free(GamepadBindings);

			// Add device key mapping for Gamepad device.
			InputManagerSettings->AddOrUpdateDeviceKeyMapping(DeviceInfo.Identifier, DeviceKeyMapping);
			ConnectedDevicesMappings.Add(DeviceInfo.InstanceId, DeviceKeyMapping);
			bCreatedNewDeviceMapping = true;
			UE_LOG(LogIndependentInput, Log, TEXT("Generated default gamepad key mapping [%s] for [%s]"), *DeviceInfo.MappingId.ToString(), *DeviceInfo.DeviceName);
			return;
		}
	}


	/*--------------------------------------------------------------------------
	* Fallback for devices that are not recognized as SDL Gamepads.
	*
	* Since no standardized layout is available, generate generic keys for
	* every button and axis using their physical indices.
	*--------------------------------------------------------------------------*/
	if (!SDLDevice.Joystick)
	{
		UE_LOG(LogIndependentInput, Error, TEXT("Tried to create device key mapping, invalid joystick for [%s]"), *DeviceInfo.DeviceName);
		return;
	}

	DeviceKeyMapping.bUseGamepadAPI = false;

	for (int32 i = 0; i < DeviceInfo.NumberOfButtons; i++)
	{
		FJoystickButtonKeyMapping ButtonMapping;
		ButtonMapping.ButtonIndex = i;
		ButtonMapping.Key = FIndependentInputKey(FString::Printf(TEXT("%s: Button %d"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), false);
		DeviceKeyMapping.ButtonMappings.Add(i, ButtonMapping);
	}

	for (int32 i = 0; i < DeviceInfo.NumberOfAxis; i++)
	{
		FJoystickAxisKeyMapping AxisMapping;
		AxisMapping.AxisIndex = i;
		AxisMapping.InputRange = FAxisValueRange(SDL_JOYSTICK_AXIS_MIN, SDL_JOYSTICK_AXIS_MAX);
		AxisMapping.OutputRange = FAxisValueRange(-1.0f, 1.0f);
		AxisMapping.Key = FIndependentInputKey(FString::Printf(TEXT("%s: Axis %d"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), true);
		DeviceKeyMapping.AxisMappings.Add(i, AxisMapping);
	}

	for (int32 i = 0; i < DeviceInfo.NumberOfHats; i++)
	{
		FJoystickHatKeyMapping HatMapping;
		HatMapping.HatIndex = i;
		HatMapping.Up = FIndependentInputKey(FString::Printf(TEXT("%s: Hat (%d) Up"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), false);
		HatMapping.Down = FIndependentInputKey(FString::Printf(TEXT("%s: Hat (%d) Down"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), false);
		HatMapping.Left = FIndependentInputKey(FString::Printf(TEXT("%s: Hat (%d) Left"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), false);
		HatMapping.Right = FIndependentInputKey(FString::Printf(TEXT("%s: Hat (%d) Right"), *DeviceInfo.ShortDeviceName, i + 1), DeviceInfo.MappingId.ToString(), false);
		DeviceKeyMapping.HatMappings.Add(i, HatMapping);
	}

	// Add device key mapping for joystick device.
	InputManagerSettings->AddOrUpdateDeviceKeyMapping(DeviceInfo.Identifier, DeviceKeyMapping);

	ConnectedDevicesMappings.Add(DeviceInfo.InstanceId, DeviceKeyMapping);
	bCreatedNewDeviceMapping = true;
	UE_LOG(LogIndependentInput, Log, TEXT("Generated joystick device KeyMapping [%s] for [%s]"), *DeviceInfo.MappingId.ToString(), *DeviceInfo.DeviceName);
}

void UIndependentInputSubsystem::ApplyInputOwnershipPolicy(const FJoystickDeviceInfo& DeviceInfo, FJoystickDeviceKeyMapping& DeviceKeyMapping)
{
	const UIndependentInputManagerSettings* InputManagerSettings = UIndependentInputManagerSettings::Get();

#if PLATFORM_WINDOWS
	if (InputManagerSettings->GetIgnoreXInputDevices())
	{
		switch (DeviceInfo.GamepadType)
		{
		case EGamepadType::Xbox360:
		case EGamepadType::XboxOne:
			DeviceKeyMapping.bUseIndependentInputAPI = false;
			break;

		default:
			break;
		}
	}
#else
	// We do not support any other platforms yet.
	DeviceKeyMapping.bUseIndependentInputAPI = false;
#endif

}

#undef LOCTEXT_NAMESPACE
