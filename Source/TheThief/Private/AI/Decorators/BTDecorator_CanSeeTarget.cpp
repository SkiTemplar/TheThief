// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Decorators/BTDecorator_CanSeeTarget.h"
#include "AI/EnemyController.h"
#include "AIController.h"

UBTDecorator_CanSeeTarget::UBTDecorator_CanSeeTarget()
{
	NodeName = "Can See Target";
}

bool UBTDecorator_CanSeeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AEnemyController* Controller = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!Controller) return false;

	bool bResult = Controller->CanSeeTarget();

	return bInvertCondition ? !bResult : bResult;
}

FString UBTDecorator_CanSeeTarget::GetStaticDescription() const
{
	return bInvertCondition ? TEXT("Cannot See Target") : TEXT("Can See Target");
}

