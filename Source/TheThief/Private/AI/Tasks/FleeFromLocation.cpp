// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/FleeFromLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UFleeFromLocation::UFleeFromLocation()
{
	NodeName = "Flee From Location";
}

EBTNodeResult::Type UFleeFromLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	FVector ThreatLocation = BB->GetValueAsVector(ThreatLocationKey);
	FVector MyLocation = AIController->GetPawn()->GetActorLocation();

	// Calcular dirección opuesta a la amenaza
	FVector FleeDirection = (MyLocation - ThreatLocation).GetSafeNormal();
	FVector FleeLocation = MyLocation + (FleeDirection * FleeDistance);

	// Buscar punto navegable
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(FleeLocation, NavLocation, FVector(500.f, 500.f, 500.f)))
		{
			BB->SetValueAsVector(FleeDestinationKey, NavLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
