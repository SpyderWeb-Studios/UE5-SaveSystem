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
	
public:
	virtual FString GetPlayerSaveSlot() override {return SaveSlots[CurrentSaveSlot];}
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool AddSlot(FString SlotName);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool AddSlotAndSetAsActive(FString SlotName);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool SetActiveSlot(int SlotIndex);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool SetActiveSlotByName(FString SlotName);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool RemoveSlot(int SlotIndex);
    
	UFUNCTION(BlueprintCallable, Category = "Save System|Multi Slot Save System")
	virtual bool RemoveSlotByName(FString SlotName);
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	TArray<FString> SaveSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Save System|Multi Slot Save System")
	int CurrentSaveSlot = 0;
};
