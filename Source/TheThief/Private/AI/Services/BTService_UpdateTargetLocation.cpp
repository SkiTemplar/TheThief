// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Services/BTService_UpdateTargetLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateTargetLocation::UBTService_UpdateTargetLocation()
{
	NodeName = "Update Target Location";
	Interval = 0.2f;
	RandomDeviation = 0.1f;
}

void UBTService_UpdateTargetLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (Target)
	{
		BB->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, Target->GetActorLocation());
	}
}

FString UBTService_UpdateTargetLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Update %s from %s position"), 
		*LastKnownLocationKey.SelectedKeyName.ToString(),
		*TargetActorKey.SelectedKeyName.ToString());
}

