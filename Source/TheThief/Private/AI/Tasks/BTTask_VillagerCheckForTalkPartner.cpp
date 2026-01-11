// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerCheckForTalkPartner.h"
#include "AI/VillagerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_VillagerCheckForTalkPartner::UBTTask_VillagerCheckForTalkPartner()
{
	NodeName = "Check For Talk Partner";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerCheckForTalkPartner::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller || !Controller->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	AVillagerCharacter* MyVillager = Cast<AVillagerCharacter>(Controller->GetPawn());
	if (!MyVillager || MyVillager->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	FVector MyLocation = MyVillager->GetActorLocation();

	TArray<AActor*> Villagers;
	UGameplayStatics::GetAllActorsOfClass(Controller->GetWorld(), AVillagerCharacter::StaticClass(), Villagers);

	for (AActor* Actor : Villagers)
	{
		AVillagerCharacter* Other = Cast<AVillagerCharacter>(Actor);
		if (!Other || Other == MyVillager || Other->IsDead())
		{
			continue;
		}

		if (FVector::Dist(MyLocation, Other->GetActorLocation()) > TalkDistance)
		{
			continue;
		}

		if (Other->GetVillagerState() == EVillagerState::Fleeing || 
			Other->GetVillagerState() == EVillagerState::AlertingGuard ||
			Other->IsTalking())
		{
			continue;
		}

		if (!Other->IsIdle())
		{
			continue;
		}

		BB->SetValueAsObject(TalkPartnerKey.SelectedKeyName, Other);
		return EBTNodeResult::Succeeded;
	}

	BB->ClearValue(TalkPartnerKey.SelectedKeyName);
	return EBTNodeResult::Failed;
}

FString UBTTask_VillagerCheckForTalkPartner::GetStaticDescription() const
{
	return FString::Printf(TEXT("Find talk partner within %.0f units"), TalkDistance);
}

