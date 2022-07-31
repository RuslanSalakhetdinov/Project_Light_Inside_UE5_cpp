// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/InputComponent.h>

ALIBaseCharacter::ALIBaseCharacter()
{

	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SrpingArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 1000.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
}

void ALIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALIBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALIBaseCharacter::MoveRight);
}

void ALIBaseCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector(), Amount);
}

void ALIBaseCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector(), Amount);
}
