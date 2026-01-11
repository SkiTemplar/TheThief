// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

// Enum para la dirección del daño recibido
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Front UMETA(DisplayName = "Front"),
	Back UMETA(DisplayName = "Back"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UCLASS(Abstract)
class THETHIEF_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	// Calcular la dirección del golpe respecto al personaje
	UFUNCTION(BlueprintPure, Category = "Combat")
	EHitDirection GetHitDirection(AActor* DamageCauser) const;

protected:
	virtual void BeginPlay() override;

	// Llamado cuando el personaje muere - implementar en clases hijas
	virtual void OnDeath();

	// Llamado cuando recibe daño pero no muere - para animaciones de hit
	virtual void OnHitReaction(EHitDirection HitDirection, AActor* DamageCauser);

	// Reproducir animación de hit según dirección
	void PlayHitAnimation(EHitDirection HitDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DestroyDelay = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsDead = false;

	// ========== HIT ANIMATIONS ==========
	// IMPORTANTE: Estas propiedades se configuran en cada Blueprint hijo (BP_Guard, BP_Villager, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	UAnimMontage* HitFrontMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	UAnimMontage* HitBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	UAnimMontage* HitLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	UAnimMontage* HitRightMontage;

	// Si true, reproduce animaciones de hit al recibir daño
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	bool bUseHitAnimations = true;

	// Tiempo que dura la animación de muerte antes de activar ragdoll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Hit Animations")
	float DeathAnimationDuration = 0.5f;

	// Timer para el ragdoll después de la animación de muerte
	FTimerHandle RagdollTimerHandle;

	// Guarda la dirección del último golpe para la animación de muerte
	EHitDirection LastHitDirection = EHitDirection::Front;

	// Activa el ragdoll (llamado por timer después de la animación)
	void ActivateRagdoll();
};
