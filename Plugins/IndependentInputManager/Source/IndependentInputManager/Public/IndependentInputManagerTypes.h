// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SDL3/SDL.h"
//#include "DualSenseWindows.h"
#include "IndependentInputManagerTypes.generated.h"

class FDualSenseWindows;
class UTexture2D;

inline FName SanitizeName(const FString& DisplayName)
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

struct FIndependentInputKeys
{
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Guide;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc1;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc2;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc3;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc4;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc5;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Misc6;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightPaddle1;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightPaddle2;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftPaddle1;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftPaddle2;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Touchpad;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_Touchpad2;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftAccelerometer_X;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftAccelerometer_Y;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftAccelerometer_Z;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightAccelerometer_X;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightAccelerometer_Y;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightAccelerometer_Z;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftGyroscope_X;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftGyroscope_Y;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_LeftGyroscope_Z;

	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightGyroscope_X;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightGyroscope_Y;
	static INDEPENDENTINPUTMANAGER_API const FKey Gamepad_RightGyroscope_Z;
	
};


struct FIndependentGamepadKeyNames
{
	static INDEPENDENTINPUTMANAGER_API const FName Guide;
	static INDEPENDENTINPUTMANAGER_API const FName Misc1;
	static INDEPENDENTINPUTMANAGER_API const FName Misc2;
	static INDEPENDENTINPUTMANAGER_API const FName Misc3;
	static INDEPENDENTINPUTMANAGER_API const FName Misc4;
	static INDEPENDENTINPUTMANAGER_API const FName Misc5;
	static INDEPENDENTINPUTMANAGER_API const FName Misc6;

	static INDEPENDENTINPUTMANAGER_API const FName RightPaddle1;
	static INDEPENDENTINPUTMANAGER_API const FName RightPaddle2;
	static INDEPENDENTINPUTMANAGER_API const FName LeftPaddle1;
	static INDEPENDENTINPUTMANAGER_API const FName LeftPaddle2;

	static INDEPENDENTINPUTMANAGER_API const FName Touchpad;
	static INDEPENDENTINPUTMANAGER_API const FName Touchpad2;
	
	static INDEPENDENTINPUTMANAGER_API const FName LeftAccelerometer_X;
	static INDEPENDENTINPUTMANAGER_API const FName LeftAccelerometer_Y;
	static INDEPENDENTINPUTMANAGER_API const FName LeftAccelerometer_Z;

	static INDEPENDENTINPUTMANAGER_API const FName RightAccelerometer_X;
	static INDEPENDENTINPUTMANAGER_API const FName RightAccelerometer_Y;
	static INDEPENDENTINPUTMANAGER_API const FName RightAccelerometer_Z;

	static INDEPENDENTINPUTMANAGER_API const FName LeftGyroscope_X;
	static INDEPENDENTINPUTMANAGER_API const FName LeftGyroscope_Y;
	static INDEPENDENTINPUTMANAGER_API const FName LeftGyroscope_Z;

	static INDEPENDENTINPUTMANAGER_API const FName RightGyroscope_X;
	static INDEPENDENTINPUTMANAGER_API const FName RightGyroscope_Y;
	static INDEPENDENTINPUTMANAGER_API const FName RightGyroscope_Z;
};


UENUM(BlueprintType)
enum class EDualSenseTrigger : uint8
{
	Left,
	Right
};


UENUM(BlueprintType)
enum class EJoystickDeviceType : uint8
{
	Unknown,
	Gamepad,
	Wheel,
	ArcadeStick,
	FlightStick,
	DancePad,
	Guitar,
	DrumKit,
	ArcadePad,
	Throttle,
	Max				UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EGamepadType : uint8
{
	Unknown,
	Standard,
	Xbox360,
	XboxOne,
	PS3,
	PS4,
	PS5,
	SwitchPro,
	SwitchJoyconLeft,
	SwitchJoyconRight,
	SwitchJoyconPair,
	GameCube,
	Max						UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EDeviceConnectionType : uint8
{
	Unknown,
	Wired,
	Wireless
};


UENUM(BlueprintType)
enum class EDeviceBatteryState : uint8
{
	Error,
	Unknown,
	OnBattery,
	NoBattery,
	Charging,
	Charged
};


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EJoystickProperties : uint8
{
	None = 0					UMETA(Hidden),
	MonoLED = 1 << 0			UMETA(DisplayName = "Mono LED"),
	RGBLED = 1 << 1				UMETA(DisplayName = "RGB LED"),
	PlayerLED = 1 << 2			UMETA(DisplayName = "Player LED"),
	Rumble = 1 << 3				UMETA(DisplayName = "Rumble"),
	TriggerRumble = 1 << 4		UMETA(DisplayName = "Trigger Rumble")
};
ENUM_CLASS_FLAGS(EJoystickProperties);


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDeviceSensorType : uint8
{
	None = 0						UMETA(Hidden),

	// Accelerometer
	Accelerometer = 1 << 0			UMETA(DisplayName = "Accelerometer"),

	// Gyroscope
	Gyroscope = 1 << 1				UMETA(DisplayName = "Gyroscope"),

	// Accelerometer for left Joy-Con controller and Wii nunchuk
	LeftAccelerometer = 1 << 2		UMETA(DisplayName = "Left Accelerometer"),

	// Gyroscope for left Joy-Con controller
	LeftGyroscope = 1 << 3			UMETA(DisplayName = "Left Gyroscope"),
	
	// Accelerometer for right Joy-Con controller
	RightAccelerometer = 1 << 4		UMETA(DisplayName = "Right Accelerometer"),

	// Gyroscope for right Joy-Con controller
	RightGyroscope = 1 << 5			UMETA(DisplayName = "Right Gyroscope")
};
ENUM_CLASS_FLAGS(EDeviceSensorType);


template<typename TEnum>
FORCEINLINE void AddFlag(int32& Flags, TEnum Flag)
{
	Flags |= static_cast<int32>(Flag);
}


template<typename TEnum>
FORCEINLINE bool HasFlag(const int32 Flags, TEnum Flag)
{
	return Flags & static_cast<int32>(Flag);
}


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FInputDeviceInstanceId
{
	GENERATED_BODY()

public:

	FInputDeviceInstanceId()
		: Value(INDEX_NONE)
	{
	}

	FInputDeviceInstanceId(int32 NewValue)
		: Value(NewValue)
	{
	}

	operator int32() const
	{
		return Value;
	}

	bool operator==(const int32& Other) const
	{
		return Equals(Other);
	}

	bool operator==(const FInputDeviceInstanceId& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FInputDeviceInstanceId& Other) const
	{
		return !Equals(Other);
	}

	bool Equals(const FInputDeviceInstanceId& Other) const
	{
		return Value == Other.Value;
	}

	friend uint32 GetTypeHash(const FInputDeviceInstanceId& Other)
	{
		return GetTypeHash(Other.Value);
	}

	bool IsValid() const
	{
		return Value != INDEX_NONE;
	}

	int32 GetId() const { return Value; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Device", DisplayName = "Device Id", meta = (ClampMin = -1, UIMin = -1))
	int32 Value;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickDeviceIdentifier
{
	GENERATED_BODY()

public:

	FJoystickDeviceIdentifier()
		: VendorId(0)
		, ProductId(0)
	{
	}

	FJoystickDeviceIdentifier(int32 InVendorId, int32 InProductId)
		: VendorId(InVendorId)
		, ProductId(InProductId)
	{
	}

	bool IsValid() const
	{
		return VendorId != 0 && ProductId != 0;
	}

	bool operator==(const FJoystickDeviceIdentifier& Other) const
	{
		return VendorId == Other.VendorId
			&& ProductId == Other.ProductId;
	}

	bool operator!=(const FJoystickDeviceIdentifier& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FJoystickDeviceIdentifier& Other)
	{
		uint32 Hash = GetTypeHash(Other.VendorId);
		Hash = HashCombine(Hash, GetTypeHash(Other.ProductId));
		return Hash;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Device Identifier")
	int32 VendorId;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Device Identifier")
	int32 ProductId;

	inline FString ToString() const
	{
		return FString::Printf(TEXT("VID_%04X_PID_%04X"), VendorId, ProductId);
	}
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FTouchpadInfo
{
	GENERATED_BODY()

public:

	FTouchpadInfo() {}

	FTouchpadInfo(int32 InTouchpadIndex, int32 InNumOfFingers)
		: TouchpadIndex(InTouchpadIndex)
		, NumOfFingers(InNumOfFingers)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TouchPad")
	int32 TouchpadIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TouchPad")
	int32 NumOfFingers = 0;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickDeviceInfo
{
	GENERATED_BODY()

	FJoystickDeviceInfo()
		: InstanceId(INDEX_NONE)
	{
	}

	/*--------------------------------------------------------------------------
	 * Device Identity
	 *--------------------------------------------------------------------------*/

	/** Unique runtime identifier assigned by SDL for this device instance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	FInputDeviceInstanceId InstanceId;

	/** User-readable device name reported by the operating system or SDL. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	FString DeviceName;

	/** A short form of DeviceName, can be empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	FString ShortDeviceName;

	/** General joystick device category. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	EJoystickDeviceType Type = EJoystickDeviceType::Unknown;

	/** Standardized gamepad layout, if the device is recognized as a gamepad. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device", meta = (EditCondition = "Type == EJoystickDeviceType::Gamepad", EditConditionHides))
	EGamepadType GamepadType = EGamepadType::Unknown;

	/** Name of the input profile used to map this device. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device")
	FName MappingId;


	/*--------------------------------------------------------------------------
	 * Hardware Information
	 *--------------------------------------------------------------------------*/

	 /** Hardware vendor and product identifier. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Hardware")
	FJoystickDeviceIdentifier Identifier;

	/** Device serial number, if reported by the hardware. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Hardware")
	FString SerialNumber;

	/** Installed firmware version. Zero if unavailable. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Hardware")
	int32 FirmwareVersion = 0;

	/** Hardware product revision. Zero if unavailable. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Hardware")
	int32 ProductVersion = 0;


	/*--------------------------------------------------------------------------
	 * Device Features
	 *--------------------------------------------------------------------------*/

	 /** Total number of physical buttons. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features")
	int32 NumberOfButtons = 0;

	/** Total number of analog axes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features")
	int32 NumberOfAxis = 0;

	/** Total number of directional hats. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features")
	int32 NumberOfHats = 0;

	/** Total number of trackballs. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features")
	int32 NumberOfBalls = 0;

	/** Information about all available touchpads. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features")
	TArray<FTouchpadInfo> Touchpads;

	/** Supported hardware features (rumble, LEDs, trigger rumble, etc.). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features", meta = (Bitmask, BitmaskEnum = "/Script/IndependentInputManager.EJoystickProperties"))
	int32 SupportedFeatures = 0;

	/** Supported motion sensors exposed by the device. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Features", meta = (Bitmask, BitmaskEnum = "/Script/IndependentInputManager.EDeviceSensorType"))
	int32 SupportedSensors = 0;


	/*--------------------------------------------------------------------------
	 * Runtime Status
	 *--------------------------------------------------------------------------*/

	/** Current physical connection type. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	EDeviceConnectionType ConnectionType = EDeviceConnectionType::Unknown;

	/** Current battery status reported by the device. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	EDeviceBatteryState BatteryState = EDeviceBatteryState::Unknown;

	/**
	* Remaining battery percentage.
	* -1 = unavailable
	* 0-100 = battery percentage
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	int32 BatteryPercent = -1;

	/** Unreal input-device ID assigned while this device is owned by the plugin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	FInputDeviceId InputDeviceId;

	/** Unreal platform user currently associated with InputDeviceId. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	FPlatformUserId PlatformUserId;

	/** Whether Independent Input Manager owns this connected device and produces gameplay input for it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	bool bUseIndependentInputAPI = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Device|Status")
	FString KeyMapping;

	bool IsValid() const
	{
		return Identifier.IsValid();
	}

	bool operator==(const FJoystickDeviceInfo& Other) const
	{
		return Identifier == Other.Identifier 
			&& InstanceId == Other.InstanceId;
	}

	bool operator!=(const FJoystickDeviceInfo& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FJoystickDeviceInfo& Other)
	{
		return HashCombine(
			GetTypeHash(Other.Identifier),
			GetTypeHash(Other.InstanceId));
	}

	void UpdateProfileName()
	{
		if (Identifier.IsValid())
		{
			MappingId = *FString::Printf(
				TEXT("VID_%04X_PID_%04X"),
				Identifier.VendorId,
				Identifier.ProductId);
		}
		else
		{
			MappingId = NAME_None;
		}
	}
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FSDLJoystickDevice
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	FInputDeviceInstanceId InstanceId;

	SDL_Joystick* Joystick = nullptr;
	SDL_Gamepad* Gamepad = nullptr;
	SDL_Haptic* Haptic = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	bool bIsGamepad = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	bool bIsDualSense = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	FString DevicePath;
};


UENUM(BlueprintType)
enum class EAxisThresholdCondition : uint8
{
	GreaterThan,
	LessThan
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FIndependentInputKey
{
	GENERATED_BODY()

public:

	FIndependentInputKey() {}

	FIndependentInputKey(const FKey& FromKey)
		: bCustomKey(false)
		, Key(FromKey)
	{
	}

	FIndependentInputKey(const FString& ByKeyName, const FString& InMappingId, bool bIsAxis, bool bIsTouch = false)
		: bCustomKey(true)
		, KeyName(ByKeyName)
		, MappingId(InMappingId)
		, bIsAxisKey(bIsAxis)
		, bIsTouchKey(bIsTouch)
	{
	}

	UPROPERTY()
	bool bCustomKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Key, meta = (EditCondition = "!bCustomKey"))
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Key, meta = (EditCondition = "bCustomKey", HideEditConditionToggle))
	FString KeyName;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Key, meta = (EditCondition = "bCustomKey", HideEditConditionToggle))
	FString MappingId;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Key, meta = (EditCondition = "bCustomKey", HideEditConditionToggle))
	bool bIsAxisKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Key, meta = (EditCondition = "bCustomKey", HideEditConditionToggle))
	bool bIsTouchKey = false;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = Key, meta = (EditCondition = "bCustomKey", HideEditConditionToggle))
	FKey RuntimeKey;

public:

	void GenerateRuntimeKeyIfNeeded(const FJoystickDeviceKeyMapping& DeviceKeyMapping, bool bUpdateAxisWithoutSamples = false);

	FString GetKeyDisplayName() const
	{
		if (bCustomKey)
		{
			return KeyName;
		}
		else
		{
			if (Key.IsValid())
			{
				return Key.GetDisplayName().ToString();
			}
			else
			{
				return "";
			}
		}
	}

	FName GetKeyName() const
	{
		if (bCustomKey)
		{
			return SanitizeName(KeyName);
		}
		else
		{
			if (Key.IsValid())
			{
				return Key.GetFName();
			}
			else
			{
				return "";
			}
		}
	}
	
	FKey GetKey() const { return bCustomKey ? (RuntimeKey.IsValid() ? RuntimeKey : FKey(FName(MappingId + GetKeyName().ToString()))) : Key; }
	bool IsValid() const { return bCustomKey ? RuntimeKey.IsValid() : Key.IsValid(); }
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickButtonKeyMapping
{
	GENERATED_BODY()

	// Physical button index reported by the device.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Button)
	int32 ButtonIndex = INDEX_NONE;

	// In-game key generated when this button is pressed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Button)
	FIndependentInputKey Key;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FAxisVirtualButtonKeyMapping
{
	GENERATED_BODY()

public:

	// In-game key generated when the condition is satisfied.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Virtual Button")
	FIndependentInputKey Key;

	// Comparison used against the axis value.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Virtual Button")
	EAxisThresholdCondition Condition = EAxisThresholdCondition::GreaterThan;

	// Axis value required to trigger the virtual button.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Virtual Button")
	float Threshold = 0.5f;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FAxisValueRange
{
	GENERATED_BODY()

public:

	FAxisValueRange() {}
	FAxisValueRange(float InMin, float InMax)
		: Min(InMin)
		, Max(InMax)
	{
	}
	
	// Lower bound of the range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Range)
	float Min = 0.0f;

	// Upper bound of the range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Range)
	float Max = 0.0f;

	FVector2D Get2DValue() const { return FVector2D(Min, Max); }
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickAxisKeyMapping
{
	GENERATED_BODY()

	// Axis index
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Axis)
	int32 AxisIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis)
	FIndependentInputKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis)
	bool bRemap = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Axis)
	FAxisValueRange InputRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis, meta = (EditCondition = bRemap))
	FAxisValueRange OutputRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis)
	float DeadZone = 0.1f;

	/*
	* Center value around which the dead zone is applied.
	* 
	* Example:
	*	Range [-1, 1], Center = 0.0, DeadZone = 0.1
	*	-> values in [-0.1, 0.1] resolve to 0.0.
	* 
	*	Range [0, 1], Center = 0.5, DeadZone = 0.1
	*	-> values in [0.4, 0.6] resolve to 0.5.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis)
	float DeadZoneCenter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Axis)
	TArray<FAxisVirtualButtonKeyMapping> VirtualButtons;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FRelativeAxisKeyMapping
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relative Axis")
	FIndependentInputKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relative Axis")
	float Scale = 1.0f;
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickHatKeyMapping
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hat)
	int32 HatIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hat)
	FIndependentInputKey Up;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hat)
	FIndependentInputKey Down;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hat)
	FIndependentInputKey Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hat)
	FIndependentInputKey Right;
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickBallKeyMapping
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball)
	int32 BallIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ball)
	FRelativeAxisKeyMapping X;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ball)
	FRelativeAxisKeyMapping Y;
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickTouchpadFingerKeyMapping
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Finger)
	int32 FingerIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Finger)
	FIndependentInputKey Touch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Finger)
	FIndependentInputKey PositionX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Finger)
	FIndependentInputKey PositionY;

};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickTouchpadKeyMapping
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Touchpad)
	int32 TouchpadIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Touchpad, EditFixedSize, meta = (EditFixedOrder))
	TArray<FJoystickTouchpadFingerKeyMapping> Fingers;
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickSensorKeyMapping
{
	GENERATED_BODY()

public:

	FJoystickSensorKeyMapping() {}

	FJoystickSensorKeyMapping(const EDeviceSensorType InSensorType)
		: SensorType(InSensorType)
	{
		SetupKeys();
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sensor)
	EDeviceSensorType SensorType = EDeviceSensorType::None;

	// Accelerometer and Gyroscope does not need key mappings since they are bound to "Acceleration" and "Rotation Rate" input events directly.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sensor, meta = (EditCondition = "SensorType != EDeviceSensorType::None && SensorType != EDeviceSensorType::Accelerometer && SensorType != EDeviceSensorType::Gyroscope"))
	FIndependentInputKey X;

	// Accelerometer and Gyroscope does not need key mappings since they are bound to "Acceleration" and "Rotation Rate" input events directly.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sensor, meta = (EditCondition = "SensorType != EDeviceSensorType::None && SensorType != EDeviceSensorType::Accelerometer && SensorType != EDeviceSensorType::Gyroscope"))
	FIndependentInputKey Y;

	// Accelerometer and Gyroscope does not need key mappings since they are bound to "Acceleration" and "Rotation Rate" input events directly.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sensor, meta = (EditCondition = "SensorType != EDeviceSensorType::None && SensorType != EDeviceSensorType::Accelerometer && SensorType != EDeviceSensorType::Gyroscope"))
	FIndependentInputKey Z;

	void SetupKeys()
	{
		switch (SensorType)
		{
			case EDeviceSensorType::LeftAccelerometer:
			{
				X = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftAccelerometer_X));
				Y = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftAccelerometer_Y));
				Z = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftAccelerometer_Z));
				break;
			}

			case EDeviceSensorType::LeftGyroscope:
			{
				X = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftGyroscope_X));
				Y = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftGyroscope_Y));
				Z = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_LeftGyroscope_Z));
				break;
			}

			case EDeviceSensorType::RightAccelerometer:
			{
				X = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightAccelerometer_X));
				Y = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightAccelerometer_Y));
				Z = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightAccelerometer_Z));
				break;
			}

			case EDeviceSensorType::RightGyroscope:
			{
				X = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightGyroscope_X));
				Y = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightGyroscope_Y));
				Z = FIndependentInputKey(FKey(FIndependentInputKeys::Gamepad_RightGyroscope_Z));
				break;
			}
		}
	}
};


/** Support and user-controlled enabled state for an optional device feature. */
USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickFeatureConfig
{
	GENERATED_BODY()

public:

	/** Whether the current device connection reports support for this feature. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = Mapping)
	bool bSupports = false;

protected:

	/** User preference applied whenever the connected device supports this feature. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	bool bEnabled = true;

public:

	/** Stores the user's enabled preference independently of current hardware support. */
	void SetEnabled(bool bInEnabled)
	{
		bEnabled = bInEnabled;
	}

	bool IsEnabledRequested() const { return bEnabled; }
	bool IsEnabled() const { return bSupports && bEnabled; }
};


USTRUCT(BlueprintType)
struct INDEPENDENTINPUTMANAGER_API FJoystickDeviceKeyMapping
{
	GENERATED_BODY()

public:

	FJoystickDeviceKeyMapping()
		: MappingId("")
		, DeviceName("")
	{
	}

	FJoystickDeviceKeyMapping(FName InMappingId, FString InDeviceName)
		: MappingId(InMappingId)
		, DeviceName(InDeviceName)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Device)
	bool bUseIndependentInputAPI = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	bool bUseGamepadAPI = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	FName MappingId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Device)
	FString DeviceName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<int32, FJoystickButtonKeyMapping> ButtonMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<int32, FJoystickAxisKeyMapping> AxisMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<int32, FJoystickHatKeyMapping> HatMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<int32, FJoystickBallKeyMapping> BallMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<int32, FJoystickTouchpadKeyMapping> TouchpadMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mapping)
	TMap<EDeviceSensorType, FJoystickSensorKeyMapping> SensorMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	FJoystickFeatureConfig Rumble;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	FJoystickFeatureConfig TriggerRumble;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Settings)
	FJoystickFeatureConfig AdaptiveTriggerEffect;

};


struct INDEPENDENTINPUTMANAGER_API FButtonState
{
	FButtonState() {}

	FButtonState(const FKey& InKey)
		: Key(InKey)
	{
	}

	bool GetValue() const { return bState; }
	bool GetPreviousValue() const { return bPreviousState; }
	bool HasChanged() const { return !PendingTransitions.IsEmpty(); }
	double GetNextRepeatTime() const { return NextRepeatTime; }
	bool IsPendingCommit() const { return !PendingTransitions.IsEmpty(); }

	void Update(bool InValue)
	{
		if (bState == InValue)
			return;

		bState = InValue;
		PendingTransitions.Add(InValue);
	}

	bool PopPendingTransition(bool& OutValue)
	{
		if (PendingTransitions.IsEmpty())
			return false;

		OutValue = PendingTransitions[0];
		PendingTransitions.RemoveAt(0, EAllowShrinking::No);
		bPreviousState = OutValue;
		return true;
	}

	void SetNextRepeatTime(double InNextRepeatTime)
	{
		NextRepeatTime = InNextRepeatTime;
	}

	void Commit()
	{
		bPreviousState = bState;
		PendingTransitions.Reset();
	}

	bool bState = false;
	bool bPreviousState = false;
	double NextRepeatTime = 0.0;
	FKey Key;

private:

	TArray<bool, TInlineAllocator<4>> PendingTransitions;
};


struct INDEPENDENTINPUTMANAGER_API FAxisVirtualButtonState
{
public:

	FAxisVirtualButtonState() {}
	
	FAxisVirtualButtonState(const FAxisVirtualButtonKeyMapping& InVirtualButtonKeyMapping)
	{
		Condition = InVirtualButtonKeyMapping.Condition;
		ButtonState = FButtonState(InVirtualButtonKeyMapping.Key.GetKey());
		Threshold = InVirtualButtonKeyMapping.Threshold;
	}

	FButtonState ButtonState;
	EAxisThresholdCondition Condition = EAxisThresholdCondition::GreaterThan;
	float Threshold = 0.5f;

	bool EvaluateIsPressed(float InValue) const
	{
		switch (Condition)
		{
		case EAxisThresholdCondition::GreaterThan:
			return InValue >= Threshold;

		case EAxisThresholdCondition::LessThan:
			return InValue <= Threshold;

		default:
			return false;
		}
	}
};


struct INDEPENDENTINPUTMANAGER_API FAxisState
{
	FAxisState() {}

	FAxisState(const FKey& InKey)
		: Key(InKey)
	{
	}

	FAxisState(const FKey& InKey, float InDeadZone, float InDeadZoneSource, bool bInRemap, FAxisValueRange InInputRange, FAxisValueRange InOutputRange)
		: Key(InKey)
		, DeadZone(InDeadZone)
		, DeadZoneCenter(InDeadZoneSource)
		, bRemap(bInRemap)
		, InputRange(InInputRange)
		, OutputRange(InOutputRange)
	{
	}

	bool IsValid() const { return Key.IsValid(); }

	TArray<FAxisVirtualButtonState> VirtualButtons;

	float GetValue() const { return Value; }

	float GetPreviousValue() const { return PreviousValue; }

	bool HasChanged() const { return !FMath::IsNearlyEqual(Value, PreviousValue, KINDA_SMALL_NUMBER); }

	bool IsPendingCommit() const { return bPendingCommit; }

	void Update(float InValue)
	{
		RawValue = InValue;
		InValue = GetProcessedValue(InValue);
		if (bPendingCommit)
		{
			/*
			* SDL input events and IInputDevice::SendControllerEvents() run independently
			* and may update at different rates.
			*
			* An axis can receive multiple SDL events before SendControllerEvents() is
			* called. Once the axis becomes dirty, PreviousValue must remain unchanged
			* until Commit() is called, otherwise intermediate updates would overwrite the
			* previous frame's value and HasChanged() would no longer represent the
			* transition since the last dispatched input event.
			*/
			Value = InValue;
			return;
		}

		PreviousValue = Value;
		Value = InValue;
		bPendingCommit = true;
	}

	void Commit()
	{
		PreviousValue = Value;
		bPendingCommit = false;
	}

	FKey Key;
	float RawValue = 0.0f;
	float Value = 0.0f;
	float PreviousValue = 0.0f;
	float DeadZone = 0.0f;
	float DeadZoneCenter = 0.0f;
	bool bRemap = false;
	FAxisValueRange InputRange;
	FAxisValueRange OutputRange;

private:

	/*
	 * True once Update() has modified the axis since the last Commit().
	 * Prevents PreviousValue from being overwritten by multiple SDL events occurring within the same input frame.
	 */
	bool bPendingCommit = false;

	float GetProcessedValue(const float InValue)
	{
		// Normalize input into configured input range
		float ProcessedValue = bRemap ? FMath::GetMappedRangeValueClamped(InputRange.Get2DValue(), OutputRange.Get2DValue(), InValue) : InValue;

		// Apply dead zone
		if (!FMath::IsNearlyZero(DeadZone) && FMath::IsNearlyEqual(ProcessedValue, DeadZoneCenter, DeadZone))
			ProcessedValue = DeadZoneCenter;

		return ProcessedValue;
	}
};


struct INDEPENDENTINPUTMANAGER_API FRelativeAxisState
{
	FRelativeAxisState() {}

	FRelativeAxisState(const FRelativeAxisKeyMapping& InMapping)
		: Key(InMapping.Key.GetKey())
		, Scale(InMapping.Scale)
	{
	}

	void Accumulate(float InRawDelta)
	{
		PendingRawDelta += InRawDelta;
	}

	bool HasPendingInput() const
	{
		return !FMath::IsNearlyZero(PendingRawDelta);
	}

	float ConsumeOutputValue()
	{
		OutputValue = PendingRawDelta * Scale;
		PendingRawDelta = 0.0f;

		return OutputValue;
	}

	float GetOutputValue() const
	{
		return OutputValue;
	}

	FKey Key;

private:

	float PendingRawDelta = 0.0f;
	float OutputValue = 0.0f;
	float Scale = 1.0f;
};


struct INDEPENDENTINPUTMANAGER_API FHatState
{
public:

	uint8 Value = 0;
	uint8 PreviousValue = 0;

	void Update(uint8 InValue)
	{
		Value = InValue;
		UpdateDirections(InValue);
	}

	bool HasChanged() const
	{
		return Up.HasChanged()
			|| Down.HasChanged()
			|| Left.HasChanged()
			|| Right.HasChanged();
	}

	void Commit()
	{
		PreviousValue = Value;

		Up.Commit();
		Down.Commit();
		Left.Commit();
		Right.Commit();
	}

	FButtonState Up;
	FButtonState Down;
	FButtonState Left;
	FButtonState Right;

private:

	void UpdateDirections(uint8 InValue)
	{
		Up.Update((InValue & SDL_HAT_UP) != 0);
		Down.Update((InValue & SDL_HAT_DOWN) != 0);
		Left.Update((InValue & SDL_HAT_LEFT) != 0);
		Right.Update((InValue & SDL_HAT_RIGHT) != 0);
	}

};


struct INDEPENDENTINPUTMANAGER_API FBallState
{
	FBallState() {}

	FBallState(const FJoystickBallKeyMapping& InMapping)
		: X(InMapping.X)
		, Y(InMapping.Y)
	{
	}

	void Accumulate(float XRel, float YRel)
	{
		X.Accumulate(XRel);
		Y.Accumulate(YRel);
	}

	bool HasPendingInput() const
	{
		return X.HasPendingInput() || Y.HasPendingInput();
	}

	FVector2D ConsumeOutputValue()
	{
		return FVector2D(X.ConsumeOutputValue(), Y.ConsumeOutputValue());
	}

	FVector2D GetOutputValue() const
	{
		return FVector2D(X.GetOutputValue(), Y.GetOutputValue());
	}

	FRelativeAxisState X;
	FRelativeAxisState Y;
};


struct INDEPENDENTINPUTMANAGER_API FTouchFingerState
{
	FTouchFingerState() {}

	FTouchFingerState(
		const FKey& InTouchKey,
		const FKey& InXKey,
		const FKey& InYKey)
		: Touch(InTouchKey)
		, X(InXKey)
		, Y(InYKey)
	{
	}

	/** Returns true if any part of the touch point has changed. */
	bool HasChanged() const
	{
		return Touch.HasChanged()
			|| X.HasChanged()
			|| Y.HasChanged();
	}

	/** Returns true if any state is waiting to be committed. */
	bool IsPendingCommit() const
	{
		return Touch.IsPendingCommit()
			|| X.IsPendingCommit()
			|| Y.IsPendingCommit();
	}

	/** Updates the complete touch point state. */
	void Update(bool bTouched, float InX, float InY)
	{
		Touch.Update(bTouched);
		X.Update(InX);
		Y.Update(InY);
	}

	/** Commits all pending state changes. */
	void Commit()
	{
		Touch.Commit();
		X.Commit();
		Y.Commit();
	}

	/** True while the finger is touching the surface. */
	FButtonState Touch;

	/** Normalized X position [0..1]. */
	FAxisState X;

	/** Normalized Y position [0..1]. */
	FAxisState Y;
};


struct INDEPENDENTINPUTMANAGER_API FTouchpadState
{
	TArray<FTouchFingerState> FingersState;

	bool HasChanged() const
	{
		for (const FTouchFingerState& FingerState : FingersState)
		{
			if (FingerState.HasChanged())
			{
				return true;
			}
		}

		return false;
	}

	void Commit()
	{
		for (FTouchFingerState& FingerState : FingersState)
		{
			FingerState.Commit();
		}
	}
};


struct INDEPENDENTINPUTMANAGER_API FSensorState
{
	FVector Value;
	FVector PreviousValue;

	FSensorState() {}
	FSensorState(FKey KeyX, FKey KeyY, FKey KeyZ)
		: X(FAxisState(KeyX))
		, Y(FAxisState(KeyY))
		, Z(FAxisState(KeyZ))
	{
	}

	bool HasChanged() const
	{
		return !Value.Equals(PreviousValue, KINDA_SMALL_NUMBER);
	}

	void Update(const FVector& InValue)
	{
		if (X.IsValid())
			X.Update(InValue.X);

		if (Y.IsValid())
			Y.Update(InValue.Y);

		if (Z.IsValid())
			Z.Update(InValue.Z);

		if (bPendingCommit)
		{
			/*
			* SDL input events and IInputDevice::SendControllerEvents() run independently
			* and may tick at different rates.
			*
			* A Sensor may receive multiple SDL events before Commit() is called.
			* Once the state becomes pending, PreviousValue must remain unchanged until
			* Commit(), otherwise intermediate updates would overwrite the previous frame's
			* state and HasChanged() would no longer represent the transition that has
			* not yet been dispatched.
			*/

			Value = InValue;
			return;
		}
		
		PreviousValue = Value;
		Value = InValue;

		bPendingCommit = true;
	}

	void Commit()
	{
		if (!bPendingCommit)
			return;

		if (X.IsValid())
			X.Commit();

		if (Y.IsValid())
			Y.Commit();

		if (Z.IsValid())
			Z.Commit();

		PreviousValue = Value;
		bPendingCommit = false;
	}

	FAxisState X;
	FAxisState Y;
	FAxisState Z;

private:

	bool bPendingCommit = false;
};


struct INDEPENDENTINPUTMANAGER_API FForceFeedbackState
{
public:

	FForceFeedbackState() = default;

	/** Large (low-frequency) left motor. */
	float LeftLarge = 0.0f;

	/** Small (high-frequency) left motor. */
	float LeftSmall = 0.0f;

	/** Large (low-frequency) right motor. */
	float RightLarge = 0.0f;

	/** Small (high-frequency) right motor. */
	float RightSmall = 0.0f;

	/** Last low-frequency value sent to SDL. */
	float LastLowFrequency = -1.0f;

	/** Last high-frequency value sent to SDL. */
	float LastHighFrequency = -1.0f;

	/** True when the force feedback values have changed. */
	bool bDirty = false;
};


struct FJoystickDeviceState
{
public:

	FInputDeviceId InputDeviceId;
	FPlatformUserId PlatformUserId;

	TMap<int32, FButtonState> Buttons;
	TMap<int32, FAxisState> Axes;
	TMap<int32, FHatState> Hats;
	TMap<int32, FBallState> Balls;
	TMap<int32, FTouchpadState> Touchpads;
	TMap<EDeviceSensorType, FSensorState> Sensors;
	FForceFeedbackState ForceFeedback;
	FJoystickFeatureConfig Rumble;
	FJoystickFeatureConfig TriggerRumble;
	FJoystickFeatureConfig AdaptiveTriggerEffect;

#if PLATFORM_WINDOWS
	TSharedPtr<FDualSenseWindows> DualSense;
#endif
};



struct FSDLInputUtils
{
public:

	static void RegisterDefaultKeys();
	static EJoystickDeviceType ConvertJoystickType(SDL_JoystickType InType);
	static EGamepadType ConvertGamepadType(SDL_GamepadType InType);
	static EDeviceConnectionType ConvertConnectionType(SDL_JoystickConnectionState InState);
	static FString GetDeviceHardwareIdentifierFromGamepadType(const EGamepadType GamepadType, const FString& InDeviceName);
	static FString GetDeviceShortNameFromGamepadType(const EGamepadType GamepadType, FString DeviceName);
	static FString GetDeviceShortNameFromDeviceName(FString DeviceName);
	static EDeviceBatteryState ConvertBatteryState(SDL_PowerState InState);
	static FKey ConvertSDLButtonToKey(SDL_GamepadButton InButton);
	static FKey ConvertSDLAxisToKey(SDL_GamepadAxis InAxis);
	static FKey ConvertSDLRawMappingToKey(FString MappingKeyName);
	static void PopulateSupportedSensors(SDL_Gamepad* Gamepad, FJoystickDeviceInfo& DeviceInfo);
	static void PopulateSupportedFeatures(SDL_Joystick* Joystick, FJoystickDeviceInfo& DeviceInfo);
	static EDeviceSensorType ConvertSensorType(Sint32 InType);
	static SDL_SensorType ConvertSensorType(EDeviceSensorType InType);
	static FName SanitizeDisplayName(const FString& DisplayName);
	static bool IsDualSense(SDL_Joystick* Joystick);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDevicePluggedIn, const FJoystickDeviceInfo&, DeviceInfo, const FSDLJoystickDevice&, SDLDevice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeviceUnplugged, const FJoystickDeviceInfo&, DeviceInfo, const FSDLJoystickDevice&, SDLDevice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeviceInfoUpdate, const FJoystickDeviceInfo&, DeviceInfo);
