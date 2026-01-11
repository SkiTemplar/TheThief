// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class UUserWidget;

/**
 * Widget del menú de pausa
 */
UCLASS()
class THETHIEF_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnResumeClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnRestartClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnControlsClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnHowToPlayClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnMainMenuClicked();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnQuitClicked();

	// Llamado desde los submenús para cerrarlos
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void CloseSubMenu();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* RestartButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* ControlsButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* HowToPlayButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* MainMenuButton;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* QuitButton;

	// Clase del widget de Controls
	UPROPERTY(EditDefaultsOnly, Category = "SubMenus")
	TSubclassOf<UUserWidget> ControlsWidgetClass;

	// Clase del widget de How To Play
	UPROPERTY(EditDefaultsOnly, Category = "SubMenus")
	TSubclassOf<UUserWidget> HowToPlayWidgetClass;

private:
	UPROPERTY()
	UUserWidget* CurrentSubMenu = nullptr;
};
