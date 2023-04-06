#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsyncTraceActor.generated.h"

// 参考: https://medium.com/@bryan.corell/using-async-collision-traces-in-unreal-engine-4-2cc312c825f5

UCLASS()
class ASYNCLINETRACESAMPLE_API AAsyncTraceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAsyncTraceActor();

protected:
	virtual void BeginPlay() override;


protected:
	UFUNCTION(BlueprintCallable)
	void SetWantsTrace();

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveOnTraceCompleted(const TArray<FHitResult>& Results);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    FVector GetTraceStart() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    FVector GetTraceEnd() const;

	FTraceHandle RequestAsyncTrace();
	void OnTraceCompleted(const FTraceHandle& Handle, FTraceDatum& Data);
	void DoWorkWithTraceResults(const FTraceDatum& TraceData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> MyTraceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	bool bUseAsyncTrace; // 非同期で足元Traceするかどうか。falseならSetWantsTrace()で通常のLineTraceする

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    bool bDelayAsyncWorkUntilNextTick; // async traceした結果の処理を次回のtickまで遅延するかどうか

private:
	FTraceHandle LastAsyncTraceHandle;
	FTraceDelegate AsyncTraceDelegate;
	uint32 bWantsAsyncTraceOnce : 1; // Async Traceのリクエストがある時にtrueになる


public:	
	virtual void Tick(float DeltaTime) override;

};
