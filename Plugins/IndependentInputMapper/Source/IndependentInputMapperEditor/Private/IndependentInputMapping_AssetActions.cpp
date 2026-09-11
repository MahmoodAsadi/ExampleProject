// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputMapping_AssetActions.h"

#include "ClassIconFinder.h"
#include "Styling/SlateIconFinder.h"

#include "IndependentInputMapperEditor.h"
#include "IndependentInputMappingContext.h"
#include "IndependentInputVisualCollection.h"

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




UInputKeyVisualCollection_Factory::UInputKeyVisualCollection_Factory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UInputKeyVisualCollection::StaticClass();
}

UObject* UInputKeyVisualCollection_Factory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UInputKeyVisualCollection>(InParent, Class, Name, Flags | RF_Transactional, Context);
}

uint32 UInputKeyVisualCollection_Factory::GetMenuCategories() const
{
	return FIndependentInputMapperEditorModule::GetIndependentInputAssetsCategory();
}

FText UInputKeyVisualCollection_Factory::GetDisplayName() const
{
	return FText::FromString("Input Key Visual Collection");
}

FString UInputKeyVisualCollection_Factory::GetDefaultNewAssetName() const
{
	return FString("NewInputKeyVisualCollection");
}

FName UInputKeyVisualCollection_Factory::GetNewAssetThumbnailOverride() const
{
	return FName("ClassThumbnail.SlateWidgetStyleAsset");
}

FName UInputKeyVisualCollection_Factory::GetNewAssetIconOverride() const
{
	return FName("ClassIcon.SlateWidgetStyleAsset");
}

FText FAssetTypeActions_InputKeyVisualCollection::GetName() const
{
	return LOCTEXT("InputVisualKeyCollectionNameLabel", "Input Key Visual Collection");
}

uint32 FAssetTypeActions_InputKeyVisualCollection::GetCategories()
{
	return FIndependentInputMapperEditorModule::GetIndependentInputAssetsCategory();
}

FText FAssetTypeActions_InputKeyVisualCollection::GetAssetDescription(const FAssetData& AssetData) const
{
	return LOCTEXT("IndependentInputContextDescLabel", "A collection of input key visual info.");
}

UClass* FAssetTypeActions_InputKeyVisualCollection::GetSupportedClass() const
{
	return UInputKeyVisualCollection::StaticClass();
}

const FSlateBrush* FAssetTypeActions_InputKeyVisualCollection::GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return FClassIconFinder::FindThumbnailForClass(USlateWidgetStyleAsset::StaticClass());
}

const FSlateBrush* FAssetTypeActions_InputKeyVisualCollection::GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return FSlateIconFinder::FindIconBrushForClass(USlateWidgetStyleAsset::StaticClass());
}

#undef LOCTEXT_NAMESPACE
