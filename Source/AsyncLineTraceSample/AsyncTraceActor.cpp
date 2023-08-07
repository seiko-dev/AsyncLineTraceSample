#include "AsyncTraceActor.h"

// 参考: https://medium.com/@bryan.corell/using-async-collision-traces-in-unreal-engine-4-2cc312c825f5

AAsyncTraceActor::AAsyncTraceActor()
    : bUseAsyncTrace(false)
    , bDelayAsyncWorkUntilNextTick(false)
    , bWantsAsyncTraceOnce(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAsyncTraceActor::BeginPlay()
{
	Super::BeginPlay();

    if (!bDelayAsyncWorkUntilNextTick) {
        AsyncTraceDelegate.BindUObject(this, &ThisClass::OnTraceCompleted);
    }
}

void AAsyncTraceActor::SetWantsTrace()
{
    if (bUseAsyncTrace) {
        // 多重にAsyncTraceを起動しないようにチェック
        // この段階でValidなHandleだった場合、既に有効な結果を得ている
        if (!GetWorld()->IsTraceHandleValid(LastAsyncTraceHandle, false)){
            // 次回のtickで非同期Traceの実行を予約
            bWantsAsyncTraceOnce = true;
        }

    } else {
        // 同期Traceする
        UWorld* World = GetWorld();
        if (World) {
            FHitResult result;

            ECollisionChannel Channel = UEngineTypes::ConvertToCollisionChannel(MyTraceType);
            FVector Start = GetTraceStart();
            FVector End = GetTraceEnd();
            FCollisionQueryParams Params(TEXT("AsyncRequestTrace"),
                                         false,
                                         this);

            World->LineTraceSingleByChannel(result,
                                            Start,
                                            End,
                                            Channel,
                                            Params);


            // 負荷比較のため、非同期Traceと同じイベントを起動する
            TArray<FHitResult> Results;
            Results.Emplace(result);
            ReceiveOnTraceCompleted(Results);
        }
    }
}

FTraceHandle AAsyncTraceActor::RequestAsyncTrace()
{
    UWorld* World = GetWorld();
    if (World == nullptr)
        return FTraceHandle();

    ECollisionChannel Channel = UEngineTypes::ConvertToCollisionChannel(MyTraceType);
    FVector Start = GetTraceStart();
    FVector End = GetTraceEnd();
    FCollisionQueryParams Params(TEXT("AsyncRequestTrace"),
                                 false,
                                 this);

    return World->AsyncLineTraceByChannel(EAsyncTraceType::Single,
                                          Start, End,
                                          Channel,
                                          Params,
                                          FCollisionResponseParams::DefaultResponseParam,
                                          &AsyncTraceDelegate);
}

void AAsyncTraceActor::OnTraceCompleted(const FTraceHandle& Handle, FTraceDatum& Data)
{
    // 自分の指示したAsync Traceの戻りであるかどうか一応確認
    ensure(Handle == LastAsyncTraceHandle);

    // 結果を使って何か作業する
    DoWorkWithTraceResults(Data);

    // 同じ結果を再利用しないように判定用の値をリセット
    LastAsyncTraceHandle.Invalidate();

    // フラグが切り替えられていたら、次回からTickで処理する
    if (bDelayAsyncWorkUntilNextTick) {
        AsyncTraceDelegate.Unbind();
    }
}

void AAsyncTraceActor::DoWorkWithTraceResults(const FTraceDatum& TraceData)
{
    // do things here
    ReceiveOnTraceCompleted(TraceData.OutHits);
}

void AAsyncTraceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 前回のTickでRequestTraceしてたら、LastAsyncTraceHandleに有効値が入ってるはず
    if (LastAsyncTraceHandle.IsValid()){
        // Trace結果を拾って処理
        FTraceDatum OutData;
        if (GetWorld()->QueryTraceData(LastAsyncTraceHandle, OutData)){
            OnTraceCompleted(LastAsyncTraceHandle, OutData);
        }

        if (!bDelayAsyncWorkUntilNextTick) {
            AsyncTraceDelegate.BindUObject(this, &ThisClass::OnTraceCompleted);
        }
    }

    if (bWantsAsyncTraceOnce){
        LastAsyncTraceHandle = RequestAsyncTrace();
        bWantsAsyncTraceOnce = false;
    }
}

