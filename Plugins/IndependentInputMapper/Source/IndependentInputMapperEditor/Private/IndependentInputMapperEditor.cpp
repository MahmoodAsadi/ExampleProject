// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputMapperEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "ToolMenus.h"

#include "IndependentInputMappingContext_AssetAction.h"

#define LOCTEXT_NAMESPACE "FIndependentInputMapperEditorModule"

class FIndependentInputMapperEditorStyle final : public FSlateStyleSet
{
public:
	FIndependentInputMapperEditorStyle()
		: FSlateStyleSet(TEXT("IndependentInputMapperEditorStyle"))
	{
		SetContentRoot(FPaths::EnginePluginsDir() / TEXT("EnhancedInput/Content/Editor/Slate"));

		static const FVector2D Icon16(16.0f, 16.0f);
		static const FVector2D Icon64(64.0f, 64.0f);

		Set(TEXT("ClassIcon.IndependentInputMappingContext"), new IMAGE_BRUSH_SVG(TEXT("Icons/InputMappingContext_16"), Icon16));
		Set(TEXT("ClassThumbnail.IndependentInputMappingContext"), new IMAGE_BRUSH_SVG(TEXT("Icons/InputMappingContext_64"), Icon64));
	}
};

EAssetTypeCategories::Type FIndependentInputMapperEditorModule::IndependentInputAssetsCategory;

void FIndependentInputMapperEditorModule::StartupModule()
{
	EditorStyle = MakeShared<FIndependentInputMapperEditorStyle>();
	FSlateStyleRegistry::RegisterSlateStyle(*EditorStyle);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	IndependentInputAssetsCategory = AssetTools.FindAdvancedAssetCategory("Input");

	// Create Asset type actions.
	RegisteredAssetTypeActions.Emplace(MakeShareable(new FAssetTypeActions_IndependentInputMappingContext()));

	// Register Asset type actions.
	for (auto Action : RegisteredAssetTypeActions)
	{
		AssetTools.RegisterAssetTypeActions(Action);
	}
}

void FIndependentInputMapperEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	if (FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		IAssetTools& AssetTools = AssetToolsModule->Get();
		for (auto Action : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action);
		}
	}

	if (EditorStyle.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*EditorStyle);
		EditorStyle.Reset();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FIndependentInputMapperEditorModule, IndependentInputMapperEditor)
