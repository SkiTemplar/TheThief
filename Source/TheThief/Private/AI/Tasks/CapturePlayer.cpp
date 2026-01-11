// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/CapturePlayer.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "AI/GuardCharacter.h"

UCapturePlayer::UCapturePlayer()
{
	NodeName = "Capture Player";
}

EBTNodeResult::Type UCapturePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(AIController->GetPawn()))
	{
		Guard->CapturePlayer();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
