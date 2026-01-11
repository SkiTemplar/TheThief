// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionPromptWidget.generated.h"

class UTextBlock;

/**
 * Widget que muestra el prompt de interacción
 */
UCLASS()
class THETHIEF_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetPromptText(const FText& Text);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PromptText;
};
