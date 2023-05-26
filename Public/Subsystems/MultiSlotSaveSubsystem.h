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
	
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FMultiSlotSaveSubsystemSlotAdded, UMultiSlotSaveSubsystem, OnSlotAdded, int, SlotIndex, FString, SlotName);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FMultiSlotSaveSubsystemSlotRemoved, UMultiSlotSaveSubsystem, OnSlotRemoved, int, SlotIndex, FString, SlotName);
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_TwoParams(FMultiSlotSaveSubsystemSaveCreated, UMultiSlotSaveSubsystem, OnSaveCreated, int, SlotIndex, FString, SlotName);

	
public:

	virtual void Deinitialize() override;

	virtual FString GetPlayerSaveSlot() override
	{
		if(SaveSlots.IsValidIndex(CurrentSaveSlot)) return SaveSlots[CurrentSaveSlot];
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
	 * @brief Adds a new Slot to the Save System
	 * @param SlotName The name of the Slot to add
	 * @param bCheckLocal Whether to check if the Slot already exists locally
	 * @return Whether the Slot was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	virtual bool AddSlot(FString SlotName, bool bCheckLocal = true);

	/**
	 * @brief Adds a new Slot to the Save System and sets it as the active Slot
	 * @param SlotName The name of the Slot to add
	 * @param bCheckLocal Whether to check if the Slot already exists locally
	 * @return Whether the Slot was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	virtual bool AddSlotAndSetAsActive(FString SlotName, bool bCheckLocal = true);

#pragma endregion

#pragma region Set Active Slot

	/**
	 * @brief Sets the Active Slot by Index
	 * @param SlotIndex The Index of the Slot to set as Active
	 * @return Whether the Active Slot was set successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Set Active Slot")
	virtual bool SetActiveSlot(int SlotIndex);

	/**
	 * @brief Sets the Active Slot by Name
	 * @param SlotName The Name of the Slot to set as Active
	 * @return Whether the Active Slot was set successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Set Active Slot")
	virtual bool SetActiveSlotByName(FString SlotName);

#pragma endregion

#pragma region Remove Slot

	/**
	 * @brief Removes a Slot by Index
	 * @param SlotIndex The Index of the Slot to remove
	 * @return Whether the Slot was removed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	virtual bool RemoveSlot(int SlotIndex);

	/**
	 * @brief Removes a Slot by Name
	 * @param SlotName The Name of the Slot to remove
	 * @return Whether the Slot was removed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	virtual bool RemoveSlotByName(FString SlotName);

#pragma endregion

#pragma region Getters

	/**
	 * @brief Gets the Slot Name by the Index in the Save System
	 * @param SlotIndex The Index of the Slot to remove
	 * @return The Name of the Slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual FString GetSlotName(int SlotIndex);

	/**
	 * @brief Gets the Slot Index by the Name in the Save System
	 * @param SlotName The Name of the Slot to remove
	 * @return The Index of the Slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual int GetSlotIndex(FString SlotName);

	/**
	 * @brief Gets the Active Slot Index
	 * @return The Index of the Active Slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual int GetActiveSlotIndex();

	/**
	 * @brief Gets the Active Slot Name
	 * @return The Name of the Active Slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual FString GetActiveSlotName();

	/**
	 * @brief Gets the Save Game Object at the Index in the Save System
	 * @param SlotIndex The Index of the Slot to get the Save Game Object from
	 * @return The Save Game Object
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual USaveGame* GetSaveGameAtSlot(int SlotIndex);

	/**
	 * @brief Gets the Save Game Object at the Name in the Save System
	 * @param SlotName The Name of the Slot to get the Save Game Object from
	 * @return The Save Game Object
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual USaveGame* GetSaveGameAtSlotByName(FString SlotName);

#pragma endregion

#pragma region Create Save Game

	/**
	 * @brief Creates a Save Game Object at the Active Slot
	 * @param bOverwriteIfPresent Whether to overwrite the Save Game Object if it already exists
	 * @return Whether the Save Game Object was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameAtActiveSlot(bool bOverwriteIfPresent = true);

	/**
	 * @brief Creates a Save Game Object at the Active Slot and sets it as the Active Slot
	 * @param SlotName The name of the Slot to add
	 * @param bOverwriteIfPresent Whether to overwrite the Save Game Object if it already exists
	 * @return Whether the Save Game Object was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameAndSetActiveSlot(FString SlotName, bool bOverwriteIfPresent = true);

	/**
	 * @brief Creates a Save Game Object at the Active Slot and sets it as the Active Slot
	 * @param SlotIndex The Index of the Slot to add
	 * @param bOverwriteIfPresent Whether to overwrite the Save Game Object if it already exists
	 * @return Whether the Save Game Object was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameWithIndexAndSetActiveSlot(int SlotIndex, bool bOverwriteIfPresent = true);

	/**
	 * @brief Creates a Save Game Object at SlotIndex
	 * @param SlotIndex The Index of the Slot to add
	 * @param bOverwriteIfPresent Whether to overwrite the Save Game Object if it already exists
	 * @return Whether the Save Game Object was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Advanced")
	virtual bool CreateSaveGameAtSlot(int SlotIndex, bool bOverwriteIfPresent = true);

	/**
	 * @brief Creates a Save Game Object at SlotName
	 * @param SlotName The Name of the Slot to add
	 * @param bOverwriteIfPresent Whether to overwrite the Save Game Object if it already exists
	 * @return Whether the Save Game Object was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Advanced")
	virtual bool CreateSaveGameAtSlotByName(FString SlotName, bool bOverwriteIfPresent = true);

	/**
	 * @brief When a Save Game Object external to the Save System is loaded, this function should be called to add it to the Save System
	 * @param SlotIndex The Index of the Slot to add
	 * @param SlotName The Name of the Slot to add
	 * @return Whether the Save Game Object was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Advanced")
	virtual bool ReceiveLoadedSaveGame(int SlotIndex, FString SlotName);

#pragma endregion

protected:
	/**
	 * @brief The Array of Save Slots in the Save System
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	TArray<FString> SaveSlots;

	/**
	 * @brief The Index of the Active Slot in the Save System
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	int CurrentSaveSlot = 0;

	
};
