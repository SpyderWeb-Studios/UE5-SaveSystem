// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MultiSlotSaveSubsystem.h"

#include "SaveGameSystem.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/SaveObjectInterface.h"
#include "Kismet/GameplayStatics.h"

void UMultiSlotSaveSubsystem::Deinitialize()
{
	// Cleaning up the event dispatchers, to prevent undefined behaviour
	OnSlotRemoved.Clear();
	OnSlotAdded.Clear();
		
	Super::Deinitialize();
}

bool UMultiSlotSaveSubsystem::AddSlot(FString SlotName, bool bVerbose)
{

	// Check if the Slot already exists, as we don't want to create a new one with the same name
	if(SaveSlots.Contains(SlotName))
	{
		// Slot already exists
		if(bVerbose) UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s already exists"), *SlotName);
		return false;
	}

	// Check if the Save Game Object already exists on disk. If not then create a new one, otherwise load it
	if(!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Creating Save Game Object for Slot %s"), *SlotName);

		// Creates a new Save Game Object, and adds it to the SaveSlots Map
		const TObjectPtr<USaveGame> NewSaveGame = UGameplayStatics::CreateSaveGameObject(GetSaveGameClass());
		if(IsValid(NewSaveGame))
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object Created for Slot %s"), *SlotName);
			SaveSlots.Add(SlotName, NewSaveGame);
			CreatedSaveGames.Add(NewSaveGame);
			OnSlotAdded.Broadcast(SlotName);
			OnSaveCreated.Broadcast(SlotName);
			return true;
		}

		if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Failed to create Save Game Object for Slot %s"), *SlotName);
		return false;
		
	}
	if(bVerbose) UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object for Slot %s already exists on Disk. Attempting to Load"), *SlotName);
	
	// Since we know that the Save Game Object already exists on disk and that the save slots don't currently have it, we can just load it
	if(LoadSlotFromDisk(SlotName, bVerbose))
	{
		// Only need to broadcast if the slot was actually loaded. No need to broadcast the OnSaveCreated event as the slot already exists on disk
		OnSlotAdded.Broadcast(SlotName);
		return true;
	}
	return false;
}

bool UMultiSlotSaveSubsystem::AddSlotAndSetActive(FString SlotName, bool bLoad, bool bVerbose)
{
	if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Adding Slot %s and setting it as active"), *SlotName);

	// If the slot was added successfully, then set it as active
	if(AddSlot(SlotName))
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Slot %s added and can be set as active"), *SlotName);
		return SetActiveSlot(SlotName, bLoad);
	}
	
	if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Failed to add Slot %s and set it as active"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::RemoveSlot(FString SlotName, bool bVerbose)
{
	// Remove the slot if it exists
	if(SaveSlots.Contains(SlotName))
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Removing Slot %s"), *SlotName);
		
		// get a reference to the save game object
		TWeakObjectPtr<USaveGame> SaveGame = SaveSlots[SlotName];

		// If the number of removed items is greater than 0, then the slot was removed as well as any duplicates that may have existed
		const bool bResult = SaveSlots.Remove(SlotName) > 0;
		OnSlotRemoved.Broadcast(SlotName);

		// If the save game object is still valid, then we can destroy it
		// TODO: This may not be necessary as the save game object should be destroyed when the slot is removed as it is a weak pointer
		if(bResult && SaveGame.IsValid())
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object for Slot %s is valid, attempting Destroy"), *SlotName);
			SaveGame.Get()->BeginDestroy();
		}
		else
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object for Slot %s is invalid, Cannot Destroy"), *SlotName);
		}
		
		return bResult;
	}

	if(bVerbose) UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s does not exist, Cannot Remove"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::RemoveActiveSlot(bool bVerbose)
{
	return RemoveSlot(CurrentSaveSlot, bVerbose);
}

bool UMultiSlotSaveSubsystem::SaveSlot(FString SlotName, bool bAsync, bool bVerbose)
{
	// Save the slot if it exists
	if(SaveSlots.Contains(SlotName) && SaveSlots[SlotName].IsValid())
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s"), *SlotName);

		// Save the slot asynchronously if requested, otherwise save it synchronously
		if(bAsync)
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s asynchronously"), *SlotName);
			
			FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
			asyncSaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveFinished);
			
			UGameplayStatics::AsyncSaveGameToSlot(SaveSlots[SlotName].Get(), SlotName, 0, asyncSaveDelegate);
		}
		else
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s synchronously"), *SlotName);
			
			// Save the slot synchronously and return the result
			if(UGameplayStatics::SaveGameToSlot(SaveSlots[SlotName].Get(), SlotName, 0))
			{
				// If the save succeeds, then for the sake of consistency, we call the OnAsyncSaveFinished function with a success result
				OnAsyncSaveFinished(SlotName, 0, true);
				return true;
			}
			
			if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Failed to save Slot %s synchronously"), *SlotName);
			// If the save fails, return false
			return false;
		}
		return true;
	}

	if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s or Save Game Object is Invalid"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::SaveActiveSlot(bool bAsync, bool bVerbose)
{
	// Save the Active Slot if it exists
	return SaveSlot(CurrentSaveSlot, bAsync, bVerbose);
}

bool UMultiSlotSaveSubsystem::SetActiveSlot(const FString& String, bool bLoad, bool bVerbose)
{
	// Set the Active Slot if it exists and is valid
	if(SaveSlots.Contains(String) && SaveSlots[String].IsValid())
	{
		CurrentSaveSlot = String;
		if(bLoad)
		{
			LoadData(bVerbose);
		}
		return true;
	}


	if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s"), *String);
	return false;
}

TArray<FString> UMultiSlotSaveSubsystem::GetAllSaveSlotNames() const
{
	// Get all the save slot names
	TArray<FString> SlotNames;
	SaveSlots.GetKeys(SlotNames);
	return SlotNames;
}

TArray<USaveGame*> UMultiSlotSaveSubsystem::GetAllSaveSlots(bool bVerbose)
{
	// Get All the Created Save Games and add them to a temp array if they are valid
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


USaveGame* UMultiSlotSaveSubsystem::GetSaveSlot(FString SlotName, bool bVerbose)
{
	if(!SaveSlots.Contains(SlotName))
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s"), *SlotName);
		return nullptr;
	}

	if(!SaveSlots[SlotName].IsValid())
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object is invalid for Slot %s"), *SlotName);
		return nullptr;
	}
	
	return SaveSlots[SlotName].Get();
}

USaveGame* UMultiSlotSaveSubsystem::GetActiveSaveSlot(bool bVerbose)
{
	return GetSaveSlot(CurrentSaveSlot, bVerbose);
}

bool UMultiSlotSaveSubsystem::LoadSlot(FString SlotName, bool bAsync, bool bVerbose)
{
	// Load the slot if it exists
	if(SaveSlots.Contains(SlotName) && SaveSlots[SlotName].IsValid())
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s"), *SlotName);

		// If the slot is being loaded asynchronously, bind the delegate and load the slot
		if(bAsync)
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s asynchronously"), *SlotName);
			
			FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
			asyncLoadDelegate.BindUObject(this, &USaveSubsystem::OnAsyncLoadFinished);
			
			UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, asyncLoadDelegate);
		}
		// If the slot is being loaded synchronously, load the slot and call the function to handle the loaded slot
		else
		{
			if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s synchronously"), *SlotName);
			
			OnAsyncLoadFinished(SlotName, 0, UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		}
		return true;
	}

	// If the Slot is not found, try to load it from disk
	if(LoadSlotFromDisk(SlotName, bVerbose))
	{
		return true;
	}

	if(bVerbose) UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s or Save Game Object is Invalid"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::LoadActiveSlot(bool bAsync, bool bVerbose)
{
	return LoadSlot(CurrentSaveSlot, bAsync, bVerbose);
}

bool UMultiSlotSaveSubsystem::LoadSlotFromDisk(FString SlotName, bool bVerbose)
{
	// Load the slot if it exists on disk
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		if(bVerbose) UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s from disk"), *SlotName);
		
		SaveSlots.Add(SlotName, UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		return true;
	}

	return false;
}
