// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/InputComponent.h>
#include "Player/LIPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All)

ALIBaseCharacter::ALIBaseCharacter()
{

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SrpingArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 1000.0f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
}

void ALIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotationControl(DeltaTime);
}

void ALIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALIBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALIBaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");
}

void ALIBaseCharacter::MoveForward(float Amount)
{
	if (FMath::IsNearlyZero(Amount)) return;

	AddMovementInput(FVector::ForwardVector, Amount);
}

void ALIBaseCharacter::MoveRight(float Amount)
{
	if (FMath::IsNearlyZero(Amount)) return;

	AddMovementInput(FVector::RightVector, Amount);
}

void ALIBaseCharacter::RotationControl(float& DeltaTime)
{
	const float ForwardAmount = GetInputAxisValue("LookUp");
	const float RightAmount = GetInputAxisValue("LookRight");
	const FVector FireDirection = FVector(ForwardAmount, RightAmount, 0.0f);

	if (FireDirection.Length() < DeadZone) return;

	GetController()->SetControlRotation(FMath::Lerp(GetController()->GetControlRotation(), FireDirection.Rotation(), RotationRate * DeltaTime));
}
