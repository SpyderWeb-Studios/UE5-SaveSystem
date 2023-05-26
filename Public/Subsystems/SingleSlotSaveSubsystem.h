// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/SaveSubsystem.h"
#include "SingleSlotSaveSubsystem.generated.h"

/**
 * The Single Slot Save Subsystem is a Save Subsystem that uses a single Save Slot rather than multiple.
 * This is useful for games that only have a single player, or for games that only need to save a single save file.
 * \n \n
 * It needs to be both Abstract and NotBlueprintType because it is a base class but we don't want it to be used directly, nor do we want it to automatically be created..
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

	/**
	 * @brief Overridable function to get the name of the Save Slot to use for the Player, if you want to use a different one
	 * @return The name of the Save Slot to use for the Player
	 */
	virtual FString GetPlayerSaveSlot() override;
};
