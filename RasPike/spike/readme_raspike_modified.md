# SPIKE microPython の対話モードで動作させる

ETロボコン2024年版でSPIKEはmicroPythonで動かしている。
SPIKEの電源をONにするとmain.pyが自動起動するようになっている。
ETロボコン後半部はmicroPythonの対話モードで動作するように変更する。
USBケーブルを通じてコマンドを送信して制御する。

手順：
1. USBケーブルでSPIKEとPC（Windows or RaspberryPi）を接続する。
2. 通信設定（9600bps）に設定する
3. コマンド"^C"にて、main.pyを終了する（main.pyで定義している変数やメソッドは使用できる）
4. USBにてコマンドを送信して、SPIKEを制御する。
参考：コマンド"^D"にて、SPIKEのソフトウェアリセットが行われる（main.pyが再起動する）

コマンド一覧は以下の通り
# Hub-api
## ■アーム制御
```python
ARM_UP()  # 上げる
ARM_DOWN()  # 下げる
ARM_SHAKE() # ARMを揺さぶる(150ms,3回)
ARM_SHAKE(300) # 万歳
```
## ■モータ制御

ポジション初期化
```python
motor_pair.preset(0, 0)
```
移動
```python
#前進(停止しない)
MP(40, 40) # POWER= 40

#後進(停止しない)
MP(-40, -40) # POWER= 40

```

停止
```python
MP(0, 0) # 停止
```


回転
```python
CW(90) #右90度回転（注意：重量があるのでずれる）
CCW(90) #左90度回転（注意：重量があるのでずれる）
```

指定位置へ移動（パラメータはcm単位, 注意：重量があるのでずれる）
```python
FW(10)  # 前進  
FWA(10, RPwr, LPwr) # 前進（減速版）
BW(10)  # 後進　
BWA(10, RPwr, LPwr) # 後進（減速版）
```


ヨー角制御
```python
setYou()  # ヨー角リセット
setYou(deg)  # ヨー角セット(-179～0～179)
getYou() # ヨー角の取得
```
RGBセンサ
```python
setRGB()  # RBG取得モード設定
getRGB()  # RBG取得（R,G,B,?)
logRGB()  # １秒間隔にRGB取得(CTRL+Cで中止)
logRGB(time)  # time秒間隔にRGB取得(CTRL+Cで中止)
```



参考：　https://lego.github.io/MINDSTORMS-Robot-Inventor-hub-API/class_motorpair.html


その他
```python
getRot() # モータ回転角の取得

RICOH() # RICOH ロゴに変更
BEEP_ON() #BEEP ON

```


