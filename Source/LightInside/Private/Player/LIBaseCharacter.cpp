// Project "Lignt Inside" Game. RedBuff Studios copyright.

#include "Player/LIBaseCharacter.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Components/LICharacterMovementComponent.h>
#include <Components/SkeletalMeshComponent.h>
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
	DashTextComponent->SetRelativeLocationAndRotation(FVector(500.0f, 200.0f, -50.0f), FRotator(0.0f, 180.0f, 0.0f));
	DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash Ability Ready"))));

	ShootTextComponent = CreateDefaultSubobject<UTextRenderComponent>("ShootTextComponent");
	ShootTextComponent->SetupAttachment(SpringArmComponent);
	ShootTextComponent->SetRelativeLocationAndRotation(FVector(500.0f, 200.0f, -100.0f), FRotator(0.0f, 180.0f, 0.0f));
	ShootTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Guns on the table"))));

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void ALIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(HealthComponent);
	check(HealthTextComponent);
	check(DashTextComponent);

	InitialLocation = GetMesh()->GetRelativeLocation();
}

void ALIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LerpAlpha();
	MeshSinMovement();
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
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ALIBaseCharacter::ShootStart);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ALIBaseCharacter::ShootFinish);
}

float ALIBaseCharacter::GetMovementDirection() const
{
	if (GetVelocity().IsZero()) return 0.0f;
	const auto VelocityNormal = GetVelocity().GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), VelocityNormal));
	const auto CrossProduct = FVector::CrossProduct(GetActorForwardVector(), VelocityNormal);
	const auto Degrees = FMath::RadiansToDegrees(AngleBetween);
	return CrossProduct.IsZero() ? Degrees : Degrees * FMath::Sign(CrossProduct.Z);
}

void ALIBaseCharacter::MeshSinMovement()
{
	FVector CurrentLocation = GetMesh()->GetRelativeLocation();
	if (GetWorld())
	{
		float Time = GetWorld()->GetTimeSeconds();
		CurrentLocation.Z = InitialLocation.Z + AmplitudeSinMovement * FMath::Sin(FrequencySinMovement * Time);

		GetMesh()->SetRelativeLocation(CurrentLocation);
	}
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

void ALIBaseCharacter::LerpAlpha()
{
	if (FMath::IsNearlyEqual(Alpha, AlphaTemp)) return;
	Alpha = FMath::Lerp(Alpha, AlphaTemp, 0.1);
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
	if (IsDashCD || MoveDirectionVector("MoveForward", "MoveRight") == FVector::Zero()) return;

	IsDashing = true;
	IsDashCD = true;
	DashTimerLeft = DashCoolDownTime + DashStartDelay;

	GetWorldTimerManager().SetTimer(TH_DashCDTimer, this, &ALIBaseCharacter::DashTimer, GetWorld()->GetDeltaSeconds(), true);
	GetWorldTimerManager().SetTimer(TH_DashCD, this, &ALIBaseCharacter::DashNoCD, DashCoolDownTime, false);
	DashTimerTemp = DashTimerLeft;
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
	LaunchCharacter(CurrentDirection * DashForce, true, true);
}

void ALIBaseCharacter::DashTimer()
{
	if (DashTimerTemp != FMath::RoundToFloat(DashTimerLeft))
	{
		DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash CD %.0f"), FMath::Abs(DashTimerLeft))));
		DashTimerTemp = FMath::RoundToFloat(DashTimerLeft);
	}
	DashTimerLeft -= GetWorld()->GetDeltaSeconds();
}

void ALIBaseCharacter::DashNoCD()
{
	IsDashing = false;
	IsDashCD = false;
	GetWorldTimerManager().ClearTimer(TH_DashCDTimer);
	DashTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Dash Ability Ready"))));
}

void ALIBaseCharacter::RunStart()
{
	IsSpeedBuffed = true;
}

void ALIBaseCharacter::RunFinish()
{
	IsSpeedBuffed = false;
}

void ALIBaseCharacter::ShootStart()
{
	ShootTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Kill them all!!!"))));
	IsShooting = true;
	AlphaTemp = 1.0f;
}

void ALIBaseCharacter::ShootFinish()
{
	ShootTextComponent->SetText(FText::FromString(FString::Printf(TEXT("Guns on the table"))));
	IsShooting = false;
	AlphaTemp = 0.0f;
}
