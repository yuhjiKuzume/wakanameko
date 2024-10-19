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
motor_A.run_for_time(300, -100, 100, 1, 1, 1)  # 上げる
motor_A.run_for_time(300, 100, 100, 1, 1, 1)  # 下げる
```
## ■モータ制御
モーターペア化(設定済)
```python
motor_pair = motor_B.pair(motor_C)
```
ポジション初期化
```python
motor_pair.preset(0, 0)
```
移動
```python
#前進、後退、停止
motor_pair.pwm(40, -40) # POWER= 40　で前進
motor_pair.pwm(-40, 40) # POWER= 40　で後進
motor_pair.pwm(0, 0) # 停止


#回転
motor_pair.run_to_position(90, 90) #右90度回転（注意：重量があるのでずれる）
motor_pair.run_to_position(-90, -90) #左90度回転（注意：重量があるのでずれる）

# 指定位置へ前進移動（注意：重量があるのでずれる）
motor_pair.run_to_position(720, -720) # 角度指定となっている。 720度なら２回転。タイヤの円周×２進む
```

## ■ヨー角制御
```python
hub.motion.yaw_pitch_roll(0) # ヨー角リセット
hub.motion.yaw_pitch_roll()[0] # ヨー角取得
```

参考：　https://lego.github.io/MINDSTORMS-Robot-Inventor-hub-API/class_motorpair.html


# オリジナル
```python
CW(90) #右90度回転
CCW(90) #左90度回転

FW(10) #10cm前進（注意：重量があるのでずれる）
BW(10) #10cm後進（注意：重量があるのでずれる）

FWA(10) #10cm前進
BWA(10) #10cm後進

getRot() # モータ回転角の取得
getYou() # ヨー角の取得

```


