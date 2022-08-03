// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/InputComponent.h>
#include "Player/LIPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "Misc/App.h"
#include <TimerManager.h>

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
	SpringArmComponent->TargetArmLength = 1400.0f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALIBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALIBaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ALIBaseCharacter::DashInputPressed);
}

FVector ALIBaseCharacter::GetXYAxisVector(FName AxisX, FName AxisY)
{
	const float ForwardAmount = GetInputAxisValue(AxisX);
	const float RightAmount = GetInputAxisValue(AxisY);

	return FVector(ForwardAmount, RightAmount, 0.0f);
}

FVector ALIBaseCharacter::MoveDirectionVector(FName AxisX, FName AxisY)
{
	const FVector MoveDirection = GetXYAxisVector(AxisX, AxisY).GetSafeNormal();
	return (CurrentDirection = MoveDirection);
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
	const FVector FireDirection = GetXYAxisVector("LookUp", "LookRight");

	if (FireDirection.Length() < DeadZone) return;

	GetController()->SetControlRotation(FMath::Lerp(GetController()->GetControlRotation(), FireDirection.Rotation(), RotationRate * DeltaTime));
}

void ALIBaseCharacter::DashInputPressed()
{
	if (IsDashCD) return;

	IsDashCD = true;
	DashTimerLeft = DashCoolDownTime + DashStartDelay;

	GetWorldTimerManager().SetTimer(TH_DashCDTimer, this, &ALIBaseCharacter::DashTimer, GetWorld()->GetDeltaSeconds(), true);
	GetWorldTimerManager().SetTimer(TH_DashCD, this, &ALIBaseCharacter::DashNoCD, DashCoolDownTime, false);
	Temp = DashTimerLeft;
	DashInputStart();
}

void ALIBaseCharacter::DashInputStart()
{
	if (MoveDirectionVector("MoveForward", "MoveRight") == FVector::Zero()) return;

	GetWorldTimerManager().SetTimer(TH_DashOpener, this, &ALIBaseCharacter::Dash, GetWorld()->GetDeltaSeconds(), true, DashStartDelay);
	GetWorldTimerManager().SetTimer(TH_DashCloser, this, &ALIBaseCharacter::DashInputFinish, DashDuration, false);
}

void ALIBaseCharacter::DashInputFinish()
{
	GetWorldTimerManager().ClearTimer(TH_DashOpener);
}

void ALIBaseCharacter::Dash()
{
	LaunchCharacter(CurrentDirection * DashLength, true, true);
}

void ALIBaseCharacter::DashTimer()
{
	if (Temp != FMath::RoundToFloat(DashTimerLeft))
	{
		UE_LOG(LogBaseCharacter, Display, TEXT("Dash in CD !Timer: %.0f"), FMath::Abs(DashTimerLeft));
		Temp = FMath::RoundToFloat(DashTimerLeft);
	}
	DashTimerLeft -= GetWorld()->GetDeltaSeconds();
}

void ALIBaseCharacter::DashNoCD()
{
	IsDashCD = false;
	GetWorldTimerManager().ClearTimer(TH_DashCDTimer);
	UE_LOG(LogBaseCharacter, Display, TEXT("You can use you Dash Ability"));
}
