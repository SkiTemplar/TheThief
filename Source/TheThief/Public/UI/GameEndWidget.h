// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEndWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class THETHIEF_API UGameEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SetAsWinScreen();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SetAsLoseScreen();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnRestartClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnMainMenuClicked();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SubtitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* MainMenuButton;

	UPROPERTY(EditDefaultsOnly, Category = "Text")
	FText WinTitle = FText::FromString("VICTORY!");

	UPROPERTY(EditDefaultsOnly, Category = "Text")
	FText WinSubtitle = FText::FromString("Thy Thief has succeeded!");

	UPROPERTY(EditDefaultsOnly, Category = "Text")
	FText LoseTitle = FText::FromString("CAPTURED!");

	UPROPERTY(EditDefaultsOnly, Category = "Text")
	FText LoseSubtitle = FText::FromString("The guards caught you...");
};
