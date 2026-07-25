// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManagerSettingsDetails.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"

#include "IndependentInputEditorSubsystem.h"
#include "IndependentInputManagerSettings.h"
#include "IndependentInputSubsystem.h"
#include "Slates/SIndependentInputDeviceManager.h"

#define LOCTEXT_NAMESPACE "FIndependentInputManagerSettingsDetails"


TSharedRef<IDetailCustomization> FIndependentInputManagerSettingsDetails::MakeInstance()
{
	return MakeShareable(new FIndependentInputManagerSettingsDetails);
}

void FIndependentInputManagerSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	check(ObjectsBeingCustomized.Num() == 1);
	TWeakObjectPtr<UIndependentInputManagerSettings> Settings = Cast<UIndependentInputManagerSettings>(ObjectsBeingCustomized[0].Get());

	IDetailCategoryBuilder& DeviceManagerCategory = DetailBuilder.EditCategory("Device Manager");
	DeviceManagerCategory.AddCustomRow(LOCTEXT("IndendentInputManagerCategoryLabel", "IndendentInputManagerCategory"))
		.ValueContent()
		[
			SNew(SButton)
				.Text(LOCTEXT("OpenDeviceManagerLabel", "Open Device Manager"))
				.ToolTipText(LOCTEXT("OpenDeviceManagerTooltipLabel", "Open device manager window."))
				.OnClicked_Lambda([this]
					{
						FGlobalTabmanager::Get()->TryInvokeTab(InputManagerTabId);

						TSharedPtr<SDockTab> InputManagerTab = FGlobalTabmanager::Get()->TryInvokeTab(InputManagerTabId);
						if (!InputManagerTab.IsValid())
							return FReply::Handled();

						TSharedRef<SWidget> TabContent = InputManagerTab->GetContent();
						TSharedPtr<SIndependentInputDeviceManager> DeviceManager = StaticCastSharedPtr<SIndependentInputDeviceManager>(TabContent.ToSharedPtr());
						if (DeviceManager)
						{
							if (const UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
							{
								for (const TPair<FInputDeviceInstanceId, FJoystickDeviceInfo>& DeviceInfo : InputSubsystem->GetConnectedDevices())
								{
									DeviceManager->DevicePluggedIn(DeviceInfo.Value);
								}
							}
						}

						return FReply::Handled();
					})
		];
}

#undef LOCTEXT_NAMESPACE
