// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Attack.generated.h"

UCLASS()
class THETHIEF_API UAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UFUNCTION()
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	UBehaviorTreeComponent* MyComponent;
};
