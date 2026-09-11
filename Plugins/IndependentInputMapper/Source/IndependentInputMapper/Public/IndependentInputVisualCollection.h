// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "IndependentInputMappingTypes.h"

#include "IndependentInputVisualCollection.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class INDEPENDENTINPUTMAPPER_API UInputKeyVisualCollection : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/**
	* Finds the visual info for a given key. If the key has specific visual info defined in the KeyVisuals map, it will return that info. Otherwise, it will return false.
	* 
	* @param Key The input key to find visual info for.
	* @param OutVisualInfo The output parameter that will hold the visual info if found.
	* 
	* @return true if visual info was found for the key, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Independent Input Mapper|Input Visuals")
	bool FindKeyVisualInfo(const FKey& Key, FInputKeyVisualInfo& OutVisualInfo) const;

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "Independent Input Mapper|Input Visuals")
	FSlateBrush GetDefaultBorderBrush() const { return DefaultBorderBrush; }

protected:

	// If a key does not have a specific visual info, this will be used as the default visual info for that key with the key display name as the label.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Visuals)
	FSlateBrush DefaultBorderBrush;

	// The Key visual info map is used to define the visual representation of specific input keys. Each entry in the map associates an FKey with its corresponding FInputKeyVisualInfo, which contains information such as the display name, icon, and other visual properties for that key.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Visuals)
	TMap<FKey, FInputKeyVisualInfo> KeyVisuals;

public:

	// The name of the device this visual collection is for. This is used to identify the device in the input mapping system.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Description)
	FName DeviceName;

	// The description of this visual collection.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Description)
	FText Description;

};
