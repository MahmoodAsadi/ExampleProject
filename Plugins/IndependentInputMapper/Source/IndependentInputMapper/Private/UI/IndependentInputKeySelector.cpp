// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/IndependentInputKeySelector.h"

#include "UObject/FrameworkObjectVersion.h"
#include "Styling/DefaultStyleCache.h"

#include "UI/Slates/SIndependentInputKeySelector.h"

#define LOCTEXT_NAMESPACE "UIndependentInputKeySelector"


UIndependentInputKeySelector::UIndependentInputKeySelector(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ButtonStyle = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetButtonStyle();
	TextStyle = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetTextBlockStyle();

	KeySelectionText = NSLOCTEXT("InputKeySelector", "ModifingKeySelectionText", "Press Any Key...");
	NoKeySpecifiedText = NSLOCTEXT("InputKeySelector", "UnAssignedKeySelectionText", "UnAssigned");
	SelectedKey = FInputChord(EKeys::Invalid);
}

void UIndependentInputKeySelector::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);
}

TSharedRef<SWidget> UIndependentInputKeySelector::RebuildWidget()
{
	MyInputKeySelector = SNew(SIndependentInputKeySelector)
		.SelectedKey(SelectedKey)
		.Margin(Margin)
		.ButtonStyle(&ButtonStyle)
		.TextStyle(&TextStyle)
		.KeySelectionText(KeySelectionText)
		.NoKeySpecifiedText(NoKeySpecifiedText)
		.InputKeyCaptureInfo(InputKeyCaptureInfo)
		.EscapeCancelsSelection(bEscapeCancelsSelection)
		.OnKeySelected(BIND_UOBJECT_DELEGATE(SIndependentInputKeySelector::FOnKeySelected, HandleKeySelected))
		.OnSelectingKeyChanged(BIND_UOBJECT_DELEGATE(SIndependentInputKeySelector::FOnSelectingKeyChanged, HandleSelectingKeyChanged))
		.OnKeySelectionCanceled(BIND_UOBJECT_DELEGATE(SIndependentInputKeySelector::FOnKeySelectionCanceled, HandleSelectingKeyCanceled));

	return MyInputKeySelector.ToSharedRef();
}

void UIndependentInputKeySelector::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyInputKeySelector.Reset();
}

const FText UIndependentInputKeySelector::GetPaletteCategory()
{
	return LOCTEXT("Advanced", "Advanced");
}

void UIndependentInputKeySelector::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyInputKeySelector.IsValid())
		return;

	MyInputKeySelector->SetSelectedKey(SelectedKey);
	MyInputKeySelector->SetMargin(Margin);
	MyInputKeySelector->SetButtonStyle(&ButtonStyle);
	MyInputKeySelector->SetTextStyle(&TextStyle);
	MyInputKeySelector->SetKeySelectionText(KeySelectionText);
	MyInputKeySelector->SetNoKeySpecifiedText(NoKeySpecifiedText);
	MyInputKeySelector->SetInputKeyCaptureInfo(InputKeyCaptureInfo);
	MyInputKeySelector->SetEscapeCancelsSelection(bEscapeCancelsSelection);
	MyInputKeySelector->SetTextVerticalAlignment(TextVerticalAlignment);
	MyInputKeySelector->SetTextHorizontalAlignment(TextHorizontalAlignment);
	MyInputKeySelector->SetTextJustification(TextJustification);
}

const FButtonStyle& UIndependentInputKeySelector::GetButtonStyle() const
{
	return ButtonStyle;
}

void UIndependentInputKeySelector::SetButtonStyle(const FButtonStyle& InButtonStyle)
{
	ButtonStyle = InButtonStyle;

	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetButtonStyle(&ButtonStyle);
}

const FTextBlockStyle& UIndependentInputKeySelector::GetTextStyle() const
{
	return TextStyle;
}

void UIndependentInputKeySelector::SetTextStyle(const FTextBlockStyle& InTextStyle)
{
	TextStyle = InTextStyle;

	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetTextStyle(&TextStyle);
}

FInputChord UIndependentInputKeySelector::GetSelectedKey() const
{
	return SelectedKey;
}

void UIndependentInputKeySelector::SetSelectedKey(const FInputChord& InSelectedKey)
{
	if (SelectedKey != InSelectedKey)
	{
		BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::SelectedKey);
		if (MyInputKeySelector.IsValid())
			MyInputKeySelector->SetSelectedKey(InSelectedKey);

		SelectedKey = InSelectedKey;
	}
}

const FText& UIndependentInputKeySelector::GetKeySelectionText() const
{
	return KeySelectionText;
}

void UIndependentInputKeySelector::SetKeySelectionText(FText InKeySelectionText)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetKeySelectionText(InKeySelectionText);

	KeySelectionText = MoveTemp(InKeySelectionText);
}

const FText& UIndependentInputKeySelector::GetNoKeySpecifiedText() const
{
	return NoKeySpecifiedText;
}

void UIndependentInputKeySelector::SetNoKeySpecifiedText(FText InNoKeySpecifiedText)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetNoKeySpecifiedText(InNoKeySpecifiedText);

	NoKeySpecifiedText = MoveTemp(InNoKeySpecifiedText);
}

const FMargin& UIndependentInputKeySelector::GetMargin() const
{
	return Margin;
}

void UIndependentInputKeySelector::SetMargin(const FMargin& InMargin)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetMargin(InMargin);

	Margin = InMargin;
}

const FIndependentInputCaptureInfo& UIndependentInputKeySelector::GetInputKeyCaptureInfo() const
{
	return InputKeyCaptureInfo;
}

void UIndependentInputKeySelector::SetInputKeyCaptureInfo(const FIndependentInputCaptureInfo& InInputKeyCaptureInfo)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetInputKeyCaptureInfo(InInputKeyCaptureInfo);

	InputKeyCaptureInfo = InInputKeyCaptureInfo;
}

bool UIndependentInputKeySelector::GetEscapeCancelsSelection() const
{
	return bEscapeCancelsSelection;
}

void UIndependentInputKeySelector::SetEscapeCancelsSelection(bool bInEscapeCancelsSelection)
{
	bEscapeCancelsSelection = bInEscapeCancelsSelection;

	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetEscapeCancelsSelection(bEscapeCancelsSelection);
}

void UIndependentInputKeySelector::SetTextVerticalAlignment(const EVerticalAlignment& VerticalAlignment)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetTextVerticalAlignment(VerticalAlignment);
}

void UIndependentInputKeySelector::SetTextHorizontalAlignment(const EHorizontalAlignment& HorizontalAlignment)
{
	if (MyInputKeySelector.IsValid())
		MyInputKeySelector->SetTextHorizontalAlignment(HorizontalAlignment);
}

void UIndependentInputKeySelector::HandleKeySelected(const FInputChord& InSelectedKey)
{
	SelectedKey = InSelectedKey;
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::SelectedKey);
	OnKeySelected.Broadcast(SelectedKey);
}

void UIndependentInputKeySelector::HandleSelectingKeyChanged()
{
	OnSelectingKeyChanged.Broadcast();
	
	if (MyInputKeySelector.IsValid() && MyInputKeySelector->GetIsSelectingKey())
	{
		if (InputKeyCaptureInfo.bClearOnStartCapture)
			SetSelectedKey(FInputChord(EKeys::Invalid));
	}
}

void UIndependentInputKeySelector::HandleSelectingKeyCanceled()
{
	OnSelectingKeyCanceled.Broadcast();
}

#undef LOCTEXT_NAMESPACE
