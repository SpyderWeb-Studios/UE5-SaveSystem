// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MultiSlotSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"

bool UMultiSlotSaveSubsystem::AddSlot(FString SlotName)
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Name already exists"));
		// If the Slot Name already exists, return false
		return false;
	}
	// If the slot name is already in the array, return false
	// Otherwise, add the slot name to the array and return true
	UE_LOG(LogSaveSystem, Display, TEXT("Adding Slot %s"), *SlotName);
	return SaveSlots.AddUnique(SlotName) >= 0;
}

bool UMultiSlotSaveSubsystem::AddSlotAndSetAsActive(FString SlotName)
{
	if(AddSlot(SlotName))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Slot Added Successfully"));
		return SetActiveSlotByName(SlotName);
	}
	return false;
}

bool UMultiSlotSaveSubsystem::SetActiveSlot(int SlotIndex)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Setting Active Slot to %d"), SlotIndex);
	
	if(SaveSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Slot Index is valid"));
		CurrentSaveSlot = SlotIndex;
		return true;
	}
	
	UE_LOG(LogSaveSystem, Error, TEXT("Slot Index is not valid"));
	return false;
}

bool UMultiSlotSaveSubsystem::SetActiveSlotByName(FString SlotName)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Setting Active Slot to %s"), *SlotName);
	return SetActiveSlot(SaveSlots.IndexOfByKey(SlotName));
}

bool UMultiSlotSaveSubsystem::RemoveSlot(int SlotIndex)
{
	if(!SaveSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Index is not valid"));
		return false;
	}

	
	UE_LOG(LogSaveSystem, Display, TEXT("Removing Slot at Index %d"), SlotIndex);
	SaveSlots.RemoveAt(SlotIndex);
	return true;
}

bool UMultiSlotSaveSubsystem::RemoveSlotByName(FString SlotName)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Removing Slot with Name %s"), *SlotName);
	return SaveSlots.Remove(SlotName) > 0;
}
