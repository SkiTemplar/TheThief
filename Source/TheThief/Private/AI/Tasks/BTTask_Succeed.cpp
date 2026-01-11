// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_Succeed.h"

UBTTask_Succeed::UBTTask_Succeed()
{
	NodeName = "Always Succeed";
}

EBTNodeResult::Type UBTTask_Succeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}

