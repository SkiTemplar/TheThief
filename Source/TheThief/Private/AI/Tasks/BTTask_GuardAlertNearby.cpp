// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_GuardAlertNearby.h"
#include "AI/GuardCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_GuardAlertNearby::UBTTask_GuardAlertNearby()
{
	NodeName = "Alert Nearby Guards";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_GuardAlertNearby::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller || !Controller->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	AGuardCharacter* MyGuard = Cast<AGuardCharacter>(Controller->GetPawn());
	if (!MyGuard)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;

	if (Target)
	{
		MyGuard->AlertNearbyGuards(Target);
	}

	return EBTNodeResult::Succeeded;
}

FString UBTTask_GuardAlertNearby::GetStaticDescription() const
{
	return FString::Printf(TEXT("Alert guards within %.0f units about %s"), AlertRadius, *TargetActorKey.SelectedKeyName.ToString());
}

