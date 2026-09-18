// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputUserSettings.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "IndependentInputMapper.h"
#include "IndependentInputLocalPlayerSubsystem.h"
#include "IndependentInputMappingContext.h"


UIndependentInputUserSettings* UIndependentInputUserSettings::GetOrCreateForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogIndependentInputMapper, Warning, TEXT("Cannot load input bindings without a local player."));
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(EnhancedInputSubsystem))
	{
		UE_LOG(
			LogIndependentInputMapper,
			Warning,
			TEXT("Cannot load input bindings because the Enhanced Input subsystem is unavailable for local player %d."),
			LocalPlayer->GetLocalPlayerIndex());
		return nullptr;
	}

	if (!EnhancedInputSubsystem->GetUserSettings())
		EnhancedInputSubsystem->InitalizeUserSettings();

	UIndependentInputUserSettings* Settings = EnhancedInputSubsystem->GetUserSettings<UIndependentInputUserSettings>();
	if (!IsValid(Settings))
	{
		UE_LOG(
			LogIndependentInputMapper,
			Warning,
			TEXT("Enhanced Input user settings for local player %d do not use UIndependentInputUserSettings."),
			LocalPlayer->GetLocalPlayerIndex());
	}

	return Settings;
}

bool UIndependentInputUserSettings::FindMappingOverrideForMappingId(const UIndependentInputMappingContext* InputMappingContext, FName MappingId, FIndependentInputBindingOverride& OutBindingOverride) const
{
	OutBindingOverride = FIndependentInputBindingOverride();
	if (!IsValid(InputMappingContext))
		return false;

	if (MappingId.IsNone() || !MappingId.IsValid() || MappingId.ToString().IsEmpty())
		return false;

	// Check if there is a mapping override for the given InputMappingContext and MappingId.
	if (const FIndependentInputBindingSet* FoundBindingSet = Mappings.Find(InputMappingContext))
	{
		if (const FIndependentInputBindingOverride* FoundBindingOverride = FoundBindingSet->Bindings.Find(MappingId))
		{
			OutBindingOverride = *FoundBindingOverride;
			return true;
		}
	}

	// If no override is found, check the default mapping definitions in the InputMappingContext.
	const FIndependentInputMappingDefinition* FoundDefinition = InputMappingContext->GetMappings().Find(MappingId);
	if (FoundDefinition)
	{
		OutBindingOverride.PrimaryKey = FoundDefinition->PrimaryMapping.Key;
		if (FoundDefinition->bSupportsSecondarySlot)
			OutBindingOverride.SecondaryKey = FoundDefinition->SecondaryMapping.Key;

		return true;
	}

	return false;
}

void UIndependentInputUserSettings::UpdatePlayerMapping(UIndependentInputMappingContext* InputMappingContext, FName MappingId, EIndependentInputBindingSlot Slot, FKey NewKey)
{
	if (!IsValid(InputMappingContext))
		return;

	if (MappingId.IsNone() || !MappingId.IsValid())
		return;

	// Check if the mapping definition exists and can be modified.
	FIndependentInputMappingDefinition FoundDefinition;
	if (!InputMappingContext->FindInputMappingDefinitionByMappingId(MappingId, FoundDefinition))
		return;

	if (!FoundDefinition.bCanBeModified)
		return;

	if (Slot != EIndependentInputBindingSlot::Primary && Slot != EIndependentInputBindingSlot::Secondary)
		return;

	if (!FoundDefinition.bSupportsSecondarySlot && Slot == EIndependentInputBindingSlot::Secondary)
		return;

	if (!FoundDefinition.InputCaptureInfo.IsKeyCompatibleForCapture(NewKey))
		return;

	FIndependentInputBindingOverride BindingOverride;
	if (!FindMappingOverrideForMappingId(InputMappingContext, MappingId, BindingOverride))
		return;

	// Start with the current mapping only when this is the first pending edit for it.
	// Subsequent slot edits must retain the other pending slot value.
	FIndependentInputBindingSet& BindingSet = PendingMappings.FindOrAdd(InputMappingContext);
	FIndependentInputBindingOverride* PendingBindingOverride = BindingSet.Bindings.Find(MappingId);
	if (!PendingBindingOverride)
	{
		PendingBindingOverride = &BindingSet.Bindings.Add(MappingId, BindingOverride);
	}

	switch (Slot)
	{
	case EIndependentInputBindingSlot::Primary:
		PendingBindingOverride->PrimaryKey = NewKey;
		break;
	case EIndependentInputBindingSlot::Secondary:
		PendingBindingOverride->SecondaryKey = NewKey;
		break;
	}

	OnSettingsChanged.Broadcast(this);
}

bool UIndependentInputUserSettings::IsSettingsDirty() const
{
	for (const TPair<UIndependentInputMappingContext*, FIndependentInputBindingSet>& PendingMappingPair : PendingMappings)
	{
		// Check if the pending mapping differs from the current mapping in Mappings.
		const FIndependentInputBindingSet* PreviousBindingSet = Mappings.Find(PendingMappingPair.Key);
		for (TPair<FName, FIndependentInputBindingOverride> BindingOverridePair : PendingMappingPair.Value.Bindings)
		{
			if (PreviousBindingSet)
			{
				// Check if the binding override differs from the previous binding set.
				const FIndependentInputBindingOverride* PreviousBinding = PreviousBindingSet->Bindings.Find(BindingOverridePair.Key);
				if (PreviousBinding)
				{
					// Check if the binding override differs from the previous binding.
					if (*PreviousBinding != BindingOverridePair.Value)
						return true;
				}
				else
				{
					// If there was no previous binding, then the pending mapping is dirty.
					return true;
				}
			}
			else
			{
				// If there was no previous binding set but there is a pending mapping, Need to check if the pending mapping differs from the default mapping in the InputMappingContext.
				FIndependentInputBindingOverride DefaultBinding;
				if (!PendingMappingPair.Key->FindDefaultBindingSetByMappingId(BindingOverridePair.Key, DefaultBinding))
					return true;

				if (BindingOverridePair.Value != DefaultBinding)
					return true;
			}
		}
	}

	// If there are no pending mappings, then the settings are not dirty.
	return false;
}

bool UIndependentInputUserSettings::IsUsingDefaultMapping(UIndependentInputMappingContext* InputMappingContext) const
{
	if (!IsValid(InputMappingContext))
		return false;

	if (Mappings.IsEmpty())
		return true;

	const TMap<FName, FIndependentInputMappingDefinition>& DefaultMappings = InputMappingContext->GetMappings();
	const FIndependentInputBindingSet* BindingSet = Mappings.Find(InputMappingContext);
	if (!BindingSet)
		return true;

	for (const TPair<FName, FIndependentInputBindingOverride>& BindingPair : BindingSet->Bindings)
	{
		if (const FIndependentInputMappingDefinition* BindingDefault = DefaultMappings.Find(BindingPair.Key))
		{
			if (BindingDefault->PrimaryMapping.Key != BindingPair.Value.PrimaryKey)
				return false;

			if (BindingDefault->bSupportsSecondarySlot)
			{
				if (BindingDefault->SecondaryMapping.Key != BindingPair.Value.SecondaryKey)
					return false;
			}
		}
	}

	return true;
}

void UIndependentInputUserSettings::ResetInputMappingToDefault(UIndependentInputMappingContext* InputMappingContext)
{
	if (!IsValid(InputMappingContext))
		return;

	UIndependentInputLocalPlayerSubsystem* IndependentInputSubsystem = UIndependentInputLocalPlayerSubsystem::Get(GetLocalPlayer());
	if (!IsValid(IndependentInputSubsystem))
		return;

	const bool bWasActive = IndependentInputSubsystem->HasInputMappingContext(InputMappingContext);
	Mappings.Remove(InputMappingContext);
	PendingMappings.Remove(InputMappingContext);

	// Reset stored overrides for inactive contexts without activating them.
	if (bWasActive)
		IndependentInputSubsystem->ApplyMappingContextMapping(InputMappingContext);

	OnSettingsChanged.Broadcast(this);
	OnSettingsApplied.Broadcast();
}

void UIndependentInputUserSettings::ResetAllMappingsToDefault()
{
	TArray<UIndependentInputMappingContext*> CurrentMappings;
	Mappings.GenerateKeyArray(CurrentMappings);
	Mappings.Empty();
	PendingMappings.Empty();

	if (UIndependentInputLocalPlayerSubsystem* IndependentInputSubsystem = UIndependentInputLocalPlayerSubsystem::Get(GetLocalPlayer()))
	{
		for (UIndependentInputMappingContext* Mapping : CurrentMappings)
		{
			if (IndependentInputSubsystem->HasInputMappingContext(Mapping))
				IndependentInputSubsystem->ApplyMappingContextMapping(Mapping);
		}
	}

	OnSettingsChanged.Broadcast(this);
	OnSettingsApplied.Broadcast();
}

void UIndependentInputUserSettings::DiscardPendingChanges()
{
	bool bBroadcast = PendingMappings.Num() > 0;
	PendingMappings.Empty();

	if (bBroadcast)
		OnSettingsChanged.Broadcast(this);
}

void UIndependentInputUserSettings::ApplySettings()
{
	UIndependentInputLocalPlayerSubsystem* IndependentInputSubsystem = UIndependentInputLocalPlayerSubsystem::Get(GetLocalPlayer());
	if (!IsValid(IndependentInputSubsystem))
		return;

	// Apply the pending mappings to the current mappings and apply them to the input subsystem.
	for (const TPair<UIndependentInputMappingContext*, FIndependentInputBindingSet>& PendingMappingPair : PendingMappings)
	{
		bool bWasDirty = false;

		// Capture previous BindingSet if already added.
		FIndependentInputBindingSet* PreviousBindingSet = Mappings.Find(PendingMappingPair.Key);

		// Making sure the PendingMapping is added to the Mappings list.
		FIndependentInputBindingSet& BindingSet = Mappings.FindOrAdd(PendingMappingPair.Key);

		for (TPair<FName, FIndependentInputBindingOverride> BindingOverridePair : PendingMappingPair.Value.Bindings)
		{
			if (PreviousBindingSet)
			{
				const FIndependentInputBindingOverride* PreviousBinding = PreviousBindingSet->Bindings.Find(BindingOverridePair.Key);
				if (PreviousBinding)
				{
					if (*PreviousBinding != BindingOverridePair.Value)
						bWasDirty = true;
				}
				else
				{
					bWasDirty = true;
				}
			}
			else
			{
				// If previous binding set was not found but we have new pending mapping, it means the mapping is dirty for given IndependentInputMappingContext.
				bWasDirty = true;
			}

			BindingSet.Bindings.FindOrAdd(BindingOverridePair.Key) = BindingOverridePair.Value;
			if (bWasDirty)
				IndependentInputSubsystem->ApplyMappingContextMapping(PendingMappingPair.Key);
		}
	}

	PendingMappings.Empty();
	Super::ApplySettings();
}
