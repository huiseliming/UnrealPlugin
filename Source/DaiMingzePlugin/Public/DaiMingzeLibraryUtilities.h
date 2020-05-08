// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DaiMingzeLibraryUtilities.generated.h"

/**
 * 
 */
UCLASS()
class DAIMINGZEPLUGIN_API UDaiMingzeLibraryUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/* Utilities BEGIN */

	template<typename TReturn, typename... TArgs>
	static void ProcessFunction(UObject* Object, UFunction* Function, TReturn& Return, TArgs&&...Args);

	template<typename TReturn, typename... TArgs>
	static void ProcessFunction(UClass* Class, UFunction* Function, TReturn& Return, TArgs&& ...Args);

	UFUNCTION(BlueprintPure, Category = "DaiMingze|Utilitie::BlueprintLibrary")
	static FString DescriptionFunction(UFunction* Function);

	/* Utilities END */

	/* Test BEGIN */
	static void GetAllFunctionsOfObject(UObject* InObject, TArray<UFunction *>& OutFunctions);

	static void GetFunctionByName(UObject* InObject, UFunction*& OutFunction, FString FuncName);

	static void GetAllPropertiesOfObject(UObject* InObject, TArray<UProperty *>& OutProperties);

	static void GetUPropertyType(UProperty* Property, UClass * Class);

	UFUNCTION(BlueprintCallable, Category = "ReflectionBlueprint")
	static void testfunc(UObject* InObject);

	UFUNCTION(BlueprintCallable, Category = "ReflectionBlueprint")
	static void testcallfunc(UObject* InObject, FString FuncName, int num, FString inx, int ret);
	/* Test END */
};
