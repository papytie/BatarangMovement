// Fill out your copyright notice in the Description page of Project Settings.


#include "BM_Batarang.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PointLightComponent.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

ABM_Batarang::ABM_Batarang()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	springArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	springArm->SetupAttachment(RootComponent);
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	camera->SetupAttachment(springArm);
	
	pitchComponent = CreateDefaultSubobject<USceneComponent>("Pitch Component");
	pitchComponent->SetupAttachment(RootComponent);
	rollComponent = CreateDefaultSubobject<USceneComponent>("Roll Component");
	rollComponent->SetupAttachment(pitchComponent);
	spinComponent = CreateDefaultSubobject<USceneComponent>("Spin Component");
	spinComponent->SetupAttachment(rollComponent);
	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	staticMesh->SetupAttachment(spinComponent);
	pointLight = CreateDefaultSubobject<UPointLightComponent>("Point Light");
	pointLight->SetupAttachment(staticMesh);
}

void ABM_Batarang::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* _inputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!_inputComponent)return;
	_inputComponent->BindAction(directionInput, ETriggerEvent::Triggered, this, &ABM_Batarang::Direction);
	_inputComponent->BindAction(directionInput, ETriggerEvent::Completed, this, &ABM_Batarang::Direction);
	_inputComponent->BindAction(speedInput, ETriggerEvent::Triggered, this, &ABM_Batarang::SpeedControl);
	_inputComponent->BindAction(speedInput, ETriggerEvent::Completed, this, &ABM_Batarang::SpeedControl);
}

void ABM_Batarang::BeginPlay()
{
	Super::BeginPlay();
	InitInput();
	SetLightColor(baseColor);
}

void ABM_Batarang::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ForwardMovement(actualSpeed);
}

void ABM_Batarang::InitInput()
{
	ULocalPlayer* _localPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (!_localPlayer)return;
	UEnhancedInputLocalPlayerSubsystem* _inputSystem = _localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!_inputSystem)return;
	_inputSystem->AddMappingContext(inputMappingContext, 0);

}

void ABM_Batarang::Direction(const FInputActionValue& _value)
{
	const FVector2d _2dAxisValue = _value.Get<FVector2d>();
	float _forwardValue = _2dAxisValue.X;
	float _rightValue = _2dAxisValue.Y;
	float _deltaTime = GetWorld()->DeltaTimeSeconds;
	
	if (_rightValue >= 0.1 || _rightValue <= -0.1)
		isRolling = true;
	else isRolling = false; // Check if righ axis is pressed
	
	if (_forwardValue >= 0.1 || _forwardValue <= -0.1)
		isPitching = true;
	else isPitching = false; // Check if forward axis is pressed
	
	AddControllerYawInput(_rightValue * _deltaTime * turnSpeed); // Update Controller Yaw
	
	// Rotate Roll Component for smooth movement -------------------------------------------
	rollComponent->AddLocalRotation(FRotator(0, 0, _rightValue * _deltaTime * turnSpeed));
	float _rollValue = rollComponent->GetRelativeRotation().Roll;
	if (_rollValue >= maxAngle) rollComponent->SetRelativeRotation(FRotator(0, 0, maxAngle));
	if (_rollValue <= minAngle) rollComponent->SetRelativeRotation(FRotator(0, 0, minAngle));
	
	AddControllerPitchInput(-1 * _forwardValue * _deltaTime * pitchSpeed); // Update Controller Pitch and Clamp min/max
	float _controllerPitch = this->GetActorRotation().Pitch;
	if (_controllerPitch >= maxAngle)this->GetController()->SetControlRotation(FRotator(maxAngle, this->GetActorRotation().Yaw, this->GetActorRotation().Roll));
	if (_controllerPitch <= minAngle)this->GetController()->SetControlRotation(FRotator(minAngle, this->GetActorRotation().Yaw, this->GetActorRotation().Roll));

	// Rotate Pitch Component for smooth movement -------------------------------------------
	pitchComponent->AddLocalRotation(FRotator(_forwardValue * _deltaTime * pitchSpeed, 0, 0));
	float _pitchValue = pitchComponent->GetRelativeRotation().Pitch;
	if (_pitchValue >= maxAngle) pitchComponent->SetRelativeRotation(FRotator(maxAngle, 0, 0));
	if (_pitchValue <= minAngle) pitchComponent->SetRelativeRotation(FRotator(minAngle, 0, 0));
}

void ABM_Batarang::SpeedControl(const FInputActionValue& _value)
{
	const float _axisValue = _value.Get<float>(); // Change color based on Axis value
	if (_axisValue >= -0.1 && _axisValue <= 0.1) SetLightColor(baseColor);
	if (_axisValue >= 0.1) SetLightColor(accelerationColor);
	if (_axisValue <= -0.1) SetLightColor(brakeColor);
	
	actualSpeed += GetWorld()->GetDeltaSeconds() * speedIncr * _axisValue; // Clamp Speed
	if (actualSpeed >= maxSpeed) actualSpeed = maxSpeed;
	if (actualSpeed <= minSpeed) actualSpeed = minSpeed;

}

void ABM_Batarang::ForwardMovement(const float& _speed)
{
	FVector _forward = GetActorForwardVector();
	FVector _location = GetActorLocation();
	float _deltaTime = GetWorld()->DeltaTimeSeconds;
	FVector _newLocation = _location + _forward * _deltaTime * _speed;
	SetActorLocation(_newLocation); // Automatic forward movement
	
	Spin(actualSpeed); // Spin activation
	
	if (!isRolling) StabilizeRoll(); // Stabilize both Roll and Pitch
	if (!isPitching) StabilizePitch();
}

void ABM_Batarang::Spin(const float& _speed)
{
	float _deltaTime = GetWorld()->DeltaTimeSeconds;
	spinComponent->AddLocalRotation(FRotator(0, -1 * _deltaTime * actualSpeed/2,0));
}

void ABM_Batarang::StabilizeRoll()
{
	float _rollValue = rollComponent->GetRelativeRotation().Roll;
	if (_rollValue >= 1) rollComponent->AddLocalRotation(FRotator(0, 0, -1));
	if (_rollValue <= 1) rollComponent->AddLocalRotation(FRotator(0, 0, 1));
	
}

void ABM_Batarang::StabilizePitch()
{
	float _pitchValue = pitchComponent->GetRelativeRotation().Pitch;
	if (_pitchValue >= 1) pitchComponent->AddLocalRotation(FRotator(-1, 0, 0));
	if (_pitchValue <= 1) pitchComponent->AddLocalRotation(FRotator(1, 0, 0));
}

void ABM_Batarang::SetLightColor(const FColor& _fColor)
{
	pointLight->SetLightFColor(_fColor);
}



