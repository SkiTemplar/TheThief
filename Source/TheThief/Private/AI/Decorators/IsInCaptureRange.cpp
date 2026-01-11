// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorators/IsInCaptureRange.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "AI/GuardCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UIsInCaptureRange::UIsInCaptureRange()
{
	NodeName = "Is In Capture Range";
}

bool UIsInCaptureRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	AGuardCharacter* Guard = Cast<AGuardCharacter>(AIController->GetPawn());
	if (!Guard) return false;

	return Guard->IsPlayerInCaptureRange();
}
