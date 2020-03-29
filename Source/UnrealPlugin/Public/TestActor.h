// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

USTRUCT(BlueprintType)
struct FTestStruct
{
	GENERATED_BODY()
	double XDouble;
};

UCLASS()
class UNREALPLUGIN_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	float XFloat;
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	int32 XInt32;
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	int64 XInt64;
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	ATestActor *  XTestActor;
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	FTestStruct XTestStruct;
	UPROPERTY(BlueprintReadWrite, Category = "Test")
	bool XBool;

	UFUNCTION(BlueprintCallable)
	void XFunction1(int a,FString b, float c);

	UFUNCTION(BlueprintCallable)
	void XFunction2();

	UFUNCTION(BlueprintCallable)
	int XFunction3();


	UFUNCTION(BlueprintCallable)
	int XFunction4(int a,FString t);

	UFUNCTION(BlueprintCallable)
	int XFunction6(int a, TArray<FString> t);

};
