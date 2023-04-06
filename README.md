# AsyncLineTraceSample
大量のActorがLineTraceを行う時、AsyncLineTraceを使う事で負荷を軽減できるかもしれません。このサンプルでは、通常の`LineTraceByChannel`と`AsyncLineTraceByChannel`の負荷を比較します。

![image](https://user-images.githubusercontent.com/40533980/230411159-e0d00263-95d6-45d4-9c0d-6c31bddb0b6b.png)

このサンプルでは接地判定をシミュレートしています。周期的にバウンドする球型のActorは、それぞれが自分の足元から3uu真下にLineTraceを行います。TraceがBlockされたら球は緑色に、そうでないときは青色になります。

## 対応バージョン
UE5.1.1以降

# 使い方
チェックボックスを操作して、`stat unit`のGameの負荷がどのように変化するか確認してみて下さい。

- `Use async trace` : 有効な場合は`AsyncLineTraceByChannel`を使います。
- `Delay async result work until next tick` : 有効な場合は`AsyncLineTraceByChannel`の終了を待たず、次Tickに持ち越します。

# 参考資料
- [Using Async Collision Traces in Unreal Engine 4 | by Bryan Corell | Medium](https://medium.com/@bryan.corell/using-async-collision-traces-in-unreal-engine-4-2cc312c825f5)
  - ほぼこちらのブログの実装を参考にさせて頂きました。有難や🙏
- [60fpsアクションを実現する秘訣を伝授 解析編【UNREAL FEST EAST 2019】](https://www.docswell.com/s/EpicGamesJapan/ZQ1XEK-UE4_UFE19_Soleil_60fpsAction_Analysis)
  - `AsyncLineTraceByChannel`以外の負荷軽減テクニックも大量に掲載されています

----
以上
