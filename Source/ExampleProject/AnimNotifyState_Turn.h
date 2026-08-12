// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Turn.generated.h"

/**
 * 
 */
UCLASS()
class EXAMPLEPROJECT_API UAnimNotifyState_Turn : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = Turn)
	FName CurveName = "Turn";

protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY()
	float CurrentDelta = 0.0f;

	UPROPERTY()
	float Duration = 0.0f;

	UPROPERTY()
	FRotator InitialRotation;

};
