// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelSaveObject.generated.h"

/**
 * 
 */
UCLASS()
class SAVESYSTEM_API ULevelSaveObject : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<TObjectPtr<AActor>, bool> InteractedWithActors;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<TObjectPtr<AActor>, FTransform> MovedActors;

	
};
