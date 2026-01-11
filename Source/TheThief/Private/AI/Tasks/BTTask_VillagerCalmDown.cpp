// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerCalmDown.h"
#include "AI/VillagerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_VillagerCalmDown::UBTTask_VillagerCalmDown()
{
	NodeName = "Villager Calm Down";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerCalmDown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Controller->GetPawn());
	if (!Villager)
	{
		return EBTNodeResult::Failed;
	}

	Villager->SetVillagerState(EVillagerState::Walking);

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->ClearValue("ThreatActor");
		BB->ClearValue("TargetGuard");
		BB->SetValueAsBool("IsFleeing", false);
		BB->SetValueAsBool("IsAlertingGuard", false);
	}

	return EBTNodeResult::Succeeded;
}

FString UBTTask_VillagerCalmDown::GetStaticDescription() const
{
	return TEXT("Reset villager to normal state");
}

