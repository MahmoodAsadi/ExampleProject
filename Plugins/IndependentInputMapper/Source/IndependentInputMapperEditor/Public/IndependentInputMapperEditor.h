#pragma once

#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;
class IAssetTypeActions;

class FIndependentInputMapperEditorModule : public IModuleInterface
{

public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	static EAssetTypeCategories::Type GetIndependentInputAssetsCategory() { return IndependentInputAssetsCategory; }

private:

	static EAssetTypeCategories::Type IndependentInputAssetsCategory;
	TSharedPtr<FSlateStyleSet> EditorStyle;
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

};
