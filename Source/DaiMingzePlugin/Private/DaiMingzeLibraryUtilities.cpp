// Fill out your copyright notice in the Description page of Project Settings.


#include "DaiMingzeLibraryUtilities.h"


#include "Engine.h"

template<typename T>
void SetParams(TFieldIterator<UProperty>& Iterator, void* ParamsBuffer, T&& t)
{
	while (Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->GetFName().ToString().StartsWith("__"))//ignore private param like __WolrdContext of function in blueprint funcion library
			continue;
		void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
		if (!(Property->PropertyFlags & CPF_ReturnParm) && (Property->PropertyFlags & CPF_Parm))
		{
			//调用构造函数
			new(PropertyBuffer) std::remove_reference<T>::type(t);
			break;
		}
		++Iterator;
	}
}

template<typename T, typename... TArgs>
void SetParams(TFieldIterator<UProperty>& Iterator, void* ParamsBuffer, T&& t, TArgs&& ...Args)
{
	while (Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->GetFName().ToString().StartsWith("__"))//ignore private param like __WolrdContext of function in blueprint funcion library
			continue;
		void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
		if (!(Property->PropertyFlags & CPF_ReturnParm) && (Property->PropertyFlags & CPF_Parm))
		{
			//调用构造函数
			new(PropertyBuffer) std::remove_reference<T>::type(t);
			++Iterator;
			return SetParams(Iterator, ParamsBuffer, Forward<TArgs>(Args)...);
		}
		++Iterator;
	}
}

template<typename T>
void FreeMemory(TFieldIterator<UProperty>& Iterator, void* ParamsBuffer, T&& t)
{
	using RemoveRefT = typename std::remove_reference<T>::type;
	while (Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->GetFName().ToString().StartsWith("__"))//ignore private param like __WolrdContext of function in blueprint funcion library
			continue;
		void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
		if (!(Property->PropertyFlags & CPF_ReturnParm) && (Property->PropertyFlags & CPF_Parm))
		{
			//调用析构函数
			((RemoveRefT*)PropertyBuffer)->~RemoveRefT();
			++Iterator;
			break;
		}
		++Iterator;
	}
}

template<typename T, typename... TArgs>
void FreeMemory(TFieldIterator<UProperty>& Iterator, void* ParamsBuffer, T&& t, TArgs&& ...Args)
{
	using RemoveRefT = typename std::remove_reference<T>::type;
	while (Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->GetFName().ToString().StartsWith("__"))//ignore private param like __WolrdContext of function in blueprint funcion library
			continue;
		void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
		if (!(Property->PropertyFlags & CPF_ReturnParm) && (Property->PropertyFlags & CPF_Parm))
		{
			//调用析构函数
			((RemoveRefT*)PropertyBuffer)->~RemoveRefT();
			++Iterator;
			return FreeMemory(Iterator, ParamsBuffer, Forward<TArgs>(Args)...);
		}
		++Iterator;
	}
}

template<typename TReturn, typename... TArgs>
void UDaiMingzeLibraryUtilities::ProcessFunction(UObject* Object, UFunction* Function, TReturn& Return, TArgs&& ...Args)
{
	//申请内存对齐的内存放置参数
	void* ParamsBuffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
	{
		//填充参数 使用placement new
		TFieldIterator<UProperty> Iterator(Function);
		SetParams(Iterator, ParamsBuffer, Forward<TArgs>(Args)...);
	}
	//执行函数
	Object->ProcessEvent(Function, ParamsBuffer);   //call function
	{
		//由于是placement new 使用调用析构函数防止内存泄漏 
		TFieldIterator<UProperty> Iterator(Function);
		FreeMemory(Iterator, ParamsBuffer, Forward<TArgs>(Args)...);
	}
	//读取返回值
	for (TFieldIterator<UProperty> Iterator(Function); Iterator; ++Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->PropertyFlags & CPF_ReturnParm)
		{
			void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
			Return = *(TReturn*)PropertyBuffer;
		}
	}
}

template<typename TReturn, typename... TArgs>
void UDaiMingzeLibraryUtilities::ProcessFunction(UClass* Class, UFunction* Function, TReturn& Return, TArgs&& ...Args)
{
	//申请内存对齐的内存放置参数
	void* ParamsBuffer = (uint8*)FMemory_Alloca(Function->ParmsSize);
	{
		//填充参数 使用placement new
		TFieldIterator<UProperty> Iterator(Function);
		SetParams(Iterator, ParamsBuffer,  Forward<TArgs>(Args)...);
	}
	//执行函数
	Class->ProcessEvent(Function, ParamsBuffer);   //call function
	{
		//由于是placement new 使用调用析构函数防止内存泄漏 
		TFieldIterator<UProperty> Iterator(Function);
		FreeMemory(Iterator, ParamsBuffer, Forward<TArgs>(Args)...);
	}
	//读取返回值
	for (TFieldIterator<UProperty> Iterator(Function); Iterator; ++Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->PropertyFlags & CPF_ReturnParm)
		{
			void* PropertyBuffer = Property->ContainerPtrToValuePtr<void*>(ParamsBuffer);
			Return = *(TReturn*)PropertyBuffer;
		}
	}
}

FString UDaiMingzeLibraryUtilities::DescriptionFunction(UFunction* Function)
{
	FString ReturnVal;
	FString FunctionName = Function->GetName();
	FString ReturnTypeName;
	TArray<FString> ArgsTypeName;
	for (TFieldIterator<UProperty> Iterator(Function); Iterator; ++Iterator)
	{
		UProperty* Property = *Iterator;
		if (Property->PropertyFlags & CPF_ReturnParm)
		{
			ReturnTypeName = Property->GetCPPType();
		}
		else if (Property->PropertyFlags & CPF_Parm)
		{
			ArgsTypeName.Add(FString::Printf(TEXT("%s %s"), *Property->GetCPPType(), *Property->GetNameCPP()));
		}
	}
	ReturnVal = ReturnTypeName + TEXT(" ") + FunctionName + TEXT("(");
	for (int32 i = 0; i < ArgsTypeName.Num(); i++)
	{
		ReturnVal += FString::Printf(TEXT("%s, "), *ArgsTypeName[i]);
	}
	ReturnVal += TEXT(")");
	return ReturnVal;
}


void UDaiMingzeLibraryUtilities::GetAllFunctionsOfObject(UObject* InObject, TArray<UFunction*>& OutFunctions)
{
	OutFunctions.Empty();
	for (TFieldIterator<UFunction> FuncIt(InObject->GetClass()); FuncIt; ++FuncIt)
	{
		UFunction * Function = *FuncIt;
		OutFunctions.Push(Function);
	}
}

void UDaiMingzeLibraryUtilities::GetFunctionByName(UObject* InObject, UFunction*& OutFunction, FString FuncName)
{
	for (TFieldIterator<UFunction> FuncIt(InObject->GetClass()); FuncIt; ++FuncIt)
	{
		UFunction * Function = *FuncIt;
		FString FunctionName = Function->GetName();
		//if (FunctionName.Find("X") == 0)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FunctionName);
		//}
		if (!Function->GetName().Compare(FuncName))
		{
			OutFunction = Function;
			return;
		}
	}
}

void UDaiMingzeLibraryUtilities::GetAllPropertiesOfObject(UObject* InObject, TArray<UProperty*>& OutProperties)
{
	OutProperties.Empty();
	for (TFieldIterator<UProperty> PropIt(InObject->GetClass()); PropIt; ++PropIt)
	{
		UProperty* Property = *PropIt;
		OutProperties.Push(Property);
		FString WhatTType = Property->GetClass()->GetName();
		FString WhatName = Property->GetName();
		FString PrintOutput = FString::Printf(TEXT(" %s : %s"), *WhatTType, *WhatName);
		if (WhatName.Find("X") == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, PrintOutput);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, Property->GetOwnerClass()->GetName());
		}
	}
}

void UDaiMingzeLibraryUtilities::GetUPropertyType(UProperty* Property, UClass * Class)
{
	Class = Property->GetClass();
}

void UDaiMingzeLibraryUtilities::testfunc(UObject* InObject)
{
	TArray<UProperty*> PS;
	GetAllPropertiesOfObject(InObject, PS);
}

void UDaiMingzeLibraryUtilities::testcallfunc(UObject* InObject, FString FuncName, int num, FString inx, int ret)
{
	UFunction * OutFunction = nullptr;
	GetFunctionByName(InObject, OutFunction, FuncName);
	//ProcessFunction1(InObject, OutFunction, num, inx);
	int32 outParams;
	TArray<FString> strarr;
	strarr.Add(inx);
	strarr.Add(inx);
	strarr.Add(inx);
	strarr.Add(inx);
	ProcessFunction(InObject, OutFunction, outParams, num, strarr);
	//InvokeFunction(InObject->GetClass(), InObject, OutFunction, outParams, num, inx);
}

