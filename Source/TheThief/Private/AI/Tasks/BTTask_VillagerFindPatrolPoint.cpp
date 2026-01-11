// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerFindPatrolPoint.h"
#include "AI/PatrolPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UBTTask_VillagerFindPatrolPoint::UBTTask_VillagerFindPatrolPoint()
{
	NodeName = "Villager Find Patrol Point";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerFindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (CachedPatrolPoints.Num() == 0)
	{
		CachePatrolPoints(Controller->GetWorld());
	}

	APatrolPoint* LastPoint = Cast<APatrolPoint>(BB->GetValueAsObject(LastPatrolPointKey.SelectedKeyName));
	APatrolPoint* NewPoint = GetRandomPatrolPoint(LastPoint);
	
	FVector TargetLocation;
	
	if (NewPoint)
	{
		TargetLocation = NewPoint->GetRandomPositionInRadius();
		BB->SetValueAsObject(LastPatrolPointKey.SelectedKeyName, NewPoint);
	}
	else
	{
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Controller->GetWorld());
		if (NavSys)
		{
			FNavLocation NavLocation;
			if (NavSys->GetRandomReachablePointInRadius(Controller->GetPawn()->GetActorLocation(), 1000.f, NavLocation))
			{
				TargetLocation = NavLocation.Location;
			}
			else
			{
				return EBTNodeResult::Failed;
			}
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}

	BB->SetValueAsVector(DestinationKey.SelectedKeyName, TargetLocation);
	return EBTNodeResult::Succeeded;
}

void UBTTask_VillagerFindPatrolPoint::CachePatrolPoints(UWorld* World)
{
	CachedPatrolPoints.Empty();
	
	TArray<AActor*> FoundPoints;
	UGameplayStatics::GetAllActorsOfClass(World, APatrolPoint::StaticClass(), FoundPoints);
	
	for (AActor* Actor : FoundPoints)
	{
		if (APatrolPoint* Point = Cast<APatrolPoint>(Actor))
		{
			CachedPatrolPoints.Add(Point);
		}
	}
}

APatrolPoint* UBTTask_VillagerFindPatrolPoint::GetRandomPatrolPoint(APatrolPoint* LastPoint)
{
	if (CachedPatrolPoints.Num() == 0) return nullptr;
	if (CachedPatrolPoints.Num() == 1) return CachedPatrolPoints[0];

	TArray<APatrolPoint*> ValidPoints;
	for (APatrolPoint* Point : CachedPatrolPoints)
	{
		if (Point && Point != LastPoint)
		{
			ValidPoints.Add(Point);
		}
	}

	if (ValidPoints.Num() == 0) return CachedPatrolPoints[0];

	return ValidPoints[FMath::RandRange(0, ValidPoints.Num() - 1)];
}

FString UBTTask_VillagerFindPatrolPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("Find patrol point -> %s"), *DestinationKey.SelectedKeyName.ToString());
}
