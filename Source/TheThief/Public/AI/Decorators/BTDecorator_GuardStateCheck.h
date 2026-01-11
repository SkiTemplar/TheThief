// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "AI/EnemyStates.h"
#include "BTDecorator_GuardStateCheck.generated.h"

UCLASS()
class THETHIEF_API UBTDecorator_GuardStateCheck : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_GuardStateCheck();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	// Para observar cambios en el Blackboard
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EGuardState StateToCheck = EGuardState::Patrol;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bInvertCondition = false;

	// Key del Blackboard para observar cambios de estado
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CurrentStateKey;
};
