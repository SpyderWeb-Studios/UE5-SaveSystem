// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/SaveSubsystem.h"
#include "SingleSlotSaveSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, NotBlueprintType)
class SAVESYSTEM_API USingleSlotSaveSubsystem : public USaveSubsystem
{
	GENERATED_BODY()
public:
	/**
 * @brief The name of the Save Slot to use for the Player
 */
	UPROPERTY(BlueprintReadWrite, Category = "Save System|Single Slot Save System")
	FString PlayerSaveSlot = "PlayerSlot";

	virtual FString GetPlayerSaveSlot() override;
};
