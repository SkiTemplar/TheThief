// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerFindNearestGuard.h"
#include "AI/GuardCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_VillagerFindNearestGuard::UBTTask_VillagerFindNearestGuard()
{
	NodeName = "Find Nearest Guard";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerFindNearestGuard::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector MyLocation = Controller->GetPawn()->GetActorLocation();

	TArray<AActor*> Guards;
	UGameplayStatics::GetAllActorsOfClass(Controller->GetWorld(), AGuardCharacter::StaticClass(), Guards);

	AGuardCharacter* NearestGuard = nullptr;
	float MinDistance = MAX_FLT;

	for (AActor* Actor : Guards)
	{
		AGuardCharacter* Guard = Cast<AGuardCharacter>(Actor);
		if (!Guard || Guard->IsDead())
		{
			continue;
		}

		float Distance = FVector::Dist(MyLocation, Guard->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			NearestGuard = Guard;
		}
	}

	if (NearestGuard)
	{
		BB->SetValueAsObject(TargetGuardKey.SelectedKeyName, NearestGuard);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_VillagerFindNearestGuard::GetStaticDescription() const
{
	return FString::Printf(TEXT("Find nearest guard -> %s"), *TargetGuardKey.SelectedKeyName.ToString());
}

