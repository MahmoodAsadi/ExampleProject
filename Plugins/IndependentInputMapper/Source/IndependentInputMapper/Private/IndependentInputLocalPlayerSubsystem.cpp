// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputLocalPlayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "IndependentInputMappingContext.h"
#include "IndependentInputUserSettings.h"


class UCustomInputMappingContext : public UInputMappingContext
{
public:

	/**
	 * Creates a new UInputMappingContext based on the provided UIndependentInputMappingContext and applies any user-defined overrides.
	 *
	 * @param IndependentInputMappingContext		The independent input mapping context to base the new context on.
	 * @param LocalPlayer							The local player for which the new context is being created.
	 * @param Querier								The object that is requesting the creation of the new context.
	 * @return A new UInputMappingContext with applied overrides, or nullptr if creation fails.
	 */
	static UInputMappingContext* MakeInputMappingContext(
		const UIndependentInputMappingContext* IndependentInputMappingContext, 
		ULocalPlayer* LocalPlayer, 
		UObject* Querier)
	{
		if (!IsValid(LocalPlayer))
			return nullptr;

		if (!IsValid(IndependentInputMappingContext))
			return nullptr;

		UCustomInputMappingContext* NewInputMappingContext = NewObject<UCustomInputMappingContext>(Querier);
		NewInputMappingContext->ContextDescription = IndependentInputMappingContext->ContextDescription;
		NewInputMappingContext->InputModeFilterOptions = IndependentInputMappingContext->GetInputModeFilterOptions();
		NewInputMappingContext->InputModeQueryOverride = IndependentInputMappingContext->GetInputModeQueryOverride();
		NewInputMappingContext->RegistrationTrackingMode = IndependentInputMappingContext->GetRegistrationTrackingMode();
		UIndependentInputUserSettings* UserSettings = UIndependentInputUserSettings::GetOrCreateForLocalPlayer(LocalPlayer);
		if (!IsValid(UserSettings))
			return nullptr;

		// Retrieve the binding set for the given independent input mapping context, if it exists.
		FIndependentInputBindingSet BindingSet;
		if (UserSettings->Mappings.Contains(IndependentInputMappingContext))
			BindingSet = UserSettings->Mappings[IndependentInputMappingContext];

		for (const TPair<FName, FIndependentInputMappingDefinition>& MappingPair : IndependentInputMappingContext->GetMappings())
		{
			const FIndependentInputMappingDefinition& Mapping = MappingPair.Value;
			
			// Determine the primary and secondary keys, applying any user-defined overrides if they exist.
			FKey PrimaryKey = Mapping.PrimaryMapping.Key;
			FKey SecondaryKey = Mapping.SecondaryMapping.Key;
			const FIndependentInputBindingOverride* InputBindingOverride = BindingSet.Bindings.Find(MappingPair.Key);
			if (InputBindingOverride != nullptr)
			{
				PrimaryKey = InputBindingOverride->PrimaryKey;
				SecondaryKey = InputBindingOverride->SecondaryKey;
			}

			// Add the primary mapping.
			FEnhancedActionKeyMapping PrimaryMapping(Mapping.InputAction.IsNull() ? nullptr : Mapping.InputAction.LoadSynchronous(), PrimaryKey);
			PrimaryMapping.Triggers = Mapping.PrimaryMapping.Triggers;
			PrimaryMapping.Modifiers = Mapping.PrimaryMapping.Modifiers;
			NewInputMappingContext->Mappings.Add(PrimaryMapping);

			// Add the secondary mapping if it is supported.
			if (Mapping.bSupportsSecondarySlot)
			{
				FEnhancedActionKeyMapping SecondaryMapping(Mapping.InputAction.IsNull() ? nullptr : Mapping.InputAction.LoadSynchronous(), SecondaryKey);
				SecondaryMapping.Triggers = Mapping.SecondaryMapping.Triggers;
				SecondaryMapping.Modifiers = Mapping.SecondaryMapping.Modifiers;
				NewInputMappingContext->Mappings.Add(SecondaryMapping);
			}
		}
		
		return NewInputMappingContext;
	}
};

UIndependentInputLocalPlayerSubsystem* UIndependentInputLocalPlayerSubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
		return nullptr;

	return InLocalPlayer->GetSubsystem<UIndependentInputLocalPlayerSubsystem>();
}

UEnhancedInputLocalPlayerSubsystem* UIndependentInputLocalPlayerSubsystem::GetEnhancedInputSubsystem() const
{
	if (!IsValid(GetLocalPlayer()))
		return nullptr;

	return GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
}

UInputMappingContext* UIndependentInputLocalPlayerSubsystem::AddInputMappingContext(
	const UIndependentInputMappingContext* InMappingContext, 
	int32 Priority, 
	const FModifyContextOptions& Options)
{
	if (!IsValid(InMappingContext))
		return nullptr;

	TObjectPtr<UInputMappingContext> MappingContext;

	/**
	 * Check if the mapping context has already been registered. If it has, use the existing mapping context.
	 * If it hasn't, create a new mapping context based on the independent input mapping context and register it.
	 */
	FIndependentInputMappingContextsInfo* FoundMappingContextInfo = RegisteredMappingContexts.Find(InMappingContext);
	if (!FoundMappingContextInfo || !IsValid(FoundMappingContextInfo->MappingContext))
		MappingContext = UCustomInputMappingContext::MakeInputMappingContext(InMappingContext, GetLocalPlayer(), this);
	else
		MappingContext = FoundMappingContextInfo->MappingContext;

	RegisteredMappingContexts.Add(InMappingContext, FIndependentInputMappingContextsInfo(MappingContext, Priority, Options));
	
	/**
	 * Add the mapping context to the enhanced input subsystem if it is valid.
	 * This ensures that the input mappings are applied and active for the local player.
	 */
	if (GetEnhancedInputSubsystem())
		GetEnhancedInputSubsystem()->AddMappingContext(MappingContext, Priority, Options);

	return MappingContext;
}

bool UIndependentInputLocalPlayerSubsystem::RemoveInputMappingContext(
	const UIndependentInputMappingContext* InMappingContext, 
	const FModifyContextOptions& Options)
{
	if (!IsValid(InMappingContext))
		return false;

	/**
	 * Check if the mapping context has been registered. If it has, remove it from the enhanced input subsystem and the registered mapping contexts.
	 * If it hasn't, return false to indicate that the mapping context was not found.
	 */
	FIndependentInputMappingContextsInfo* FoundMappingContextInfo = RegisteredMappingContexts.Find(InMappingContext);
	if (FoundMappingContextInfo && IsValid(FoundMappingContextInfo->MappingContext) && GetEnhancedInputSubsystem())
	{
		RegisteredMappingContexts.Remove(InMappingContext);

		/**
		 * Remove the mapping context from the enhanced input subsystem.
		 * This ensures that the input mappings are no longer active for the local player.
		 */
		GetEnhancedInputSubsystem()->RemoveMappingContext(FoundMappingContextInfo->MappingContext, Options);
		return true;
	}

	return false;
}

bool UIndependentInputLocalPlayerSubsystem::HasInputMappingContext(const UIndependentInputMappingContext* InMappingContext) const
{
	if (!IsValid(InMappingContext))
		return false;

	/**
	 * Check if the mapping context has been registered. If it has, check if it is present in the enhanced input subsystem.
	 * If it hasn't, return false to indicate that the mapping context was not found.
	 */
	const FIndependentInputMappingContextsInfo* MappingContextInfo = RegisteredMappingContexts.Find(InMappingContext);
	if (!MappingContextInfo || !IsValid(MappingContextInfo->MappingContext))
		return false;

	/**
	 * Check if the mapping context is present in the enhanced input subsystem.
	 * This ensures that the input mappings are active for the local player.
	 */
	if (GetEnhancedInputSubsystem())
		return GetEnhancedInputSubsystem()->HasMappingContext(MappingContextInfo->MappingContext);

	return false;
}

UInputMappingContext* UIndependentInputLocalPlayerSubsystem::FindInputMappingContextForIndependentInputMapping(const UIndependentInputMappingContext* InMappingContext) const
{
	if (!IsValid(InMappingContext))
		return nullptr;

	// Check if the mapping context has been registered. If it has, return the associated input mapping context.
	const FIndependentInputMappingContextsInfo* MappingContextInfo = RegisteredMappingContexts.Find(InMappingContext);
	if (!MappingContextInfo)
		return nullptr;

	return MappingContextInfo->MappingContext;
}

void UIndependentInputLocalPlayerSubsystem::ApplyMappingContextMapping(const UIndependentInputMappingContext* InMappingContext)
{
	if (!IsValid(InMappingContext))
		return;

	/**
	* Reapply the mapping context to ensure that the latest bindings are used.
	* This is done by removing the mapping context and then adding it back with the same priority and options.
	*/
	int32 Priority = 0;
	FModifyContextOptions ModifyOptions = FModifyContextOptions();
	if (FIndependentInputMappingContextsInfo* MappingContextInfo = RegisteredMappingContexts.Find(InMappingContext))
	{
		ModifyOptions = MappingContextInfo->ModifyOptions;
		Priority = MappingContextInfo->Priority;
		RemoveInputMappingContext(InMappingContext);
	}

	AddInputMappingContext(InMappingContext, Priority, ModifyOptions);
}
