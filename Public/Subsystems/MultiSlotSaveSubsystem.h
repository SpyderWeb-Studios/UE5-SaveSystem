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

	/**
	 * @brief Add a Slot to the Save Slots TMap, and create a Save Game Object if it doesn't already exist  Disk.
	 * @param SlotName The Name of the Slot to add
	 * @param bVerbose If the function should print out to the log
	 * @return If the Slot was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	bool AddSlot(FString SlotName, bool bVerbose = true);

	/**
	 * @brief Add a Slot to the Save Slots TMap, and create a Save Game Object on the Disk. This will also set the Active Slot to the Slot that was added.
	 * @param SlotName The Name of the Slot to add
	 * @param bLoad If the Save Game Object should be loaded after it is created
	 * @param bVerbose If the function should print out to the log
	 * @return If the Slot was added successfully and set as the Active Slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	bool AddSlotAndSetActive(FString SlotName, bool bLoad = false, bool bVerbose = true);

#pragma endregion

#pragma region Remove Slot

	/**
	 * @brief Remove a Slot from the Save Slots TMap, and delete the Save Game Object from the Disk
	 * @param SlotName The Name of the Slot to remove
	 * @param bVerbose If the function should print out to the log
	 * @return If the Slot was removed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	bool RemoveSlot(FString SlotName, bool bVerbose = true);

	/**
	 * @brief Remove the Active Slot from the Save Slots TMap, and delete the Save Game Object from the Disk
	 * @param bVerbose If the function should print out to the log
	 * @return If the Active Slot was removed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	bool RemoveActiveSlot(bool bVerbose = true);

#pragma endregion

#pragma region Save Slot

	/**
	 * @brief Save a Save Game Object to the Disk
	 * @param SlotName The Name of the Slot to save the Save Game Object from
	 * @param bAsync If the Save Game Object should be saved asynchronously or not
	 * @param bVerbose If the function should print out to the log
	 * @return If the Save Game Object was saved successfully. If Async is true, this will always return true.
	 * You'll need to check the OnPlayerDataSaved Event to see if it was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Save Slot")
	bool SaveSlot(FString SlotName, bool bAsync = true, bool bVerbose = true);

	/**
	 * @brief Save the Active Slot in the Save Slots TMap to the Disk
	 * @param bAsync If the Save Game Object should be saved asynchronously or not
	 * @param bVerbose If the function should print out to the log
	 * @return If the Save Game Object was saved successfully. If Async is true, this will always return true.
	 * You'll need to check the OnPlayerDataSaved Event to see if it was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Save Slot")
	bool SaveActiveSlot(bool bAsync = true, bool bVerbose = true);

#pragma endregion 

#pragma region Load Slot


	/**
	 * @brief Load a Save Game Object, this will not set the Active Slot or call the OnSlotAdded Event. If the Slot is not found in the TMap,
	 * it will attempt to load it from the Disk if it exists. If it does not exist on the Disk, it will return false
	 * @param SlotName The Name of the Slot to load
	 * @param bAsync If the Save Game Object should be loaded asynchronously or not
	 * @param bVerbose If the function should print out to the log
	 * @return If the Save Game Object was loaded successfully. If Async is true, this will always return true.
	 * You'll need to check the OnPlayerDataLoaded Event to see if it was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadSlot(FString SlotName, bool bAsync = true, bool bVerbose = true);

	/**
	 * @brief Load the Active Slot in the Save System, this will not set the Active Slot or call the OnSlotAdded Event
	 * @param bAsync If the Save Game Object should be loaded asynchronously or not
	 * @param bVerbose If the function should print out to the log
	 * @return If the Save Game Object was loaded successfully. If Async is true, this will always return true.
	 * You'll need to check the OnPlayerDataLoaded Event to see if it was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadActiveSlot(bool bAsync = true, bool bVerbose = true);

	/**
	 * @brief Load a Save Game Object from the Disk by Slot Name, this will not set the Active Slot or call the OnSlotAdded Event
	 * @param SlotName The Name of the Slot to load from the Disk
	 * @param bVerbose If the function should print out to the log
	 * @return If the Save Game Object was loaded successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Load Slot")
	bool LoadSlotFromDisk(FString SlotName, bool bVerbose = true);

#pragma endregion

	/**
	 * @brief Set the Active Slot in the Save System by Slot Name. This will also load the Save Game Object if bLoad is true
	 * @param String The Name of the Slot to set as the Active Slot
	 * @param bLoad If the Save Game Object should be loaded after setting the Active Slot
	 * @param bVerbose If the function should print out to the log
	 * @return If the Active Slot was set successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	bool SetActiveSlot(const FString& String, bool bLoad, bool bVerbose = true);

	/**
	 * @brief Get the Active Slot Name in the Save System
	 * @return The Active Slot Name
	 */
	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	FString GetActiveSlot() {return CurrentSaveSlot;}
	
	/**
	 * @brief Get All the Save Slots in the Save System
	 * @param bVerbose If the function should print out to the log
	 * @return An Array of Save Game Objects
	 */
	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	TArray<USaveGame*> GetAllSaveSlots(bool bVerbose = true);

	/**
	 * @brief Get All the Save Slot Names in the Save System, this is useful for displaying the Save Slots in a UI
	 * @return An Array of Save Slot Names
	 */
	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	TArray<FString> GetAllSaveSlotNames() const;
	
protected:
	
	/**
	 * @brief Get the Save Game Object from the Save System by Slot Name
	 * @param SlotName The Name of the Slot to get the Save Game Object from
	 * @param bVerbose If the function should print out to the log
	 * @return The Save Game Object from the Slot Name
	 */
	UFUNCTION(BlueprintPure, Category = "Save System|Multi Slot Save System")
	USaveGame* GetSaveSlot(FString SlotName, bool bVerbose = true);

	/**
	 * @brief Get the Active Save Game Object in the Save System
	 * @param bVerbose If the function should print out to the log
	 * @return The Active Save Game Object
	 */
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

	/**
	 * @brief Test array to see if the Save Game is being created properly and to make sure there were no memory leaks.
	 */
	TArray<TWeakObjectPtr<USaveGame>> CreatedSaveGames;
};

