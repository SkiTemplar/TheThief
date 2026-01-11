// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerTalk.h"
#include "AI/VillagerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_VillagerTalk::UBTTask_VillagerTalk()
{
	NodeName = "Villager Talk";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_VillagerTalk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	MyVillager = Cast<AVillagerCharacter>(Controller->GetPawn());
	Partner = Cast<AVillagerCharacter>(BB->GetValueAsObject(TalkPartnerKey.SelectedKeyName));

	if (!MyVillager.IsValid() || !Partner.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	if (Partner->IsDead() || 
		Partner->GetVillagerState() == EVillagerState::Fleeing ||
		Partner->GetVillagerState() == EVillagerState::AlertingGuard)
	{
		return EBTNodeResult::Failed;
	}

	MyVillager->SetVillagerState(EVillagerState::Talking);
	
	// Mirar al compañero
	FVector Dir = Partner->GetActorLocation() - MyVillager->GetActorLocation();
	Dir.Z = 0.f;
	if (!Dir.IsNearlyZero())
	{
		MyVillager->SetActorRotation(Dir.Rotation());
	}

	RemainingTime = FMath::RandRange(MinTalkDuration, MaxTalkDuration);
	return EBTNodeResult::InProgress;
}

void UBTTask_VillagerTalk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!MyVillager.IsValid() || !Partner.IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Partner->IsDead() || 
		Partner->GetVillagerState() == EVillagerState::Fleeing ||
		Partner->GetVillagerState() == EVillagerState::AlertingGuard)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (MyVillager->GetVillagerState() == EVillagerState::Fleeing ||
		MyVillager->GetVillagerState() == EVillagerState::AlertingGuard)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	RemainingTime -= DeltaSeconds;
	if (RemainingTime <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_VillagerTalk::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (MyVillager.IsValid() && MyVillager->GetVillagerState() == EVillagerState::Talking)
	{
		MyVillager->SetVillagerState(EVillagerState::Walking);
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->ClearValue(TalkPartnerKey.SelectedKeyName);
	}
}

FString UBTTask_VillagerTalk::GetStaticDescription() const
{
	return FString::Printf(TEXT("Talk for %.1f-%.1f sec"), MinTalkDuration, MaxTalkDuration);
}

