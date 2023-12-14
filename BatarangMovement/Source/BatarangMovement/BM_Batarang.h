// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "C:/Program Files/Epic Games/UE_5.2/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"
#include "BM_Batarang.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPointLightComponent;

UCLASS()
class BATARANGMOVEMENT_API ABM_Batarang : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<USceneComponent> spinComponent = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<USceneComponent> rollComponent = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<USceneComponent> pitchComponent = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<UStaticMeshComponent> staticMesh = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<USpringArmComponent> springArm = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<UCameraComponent> camera = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Settings|Components")
	TObjectPtr<UPointLightComponent> pointLight = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Settings|InputsMapping")
	TObjectPtr<UInputAction> speedInput = nullptr;
	UPROPERTY(EditAnywhere, Category = "Settings|InputsMapping")
	TObjectPtr<UInputAction> directionInput = nullptr;
	UPROPERTY(EditAnywhere, Category = "Settings|InputsMapping")
	TObjectPtr<UInputMappingContext> inputMappingContext = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float turnSpeed = 50;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float pitchSpeed = 25;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float speedIncr = 500;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float maxAngle = 30;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	float minAngle = -45;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	bool isRolling = false;
	UPROPERTY(EditAnywhere, Category = "Settings|Movement")
	bool isPitching = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float minSpeed = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float maxSpeed = 3000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float actualSpeed = 1500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	FColor accelerationColor = FColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	FColor brakeColor = FColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	FColor baseColor = FColor::Cyan;

public:
	ABM_Batarang();

protected:
	virtual void BeginPlay() override;
	
	void ForwardMovement(const float& _speed);
	void Spin(const float& _speed);
	void SetLightColor(const FColor& _fColor);
	void StabilizeRoll();
	void StabilizePitch();

	void InitInput();
	void Direction(const FInputActionValue& _value);
	void SpeedControl(const FInputActionValue& _value);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
