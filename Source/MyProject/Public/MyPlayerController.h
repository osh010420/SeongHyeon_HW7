// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;

UCLASS()
class MYPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputMappingContext* InputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputAction* UpDownAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputAction* RollAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input");
	UInputAction* SwitchAction;
};
