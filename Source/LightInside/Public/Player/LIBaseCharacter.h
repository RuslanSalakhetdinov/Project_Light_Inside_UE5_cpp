// Project "Lignt Inside" Game. RedBuff Studios copyright.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LIBaseCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LIGHTINSIDE_API ALIBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALIBaseCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashLength = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashStartDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashDuration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCoolDownTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	APlayerController* PlayerController;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	const float RotationRate = 5.0f;
	const float DeadZone = 0.1f;
	FVector CurrentDirection = FVector::Zero();
	bool IsDashCD = false;
	float DashTimerLeft = 0.0f;
	float Temp = 0.0f;

	FTimerHandle TH_DashOpener;
	FTimerHandle TH_DashCloser;
	FTimerHandle TH_DashCDTimer;
	FTimerHandle TH_DashCD;

	FVector GetXYAxisVector(FName AxisX, FName AxisY);
	FVector MoveDirectionVector(FName AxisX, FName AxisY);

	void MoveForward(float Amount);
	void MoveRight(float Amount);
	void RotationControl(float& DeltaTime);

	void DashInputPressed();
	void DashInputStart();
	void DashInputFinish();
	void Dash();
	void DashTimer();
	void DashNoCD();
};
