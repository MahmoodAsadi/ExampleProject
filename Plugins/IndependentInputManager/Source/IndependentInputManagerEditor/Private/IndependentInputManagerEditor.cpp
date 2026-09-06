// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManagerEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/Paths.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#include "IndependentInputEditorSubsystem.h"
#include "IndependentInputManagerSettings.h"
#include "IndependentInputManagerSettingsDetails.h"
#include "Slates/SIndependentInputDeviceManager.h"

#define LOCTEXT_NAMESPACE "FIndependentInputManagerEditorModule"


void FIndependentInputManagerEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		InputManagerTabId,
		FOnSpawnTab::CreateRaw(this, &FIndependentInputManagerEditorModule::SpawnInputDeviceManagerTab))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.PadEvent_16x"))
		.SetDisplayName(LOCTEXT("InputDeviceManagerTabDisplayNameLabel", "Input Device Manager"))
		.SetTooltipText(LOCTEXT("InputDeviceManagerTabTooltipLabel", "Open the Independent Input Device Manager."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this, &FIndependentInputManagerEditorModule::RegisterMenus));

	RegisterPropertyLayout();
}

void FIndependentInputManagerEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(InputManagerTabId);
	UnregisterPropertyLayout();

}

TSharedRef<SDockTab> FIndependentInputManagerEditorModule::SpawnInputDeviceManagerTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SIndependentInputDeviceManager)
		];
}

void FIndependentInputManagerEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	const FText InputDeviceManagerLabel = LOCTEXT("InputDeviceManagerTabLabel", "Input Device Manager");
	const FText InputDeviceManagerTooltip = LOCTEXT("InputDeviceManagerTabTooltipLabel", "Open the Independent Input Device Manager.");
	const FSlateIcon InputDeviceManagerIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.PadEvent_16x");
	const FUIAction OpenInputDeviceManagerAction(FExecuteAction::CreateLambda([]
		{
			FGlobalTabmanager::Get()->TryInvokeTab(InputManagerTabId);
		}));

	UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
	FToolMenuSection& ToolsSection = ToolsMenu->FindOrAddSection(TEXT("IndependentInputManager"));

	ToolsSection.AddMenuEntry(
		TEXT("OpenInputDeviceManager"),
		InputDeviceManagerLabel,
		InputDeviceManagerTooltip,
		InputDeviceManagerIcon,
		OpenInputDeviceManagerAction
	);

	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.LevelEditorToolBar.User"));
	FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection(TEXT("IndependentInputManager"));

	ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(
		TEXT("OpenInputDeviceManagerToolbar"),
		OpenInputDeviceManagerAction,
		InputDeviceManagerLabel,
		InputDeviceManagerTooltip,
		InputDeviceManagerIcon));
}

void FIndependentInputManagerEditorModule::RegisterPropertyLayout() const
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->RegisterCustomClassLayout(UIndependentInputManagerSettings::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FIndependentInputManagerSettingsDetails::MakeInstance));
	}
}

void FIndependentInputManagerEditorModule::UnregisterPropertyLayout() const
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomClassLayout(UIndependentInputManagerSettings::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FIndependentInputManagerEditorModule, IndependentInputManagerEditor)
