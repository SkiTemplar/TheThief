// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorators/BTDecorator_VillagerStateCheck.h"
#include "AI/VillagerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_VillagerStateCheck::UBTDecorator_VillagerStateCheck()
{
	NodeName = "Villager State Check";
}

bool UBTDecorator_VillagerStateCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return false;

	AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Controller->GetPawn());
	if (!Villager) return false;

	bool bResult = false;

	switch (StateToCheck)
	{
	case EVillagerStateCheckType::IsIdle:
		bResult = Villager->IsIdle();
		break;
	case EVillagerStateCheckType::IsFleeing:
		bResult = Villager->GetVillagerState() == EVillagerState::Fleeing;
		break;
	case EVillagerStateCheckType::IsAlertingGuard:
		bResult = Villager->GetVillagerState() == EVillagerState::AlertingGuard;
		break;
	case EVillagerStateCheckType::IsTalking:
		bResult = Villager->IsTalking();
		break;
	case EVillagerStateCheckType::IsWalking:
		bResult = Villager->GetVillagerState() == EVillagerState::Walking;
		break;
	}

	return bInvertCondition ? !bResult : bResult;
}

FString UBTDecorator_VillagerStateCheck::GetStaticDescription() const
{
	FString StateStr;
	switch (StateToCheck)
	{
	case EVillagerStateCheckType::IsIdle: StateStr = TEXT("Idle"); break;
	case EVillagerStateCheckType::IsFleeing: StateStr = TEXT("Fleeing"); break;
	case EVillagerStateCheckType::IsAlertingGuard: StateStr = TEXT("Alerting Guard"); break;
	case EVillagerStateCheckType::IsTalking: StateStr = TEXT("Talking"); break;
	case EVillagerStateCheckType::IsWalking: StateStr = TEXT("Walking"); break;
	}

	return FString::Printf(TEXT("%s: %s"), bInvertCondition ? TEXT("Is NOT") : TEXT("Is"), *StateStr);
}
