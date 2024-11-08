pyspike

# 概要
　このプログラムはRaspberryPi上で動作し、USBケーブルで接続したLEGO SPIKE をコントロールするソフトである。

# 構成
以下の構成となっている。
pyspike
```
|   pyspike.py  メインプログラム
|           
+---device
|   |   bluetooth_control.md
|   |   bluetooth_control.py
|   |   camera_control.py
|   |   keyboard_control.py
|   |   motor_control.py
|   |   picture_control.py
|   |   serial_control.py
|   |   stream_control.py
|   |   
|   +---templates
|   |       stream.html
|       
+---scenario
|   |   scene_goal.py
|   |   scene_go_landing.py
|   |   scene_moveblock.py
|   |   scene_smartmove.py
|   |   scene_smartmove_test.py
|   |   scene_start.py
|   |   scene_test.py

```