// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"

#include "IndependentInputMappingTypes.h"

#include "IndependentInputKeySelector.generated.h"

class SIndependentInputKeySelector;

/**
 * 
 */
UCLASS()
class INDEPENDENTINPUTMAPPER_API UIndependentInputKeySelector : public UWidget
{
	GENERATED_UCLASS_BODY()
	
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeySelected, FInputChord, SelectedKey);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKeySelectionUpdated);


public:

	/** Called whenever a new key is selected by the user. */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnKeySelected OnKeySelected;

	/** Called whenever the key selection mode starts or stops. */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnKeySelectionUpdated OnSelectingKeyChanged;

	/** Called whenever the key selection mode canceled (by pressing scape key(s)). */
	UPROPERTY(BlueprintAssignable, Category = "Widget Event")
	FOnKeySelectionUpdated OnSelectingKeyCanceled;

protected:

	/** The button style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Appearance)
	FButtonStyle ButtonStyle;

	/** The button style used at runtime */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Appearance)
	FTextBlockStyle TextStyle;

	/** The currently selected key chord. */
	UPROPERTY(BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetSelectedKey", FieldNotify, Category = "Key Selection")
	FInputChord SelectedKey;

	/** The amount of blank space around the text used to display the currently selected key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Appearance)
	FMargin Margin;

	/** Sets the text which is displayed while selecting keys. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetKeySelectionText", Category = Appearance)
	FText KeySelectionText;

	/** Sets the text to display when no key text is available or not selecting a key. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter = "SetNoKeySpecifiedText", Category = Appearance)
	FText NoKeySpecifiedText;

	/** The information about the key capture. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, Category = Appearance)
	FIndependentInputCaptureInfo InputKeyCaptureInfo;

	/** Whether Escape and configured cancel keys cancel selection instead of being captured. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter = "GetEscapeCancelsSelection", Setter = "SetEscapeCancelsSelection", Category = "Key Selection")
	bool bEscapeCancelsSelection = true;

	/** How the key text should be justified within its display area (left/center/right). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	TEnumAsByte<ETextJustify::Type> TextJustification;

	/** Vertical alignment of the key text inside the widget (top/center/bottom). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	TEnumAsByte<EVerticalAlignment> TextVerticalAlignment;

	/** Horizontal alignment of the key text inside the widget (left/center/right). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	TEnumAsByte<EHorizontalAlignment> TextHorizontalAlignment;

	//~ Begin UObject Interface

	/** Serializes this widget's state to or from an archive. */
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

	//~ Begin UWidget Interface

	/** Rebuilds the underlying Slate widget. Called by the UMG system. */
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface

	/** Releases any held Slate resources (widgets, brushes, etc.). */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

//~ Begin UWidget Interface
#if WITH_EDITOR	
	/** Returns the palette category shown in the editor for this widget. */
	virtual const FText GetPaletteCategory() override;
#endif

	/** Synchronizes UPROPERTY values to the Slate widget. */
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

public:

	/** Returns the style of the button used to start key selection mode. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FButtonStyle& GetButtonStyle() const;

	/** Sets the style of the button used to start key selection mode. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetButtonStyle(const FButtonStyle& InButtonStyle);

	/** Returns the style of the text used inside the button. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FTextBlockStyle& GetTextStyle() const;

	/** Sets the style of the text used inside the button. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetTextStyle(const FTextBlockStyle& InTextStyle);

	/** Returns the currently selected key. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	FInputChord GetSelectedKey() const;

	/** Sets the currently selected key. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetSelectedKey(const FInputChord& InSelectedKey);

	/** Returns the text which is displayed while selecting keys. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FText& GetKeySelectionText() const;

	/** Sets the text which is displayed while selecting keys. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetKeySelectionText(FText InKeySelectionText);

	/** Returns the text to display when no key text is available or not selecting a key. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FText& GetNoKeySpecifiedText() const;

	/** Sets the text to display when no key text is available or not selecting a key. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetNoKeySpecifiedText(FText InNoKeySpecifiedText);

	/** Returns the amount of blank space around the text used to display the currently selected key. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FMargin& GetMargin() const;

	/** Sets the amount of blank space around the text used to display the currently selected key. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetMargin(const FMargin& InMargin);

	/** Returns the capture settings that control how input keys are detected. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	const FIndependentInputCaptureInfo& GetInputKeyCaptureInfo() const;

	/** Sets the capture settings that control how input keys are detected. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetInputKeyCaptureInfo(const FIndependentInputCaptureInfo& InInputKeyCaptureInfo);

	/** Returns whether Escape and configured cancel keys cancel key selection. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	bool GetEscapeCancelsSelection() const;

	/** Sets whether Escape and configured cancel keys cancel key selection. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetEscapeCancelsSelection(bool bInEscapeCancelsSelection);

	/** Returns the current vertical alignment used for key text rendering. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	EVerticalAlignment GetTextVerticalAlignment() const { return TextVerticalAlignment; }

	/** Sets the vertical alignment used for key text rendering. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetTextVerticalAlignment(const EVerticalAlignment& VerticalAlignment);

	/** Returns the current horizontal alignment used for key text rendering. */
	UFUNCTION(BlueprintPure, Category = "Independent Input Key Selector")
	EHorizontalAlignment GetTextHorizontalAlignment() const { return TextHorizontalAlignment; }

	/** Sets the horizontal alignment used for key text rendering. */
	UFUNCTION(BlueprintCallable, Category = "Independent Input Key Selector")
	void SetTextHorizontalAlignment(const EHorizontalAlignment& HorizontalAlignment);

private:

	/** Called when a key has been selected in the Slate widget; forwards selection to this UObject. */
	virtual void HandleKeySelected(const FInputChord& InSelectedKey);

	/** Called when the Slate widget enters or exits key selection mode. */
	void HandleSelectingKeyChanged();

	/** Called when key selection was canceled (for example via Escape). */
	void HandleSelectingKeyCanceled();

	/** The input key selector widget managed by this object. */
	TSharedPtr<SIndependentInputKeySelector> MyInputKeySelector;

};
