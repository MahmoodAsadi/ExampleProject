// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SHatInputView.h"

#include "Rendering/SlateRenderTransform.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SHatInputView"

enum ESDLHatDirection : uint8
{
	ECentered = 0x00u,
	EUp = 0x01u,
	ERight = 0x02u,
	EDown = 0x04u,
	ELeft = 0x08u,
	ERightUp = (ERight | EUp),
	ERightDown = (ERight | EDown),
	ELeftUp = (ELeft | EUp),
	ELeftDown = (ELeft | EDown)

};


void SHatInputView::Construct(const FArguments& InArgs)
{
	HatMapping = InArgs._HatMapping;
	Direction = InArgs._Direction;
	OnClicked = InArgs._OnClicked;

	CircleBrush.TintColor = FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f, 0.0f));
	CircleBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
	CircleBrush.OutlineSettings.Color = FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f, 1.0f));
	CircleBrush.OutlineSettings.Width = 2.0f;
	CircleBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
	CircleBrush.OutlineSettings.bUseBrushTransparency = false;

	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("HatInputViewTooltip", "Click to configure this Hat Mapping."))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.13f, 0.15f, 0.18f, 1.0f))
				.Padding(FMargin(12.0f, 10.0f))
				[
					SNew(SVerticalBox)

					// Title
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
							{
								return FText::Format(LOCTEXT("HatTitleLabel", "Hat {0}"),
									FText::AsNumber(HatMapping.HatIndex));
							})
						.TextStyle(FAppStyle::Get(), "NormalText")
						.Font(FAppStyle::GetFontStyle("NormalFontBold"))
					]

					// Hat Surface
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(10.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(SBox)
						.HeightOverride(120.0f)
						.WidthOverride(120.0f)
						[
							SNew(SOverlay)

							// Circle
							+ SOverlay::Slot()
							.Padding(10.0f)
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							[
								SNew(SBorder)
								.BorderImage(&CircleBrush)
								.ColorAndOpacity(FLinearColor(0.45f, 0.45f, 0.45f, 0.0f))
							]

							// Center dot.
							+ SOverlay::Slot()
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("●")))
								.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 9.0f))
								.ShadowOffset(0.0f)
								.ColorAndOpacity(FLinearColor(0.45f, 0.45f, 0.45f, 1.0f))
							]

							// Arrow
							+ SOverlay::Slot()
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("→")))
								.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), 16.0f))
								.ShadowOffset(0.0f)
								.ColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
								.RenderTransformPivot(FVector2D(0.5f, 0.5f))
								.Visibility_Lambda([this]
									{
										ESDLHatDirection HatDirection = (ESDLHatDirection)Direction.Get();
										return HatDirection == ESDLHatDirection::ECentered
											? EVisibility::Hidden
											: EVisibility::HitTestInvisible;
									})
								.RenderTransform_Lambda([this]
									{
										float AngleDegree = 0.0f;
										ESDLHatDirection HatDirection = (ESDLHatDirection)Direction.Get();
										switch (HatDirection)
										{
											case ECentered: AngleDegree = 0.0f; break;
											case EUp: AngleDegree = -90.0f; break;
											case ERight: AngleDegree = 0.0f; break;
											case EDown: AngleDegree = 90.0f; break;
											case ELeft: AngleDegree = 180.0f; break;
											case ERightUp: AngleDegree = -45.0f; break;
											case ERightDown: AngleDegree = 45.0f; break;
											case ELeftUp: AngleDegree = -135.0f; break;
											case ELeftDown: AngleDegree = 135.0f; break;
											default: AngleDegree = 0.0f; break;
										}
										const float Angle = FMath::DegreesToRadians(AngleDegree);
										const FVector2D Translation(62.0f * FMath::Cos(Angle), 62.0f * FMath::Sin(Angle));
										return FSlateRenderTransform(FQuat2D(Angle), Translation);
									})
							]
						]
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
