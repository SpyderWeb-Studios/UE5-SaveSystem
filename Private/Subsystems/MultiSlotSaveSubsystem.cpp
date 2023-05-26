// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MultiSlotSaveSubsystem.h"

#include "GameFramework/SaveGame.h"
#include "Interfaces/SaveObjectInterface.h"
#include "Kismet/GameplayStatics.h"

void UMultiSlotSaveSubsystem::Deinitialize()
{
	OnSlotRemoved.Clear();
	OnSlotAdded.Clear();
		
	Super::Deinitialize();
}

bool UMultiSlotSaveSubsystem::AddSlot(FString SlotName, bool bCheckLocal)
{
	if(SlotName.IsEmpty() || SlotName.Len() < 1 || SlotName.Equals(TEXT("")) || SlotName.Equals(TEXT(" ")) )
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Name is Empty"));
		// If the Slot Name is empty, return false
		return false;
	}
	
	if(bCheckLocal && UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Name already exists locally"));
		// If the Slot Name already exists, return false
		return false;
	}

	if(SaveSlots.Contains(SlotName))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Name already exists"));
		// If the Slot Name already exists, return true as it is already in the array
		return true;
	}
	
	// If the slot name is already in the array, return false
	// Otherwise, add the slot name to the array and return true
	UE_LOG(LogSaveSystem, Display, TEXT("Adding Slot %s"), *SlotName);
	const int Index = SaveSlots.AddUnique(SlotName);
	if(SaveSlots.IsValidIndex(Index))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Slot Added Successfully"));
		OnSlotAdded.Broadcast(Index, SlotName);
		return true;
	}

	UE_LOG(LogSaveSystem, Warning, TEXT("Slot was not added successfully"));
	return false;
}

bool UMultiSlotSaveSubsystem::AddSlotAndSetAsActive(FString SlotName, bool bCheckLocal)
{
	if(AddSlot(SlotName, bCheckLocal))
	{
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
	const FString SlotName = SaveSlots[SlotIndex];
	SaveSlots.RemoveAt(SlotIndex);
	OnSlotRemoved.Broadcast(SlotIndex, SlotName);
	return true;
}

bool UMultiSlotSaveSubsystem::RemoveSlotByName(FString SlotName)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Removing Slot with Name %s"), *SlotName);
	return RemoveSlot(SaveSlots.IndexOfByKey(SlotName));
}

FString UMultiSlotSaveSubsystem::GetSlotName(int SlotIndex)
{
	return SaveSlots[SlotIndex];
}

int UMultiSlotSaveSubsystem::GetSlotIndex(FString SlotName)
{
	return SaveSlots.IndexOfByKey(SlotName);
}

int UMultiSlotSaveSubsystem::GetActiveSlotIndex()
{
	return CurrentSaveSlot;
}

FString UMultiSlotSaveSubsystem::GetActiveSlotName()
{
	return SaveSlots[CurrentSaveSlot];
}

USaveGame* UMultiSlotSaveSubsystem::GetSaveGameAtSlot(int SlotIndex)
{
	if(!SaveSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Slot Index is not valid"));
		return nullptr;
	}

	// If it is the current slot, return the current save game
	if(SlotIndex == CurrentSaveSlot)
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Slot Index is the current slot"));
		return GetSaveGameObject(USaveGame::StaticClass());
	}
	
	return UGameplayStatics::LoadGameFromSlot(SaveSlots[SlotIndex], 0);
}

USaveGame* UMultiSlotSaveSubsystem::GetSaveGameAtSlotByName(FString SlotName)
{
	return GetSaveGameAtSlot(SaveSlots.IndexOfByKey(SlotName));
}

bool UMultiSlotSaveSubsystem::CreateSaveGameAtSlot(int SlotIndex, bool bOverwriteIfPresent)
{
	const int OldSlot = CurrentSaveSlot;
	SetActiveSlot(SlotIndex);
	if(bOverwriteIfPresent)
	{
		StartNewSave(true);
		OnSaveCreated.Broadcast(CurrentSaveSlot, SaveSlots[CurrentSaveSlot]);
	}
	if(OldSlot > -1) SetActiveSlot(OldSlot);
	
	return bOverwriteIfPresent;
}

bool UMultiSlotSaveSubsystem::CreateSaveGameAtSlotByName(FString SlotName, bool bOverwriteIfPresent)
{
	if(!SaveSlots.Contains(SlotName))
	{
		UE_LOG(LogSaveSystem, Warning, TEXT("Slot Name is not valid"));
		if(!AddSlot(SlotName))
		{
			UE_LOG(LogSaveSystem, Error, TEXT("Failed to add Slot"));
			return false;
		}
	}
	
	return CreateSaveGameAtSlot(SaveSlots.IndexOfByKey(SlotName), bOverwriteIfPresent);
}

bool UMultiSlotSaveSubsystem::ReceiveLoadedSaveGame(int SlotIndex, FString SlotName)
{

	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
	
	// Log the Slot Name and Index
	UE_LOG(LogSaveSystem, Display, TEXT("Loaded Save Game from Slot %s at Index %d"), *SlotName, SlotIndex);
	
	if(!SaveGame)
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game is not valid"));
		return false;
	}

	if(!SaveGame->GetClass())
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Class is not valid"));
		return false;
	}
	
	// Create a slot and save game, with the loaded save game
	if(!AddSlot(SlotName, false)) return false;
	const int OldSlot = CurrentSaveSlot;

	// Creates a new save game at the slot index, and sets it as the active slot
	SetActiveSlot(SlotIndex);
	SetSaveGameClass(SaveGame->GetClass(), false);
	LoadData();

	// Assign the loaded save game to the current save game
	AssignSaveGameObject(SaveGame);
	if(SaveGame->GetClass()->ImplementsInterface(USaveObjectInterface::StaticClass()))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object Implements Save Object Interface"));
		ISaveObjectInterface::Execute_OnObjectLoaded(SaveGame);
	}
	else
	{
		UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object Does NOT Implement Save Object Interface"));
	}
	
	OnSaveCreated.Broadcast(CurrentSaveSlot, SaveSlots[CurrentSaveSlot]);
	
	if(OldSlot > -1) SetActiveSlot(OldSlot);
	
	return true;
}

bool UMultiSlotSaveSubsystem::CreateSaveGameAtActiveSlot(bool bOverwriteIfPresent)
{
	return CreateSaveGameAtSlot(CurrentSaveSlot, bOverwriteIfPresent);
}

bool UMultiSlotSaveSubsystem::CreateSaveGameAndSetActiveSlot(FString SlotName, bool bOverwriteIfPresent)
{
	if(AddSlotAndSetAsActive(SlotName))	return CreateSaveGameAtSlotByName(SlotName, bOverwriteIfPresent);

	UE_LOG(LogSaveSystem, Error, TEXT("Slot Name is not valid"));
	return false;

}

bool UMultiSlotSaveSubsystem::CreateSaveGameWithIndexAndSetActiveSlot(int SlotIndex, bool bOverwriteIfPresent)
{
	if(SaveSlots.IsValidIndex(SlotIndex)) return CreateSaveGameAndSetActiveSlot(SaveSlots[SlotIndex], bOverwriteIfPresent);

	UE_LOG(LogSaveSystem, Error, TEXT("Slot Index is not valid"));
	return false;
}
