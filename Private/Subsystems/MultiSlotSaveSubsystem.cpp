// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/MultiSlotSaveSubsystem.h"

#include "SaveGameSystem.h"
#include "ShaderPrintParameters.h"
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

bool UMultiSlotSaveSubsystem::AddEmptySlot(FString SlotName)
{
	// Check if the Slot already exists, as we don't want to create a new one with the same name
	if(SaveSlots.Contains(SlotName))
	{
		// Slot already exists
		UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s already exists"), *SlotName);
		return false;
	}

	
	// Since we are creating an empty slot, we don't want to create a new Save Game Object, so we pass nullptr
	SaveSlots.Add(SlotName, nullptr);

	UE_LOG(LogSaveSystem, Display, TEXT("Empty Slot %s added"), *SlotName);
	
	return true;
}

bool UMultiSlotSaveSubsystem::AddSlot(FString SlotName)
{

	// Check if the Slot already exists, as we don't want to create a new one with the same name
	if(SaveSlots.Contains(SlotName))
	{
		// Slot already exists
		UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s already exists"), *SlotName);
		return true;
	}

	

	// Check if the Save Game Object already exists on disk. If not then create a new one, otherwise load it
	if(!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		
		UE_LOG(LogSaveSystem, Display, TEXT("Creating Save Game Object for Slot %s"), *SlotName);

		// Creates a new Save Game Object, and adds it to the SaveSlots Map
		const TObjectPtr<USaveGame> NewSaveGame = UGameplayStatics::CreateSaveGameObject(GetSaveGameClass());
		if(IsValid(NewSaveGame))
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object Created for Slot %s"), *SlotName);
			SaveSlots.Add(SlotName, NewSaveGame);
			CreatedSaveGames.Add(NewSaveGame);
			OnSlotAdded.Broadcast(SlotName);
			OnSaveCreated.Broadcast(SlotName);
			return true;
		}

		UE_LOG(LogSaveSystem, Error, TEXT("Failed to create Save Game Object for Slot %s"), *SlotName);
		return false;
		
	}
	UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object for Slot %s already exists on Disk. Attempting to Load"), *SlotName);
	
	// Since we know that the Save Game Object already exists on disk and that the save slots don't currently have it, we can just load it
	if(LoadSlotFromDisk(SlotName))
	{
		// Only need to broadcast if the slot was actually loaded. No need to broadcast the OnSaveCreated event as the slot already exists on disk
		OnSlotAdded.Broadcast(SlotName);
		return true;
	}
	return false;
}

bool UMultiSlotSaveSubsystem::AddSlotAndSetActive(FString SlotName, bool bLoad)
{
	UE_LOG(LogSaveSystem, Display, TEXT("Adding Slot %s and setting it as active"), *SlotName);

	// If the slot was added successfully or the Slot already exists, then set it as active
	if(AddSlot(SlotName))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Slot %s added and can be set as active"), *SlotName);
		return SetActiveSlot(SlotName, bLoad);
	}
	
	UE_LOG(LogSaveSystem, Error, TEXT("Failed to add Slot %s and set it as active"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::RemoveSlot(FString SlotName)
{
	// Remove the slot if it exists
	if(SaveSlots.Contains(SlotName))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Removing Slot %s"), *SlotName);
		
		// get a reference to the save game object
		TWeakObjectPtr<USaveGame> SaveGame = SaveSlots[SlotName];

		// If the number of removed items is greater than 0, then the slot was removed as well as any duplicates that may have existed
		const bool bResult = SaveSlots.Remove(SlotName) > 0;
		OnSlotRemoved.Broadcast(SlotName);

		// If the save game object is still valid, then we can destroy it
		// TODO: This may not be necessary as the save game object should be destroyed when the slot is removed as it is a weak pointer
		if(bResult && SaveGame.IsValid())
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object for Slot %s is valid, attempting Destroy"), *SlotName);
			SaveGame.Get()->ConditionalBeginDestroy();
		}
		else if(!SaveGame.IsValid())
		{
			UE_LOG(LogSaveSystem, Warning, TEXT("Save Game Object for Slot %s is invalid, Cannot Destroy"), *SlotName);
		}

		if(bResult)
		{
			// Check that the SlotName was not the active slot
			if(SlotName == CurrentSaveSlot)
			{
				UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s was the active slot, assigning to empty string"), *SlotName);
				// Assign it to an empty string
				CurrentSaveSlot = "";
			}
		}
		
		return bResult;
	}

	UE_LOG(LogSaveSystem, Warning, TEXT("Slot %s does not exist, Cannot Remove"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::RemoveActiveSlot()
{
	return RemoveSlot(CurrentSaveSlot);
}

bool UMultiSlotSaveSubsystem::DeleteSlot(FString SlotName)
{
	if(RemoveSlot(SlotName) && UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Deleting Slot %s"), *SlotName);
		return UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}

	return false;
}

bool UMultiSlotSaveSubsystem::DeleteActiveSlot()
{
	return DeleteSlot(GetActiveSlot());
}

bool UMultiSlotSaveSubsystem::SaveSlot(FString SlotName, bool bAsync)
{
	
	// Save the slot if it exists
	if(SaveSlots.Contains(SlotName) && SaveSlots[SlotName].IsValid())
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s"), *SlotName);

		// Call the OnObjectPreSave Interface on the Save Game Object
		if(SaveSlots[SlotName].Get()->GetClass()->ImplementsInterface(USaveObjectInterface::StaticClass()))
		{
			ISaveObjectInterface::Execute_OnObjectPreSave(SaveSlots[SlotName].Get(), this);
		}
		
		// Save the slot asynchronously if requested, otherwise save it synchronously
		if(bAsync)
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s asynchronously"), *SlotName);
			
			FAsyncSaveGameToSlotDelegate asyncSaveDelegate;
			asyncSaveDelegate.BindUObject(this, &USaveSubsystem::OnAsyncSaveFinished);
			
			UGameplayStatics::AsyncSaveGameToSlot(SaveSlots[SlotName].Get(), SlotName, 0, asyncSaveDelegate);
		}
		else
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Saving Slot %s synchronously"), *SlotName);
			
			// Save the slot synchronously and return the result
			if(UGameplayStatics::SaveGameToSlot(SaveSlots[SlotName].Get(), SlotName, 0))
			{
				// If the save succeeds, then for the sake of consistency, we call the OnAsyncSaveFinished function with a success result
				OnAsyncSaveFinished(SlotName, 0, true);

				if(SaveSlots[SlotName].Get()->GetClass()->ImplementsInterface(USaveObjectInterface::StaticClass()))
				{
					ISaveObjectInterface::Execute_OnObjectSaved(SaveSlots[SlotName].Get(), this);
				}
				
				return true;
			}
			
			UE_LOG(LogSaveSystem, Error, TEXT("Failed to save Slot %s synchronously"), *SlotName);
			// If the save fails, return false
			return false;
		}
		return true;
	}

	UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s or Save Game Object is Invalid"), *SlotName);
	return false;
	
}

bool UMultiSlotSaveSubsystem::SaveActiveSlot(bool bAsync)
{
	// Save the Active Slot if it exists
	return SaveSlot(GetActiveSlot(), bAsync);

}

bool UMultiSlotSaveSubsystem::SetActiveSlot(const FString& String, bool bLoad)
{
	// Set the Active Slot if it exists and is valid
	if(SaveSlots.Contains(String) && SaveSlots[String].IsValid())
	{
		CurrentSaveSlot = String;
		if(bLoad)
		{
			LoadData();
		}
		return true;
	}
	if(SaveSlots.Contains(String) && bLoad)
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Save Game Object for Slot %s is invalid, attempting to load from disk"), *String);
		return LoadSlot(String);
	}


	UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s"), *String);
	return false;
}

TArray<FString> UMultiSlotSaveSubsystem::GetAllSaveSlotNames() const
{
	// Get all the save slot names
	TArray<FString> SlotNames;
	SaveSlots.GetKeys(SlotNames);
	// Iterate over the slot names
	for(auto SlotName : SlotNames)
	{
		// If the slot name is empty, remove it from the array
		if(SlotName.IsEmpty())
		{
			SlotNames.Remove(SlotName);
		}

		UE_LOG(LogSaveSystem, Display, TEXT("Get All Save Slot Names: Slot Name: %s"), *SlotName);
	}
	return SlotNames;
}

TArray<USaveGame*> UMultiSlotSaveSubsystem::GetAllSaveSlots()
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

	UE_LOG(LogTemp, Warning, TEXT("Created and Valid Save Games: %d"), TempArray.Num());
	
	return TempArray;
}

USaveGame* UMultiSlotSaveSubsystem::GetSaveSlot(FString SlotName)
{
	if(!SaveSlots.Contains(SlotName))
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s"), *SlotName);
		return nullptr;
	}

	if(!SaveSlots[SlotName].IsValid())
	{
		UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object is invalid for Slot %s"), *SlotName);
		return nullptr;
	}
	
	return SaveSlots[SlotName].Get();
}

USaveGame* UMultiSlotSaveSubsystem::GetActiveSaveSlot()
{
	return GetSaveSlot(CurrentSaveSlot);
}

USaveGame* UMultiSlotSaveSubsystem::GetSaveGameObject(const TSubclassOf<USaveGame> SaveGameClass)
{
	// Get the Active Save Game if it exists
 	if(SaveSlots.Contains(GetPlayerSaveSlot()))
 	{
 		return SaveSlots[CurrentSaveSlot].Get();
 	}
 	return nullptr;
}

USaveGame* UMultiSlotSaveSubsystem::GetRawSaveGameObject()
{
	// Get the Active Save Game if it exists
	if(SaveSlots.Contains(GetPlayerSaveSlot()))
	{
		return SaveSlots[CurrentSaveSlot].Get();
	}
	return nullptr;
}


bool UMultiSlotSaveSubsystem::LoadSlot(FString SlotName, bool bAsync)
{
	// Load the slot if it exists
	if(SaveSlots.Contains(SlotName) && SaveSlots[SlotName].IsValid())
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s"), *SlotName);

		// If the slot is being loaded asynchronously, bind the delegate and load the slot
		if(bAsync)
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s asynchronously"), *SlotName);
			
			FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
			asyncLoadDelegate.BindUObject(this, &USaveSubsystem::OnAsyncLoadFinished);
			
			UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, asyncLoadDelegate);
		}
		// If the slot is being loaded synchronously, load the slot and call the function to handle the loaded slot
		else
		{
			UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s synchronously"), *SlotName);
			
			OnAsyncLoadFinished(SlotName, 0, UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		}
		return true;
	}

	// If the Slot is not found, try to load it from disk
	if(LoadSlotFromDisk(SlotName))
	{
		return true;
	}

	UE_LOG(LogSaveSystem, Error, TEXT("Save Game Object does not exist for Slot %s or Save Game Object is Invalid"), *SlotName);
	return false;
}

bool UMultiSlotSaveSubsystem::LoadActiveSlot(bool bAsync)
{
	return LoadSlot(CurrentSaveSlot, bAsync);
}

bool UMultiSlotSaveSubsystem::LoadSlotFromDisk(FString SlotName)
{
	// Load the slot if it exists on disk
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogSaveSystem, Display, TEXT("Loading Slot %s from disk"), *SlotName);

		FAsyncLoadGameFromSlotDelegate asyncLoadDelegate;
		asyncLoadDelegate.BindLambda([this](const FString& SlotName, const int32 UserIndex, USaveGame* LoadedSaveGame)
		{
			if(!IsValid(LoadedSaveGame))
			{
				UE_LOG(LogSaveSystem, Error, TEXT("Failed to load Slot %s from disk"), *SlotName);
				return;
			}
		
			SaveSlots.Add(SlotName, LoadedSaveGame);
			
			if(SaveSlots[SlotName].IsValid() && SaveSlots[SlotName]->Implements<USaveObjectInterface>())
			{
				ISaveObjectInterface::Execute_OnObjectLoaded(SaveSlots[SlotName].Get(), this);
			}
			UE_LOG(LogSaveSystem, Display, TEXT("Successful Async Load Slot %s from disk"), *SlotName);
			OnPlayerDataLoaded.Broadcast(SaveSlots[SlotName].Get());
		});
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, asyncLoadDelegate);
		return true;
	}
	return false;
}
