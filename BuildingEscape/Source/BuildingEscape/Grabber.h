// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	// How far ahead of the player can we reach in cm
	float Reach = 100.f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	// Ray-cast and grab what is in reach
	void Grab();

	// Call when Grab released
	void Release();

	// Find attached physics component
	void FindPhysicsHandleComponent();

	// Setup attached input component
	void SetupInputComponent();

	// Return hit for first physics body in reach
	const FHitResult GetFirstPhysicsBodyInReach();
};
