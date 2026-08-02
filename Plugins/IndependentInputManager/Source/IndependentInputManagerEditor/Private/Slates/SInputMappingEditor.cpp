// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SInputMappingEditor.h"

#include "IDetailsView.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#include "DeviceInputMappings.h"

#define LOCTEXT_NAMESPACE "SInputMappingEditor"


void SInputMappingEditor::Construct(const FArguments& InArgs)
{
	OnSave = InArgs._OnSave;
	OnCancel = InArgs._OnCancel;
	ObjectToModify.Reset(InArgs._InputMappingObject);

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bShowObjectLabel = false;
	DetailsViewArgs.bShowDifferingPropertiesOption = false;
	DetailsViewArgs.bShowModifiedPropertiesOption = false;
	DetailsViewArgs.bShowSectionSelector = false;

	FPropertyEditorModule& PropertyEditorModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(ObjectToModify.Get());

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				DetailsView
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 8.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNullWidget::NullWidget
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString("Save"))
					.OnClicked_Lambda([this]
						{
							if (const UDeviceInputMappingBase* InputMapping =
								Cast<UDeviceInputMappingBase>(ObjectToModify.Get()))
							{
								FText ValidationError;
								if (!InputMapping->ValidateMapping(ValidationError))
								{
									FMessageDialog::Open(
										EAppMsgCategory::Warning,
										EAppMsgType::Ok,
										ValidationError,
										LOCTEXT("InvalidInputKeyTitle", "Invalid Input Key"));
									return FReply::Handled();
								}
							}

							OnSave.ExecuteIfBound(ObjectToModify.Get());
							return FReply::Handled();
						})
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(8.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SButton)
					.Text(FText::FromString("Cancel"))
					.OnClicked(OnCancel)
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE
