// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveObjectInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface is used with UObject derived classes that are saved to be able to receive a callback when the object is loaded.
 */
class SAVESYSTEM_API ISaveObjectInterface
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveSystem|Interface Functions")
	void OnObjectPreSave(UObject* WorldContext);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveSystem|Interface Functions")
    void OnObjectLoaded(UObject* WorldContext);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SaveSystem|Interface Functions")
	void OnObjectSaved(UObject* WorldContext);
};
