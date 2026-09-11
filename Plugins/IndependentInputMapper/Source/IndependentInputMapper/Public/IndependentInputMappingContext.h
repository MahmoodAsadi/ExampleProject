// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "IndependentInputMappingTypes.h"

#include "IndependentInputMappingContext.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, config = Input)
class INDEPENDENTINPUTMAPPER_API UIndependentInputMappingContext : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/** Returns the input mapping definitions for this context. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Mapping Context")
	const TMap<FName, FIndependentInputMappingDefinition>& GetMappings() const { return Mappings; }

	/**
	* Finds the default binding set for a given mapping ID. 
	* 
	* @param MappingId The ID of the mapping to find the default binding set for.
	* @param OutBindingSet The output parameter that will hold the default binding set if found.
	* 
	* @return true if the default binding set was found, false otherwise.
	*/
	UFUNCTION(BlueprintPure, Category = "Independent Input Mapping Context")
	bool FindDefaultBindingSetByMappingId(FName MappingId, FIndependentInputBindingOverride& OutBindingSet) const;

	/**
	* Finds the input mapping definition for a given mapping ID.
	* 
	* @param MappingId The ID of the mapping to find the definition for.
	* @param OutMappingDefinition The output parameter that will hold the input mapping definition if found.
	* 
	* @return true if the input mapping definition was found, false otherwise.
	*/
	UFUNCTION(BlueprintPure, Category = "Independent Input Mapping Context")
	bool FindInputMappingDefinitionByMappingId(FName MappingId, FIndependentInputMappingDefinition& OutMappingDefinition) const;

	// Localized context descriptor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Description, DisplayName = "Description")
	FText ContextDescription;

	EMappingContextInputModeFilterOptions GetInputModeFilterOptions() const { return InputModeFilterOptions; }
	FGameplayTagQuery GetInputModeQueryOverride() const { return InputModeQueryOverride; }
	EMappingContextRegistrationTrackingMode GetRegistrationTrackingMode() const { return RegistrationTrackingMode; }
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR


protected:

	/** Input binding definitions, Map of Binding name and the binding definition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = InputMapping)
	TMap<FName, FIndependentInputMappingDefinition> Mappings;

	/**
	 * Defines how this input mapping context should be filtered based on the current input mode.
	 *
	 * Default is Use Project Default Query.
	 *
	 * @Note: bEnableInputModeFiltering must be enabled in the UEnhancedInputDeveloperSettings for this to be considered.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Modes", meta = (EditCondition = "ShouldShowInputModeQuery()"))
	EMappingContextInputModeFilterOptions InputModeFilterOptions = EMappingContextInputModeFilterOptions::UseProjectDefaultQuery;

	/**
	 * Tag Query which will be matched against the current Enhanced Input Subsystem's input mode if InputModeFilterOptions is set to UseCustomQuery.
	 *
	 * If this tag query does not match with the current input mode tag container, then the mappings
	 * will not be processed.
	 *
	 * @Note: bEnableInputModeFiltering must be enabled in the UEnhancedInputDeveloperSettings for this to be considered.
	 */
	UPROPERTY(config, EditDefaultsOnly, BlueprintReadOnly, Category = "Input Modes", meta = (EditConditionHides, EditCondition = "ShouldShowInputModeQuery() && InputModeFilterOptions == EMappingContextInputModeFilterOptions::UseCustomQuery"))
	FGameplayTagQuery InputModeQueryOverride;

	/**
	 * Select the behaviour when multiple AddMappingContext() calls are made for this Input Mapping Context
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Registration")
	EMappingContextRegistrationTrackingMode RegistrationTrackingMode = EMappingContextRegistrationTrackingMode::Untracked;

	/** UFUNCTION helper to be used as en edit condition for displaying input mode query releated properties. */
	UFUNCTION()
	static bool ShouldShowInputModeQuery();

};
