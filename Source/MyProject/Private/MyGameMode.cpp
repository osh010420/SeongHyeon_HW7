// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyPawn.h"
#include "MyPlayerController.h"
AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AMyPawn::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
}
