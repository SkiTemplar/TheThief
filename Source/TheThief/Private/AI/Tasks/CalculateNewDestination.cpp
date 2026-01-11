// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/CalculateNewDestination.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/PatrolPoint.h"
#include "Kismet/GameplayStatics.h"

UCalculateNewDestination::UCalculateNewDestination()
{
	NodeName = "Calculate New Destination";
}

EBTNodeResult::Type UCalculateNewDestination::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!MyController) 
	{
		UE_LOG(LogTemp, Warning, TEXT("CalculateNewDestination: No AIController!"));
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}
	
	// Cachear puntos si no lo hemos hecho
	if (PatrolPoints.Num() == 0)
	{
		CachePatrolPoints(MyController->GetWorld());
		UE_LOG(LogTemp, Warning, TEXT("CalculateNewDestination: Found %d patrol points"), PatrolPoints.Num());
	}
	
	// Obtener el último punto del Blackboard
	APatrolPoint* PreviousPoint = Cast<APatrolPoint>(BB->GetValueAsObject(LastPatrolPointKey.SelectedKeyName));
	
	// Obtener punto aleatorio
	APatrolPoint* NextPoint = GetRandomPatrolPoint(PreviousPoint);
	
	FVector Destination;
	
	if (NextPoint)
	{
		// Usar patrol point
		Destination = NextPoint->GetRandomPositionInRadius();
		// Guardar el PatrolPoint en el Blackboard para que el Wait lo use
		BB->SetValueAsObject(LastPatrolPointKey.SelectedKeyName, NextPoint);
		UE_LOG(LogTemp, Display, TEXT("CalculateNewDestination: Using PatrolPoint, Destination: %s"), *Destination.ToString());
	}
	else
	{
		// FALLBACK: Si no hay patrol points, moverse a una posición aleatoria cercana
		APawn* Pawn = MyController->GetPawn();
		if (!Pawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("CalculateNewDestination: No Pawn!"));
			return EBTNodeResult::Failed;
		}
		
		FVector CurrentLocation = Pawn->GetActorLocation();
		float RandomAngle = FMath::RandRange(0.f, 360.f);
		float RandomDistance = FMath::RandRange(300.f, 800.f);
		
		Destination = CurrentLocation + FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomDistance,
			0.f
		);
		
		BB->ClearValue(LastPatrolPointKey.SelectedKeyName);
		UE_LOG(LogTemp, Warning, TEXT("CalculateNewDestination: NO PatrolPoints! Using random position: %s"), *Destination.ToString());
	}
	
	BB->SetValueAsVector(DestinationKey.SelectedKeyName, Destination);
	
	return EBTNodeResult::Succeeded;
}

void UCalculateNewDestination::CachePatrolPoints(UWorld* World)
{
	PatrolPoints.Empty();
	
	TArray<AActor*> FoundPoints;
	UGameplayStatics::GetAllActorsOfClass(World, APatrolPoint::StaticClass(), FoundPoints);
	
	for (AActor* Actor : FoundPoints)
	{
		if (APatrolPoint* Point = Cast<APatrolPoint>(Actor))
		{
			PatrolPoints.Add(Point);
		}
	}
}

APatrolPoint* UCalculateNewDestination::GetRandomPatrolPoint(APatrolPoint* PreviousPoint)
{
	if (PatrolPoints.Num() == 0) return nullptr;
	if (PatrolPoints.Num() == 1) return PatrolPoints[0];
	
	// Filtrar el punto anterior
	TArray<APatrolPoint*> ValidPoints;
	for (APatrolPoint* Point : PatrolPoints)
	{
		if (Point != PreviousPoint)
		{
			ValidPoints.Add(Point);
		}
	}
	
	if (ValidPoints.Num() == 0) return PatrolPoints[0];
	
	return ValidPoints[FMath::RandRange(0, ValidPoints.Num() - 1)];
}

FString UCalculateNewDestination::GetStaticDescription() const
{
	return FString::Printf(TEXT("Find patrol point -> %s, %s"), 
		*DestinationKey.SelectedKeyName.ToString(),
		*LastPatrolPointKey.SelectedKeyName.ToString());
}
