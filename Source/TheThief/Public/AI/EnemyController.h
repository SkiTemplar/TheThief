// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/EnemyStates.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

class UAIPerceptionComponent;

UCLASS()
class THETHIEF_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetState(EGuardState NewState);
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ChaseTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void LoseTarget();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ReceiveAlert(FVector LastKnownPlayerLocation);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void GoToInvestigate(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ReturnToPatrol();

	UFUNCTION(BlueprintPure, Category = "AI")
	bool CanSeeTarget() const { return bCanSeeTarget; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	UAIPerceptionComponent* AIPerceptionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float SightRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float LoseSightRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float PeripheralVisionAngle = 70.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float LoseTargetDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float InvestigationDuration = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	bool bInvestigateOnLoseSight = true;

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void SetupPerceptionSystem();
	void OnLoseTargetTimer();
	void OnInvestigationComplete();
	void OnChaseToInvestigateTimer();

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	bool bCanSeeTarget = false;
	bool bHasBeenAlerted = false;
	bool bIsInvestigating = false;
	FVector LastKnownLocation = FVector::ZeroVector;
	FTimerHandle LoseTargetTimerHandle;
	FTimerHandle InvestigationTimerHandle;
	FTimerHandle ChaseToInvestigateTimerHandle;
};
