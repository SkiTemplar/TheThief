// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseEnemy.h"
#include "AI/EnemyStates.h"
#include "GuardCharacter.generated.h"

class UWidgetComponent;

UCLASS()
class THETHIEF_API AGuardCharacter : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AGuardCharacter();

	UFUNCTION(BlueprintCallable, Category = "Guard")
	void CapturePlayer();

	UFUNCTION(BlueprintCallable, Category = "Guard")
	void SetGuardState(EGuardState NewState);

	UFUNCTION(BlueprintPure, Category = "Guard")
	EGuardState GetGuardState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Guard")
	bool IsPlayerInCaptureRange() const;

	UFUNCTION(BlueprintCallable, Category = "Guard")
	void AlertNearbyGuards(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Guard")
	void OnAlerted(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PlayAttackMontage();

	UFUNCTION(BlueprintPure, Category = "Guard")
	ENPCWidgetIcon GetCurrentWidgetIcon() const { return CurrentWidgetIcon; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnDeath() override;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float CaptureRange = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float AlertRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float PatrolSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	float ChaseSpeed = 450.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	EGuardState CurrentState = EGuardState::Patrol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	ENPCWidgetIcon CurrentWidgetIcon = ENPCWidgetIcon::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	UWidgetComponent* StatusWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mis Variables")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mis Variables")
	UStaticMeshComponent* WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mis Variables")
	FName WeaponSocketName = "WeaponSocket";

	// === SONIDOS DEL GUARDIA ===
	
	// Sonidos que dice durante la persecución (cada X tiempo)
	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundBase*> ChaseSounds;

	// Sonido cuando empieza a perseguir (entra en estado Chase)
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* ChaseStartSound = nullptr;

	// Sonido cuando captura al jugador
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* CaptureSound = nullptr;

	// Intervalo entre frases durante la persecución
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float MinChaseVoiceInterval = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float MaxChaseVoiceInterval = 5.0f;

	// Variación de pitch para los sonidos
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float PitchVariation = 0.1f;

	// Atenuación de sonido
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundAttenuation* SoundAttenuation = nullptr;

private:
	void UpdateWidgetIcon();
	void PlayChaseVoiceLine();
	void StartChaseVoiceTimer();
	void StopChaseVoiceTimer();
	void PlayChaseStartSound();
	void PlayCaptureSound();

	FTimerHandle ChaseVoiceTimerHandle;
	EGuardState PreviousState = EGuardState::Patrol;
	bool bHasCapturedPlayer = false;
};
