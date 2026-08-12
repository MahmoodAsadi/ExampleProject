// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_Turn.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"


void UAnimNotifyState_Turn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsValid(MeshComp))
		return;

	UAnimInstance* OwningAnimInstance = MeshComp->GetAnimInstance();
	if (!IsValid(OwningAnimInstance))
		return;

	APawn* OwningPawn = OwningAnimInstance->TryGetPawnOwner();
	if (!IsValid(OwningPawn))
		return;
	
	CurrentDelta = 0.0f;
	Duration = TotalDuration;
	InitialRotation = OwningPawn->GetActorRotation();
}

void UAnimNotifyState_Turn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp))
		return;

	UAnimInstance* OwningAnimInstance = MeshComp->GetAnimInstance();
	if (!IsValid(OwningAnimInstance))
		return;

	APawn* OwningPawn = OwningAnimInstance->TryGetPawnOwner();
	if (!IsValid(OwningPawn))
		return;

	const FRotator& ControlRotation = FRotator(0.0f, OwningPawn->GetControlRotation().Yaw, 0.0f);
	const float Alpha = CurrentDelta / Duration;
	CurrentDelta += FrameDeltaTime;
	FRotator NewRotation = UKismetMathLibrary::RLerp(InitialRotation, ControlRotation, Alpha, true);
	OwningPawn->SetActorRotation(NewRotation);
}
