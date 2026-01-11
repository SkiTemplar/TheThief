// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerFleeFrom.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_VillagerFleeFrom::UBTTask_VillagerFleeFrom()
{
	NodeName = "Villager Flee From Threat";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerFleeFrom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller || !Controller->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	AActor* ThreatActor = Cast<AActor>(BB->GetValueAsObject(ThreatActorKey.SelectedKeyName));
	if (!ThreatActor)
	{
		return EBTNodeResult::Failed;
	}

	APawn* MyPawn = Controller->GetPawn();
	FVector MyLocation = MyPawn->GetActorLocation();
	FVector ThreatLocation = ThreatActor->GetActorLocation();

	FVector FleeDirection = (MyLocation - ThreatLocation).GetSafeNormal();
	FVector FleeLocation = MyLocation + FleeDirection * FleeDistance;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Controller->GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(FleeLocation, NavLocation, FVector(300.f, 300.f, 300.f)))
		{
			FleeLocation = NavLocation.Location;
		}
	}

	BB->SetValueAsVector(FleeDestinationKey.SelectedKeyName, FleeLocation);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_VillagerFleeFrom::GetStaticDescription() const
{
	return FString::Printf(TEXT("Flee from %s -> %s"), 
		*ThreatActorKey.SelectedKeyName.ToString(),
		*FleeDestinationKey.SelectedKeyName.ToString());
}

