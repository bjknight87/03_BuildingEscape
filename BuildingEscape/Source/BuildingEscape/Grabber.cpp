// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::SetupInputComponent() 
{	
	// Get input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Input component component not found in SetupInputComponent method for %s!\n"), *GetOwner()->GetName());
	}
	else if (InputComponent) 
	{
		/// Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Input Component not found for %s!\n"), *GetOwner()->GetName());
	}
}

void UGrabber::FindPhysicsHandleComponent() 
{
	/// Look for attached physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("Physics handle component not found for %s!\n"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// if the physics handle is attached
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Physics handle component not found in TickComponent method for %s!\n"), *GetOwner()->GetName()); 
		return; 
	}
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we are holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab()
{
	/// Line trace and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent(); // gets the mesh in our case
	auto ActorHit = HitResult.GetActor();

	/// If we hit something then attach a physics handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { 
			UE_LOG(LogTemp, Error, TEXT("Physics handle component not found in Grab method for %s!\n"), *GetOwner()->GetName());
			return; 
		}
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // no bones so no name needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { 
		UE_LOG(LogTemp, Error, TEXT("Physics handle component not found in Release method for %s!\n"), *GetOwner()->GetName()); 
		return; 
	}
	PhysicsHandle->ReleaseComponent();
}


const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Setup query parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	// Logs what got hit
	WhatGotHit(HitResult);
	return HitResult;
}

void UGrabber::WhatGotHit(FHitResult HitResult)
{
	/// See what we hit
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit == nullptr) { 
		UE_LOG(LogTemp, Error, TEXT("Hit Result not found, null pointer!"));
		return; 
	} else if (ActorHit) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()));
	}
}

FVector UGrabber::GetReachLineEnd()
{
	/// Get Player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetReachLineStart()
{
	/// Get Player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}