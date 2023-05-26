// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/SaveSubsystem.h"
#include "MultiSlotSaveSubsystem.generated.h"



/**
 * 
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
	
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSlotAdded OnSlotAdded;
	
	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSlotRemoved OnSlotRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Save System|Event Dispatchers|Multi Slot Save System")
	FMultiSlotSaveSubsystemSaveCreated OnSaveCreated;

#pragma endregion 

#pragma region Add Slot
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	virtual bool AddSlot(FString SlotName);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Add Slot")
	virtual bool AddSlotAndSetAsActive(FString SlotName);

#pragma endregion

#pragma region Set Active Slot

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Set Active Slot")
	virtual bool SetActiveSlot(int SlotIndex);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Set Active Slot")
	virtual bool SetActiveSlotByName(FString SlotName);

#pragma endregion

#pragma region Remove Slot
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	virtual bool RemoveSlot(int SlotIndex);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Remove Slot")
	virtual bool RemoveSlotByName(FString SlotName);

#pragma endregion

#pragma region Getters
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual FString GetSlotName(int SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual int GetSlotIndex(FString SlotName);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual int GetActiveSlotIndex();

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual FString GetActiveSlotName();

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual USaveGame* GetSaveGameAtSlot(int SlotIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Get Slot")
	virtual USaveGame* GetSaveGameAtSlotByName(FString SlotName);

#pragma endregion

#pragma region Create Save Game
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameAtActiveSlot(bool bOverwriteIfPresent = true);

    UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameAndSetActiveSlot(FString SlotName, bool bOverwriteIfPresent = true);

    UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Base")
    virtual bool CreateSaveGameWithIndexAndSetActiveSlot(int SlotIndex, bool bOverwriteIfPresent = true);
	
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Advanced")
	virtual bool CreateSaveGameAtSlot(int SlotIndex, bool bOverwriteIfPresent = true);

	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System|Create Save Game|Advanced")
	virtual bool CreateSaveGameAtSlotByName(FString SlotName, bool bOverwriteIfPresent = true);

#pragma endregion

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	TArray<FString> SaveSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	int CurrentSaveSlot = 0;

	
};
