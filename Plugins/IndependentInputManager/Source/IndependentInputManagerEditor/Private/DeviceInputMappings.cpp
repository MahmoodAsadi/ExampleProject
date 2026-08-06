// Fill out your copyright notice in the Description page of Project Settings.

#include "DeviceInputMappings.h"

#include "InputCoreTypes.h"

#define LOCTEXT_NAMESPACE "DeviceInputMappings"

namespace
{
	enum class ECustomKeyType : uint8
	{
		Button,
		Axis1D,
		Axis2D
	};

	struct FCustomKeyUse
	{
		FString DisplayName;
		FString Source;
		ECustomKeyType Type = ECustomKeyType::Button;
	};

	const TCHAR* GetCustomKeyTypeName(ECustomKeyType Type)
	{
		switch (Type)
		{
			case ECustomKeyType::Button:
				return TEXT("Button");

			case ECustomKeyType::Axis1D:
				return TEXT("Axis1D");

			case ECustomKeyType::Axis2D:
				return TEXT("Axis2D");

			default:
				return TEXT("Unknown");
		}
	}

	void GetPairedKeyNames(const FIndependentInputKey& KeyX, FString& OutKeyName, FString& OutDisplayName)
	{
		OutDisplayName = KeyX.GetKeyDisplayName();

		if (OutDisplayName.EndsWith(TEXT("X Delta")))
		{
			if (!OutDisplayName.RemoveFromEnd(TEXT(" X Delta")))
				OutDisplayName.RemoveFromEnd(TEXT("X Delta"));

			OutDisplayName.Append(TEXT(" Delta 2D"));
			OutKeyName = SanitizeName(OutDisplayName).ToString();
		}
		else if (OutDisplayName.EndsWith(TEXT("X-Axis")))
		{
			if (!OutDisplayName.RemoveFromEnd(TEXT(" X-Axis")))
				OutDisplayName.RemoveFromEnd(TEXT("X-Axis"));

			OutKeyName = SanitizeName(OutDisplayName).ToString();
			OutKeyName.Append(TEXT("_2D"));
			OutDisplayName.Append(TEXT(" 2D-Axis"));
		}
		else if (OutDisplayName.EndsWith(TEXT("X")))
		{
			if (!OutDisplayName.RemoveFromEnd(TEXT(" X")))
				OutDisplayName.RemoveFromEnd(TEXT("X"));

			OutDisplayName.Append(TEXT(" 2D"));
			OutKeyName = SanitizeName(OutDisplayName).ToString();
		}
		else
		{
			OutDisplayName.Append(TEXT(" 2D"));
			OutKeyName = SanitizeName(OutDisplayName).ToString();
		}
	}

	FKey GetMappedRuntimeKey(
		const FJoystickDeviceKeyMapping& DeviceKeyMapping,
		const FIndependentInputKey& Key)
	{
		if (!Key.bCustomKey)
			return Key.Key;

		return FKey(FName(*(DeviceKeyMapping.MappingId.ToString() + Key.GetKeyName().ToString())));
	}

	bool ValidateRelativeBallKey(
		const FJoystickDeviceIdentifier& DeviceIdentifier,
		const FKey& RuntimeKey,
		const FString& Source,
		FText& OutValidationError)
	{
		const TSharedPtr<FKeyDetails> KeyDetails = EKeys::GetKeyDetails(RuntimeKey);
		if (!KeyDetails || KeyDetails->ShouldUpdateAxisWithoutSamples())
			return true;

		OutValidationError = FText::Format(
			LOCTEXT(
				"BallKeyDoesNotResetWithoutSamples",
				"{0}: {1} uses key \"{2}\", which retains its previous axis value when no input sample is received. "
				"Ball input requires a relative-axis key registered with UpdateAxisWithoutSamples. Choose a compatible "
				"relative-axis key or create a new custom key."),
			FText::FromString(DeviceIdentifier.ToString()),
			FText::FromString(Source),
			FText::FromName(RuntimeKey.GetFName()));
		return false;
	}

	bool ValidatePairedKeyRegistration(
		const FJoystickDeviceIdentifier& DeviceIdentifier,
		const FJoystickDeviceKeyMapping& DeviceKeyMapping,
		const FIndependentInputKey& KeyX,
		const FIndependentInputKey& KeyY,
		const FString& PairedKeyName,
		const FString& Source,
		FText& OutValidationError)
	{
		const FKey RuntimeKeyX = GetMappedRuntimeKey(DeviceKeyMapping, KeyX);
		const FKey RuntimeKeyY = GetMappedRuntimeKey(DeviceKeyMapping, KeyY);
		const FKey PairedKey(FName(*(DeviceKeyMapping.MappingId.ToString() + PairedKeyName)));

		if (const EKeys::FPairedKeyDetails* ExistingPair = EKeys::GetPairedKeyDetails(PairedKey))
		{
			const FKey ExistingKeyX =
				ExistingPair->XKeyDetails.IsValid() ? ExistingPair->XKeyDetails->GetKey() : FKey();
			const FKey ExistingKeyY =
				ExistingPair->YKeyDetails.IsValid() ? ExistingPair->YKeyDetails->GetKey() : FKey();
			if (ExistingKeyX != RuntimeKeyX || ExistingKeyY != RuntimeKeyY)
			{
				OutValidationError = FText::Format(
					LOCTEXT(
						"ExistingPairedKeyComponentsConflict",
						"{0}: {1} cannot use X key \"{2}\" and Y key \"{3}\" because paired key \"{4}\" is already "
						"registered with X key \"{5}\" and Y key \"{6}\". Rename both X and Y keys together."),
					FText::FromString(DeviceIdentifier.ToString()),
					FText::FromString(Source),
					FText::FromName(RuntimeKeyX.GetFName()),
					FText::FromName(RuntimeKeyY.GetFName()),
					FText::FromName(PairedKey.GetFName()),
					FText::FromName(ExistingKeyX.GetFName()),
					FText::FromName(ExistingKeyY.GetFName()));
				return false;
			}

			return true;
		}

		const TSharedPtr<FKeyDetails> KeyXDetails = EKeys::GetKeyDetails(RuntimeKeyX);
		if (KeyXDetails
			&& KeyXDetails->GetPairedAxis() != EPairedAxis::Unpaired
			&& KeyXDetails->GetPairedAxisKey() != PairedKey)
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"PairedXKeyAlreadyInUse",
					"{0}: {1} cannot use X key \"{2}\" because it is already part of paired key \"{3}\". "
					"Rename both X and Y keys together, or choose an unpaired Axis1D key."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Source),
				FText::FromName(RuntimeKeyX.GetFName()),
				FText::FromName(KeyXDetails->GetPairedAxisKey().GetFName()));
			return false;
		}

		const TSharedPtr<FKeyDetails> KeyYDetails = EKeys::GetKeyDetails(RuntimeKeyY);
		if (KeyYDetails
			&& KeyYDetails->GetPairedAxis() != EPairedAxis::Unpaired
			&& KeyYDetails->GetPairedAxisKey() != PairedKey)
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"PairedYKeyAlreadyInUse",
					"{0}: {1} cannot use Y key \"{2}\" because it is already part of paired key \"{3}\". "
					"Rename both X and Y keys together, or choose an unpaired Axis1D key."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Source),
				FText::FromName(RuntimeKeyY.GetFName()),
				FText::FromName(KeyYDetails->GetPairedAxisKey().GetFName()));
			return false;
		}

		return true;
	}

	bool HasExpectedType(const FKeyDetails& KeyDetails, ECustomKeyType ExpectedType)
	{
		switch (ExpectedType)
		{
			case ECustomKeyType::Button:
				return KeyDetails.IsDigital();

			case ECustomKeyType::Axis1D:
				return KeyDetails.IsAxis1D();

			case ECustomKeyType::Axis2D:
				return KeyDetails.IsAxis2D();

			default:
				return false;
		}
	}

	bool AddDerivedKey(
		const FJoystickDeviceIdentifier& DeviceIdentifier,
		const FJoystickDeviceKeyMapping& DeviceKeyMapping,
		const FString& SanitizedKeyName,
		const FString& DisplayName,
		const FString& Source,
		ECustomKeyType Type,
		TMap<FName, FCustomKeyUse>& KeyUses,
		FText& OutValidationError)
	{
		if (SanitizedKeyName.IsEmpty())
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"EmptySanitizedKeyName",
					"{0}: {1} does not produce a valid Unreal key name. Enter a descriptive key name."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Source));
			return false;
		}

		const FName RuntimeKeyName(*(DeviceKeyMapping.MappingId.ToString() + SanitizedKeyName));
		if (const FCustomKeyUse* ExistingUse = KeyUses.Find(RuntimeKeyName))
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"DuplicateInputKey",
					"{0}: {1} uses the key \"{2}\", which is already used by {3} as {4}. "
					"Give each input a unique key."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Source),
				FText::FromName(RuntimeKeyName),
				FText::FromString(ExistingUse->Source),
				FText::FromString(GetCustomKeyTypeName(ExistingUse->Type)));
			return false;
		}

		const FKey RuntimeKey(RuntimeKeyName);
		if (const TSharedPtr<FKeyDetails> ExistingKeyDetails = EKeys::GetKeyDetails(RuntimeKey))
		{
			if (!HasExpectedType(*ExistingKeyDetails, Type))
			{
				OutValidationError = FText::Format(
					LOCTEXT(
						"InputKeyTypeConflict",
						"{0}: {1} uses the key \"{2}\", but that key is registered as a different input type. "
						"Choose a {3} key."),
					FText::FromString(DeviceIdentifier.ToString()),
					FText::FromString(Source),
					FText::FromName(RuntimeKeyName),
					FText::FromString(GetCustomKeyTypeName(Type)));
				return false;
			}

			if (!ExistingKeyDetails->GetDisplayName().ToString().Equals(DisplayName, ESearchCase::CaseSensitive))
			{
				OutValidationError = FText::Format(
					LOCTEXT(
						"ExistingCustomKeyDisplayNameConflict",
						"{0}: The custom key name \"{1}\" used by {2} resolves to \"{3}\", which is already registered with "
						"the display name \"{4}\". Enter a name that produces a unique key."),
					FText::FromString(DeviceIdentifier.ToString()),
					FText::FromString(DisplayName),
					FText::FromString(Source),
					FText::FromName(RuntimeKeyName),
					ExistingKeyDetails->GetDisplayName());
				return false;
			}
		}

		KeyUses.Add(RuntimeKeyName, { DisplayName, Source, Type });
		return true;
	}

	bool AddInputKey(
		const FJoystickDeviceIdentifier& DeviceIdentifier,
		const FJoystickDeviceKeyMapping& DeviceKeyMapping,
		const FIndependentInputKey& Key,
		const FString& Source,
		ECustomKeyType Type,
		TMap<FName, FCustomKeyUse>& KeyUses,
		FText& OutValidationError)
	{
		if (!Key.bCustomKey)
		{
			if (!Key.Key.IsValid())
				return true;

			const FName RuntimeKeyName = Key.Key.GetFName();
			if (const FCustomKeyUse* ExistingUse = KeyUses.Find(RuntimeKeyName))
			{
				OutValidationError = FText::Format(
					LOCTEXT(
						"DuplicateUnrealInputKey",
						"{0}: {1} uses the Unreal key \"{2}\", which is already used by {3} as {4}. "
						"Give each input a unique key."),
					FText::FromString(DeviceIdentifier.ToString()),
					FText::FromString(Source),
					Key.Key.GetDisplayName(),
					FText::FromString(ExistingUse->Source),
					FText::FromString(GetCustomKeyTypeName(ExistingUse->Type)));
				return false;
			}

			const TSharedPtr<FKeyDetails> KeyDetails = EKeys::GetKeyDetails(Key.Key);
			if (!KeyDetails || !HasExpectedType(*KeyDetails, Type))
			{
				OutValidationError = FText::Format(
					LOCTEXT(
						"UnrealInputKeyTypeConflict",
						"{0}: {1} uses the Unreal key \"{2}\", but it is not a compatible {3} key."),
					FText::FromString(DeviceIdentifier.ToString()),
					FText::FromString(Source),
					Key.Key.GetDisplayName(),
					FText::FromString(GetCustomKeyTypeName(Type)));
				return false;
			}

			KeyUses.Add(RuntimeKeyName, { Key.Key.GetDisplayName().ToString(), Source, Type });
			return true;
		}

		FString TrimmedKeyName = Key.KeyName;
		TrimmedKeyName.TrimStartAndEndInline();
		if (TrimmedKeyName.IsEmpty())
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"EmptyCustomKeyName",
					"{0}: {1} has an empty custom key name. Enter a descriptive name before saving."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Source));
			return false;
		}

		if (!TrimmedKeyName.Equals(Key.KeyName, ESearchCase::CaseSensitive))
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"CustomKeyNameWhitespace",
					"{0}: The key name \"{1}\" used by {2} has leading or trailing whitespace. Remove the extra "
					"whitespace before saving."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Key.KeyName),
				FText::FromString(Source));
			return false;
		}

		const FName SanitizedKeyName = SanitizeName(TrimmedKeyName);
		if (SanitizedKeyName.IsNone())
		{
			OutValidationError = FText::Format(
				LOCTEXT(
					"InvalidCustomKeyName",
					"{0}: The key name \"{1}\" used by {2} does not produce a valid Unreal key name. Enter a "
					"different name."),
				FText::FromString(DeviceIdentifier.ToString()),
				FText::FromString(Key.KeyName),
				FText::FromString(Source));
			return false;
		}

		return AddDerivedKey(
			DeviceIdentifier,
			DeviceKeyMapping,
			SanitizedKeyName.ToString(),
			Key.KeyName,
			Source,
			Type,
			KeyUses,
			OutValidationError);
	}

	bool ValidateDeviceKeyMapping(
		const FJoystickDeviceIdentifier& DeviceIdentifier,
		const FJoystickDeviceKeyMapping& DeviceKeyMapping,
		FText& OutValidationError)
	{
		if (DeviceKeyMapping.MappingId.IsNone())
		{
			OutValidationError = LOCTEXT(
				"InvalidDeviceMappingId",
				"The device profile does not have a valid mapping ID, so custom keys cannot be saved.");
			return false;
		}

		TMap<FName, FCustomKeyUse> KeyUses;

		TArray<int32> ButtonIndices;
		DeviceKeyMapping.ButtonMappings.GenerateKeyArray(ButtonIndices);
		ButtonIndices.Sort();
		for (const int32 ButtonIndex : ButtonIndices)
		{
			const FJoystickButtonKeyMapping& ButtonMapping =
				DeviceKeyMapping.ButtonMappings.FindChecked(ButtonIndex);
			if (!AddInputKey(
				DeviceIdentifier,
				DeviceKeyMapping,
				ButtonMapping.Key,
				FString::Printf(TEXT("Button %d"), ButtonIndex),
				ECustomKeyType::Button,
				KeyUses,
				OutValidationError))
			{
				return false;
			}
		}

		TArray<int32> AxisIndices;
		DeviceKeyMapping.AxisMappings.GenerateKeyArray(AxisIndices);
		AxisIndices.Sort();
		for (const int32 AxisIndex : AxisIndices)
		{
			const FJoystickAxisKeyMapping& AxisMapping =
				DeviceKeyMapping.AxisMappings.FindChecked(AxisIndex);
			if (!AddInputKey(
				DeviceIdentifier,
				DeviceKeyMapping,
				AxisMapping.Key,
				FString::Printf(TEXT("Axis %d"), AxisIndex),
				ECustomKeyType::Axis1D,
				KeyUses,
				OutValidationError))
			{
				return false;
			}

			for (int32 VirtualButtonIndex = 0; VirtualButtonIndex < AxisMapping.VirtualButtons.Num(); ++VirtualButtonIndex)
			{
				if (!AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					AxisMapping.VirtualButtons[VirtualButtonIndex].Key,
					FString::Printf(TEXT("Axis %d, Virtual Button %d"), AxisIndex, VirtualButtonIndex),
					ECustomKeyType::Button,
					KeyUses,
					OutValidationError))
				{
					return false;
				}
			}
		}

		TArray<int32> BallIndices;
		DeviceKeyMapping.BallMappings.GenerateKeyArray(BallIndices);
		BallIndices.Sort();
		for (const int32 BallIndex : BallIndices)
		{
			const FJoystickBallKeyMapping& BallMapping =
				DeviceKeyMapping.BallMappings.FindChecked(BallIndex);
			const FString BallSource = FString::Printf(TEXT("Ball %d"), BallIndex);
			if (!AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					BallMapping.X.Key,
					BallSource + TEXT(", X"),
					ECustomKeyType::Axis1D,
					KeyUses,
					OutValidationError)
				|| !ValidateRelativeBallKey(
					DeviceIdentifier,
					GetMappedRuntimeKey(DeviceKeyMapping, BallMapping.X.Key),
					BallSource + TEXT(", X"),
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					BallMapping.Y.Key,
					BallSource + TEXT(", Y"),
					ECustomKeyType::Axis1D,
					KeyUses,
					OutValidationError)
				|| !ValidateRelativeBallKey(
					DeviceIdentifier,
					GetMappedRuntimeKey(DeviceKeyMapping, BallMapping.Y.Key),
					BallSource + TEXT(", Y"),
					OutValidationError))
			{
				return false;
			}

			FString PairedKeyName;
			FString PairedKeyDisplayName;
			GetPairedKeyNames(BallMapping.X.Key, PairedKeyName, PairedKeyDisplayName);
			if (!AddDerivedKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					PairedKeyName,
					PairedKeyDisplayName,
					BallSource + TEXT(", Delta 2D"),
					ECustomKeyType::Axis2D,
					KeyUses,
					OutValidationError)
				|| !ValidatePairedKeyRegistration(
					DeviceIdentifier,
					DeviceKeyMapping,
					BallMapping.X.Key,
					BallMapping.Y.Key,
					PairedKeyName,
					BallSource + TEXT(", Delta 2D"),
					OutValidationError)
				|| !ValidateRelativeBallKey(
					DeviceIdentifier,
					FKey(FName(*(DeviceKeyMapping.MappingId.ToString() + PairedKeyName))),
					BallSource + TEXT(", Delta 2D"),
					OutValidationError))
			{
				return false;
			}
		}

		TArray<int32> HatIndices;
		DeviceKeyMapping.HatMappings.GenerateKeyArray(HatIndices);
		HatIndices.Sort();
		for (const int32 HatIndex : HatIndices)
		{
			const FJoystickHatKeyMapping& HatMapping =
				DeviceKeyMapping.HatMappings.FindChecked(HatIndex);
			const FString HatSource = FString::Printf(TEXT("Hat %d"), HatIndex);
			if (!AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					HatMapping.Up,
					HatSource + TEXT(", Up"),
					ECustomKeyType::Button,
					KeyUses,
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					HatMapping.Down,
					HatSource + TEXT(", Down"),
					ECustomKeyType::Button,
					KeyUses,
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					HatMapping.Left,
					HatSource + TEXT(", Left"),
					ECustomKeyType::Button,
					KeyUses,
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					HatMapping.Right,
					HatSource + TEXT(", Right"),
					ECustomKeyType::Button,
					KeyUses,
					OutValidationError))
			{
				return false;
			}
		}

		TArray<int32> TouchpadIndices;
		DeviceKeyMapping.TouchpadMappings.GenerateKeyArray(TouchpadIndices);
		TouchpadIndices.Sort();
		for (const int32 TouchpadIndex : TouchpadIndices)
		{
			const FJoystickTouchpadKeyMapping& TouchpadMapping =
				DeviceKeyMapping.TouchpadMappings.FindChecked(TouchpadIndex);
			for (int32 FingerArrayIndex = 0; FingerArrayIndex < TouchpadMapping.Fingers.Num(); ++FingerArrayIndex)
			{
				const FJoystickTouchpadFingerKeyMapping& Finger = TouchpadMapping.Fingers[FingerArrayIndex];
				const int32 FingerIndex = Finger.FingerIndex == INDEX_NONE ? FingerArrayIndex : Finger.FingerIndex;
				const FString FingerSource = FString::Printf(TEXT("Touchpad %d, Finger %d"), TouchpadIndex, FingerIndex);

				if (!AddInputKey(
						DeviceIdentifier,
						DeviceKeyMapping,
						Finger.Touch,
						FingerSource + TEXT(", Touch"),
						ECustomKeyType::Button,
						KeyUses,
						OutValidationError)
					|| !AddInputKey(
						DeviceIdentifier,
						DeviceKeyMapping,
						Finger.PositionX,
						FingerSource + TEXT(", Position X"),
						ECustomKeyType::Axis1D,
						KeyUses,
						OutValidationError)
					|| !AddInputKey(
						DeviceIdentifier,
						DeviceKeyMapping,
						Finger.PositionY,
						FingerSource + TEXT(", Position Y"),
						ECustomKeyType::Axis1D,
						KeyUses,
						OutValidationError)
					|| !AddInputKey(
						DeviceIdentifier,
						DeviceKeyMapping,
						Finger.Pressure,
						FingerSource + TEXT(", Pressure"),
						ECustomKeyType::Axis1D,
						KeyUses,
						OutValidationError))
				{
					return false;
				}

				FString PairedKeyName;
				FString PairedKeyDisplayName;
				GetPairedKeyNames(Finger.PositionX, PairedKeyName, PairedKeyDisplayName);
				if (!AddDerivedKey(
						DeviceIdentifier,
						DeviceKeyMapping,
						PairedKeyName,
						PairedKeyDisplayName,
						FingerSource + TEXT(", Position 2D"),
						ECustomKeyType::Axis2D,
						KeyUses,
						OutValidationError)
					|| !ValidatePairedKeyRegistration(
						DeviceIdentifier,
						DeviceKeyMapping,
						Finger.PositionX,
						Finger.PositionY,
						PairedKeyName,
						FingerSource + TEXT(", Position 2D"),
						OutValidationError))
				{
					return false;
				}
			}
		}

		TArray<EDeviceSensorType> SensorTypes;
		DeviceKeyMapping.SensorMappings.GenerateKeyArray(SensorTypes);
		SensorTypes.Sort([](EDeviceSensorType Left, EDeviceSensorType Right)
			{
				return static_cast<uint8>(Left) < static_cast<uint8>(Right);
			});
		for (const EDeviceSensorType SensorType : SensorTypes)
		{
			const FJoystickSensorKeyMapping& SensorMapping =
				DeviceKeyMapping.SensorMappings.FindChecked(SensorType);
			const FString SensorSource =
				FString::Printf(TEXT("Sensor %d"), static_cast<int32>(SensorType));
			if (!AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					SensorMapping.X,
					SensorSource + TEXT(", X"),
					ECustomKeyType::Axis1D,
					KeyUses,
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					SensorMapping.Y,
					SensorSource + TEXT(", Y"),
					ECustomKeyType::Axis1D,
					KeyUses,
					OutValidationError)
				|| !AddInputKey(
					DeviceIdentifier,
					DeviceKeyMapping,
					SensorMapping.Z,
					SensorSource + TEXT(", Z"),
					ECustomKeyType::Axis1D,
					KeyUses,
					OutValidationError))
			{
				return false;
			}
		}

		OutValidationError = FText::GetEmpty();
		return true;
	}
}

void UDeviceInputMappingBase::InitializeValidationContext(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping)
{
	DeviceIdentifier = InDeviceIdentifier;
	DeviceKeyMappingSnapshot = InDeviceKeyMapping;
	ValidateMapping(ValidationError);
}

bool UDeviceInputMappingBase::ValidateMapping(FText& OutValidationError) const
{
	FJoystickDeviceIdentifier EditedDeviceIdentifier;
	FJoystickDeviceKeyMapping EditedDeviceKeyMapping;
	return BuildValidatedDeviceKeyMapping(
		EditedDeviceIdentifier,
		EditedDeviceKeyMapping,
		OutValidationError);
}

bool UDeviceInputMappingBase::BuildValidatedDeviceKeyMapping(
	FJoystickDeviceIdentifier& OutDeviceIdentifier,
	FJoystickDeviceKeyMapping& OutDeviceKeyMapping,
	FText& OutValidationError) const
{
	OutDeviceIdentifier = DeviceIdentifier;
	OutDeviceKeyMapping = DeviceKeyMappingSnapshot;
	ApplyEditedMapping(OutDeviceKeyMapping);
	return ValidateDeviceKeyMapping(OutDeviceIdentifier, OutDeviceKeyMapping, OutValidationError);
}

void UDeviceInputMappingBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ValidateMapping(ValidationError);
}

void UButtonInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
	int32 InButtonIndex,
	const FJoystickButtonKeyMapping& InButtonMapping)
{
	ButtonIndex = InButtonIndex;
	Key = InButtonMapping.Key;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void UButtonInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	FJoystickButtonKeyMapping& ButtonMapping = InOutDeviceKeyMapping.ButtonMappings.FindOrAdd(ButtonIndex);
	ButtonMapping.ButtonIndex = ButtonIndex;
	ButtonMapping.Key = Key;
}

void UAxisInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
	int32 InAxisIndex,
	const FJoystickAxisKeyMapping& InAxisMapping)
{
	AxisIndex = InAxisIndex;
	AxisMapping = InAxisMapping;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void UAxisInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	InOutDeviceKeyMapping.AxisMappings.Add(AxisIndex, AxisMapping);
}

void UHatInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
	int32 InHatIndex,
	const FJoystickHatKeyMapping& InHatMapping)
{
	HatIndex = InHatIndex;
	HatMapping = InHatMapping;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void UHatInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	InOutDeviceKeyMapping.HatMappings.Add(HatIndex, HatMapping);
}

void UBallInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier, 
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping, 
	int32 InBallIndex, 
	const FJoystickBallKeyMapping& InBallMapping)
{
	BallIndex = InBallIndex;
	BallMapping = InBallMapping;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void UBallInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	InOutDeviceKeyMapping.BallMappings.Add(BallIndex, BallMapping);
}

void UTouchpadInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
	int32 InTouchpadIndex,
	const FJoystickTouchpadKeyMapping& InTouchpadKeyMapping)
{
	TouchpadIndex = InTouchpadIndex;
	TouchpadKeyMapping = InTouchpadKeyMapping;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void UTouchpadInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	InOutDeviceKeyMapping.TouchpadMappings.Add(TouchpadIndex, TouchpadKeyMapping);
}

void USensorInputMapping::Initialize(
	const FJoystickDeviceIdentifier& InDeviceIdentifier,
	const FJoystickDeviceKeyMapping& InDeviceKeyMapping,
	EDeviceSensorType InSensorType,
	const FJoystickSensorKeyMapping& InSensorKeyMapping)
{
	SensorType = InSensorType;
	SensorKeyMapping = InSensorKeyMapping;
	InitializeValidationContext(InDeviceIdentifier, InDeviceKeyMapping);
}

void USensorInputMapping::ApplyEditedMapping(FJoystickDeviceKeyMapping& InOutDeviceKeyMapping) const
{
	InOutDeviceKeyMapping.SensorMappings.Add(SensorType, SensorKeyMapping);
}

#undef LOCTEXT_NAMESPACE
