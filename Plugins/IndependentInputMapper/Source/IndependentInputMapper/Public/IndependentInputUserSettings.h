// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include "IndependentInputMappingTypes.h"

#include "IndependentInputUserSettings.generated.h"

class UIndependentInputMappingContext;

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMAPPER_API UIndependentInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()
	
public:

	/** Returns the settings object owned by LocalPlayer, initializing it when possible. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	static UIndependentInputUserSettings* GetOrCreateForLocalPlayer(ULocalPlayer* LocalPlayer);


	/** Controller overrides partitioned by stable hardware profile identifier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = InputMapping)
	TMap<UIndependentInputMappingContext*, FIndependentInputBindingSet> Mappings;

	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	bool FindMappingOverrideForMappingId(const UIndependentInputMappingContext* InputMappingContext, FName MappingId, FIndependentInputBindingOverride& OutBindingOverride) const;

	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	void UpdatePlayerMapping(UIndependentInputMappingContext* InputMappingContext, FName MappingId, EIndependentInputBindingSlot Slot, FKey NewKey);

	UFUNCTION(BlueprintPure, Category = "Independent Input|Independent Input User Settings")
	bool IsSettingsDirty() const;

	UFUNCTION(BlueprintPure, Category = "Independent Input|Independent Input User Settings")
	bool IsUsingDefaultMapping(UIndependentInputMappingContext* InputMappingContext) const;

	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	void ResetInputMappingToDefault(UIndependentInputMappingContext* InputMappingContext);

	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	void ResetAllMappingsToDefault();

	UFUNCTION(BlueprintCallable, Category = "Independent Input|Independent Input User Settings")
	void DiscardPendingChanges();

	UPROPERTY(Transient)
	TMap<UIndependentInputMappingContext*, FIndependentInputBindingSet> PendingMappings;

	virtual void ApplySettings() override;

};
