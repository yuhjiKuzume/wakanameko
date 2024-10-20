# RaspberryPiのUSB環境設定

# 概要

RaspberryPiのUSB（ホスト）とSPIKEのUSB（デバイス）をUSBケーブルで繋ぎ、
通信するための環境設定方法です。


# １．設定方法
以下のツールを実行してください。

```
InstallUSBserialForRaspberryPi.sh
```

# ２．確認方法

## デバイスの登録確認
以下のコマンドを入力してください。
```
ls /dev/USB_SPIKE
```
以下が表示されたら成功です。設定終了です。
```
/dev/USB_SPIKE
```

# ３．トラブルシューティング

## （１） ls: '/dev/USB_SPIKE'にアクセスできません：そのようなファイルやディレクトリはありません

### 1.マニュアル設定モードで起動する
再度、以下のツールを"-MANUAL"オプションをつけて実行してください。

```
InstallUSBserialForRaspberryPi.sh -MANUAL
```

USBデバイスリストが表示されます。LEGOのUSBデバイスを選択してください。
```
[1]Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
[2]Bus 001 Device 004: ID 0694:0009 Lego Group LEGO Technic Large Hub in FS Mode
[3]Bus 001 Device 002: ID 2109:3431 VIA Labs, Inc. Hub
[4]Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
番号を選んでください: 
```

参考までに、0694で登録されたベンダーを調べると"The LEGO Company"です。
プロダクトは製品によって異なります。

https://the-sz.com/products/usbid/index.php?v=0694&p=0009&n=