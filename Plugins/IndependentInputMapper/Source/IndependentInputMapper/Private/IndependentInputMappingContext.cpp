// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputMappingContext.h"

#include "EnhancedInputDeveloperSettings.h"


bool UIndependentInputMappingContext::FindDefaultBindingSetByMappingId(FName MappingId, FIndependentInputBindingOverride& OutBindingSet) const
{
    OutBindingSet = FIndependentInputBindingOverride();
    const FIndependentInputMappingDefinition* FoundDefinition = Mappings.Find(MappingId);
    if (!FoundDefinition)
        return false;

    OutBindingSet.PrimaryKey = FoundDefinition->PrimaryMapping.Key.IsValid() ? FoundDefinition->PrimaryMapping.Key : FKey();
    OutBindingSet.SecondaryKey = FoundDefinition->SecondaryMapping.Key.IsValid() && FoundDefinition->bSupportsSecondarySlot ? FoundDefinition->SecondaryMapping.Key : FKey();
    return true;
}

bool UIndependentInputMappingContext::FindInputMappingDefinitionByMappingId(FName MappingId, FIndependentInputMappingDefinition& OutMappingDefinition) const
{
    OutMappingDefinition = FIndependentInputMappingDefinition();
	const FIndependentInputMappingDefinition* FoundDefinition = Mappings.Find(MappingId);
	if (FoundDefinition)
	{
		OutMappingDefinition = *FoundDefinition;
		return true;
	}

    return false;
}

#if WITH_EDITOR
void UIndependentInputMappingContext::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (!PropertyChangedEvent.Property)
        return;

    for (TPair<FName, FIndependentInputMappingDefinition>& Mapping : Mappings)
    {
        Mapping.Value.bCanEditMappingId = false;
        Mapping.Value.OwningMappingContext = this;
        Mapping.Value.MappingId = Mapping.Key;
    }
}
#endif // WITH_EDITOR

bool UIndependentInputMappingContext::ShouldShowInputModeQuery()
{
    return GetDefault<UEnhancedInputDeveloperSettings>()->bEnableInputModeFiltering;
}
