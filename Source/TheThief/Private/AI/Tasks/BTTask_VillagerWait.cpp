// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerWait.h"
#include "AI/PatrolPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_VillagerWait::UBTTask_VillagerWait()
{
	NodeName = "Villager Wait";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_VillagerWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	// Intentar obtener el tiempo del PatrolPoint
	if (BB)
	{
		APatrolPoint* PatrolPoint = Cast<APatrolPoint>(BB->GetValueAsObject(LastPatrolPointKey.SelectedKeyName));
		if (PatrolPoint)
		{
			RemainingTime = PatrolPoint->GetRandomWaitTime();
			return EBTNodeResult::InProgress;
		}
	}
	
	// Fallback: usar tiempo por defecto
	RemainingTime = FMath::RandRange(DefaultMinWaitTime, DefaultMaxWaitTime);
	return EBTNodeResult::InProgress;
}

void UBTTask_VillagerWait::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	RemainingTime -= DeltaSeconds;
	
	if (RemainingTime <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_VillagerWait::GetStaticDescription() const
{
	if (LastPatrolPointKey.SelectedKeyName != NAME_None)
	{
		return FString::Printf(TEXT("Wait (from %s or %.1f-%.1f sec)"), 
			*LastPatrolPointKey.SelectedKeyName.ToString(),
			DefaultMinWaitTime, DefaultMaxWaitTime);
	}
	return FString::Printf(TEXT("Wait %.1f-%.1f sec"), DefaultMinWaitTime, DefaultMaxWaitTime);
}
