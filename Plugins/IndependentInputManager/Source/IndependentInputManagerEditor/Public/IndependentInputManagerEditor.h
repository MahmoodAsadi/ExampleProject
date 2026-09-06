#pragma once

#include "AssetTypeCategories.h"
#include "Modules/ModuleManager.h"

class FSlateStyleSet;
class FSpawnTabArgs;
class IAssetTypeActions;
class SDockTab;

class FIndependentInputManagerEditorModule : public IModuleInterface
{

public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:

	TSharedRef<SDockTab> SpawnInputDeviceManagerTab(const FSpawnTabArgs& SpawnTabArgs);
	void RegisterMenus();
	void RegisterPropertyLayout() const;
	void UnregisterPropertyLayout() const;

};
