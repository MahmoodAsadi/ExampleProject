// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "AssetTypeActions/AssetTypeActions_DataAsset.h"
#include "AssetTypeCategories.h"
#include "Modules/ModuleInterface.h"

#include "IndependentInputMappingContext_AssetAction.generated.h"

class UIndependentInputMappingContext;

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMAPPEREDITOR_API UIndependentInputMappingContext_Factory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
public:

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	virtual FText GetDisplayName() const override;
	virtual FString GetDefaultNewAssetName() const override;
	virtual bool ShouldShowInNewMenu() const override { return true; }
	virtual FName GetNewAssetThumbnailOverride() const override;
	virtual FName GetNewAssetIconOverride() const override;
	// End of UFactory interface

};

class INDEPENDENTINPUTMAPPEREDITOR_API FAssetTypeActions_IndependentInputMappingContext : public FAssetTypeActions_DataAsset
{
public:

	// ~Start IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override { return FColor(255, 153, 121, 255); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual UClass* GetSupportedClass() const override;
	virtual const FSlateBrush* GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const override;
	virtual const FSlateBrush* GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const override;
	// ~End IAssetTypeActions interface

};
