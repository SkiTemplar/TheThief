// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_GuardInvestigateArea.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_GuardInvestigateArea::UBTTask_GuardInvestigateArea()
{
	NodeName = "Guard Investigate Area";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_GuardInvestigateArea::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector InvestigateLocation = BB->GetValueAsVector("LastKnownLocation");
	
	// Verificar si la ubicación es válida (no es FLT_MAX o cero)
	if (InvestigateLocation.IsNearlyZero() || InvestigateLocation.X > 1e30f)
	{
		// Fallback: usar posición actual del guardia
		InvestigateLocation = Controller->GetPawn()->GetActorLocation();
	}
	
	// Generar un punto aleatorio dentro del radio de investigación
	FNavLocation RandomNavLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Controller->GetWorld());
	
	if (NavSys)
	{
		// Intentar encontrar un punto navegable aleatorio dentro del radio
		bool bFound = NavSys->GetRandomReachablePointInRadius(InvestigateLocation, InvestigationRadius, RandomNavLocation);
		
		if (bFound)
		{
			BB->SetValueAsVector("CurrentDestination", RandomNavLocation.Location);
			UE_LOG(LogTemp, Warning, TEXT("GuardInvestigateArea: Found random point at %s"), *RandomNavLocation.Location.ToString());
			return EBTNodeResult::Succeeded;
		}
	}
	
	// Fallback: si no encuentra punto navegable, usar un offset aleatorio simple
	FVector RandomOffset = FVector(
		FMath::RandRange(-InvestigationRadius, InvestigationRadius),
		FMath::RandRange(-InvestigationRadius, InvestigationRadius),
		0.f
	);
	
	FVector FinalDestination = InvestigateLocation + RandomOffset;
	BB->SetValueAsVector("CurrentDestination", FinalDestination);
	
	UE_LOG(LogTemp, Warning, TEXT("GuardInvestigateArea: Using fallback point at %s"), *FinalDestination.ToString());
	
	return EBTNodeResult::Succeeded;
}

FString UBTTask_GuardInvestigateArea::GetStaticDescription() const
{
	return FString::Printf(TEXT("Get random point within %.0f units of LastKnownLocation"), InvestigationRadius);
}
