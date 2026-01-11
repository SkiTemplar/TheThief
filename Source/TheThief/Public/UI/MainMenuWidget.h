// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UUserWidget;

UCLASS()
class THETHIEF_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnPlayClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnControlsClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnHowToPlayClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnQuitClicked();

	// Llamado desde los submenús para cerrarlos
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void CloseSubMenu();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ControlsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* HowToPlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* QuitButton;

	// Clase del widget de Controls
	UPROPERTY(EditDefaultsOnly, Category = "SubMenus")
	TSubclassOf<UUserWidget> ControlsWidgetClass;

	// Clase del widget de How To Play
	UPROPERTY(EditDefaultsOnly, Category = "SubMenus")
	TSubclassOf<UUserWidget> HowToPlayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FName GameLevelName = "Lvl_FirstPerson";

private:
	UPROPERTY()
	UUserWidget* CurrentSubMenu = nullptr;
};
