// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputMappingContext_AssetAction.h"

#include "ClassIconFinder.h"
#include "Styling/SlateIconFinder.h"

#include "IndependentInputMapperEditor.h"
#include "IndependentInputMappingContext.h"

#define LOCTEXT_NAMESPACE "IndependentInputMappingContext_AssetAction"

UIndependentInputMappingContext_Factory::UIndependentInputMappingContext_Factory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UIndependentInputMappingContext::StaticClass();
}

UObject* UIndependentInputMappingContext_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UIndependentInputMappingContext>(InParent, Class, Name, Flags | RF_Transactional, Context);
}

uint32 UIndependentInputMappingContext_Factory::GetMenuCategories() const
{
	return FIndependentInputMapperEditorModule::GetIndependentInputAssetsCategory();
}

FText UIndependentInputMappingContext_Factory::GetDisplayName() const
{
	return FText::FromString("Independent Input Mapping Context");
}

FString UIndependentInputMappingContext_Factory::GetDefaultNewAssetName() const
{
	return FString("NewIndependentInputMappingContext");
}

FName UIndependentInputMappingContext_Factory::GetNewAssetThumbnailOverride() const
{
	return FName("ClassThumbnail.IndependentInputMappingContext");
}

FName UIndependentInputMappingContext_Factory::GetNewAssetIconOverride() const
{
	return FName("ClassIcon.IndependentInputMappingContext");
}

FText FAssetTypeActions_IndependentInputMappingContext::GetName() const
{
	return LOCTEXT("IndependentInputMappingContextNameLabel", "Independent Input Mapping Context");
}

uint32 FAssetTypeActions_IndependentInputMappingContext::GetCategories()
{
	return FIndependentInputMapperEditorModule::GetIndependentInputAssetsCategory();
}

FText FAssetTypeActions_IndependentInputMappingContext::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("IndependentInputContextDescLabel", "A collection of device input to action mappings.");
}

UClass* FAssetTypeActions_IndependentInputMappingContext::GetSupportedClass() const
{
	return UIndependentInputMappingContext::StaticClass();
}

const FSlateBrush* FAssetTypeActions_IndependentInputMappingContext::GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return FClassIconFinder::FindThumbnailForClass(UIndependentInputMappingContext::StaticClass());
}

const FSlateBrush* FAssetTypeActions_IndependentInputMappingContext::GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return FSlateIconFinder::FindIconBrushForClass(UIndependentInputMappingContext::StaticClass());
}

#undef LOCTEXT_NAMESPACE
