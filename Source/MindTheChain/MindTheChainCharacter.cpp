// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MindTheChainCharacter.h"
#include "MindTheChainProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/WorldSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMindTheChainCharacter

AMindTheChainCharacter::AMindTheChainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PowerLevel = 0.0f;
	Tether = FVector(0.0f, 0.0f, 0.0f);
	Tether_flag = false;
	Tether_distance = 0.0f;
	distance_calc = FVector(0.0f, 0.0f, 0.0f);

	portal_bridge_floor = FVector(0.0f, 0.0f, 0.0f);
	portal_bridge_ceiling = FVector(0.0f, 0.0f, 0.0f);
	portal_bridge_floor_active = false;
	portal_bridge_ceiling_active = false;
	recently_ported = false;

	world_flipped = false;
	object_in_transit = false;
	pushing = false;
	

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//volume

	soundvolfloat = 1.0f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);


}

void AMindTheChainCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMindTheChainCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMindTheChainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMindTheChainCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMindTheChainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMindTheChainCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMindTheChainCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMindTheChainCharacter::LookUpAtRate);
}

void AMindTheChainCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
		
	}
}

void AMindTheChainCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMindTheChainCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMindTheChainCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMindTheChainCharacter::add_power()
{
	PowerLevel = PowerLevel + 1.0f;	
	if (PowerLevel >= 4.f) {
		Tether_flag = true;
		UE_LOG(LogTemp, Warning, TEXT("TETHER!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("ADD: Your power level is %f"), PowerLevel);
}

void AMindTheChainCharacter::remove_power()
{
	Tether_flag = false;
	PowerLevel = PowerLevel - 1.0f;
	UE_LOG(LogTemp, Warning, TEXT("REMOVE: Your power level is %f"), PowerLevel);
}

float AMindTheChainCharacter::get_power()
{
	return PowerLevel;
}

void AMindTheChainCharacter::Set_Tether(FVector tether_update)
{
	Tether = tether_update;
}

void AMindTheChainCharacter::Set_Tether_Flag(bool flag)
{
	Tether_flag = flag;
}

void AMindTheChainCharacter::set_portal_dictionary(FVector loc)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin set portal: Your location is %s"), *loc.ToString());
	if (world_flipped) {
		UE_LOG(LogTemp, Warning, TEXT("ceiling"));
		if (portal_bridge_ceiling_active) {
			UE_LOG(LogTemp, Warning, TEXT("ceiling bridge has been set to inactive"));
			portal_dictionary_ceiling[portal_bridge_ceiling] = loc;
			portal_bridge_ceiling_active = false;
			portal_bridge_ceiling = loc;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ceiling bridge has been set to active"));
			portal_dictionary_ceiling.Add(loc, FVector(0.0f, 0.0f, 0.0f));
			portal_bridge_ceiling_active = true;
			portal_bridge_ceiling = loc;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("floor"));
		if (portal_bridge_floor_active) {
			UE_LOG(LogTemp, Warning, TEXT("floor bridge has been set to inactive"));
			portal_dictionary_floor [portal_bridge_floor] = loc;
			portal_bridge_floor_active= false;
			portal_bridge_floor = loc;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("floor bridge has been set to active"));
			portal_dictionary_floor .Add(loc, FVector(0.0f, 0.0f, 0.0f));
			portal_bridge_floor_active = true;
			portal_bridge_floor = loc;
		}
	}
	
}

FVector AMindTheChainCharacter::get_portal_coordinates(FVector loc)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting portal coords: Beginning Iteration"));
	UE_LOG(LogTemp, Warning, TEXT("Your location is %s"), *loc.ToString());
	if (world_flipped){
		UE_LOG(LogTemp, Warning, TEXT("ceiling"));
		for (auto& Elem : portal_dictionary_ceiling) {
			UE_LOG(LogTemp, Warning, TEXT("Key is  %s"), *Elem.Key.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Value is  %s"), *Elem.Value.ToString());
			if (UKismetMathLibrary::EqualEqual_VectorVector(Elem.Key, loc)) {
				UE_LOG(LogTemp, Warning, TEXT("Value was returned  %s"), *Elem.Value.ToString());
				return Elem.Value;
			}
			if (UKismetMathLibrary::EqualEqual_VectorVector(Elem.Value, loc)) {
				UE_LOG(LogTemp, Warning, TEXT("Key was returned  %s"), *Elem.Key.ToString());
				return Elem.Key;
			}
		}

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("floor"));
		for (auto& Elem : portal_dictionary_floor) {
			UE_LOG(LogTemp, Warning, TEXT("Key is  %s"), *Elem.Key.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Value is  %s"), *Elem.Value.ToString());
			if (UKismetMathLibrary::EqualEqual_VectorVector(Elem.Key, loc, 1.0f)) {
				UE_LOG(LogTemp, Warning, TEXT("Value was returned  %s"), *Elem.Value.ToString());
				return Elem.Value;
			}
			if (UKismetMathLibrary::EqualEqual_VectorVector(Elem.Value, loc, 1.0f)) {
				UE_LOG(LogTemp, Warning, TEXT("Key was returned  %s"), *Elem.Key.ToString());
				return Elem.Key;
			}
		}
	}
	return FVector(0.f, 0.f, 0.f);
}

bool AMindTheChainCharacter::get_recently_ported()
{
	return recently_ported;
}

void AMindTheChainCharacter::set_recently_ported(bool ported)
{
	recently_ported = ported;
}

bool AMindTheChainCharacter::get_flipped()
{
	return world_flipped;
}

void AMindTheChainCharacter::set_flipped(bool flip)
{
	world_flipped = flip;
}

bool AMindTheChainCharacter::get_transit()
{
	return object_in_transit;
}


void AMindTheChainCharacter::set_transit(bool transit)
{
	object_in_transit = transit;
}

bool AMindTheChainCharacter::get_portal_location_definition(FVector loc)
{
	return false;
}

void AMindTheChainCharacter::set_turn_rate(float rate)
{
	BaseTurnRate = rate;
	BaseLookUpRate = rate;
}

float AMindTheChainCharacter::get_turn_rate()
{
	return BaseTurnRate;
}

void AMindTheChainCharacter::set_vol(float vol)
{
	soundvolfloat = vol;
}

float AMindTheChainCharacter::get_vol()
{
	return soundvolfloat;
}

bool AMindTheChainCharacter::get_pushing()
{
	return pushing;
}

void AMindTheChainCharacter::set_pushing(bool whatislove)
{
	pushing = whatislove;
}

void AMindTheChainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Tether_flag) {
	distance_calc = Tether - this->GetActorLocation(); 
	Tether_distance = distance_calc.Size();
	GetCharacterMovement()->MaxWalkSpeed = 600.0f - (Tether_distance/3);
	}
	else {
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}
