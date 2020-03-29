// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TestActor.h"
#include "Engine.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestActor::XFunction1(int a, FString b, float c)
{

}

void ATestActor::XFunction2()
{

}

int ATestActor::XFunction3()
{
	return 0;
}

int ATestActor::XFunction4(int a, FString t)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, t);
	return a;
}

int ATestActor::XFunction6(int a, TArray<FString> t)
{
	for (auto x : t)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, x);
	return a;
}

