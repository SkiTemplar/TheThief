// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HowToPlayWidget.generated.h"

class UButton;

/**
 * Widget que muestra cómo jugar
 * Se abre encima del Main Menu o Pause Menu
 */
UCLASS()
class THETHIEF_API UHowToPlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnBackClicked();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;
};
