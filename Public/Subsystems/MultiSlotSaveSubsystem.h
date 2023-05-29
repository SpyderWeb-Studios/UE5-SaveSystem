// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/SaveSubsystem.h"
#include "MultiSlotSaveSubsystem.generated.h"



/**
 * The Multi Slot Save Subsystem is a Save Subsystem that uses multiple Save Slots rather than a single one. This is useful for games that have multiple players, or for games that need to save multiple save files.
 * \n \n
 * This is a base class that should be extended to add functionality, and is not meant to be used directly. It needs to be both Abstract and NotBlueprintType because it is a base class but we don't want it to be used directly, nor do we want it to automatically be created.
 */
UCLASS(Abstract, NotBlueprintType)
class SAVESYSTEM_API UMultiSlotSaveSubsystem : public USaveSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FMultiSlotSaveSubsystemSlotAdded, UMultiSlotSaveSubsystem, OnSlotAdded, FString, SlotName);
	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FMultiSlotSaveSubsystemSlotRemoved, UMultiSlotSaveSubsystem, OnSlotRemoved, FString, SlotName);
	DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FMultiSlotSaveSubsystemSaveCreated, UMultiSlotSaveSubsystem, OnSaveCreated, FString, SlotName);

	
public:

	virtual void Deinitialize() override;

	virtual FString GetPlayerSaveSlot() override
	{
		// Check if the Current Save Slot is valid and return it if it is
		if (SaveSlots.Contains(CurrentSaveSlot))
		{
			return CurrentSaveSlot;
		}
		
		return "";
	}

#pragma region Event Dispatchers

	/**
	 * @brief Event Dispatcher for when a Slot is added
	 */
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSlotAdded OnSlotAdded;

	
	/**
	 * @brief Event Dispatcher for when a Slot is removed
	 */
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSlotRemoved OnSlotRemoved;

	/**
	 * @brief Event Dispatcher for when a Save Game Object is created
	 */
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSaveCreated OnSaveCreated;

#pragma endregion 

#pragma region Add Slot

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	bool AddSlot(FString SlotName, bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	bool AddSlotAndSetActive(FString SlotName, bool bLoad = false, bool bVerbose = true);

#pragma endregion

#pragma region Remove Slot

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	bool RemoveSlot(FString SlotName, bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	bool RemoveActiveSlot(bool bVerbose = true);

#pragma endregion

#pragma region Save Slot

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Save Slot")
	bool SaveSlot(FString SlotName, bool bAsync = true, bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Save Slot")
	bool SaveActiveSlot(bool bAsync = true, bool bVerbose = true);

#pragma endregion 

#pragma region Load Slot

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadSlot(FString SlotName, bool bAsync = true, bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadActiveSlot(bool bAsync = true, bool bVerbose = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadSlotFromDisk(FString SlotName, bool bVerbose = true);

#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	bool SetActiveSlot(const FString& String, bool bLoad, bool bVerbose = true);

	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	FString GetActiveSlot() {return CurrentSaveSlot;}

	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	TArray<USaveGame*> GetAllSaveSlots(bool bVerbose = true);
	
protected:

	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	USaveGame* GetSaveSlot(FString SlotName, bool bVerbose = true);

	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	USaveGame* GetActiveSaveSlot(bool bVerbose = true);
	
	/**
	 * @brief The Map of Save Slots in the Save System
	 */
	TMap<FString, TWeakObjectPtr<USaveGame>> SaveSlots;

	/**
	 * @brief The Index of the Active Slot in the Save System
	 */
	FString CurrentSaveSlot = "";

	TArray<TWeakObjectPtr<USaveGame>> CreatedSaveGames;
};

inline TArray<USaveGame*> UMultiSlotSaveSubsystem::GetAllSaveSlots(bool bVerbose)
{
	// Get All the Created Save Games and add them to a temp array
	TArray<USaveGame*> TempArray;
	for (auto SaveGame : CreatedSaveGames)
	{
		if(SaveGame.IsValid())
		{
			TempArray.Add(SaveGame.Get());
		}
	}

	if(bVerbose) UE_LOG(LogTemp, Warning, TEXT("Created and Valid Save Games: %d"), TempArray.Num());
	
	return TempArray;
}


