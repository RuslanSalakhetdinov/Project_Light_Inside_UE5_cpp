// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/LICharacterMovementComponent.h>
#include <Components/LIHealthComponent.h>
#include <Components/InputComponent.h>
#include <Components/TextRenderComponent.h>
#include <Player/LIPlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <TimerManager.h>

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, All, All)

ALIBaseCharacter::ALIBaseCharacter(const FObjectInitializer& ObjInit)
	: Super(ObjInit.SetDefaultSubobjectClass<ULICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

	HealthComponent = CreateDefaultSubobject<ULIHealthComponent>("HealthComponent");

	HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
	HealthTextComponent->SetupAttachment(SpringArmComponent);
	HealthTextComponent->SetRelativeLocationAndRotation(FVector(500.0f, 200.0f, 0.0f), FRotator(0.0f, 180.0f, 0.0f));

	DashTextComponent = CreateDefaultSubobject<UTextRenderComponent>("DashTextComponent");
	DashTextComponent->SetupAttachment(SpringArmComponent);
	DashTextComponent->SetRelativeLocationAndRotation(FVector(500.0f, 200.0f, -100.0f), FRotator(0.0f, 180.0f, 0.0f));
	DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash Ability Ready!!!"))));

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ALIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(HealthComponent);
	check(HealthTextComponent);
	check(DashTextComponent);
}

void ALIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotationControl(DeltaTime);
	const auto Health = HealthComponent->GetHealth();
	HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Current Health: %.0f"), Health)));
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
	PlayerInputComponent->BindAction("SpeedBuff", IE_Pressed, this, &ALIBaseCharacter::RunStart);
	PlayerInputComponent->BindAction("SpeedBuff", IE_Released, this, &ALIBaseCharacter::RunFinish);
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
		DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash CD %.0f"), FMath::Abs(DashTimerLeft))));
		Temp = FMath::RoundToFloat(DashTimerLeft);
	}
	DashTimerLeft -= GetWorld()->GetDeltaSeconds();
}

void ALIBaseCharacter::DashNoCD()
{
	IsDashCD = false;
	GetWorldTimerManager().ClearTimer(TH_DashCDTimer);
	DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash Ability Ready!!!"))));
}

void ALIBaseCharacter::RunStart()
{
	IsSpeedBuffed = true;
}

void ALIBaseCharacter::RunFinish()
{
	IsSpeedBuffed = false;
}
