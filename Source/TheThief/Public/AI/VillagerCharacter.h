// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseEnemy.h"
#include "AI/EnemyStates.h"
#include "Components/AudioComponent.h"
#include "VillagerCharacter.generated.h"

class UWidgetComponent;
class AGuardCharacter;

UENUM(BlueprintType)
enum class EVillagerState : uint8
{
	Walking,
	Talking,
	Fleeing,
	AlertingGuard
};

UCLASS()
class THETHIEF_API AVillagerCharacter : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AVillagerCharacter();

	UFUNCTION(BlueprintCallable, Category = "Villager")
	void AlertGuards(AActor* SuspiciousActor);

	UFUNCTION(BlueprintCallable, Category = "Villager")
	void SetVillagerState(EVillagerState NewState);

	UFUNCTION(BlueprintPure, Category = "Villager")
	EVillagerState GetVillagerState() const { return CurrentVillagerState; }

	UFUNCTION(BlueprintCallable, Category = "Villager")
	void OnNearbyVillagerAttacked(AActor* Attacker);

	UFUNCTION(BlueprintPure, Category = "Villager")
	bool IsTalking() const { return bIsTalking; }

	UFUNCTION(BlueprintPure, Category = "Villager")
	bool IsIdle() const { return bIsIdle; }

	UFUNCTION(BlueprintCallable, Category = "Villager")
	void OnRobbed(AActor* Thief);

	UFUNCTION(BlueprintPure, Category = "Villager")
	bool HasCoin() const { return bHasCoin; }

	UFUNCTION(BlueprintCallable, Category = "Villager")
	void SetHasCoin(bool bNewHasCoin);

	UFUNCTION(BlueprintPure, Category = "Villager")
	bool CanBeRobbed() const { return bHasCoin && !bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Villager")
	ENPCWidgetIcon GetCurrentWidgetIcon() const { return CurrentWidgetIcon; }

	void OnWitnessedTheft(AActor* Thief);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float AlertRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float WalkSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float FleeSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float FleeDuration = 5.f;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	bool bHasCoin = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	UWidgetComponent* StatusWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	TArray<UAnimSequence*> TalkAnimations;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float TalkDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float IdleTimeToTalk = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	TArray<USoundBase*> PanicSounds;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	TArray<USoundBase*> TalkSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float PitchVariation = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	USoundBase* DeathSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	USoundAttenuation* SoundAttenuation = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float TalkVolumeMultiplier = 0.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	EVillagerState CurrentVillagerState = EVillagerState::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	ENPCWidgetIcon CurrentWidgetIcon = ENPCWidgetIcon::None;

private:
	float WitnessRadius = 800.f;  // Aumentado de 300 a 800 para que detecten robos desde más lejos
	bool bHasAlertedGuards = false;
	bool bIsTalking = false;
	bool bIsIdle = false;
	float IdleTimer = 0.f;
	FTimerHandle CalmDownTimerHandle;
	FTimerHandle TalkSoundTimerHandle;
	FVector LastKnownThiefLocation = FVector::ZeroVector;
	// Sistema para que solo UN aldeano alerte al guardia por crimen
	static AActor* CurrentAlertedThief;  // El ladrón que ya está siendo reportado
	static AVillagerCharacter* CurrentAlerter;  // El aldeano que está alertando
	
	UPROPERTY()
	AVillagerCharacter* TalkPartner = nullptr;
	
	UPROPERTY()
	UAudioComponent* CurrentTalkAudioComponent = nullptr;
	
	UPROPERTY()
	AActor* WitnessedThief = nullptr;
	
	void StartFleeing();
	void CalmDown();
	void AlertNearbyVillagers(AActor* Attacker);
	void PlayPanicSound();
	void PlayDeathSound();
	void PlayTalkSound();
	void CheckForNearbyVillagers();
	void StartTalking(AVillagerCharacter* Partner);
	void StopTalking();
	void PlayRandomTalkAnimation();
	void UpdateWidgetIcon();
	void NotifyNearbyWitnesses(AActor* Thief);
	void FleeToNearestGuard();
	AGuardCharacter* FindNearestGuard();
	bool CanSeeActor(AActor* Target) const;
};
