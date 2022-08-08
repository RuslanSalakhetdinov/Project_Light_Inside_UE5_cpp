// Project "Lignt Inside" Game. RedBuff Studios copyright.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LIBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULIHealthComponent;
class UTextRenderComponent;

UCLASS()
class LIGHTINSIDE_API ALIBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALIBaseCharacter(const FObjectInitializer& ObjInit);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULIHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* HealthTextComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* DashTextComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* ShootTextComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashForce = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashStartDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashDuration = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCoolDownTime = 2.0f;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float AmplitudeSinMovement = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float FrequencySinMovement = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool IsSpeedBuffed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	bool IsDashing = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool IsShooting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement",
		meta = (ClampMin = "1.2", ClampMax = "10.0"))
	float RunModifier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	APlayerController* PlayerController;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

	UPROPERTY(BlueprintReadWrite)
	float Alpha = 0.0f;

private:
	const float RotationRate = 5.0f;
	const float DeadZone = 0.1f;
	FVector CurrentDirection = FVector::Zero();
	FVector InitialLocation = FVector::Zero();
	bool IsDashCD = false;
	float DashTimerLeft = 0.0f;
	float DashTimerTemp = 0.0f;
	float AlphaTemp = 0.0f;

	FTimerHandle TH_DashOpener;
	FTimerHandle TH_DashCloser;
	FTimerHandle TH_DashCDTimer;
	FTimerHandle TH_DashCD;

	void MeshSinMovement();

	FVector GetXYAxisVector(FName AxisX, FName AxisY);
	FVector MoveDirectionVector(FName AxisX, FName AxisY);
	void LerpAlpha();

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void RotationControl(float& DeltaTime);

	void DashInputPressed();
	void DashInputStart();
	void DashInputFinish();
	void Dash();
	void DashTimer();
	void DashNoCD();

	void RunStart();
	void RunFinish();

	void ShootStart();
	void ShootFinish();
};
