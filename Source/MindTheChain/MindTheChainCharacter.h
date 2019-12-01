// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MindTheChainCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AMindTheChainCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;


public:
	AMindTheChainCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMindTheChainProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Powah", meta = (AllowPrivateAccess = "true"))
	float PowerLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Powah", meta = (AllowPrivateAccess = "true"))
	FVector Tether;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Powah", meta = (AllowPrivateAccess = "true"))
	bool Tether_flag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Powah", meta = (AllowPrivateAccess = "true"))
	float Tether_distance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Powah", meta = (AllowPrivateAccess = "true"))
	FVector distance_calc;

	TMap<FVector, FVector> portal_dictionary_floor;

	TMap<FVector, FVector> portal_dictionary_ceiling;

	bool portal_bridge_floor_active;

	bool portal_bridge_ceiling_active;

	FVector portal_bridge_floor;

	FVector portal_bridge_ceiling;

	bool recently_ported;

	bool world_flipped;

	bool object_in_transit;

	bool pushing;

	float soundvolfloat;

protected:
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);


	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface




public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void add_power();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void remove_power();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	float get_power();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void Set_Tether(FVector tether_update);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void Set_Tether_Flag(bool flag);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_portal_dictionary(FVector loc);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	FVector get_portal_coordinates(FVector loc);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	bool get_recently_ported();
	
	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_recently_ported(bool ported);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	bool get_flipped();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_flipped(bool flip);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	bool get_transit();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_transit(bool transit);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	bool get_portal_location_definition(FVector loc);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_turn_rate(float rate);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	float get_turn_rate();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_vol(float vol);

	UFUNCTION(BlueprintCallable, Category = "Powah")
	float get_vol();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	bool get_pushing();

	UFUNCTION(BlueprintCallable, Category = "Powah")
	void set_pushing(bool whatislove);

	void Tick(float DeltaTime);

};

