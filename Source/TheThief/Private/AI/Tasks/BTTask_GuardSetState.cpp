// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_GuardSetState.h"
#include "AI/GuardCharacter.h"
#include "AI/EnemyController.h"
#include "AIController.h"

UBTTask_GuardSetState::UBTTask_GuardSetState()
{
	NodeName = "Guard Set State";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_GuardSetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	if (AEnemyController* EnemyController = Cast<AEnemyController>(Controller))
	{
		EnemyController->SetState(NewState);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_GuardSetState::GetStaticDescription() const
{
	FString StateStr;
	switch (NewState)
	{
	case EGuardState::Patrol: StateStr = TEXT("Patrol"); break;
	case EGuardState::Investigate: StateStr = TEXT("Investigate"); break;
	case EGuardState::Chase: StateStr = TEXT("Chase"); break;
	case EGuardState::Attack: StateStr = TEXT("Attack"); break;
	}
	return FString::Printf(TEXT("Set State: %s"), *StateStr);
}

