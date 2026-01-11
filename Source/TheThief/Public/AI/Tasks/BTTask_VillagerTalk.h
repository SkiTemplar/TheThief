// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_VillagerTalk.generated.h"

class AVillagerCharacter;

UCLASS()
class THETHIEF_API UBTTask_VillagerTalk : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_VillagerTalk();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TalkPartnerKey;

	UPROPERTY(EditAnywhere, Category = "Talk")
	float MinTalkDuration = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Talk")
	float MaxTalkDuration = 8.0f;

private:
	float RemainingTime = 0.f;
	TWeakObjectPtr<AVillagerCharacter> MyVillager;
	TWeakObjectPtr<AVillagerCharacter> Partner;
};

