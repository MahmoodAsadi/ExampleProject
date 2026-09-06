// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Subsystems/LocalPlayerSubsystem.h"

#include "IndependentInputMappingTypes.h"

#include "IndependentInputLocalPlayerSubsystem.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UIndependentInputMappingContext;
class UIndependentInputUserSettings;

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMAPPER_API UIndependentInputLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
	friend UIndependentInputUserSettings;

public:

	static UIndependentInputLocalPlayerSubsystem* Get(const ULocalPlayer* InLocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

	/**
	 * Adds an input mapping context to the local player subsystem.
	 *
	 * @param InMappingContext		The input mapping context to add.
	 * @param Priority				The priority of the input mapping context.
	 * @param Options				Options for modifying the context.
	 * 
	 * @return The added input mapping context, or nullptr if the context was not added.
	 */
	UFUNCTION(BlueprintCallable, Category = IndependentInputLocalPlayerSubsystem, meta = (AutoCreateRefTerm = "Options"))
	virtual UInputMappingContext* AddInputMappingContext(const UIndependentInputMappingContext* InMappingContext, int32 Priority, const FModifyContextOptions& Options = FModifyContextOptions());

	/**
	 * Removes an input mapping context from the local player subsystem.
	 *
	 * @param InMappingContext		The input mapping context to remove.
	 * @param Options				Options for modifying the context.
	 * 
	 * @return true if the input mapping context was removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = IndependentInputLocalPlayerSubsystem, meta = (AutoCreateRefTerm = "Options"))
	virtual bool RemoveInputMappingContext(const UIndependentInputMappingContext* InMappingContext, const FModifyContextOptions& Options = FModifyContextOptions());

	/**
	 * Checks if the local player subsystem has the specified input mapping context.
	 *
	 * @param InMappingContext		The input mapping context to check.
	 * 
	 * @return true if the input mapping context is present, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = IndependentInputLocalPlayerSubsystem)
	bool HasInputMappingContext(const UIndependentInputMappingContext* InMappingContext) const;

	/**
	 * Finds the input mapping context associated with the specified independent input mapping context.
	 *
	 * @param InMappingContext		The independent input mapping context to find.
	 * 
	 * @return The associated input mapping context, or nullptr if not found.
	 */
	UFUNCTION(BlueprintPure, Category = IndependentInputLocalPlayerSubsystem)
	UInputMappingContext* FindInputMappingContextForIndependentInputMapping(const UIndependentInputMappingContext* InMappingContext) const;

protected:

	/**
	 * Applies the mapping context associated with the specified independent input mapping context.
	 *
	 * @param InMappingContext		The independent input mapping context to apply.
	 */
	void ApplyMappingContextMapping(const UIndependentInputMappingContext* InMappingContext);


	// Mapping context information for registered independent input mapping contexts.
	UPROPERTY()
	TMap<TObjectPtr<const UIndependentInputMappingContext>, FIndependentInputMappingContextsInfo> RegisteredMappingContexts;

};
