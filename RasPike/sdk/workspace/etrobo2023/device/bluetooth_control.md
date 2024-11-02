# SPIKE情報
# hub.bluetooth.info()
# {'mac_addr': '30:E2:83:05:4B:E9', 'service_uuid': '', 'name': 'LEGO Hub 30:E2:83:05:4B:E9', 'iap': {'device_version': 7, 'authentication_revision': 1, 'device_id': -1, 'certificate_serial_no': '44A6535EFD9248D1F7E83A3821AAC958', 'protocol_major_version': 3, 'protocol_minor_version': 0}, 'known_devices': ['FD:BC:30:DF:09:90'], 'device_uuid': '03970000-3700-3700-0250-52424E313220'}
# 未実装であった
# hub.bluetooth.rfcomm_connect(アドレス: str ) → bool
# hub.bluetooth.rfcomm_connect(90-09-DF-30-BC-FD) → bool
# hub.BT_VCP(0).write("A")　送信できた。
# SPIKEは、

# windowsのBT Mac確認方法
# getmac /fo list /v

#接続名:           Bluetooth ネットワーク接続
#アダプター:       Bluetooth Device (Personal Area Network)
#物理アドレス:     90-09-DF-30-BC-FD
#トランスポート名: メディアが切断されています

# hub.bluetooth.info()
# {'mac_addr': '30:E2:83:05:4B:E9', 'service_uuid': '', 'name': 'LEGO Hub 30:E2:83:05:4B:E9', 'iap': {'device_version': 7, 'authentication_revision': 1, 'device_id': -1, 'certificate_serial_no': '44A6535EFD9248D1F7E83A3821AAC958', 'protocol_major_version': 3, 'protocol_minor_version': 0}, 'known_devices': ['FD:BC:30:DF:09:90'], 'device_uuid': '03970000-3700-3700-0250-52424E313220'}
#'known_devices': ['FD:BC:30:DF:09:90']に現れるようになった。
>>> hub.bluetooth.info()
{'mac_addr': '30:E2:83:05:4B:E9', 'service_uuid': '', 'name': 'LEGO Hub 30:E2:83:05:4B:E9', 'iap': {'device_version': 7, 'authentication_revision': 1, 'device_id': -1, 'certificate_serial_no': '44A6535EFD9248D1F7E83A3821AAC958', 'protocol_major_version': 3, 'protocol_minor_version': 0}, 'known_devices': ['FD:BC:30:DF:09:90', 'CA:E9:C9:01:5F:E4'], 'device_uuid': '03970000-3700-3700-0250-52424E313220'}


# SPIKEでBTボタンをおし、Legacy AppでBluetoothを検索、接続した。
# TeratermにBT経由のシリアルが出現
# hub.BT_VCP(0).write("A")　送信できた。
# が受信できない。
# USB接続しているケーブルを抜くとシリアル通信してくれた。

waka2@raspberrypi:~ $ bluetoothctl
Agent registered
[DEL] Device 90:09:DF:30:BC:FD A080927476
[DEL] Device 18:93:D7:56:A3:21 NESCAFE i
[DEL] Device D0:62:2C:25:46:74 Xiaomi Smart Band 8 4674
[DEL] Device D5:93:D7:56:A3:21 D5-93-D7-56-A3-21
[bluetooth]# scan on
Discovery started
[CHG] Controller E4:5F:01:C9:E9:CA Discovering: yes
[NEW] Device D5:93:D7:56:A3:21 D5-93-D7-56-A3-21
[NEW] Device 18:93:D7:56:A3:21 NESCAFE i
[NEW] Device D0:62:2C:25:46:74 Xiaomi Smart Band 8 4674
[NEW] Device 30:E2:83:05:4B:E9 30-E2-83-05-4B-E9
[NEW] Device 90:09:DF:30:BC:FD A080927476
[CHG] Device 30:E2:83:05:4B:E9 Name: LEGO Hub 30:E2:83:05:4B:E9
[CHG] Device 30:E2:83:05:4B:E9 Alias: LEGO Hub 30:E2:83:05:4B:E9
[CHG] Device 18:93:D7:56:A3:21 RSSI: -67
[CHG] Device D0:62:2C:25:46:74 RSSI: -51
bluetooth]# pair 30:E2:83:05:4B:E9
Attempting to pair with 30:E2:83:05:4B:E9
[CHG] Device 30:E2:83:05:4B:E9 Connected: yes
[CHG] Device 30:E2:83:05:4B:E9 Modalias: bluetooth:v0397p0001d0001
[CHG] Device 30:E2:83:05:4B:E9 UUIDs: 00000000-deca-fade-deca-deafdecacaff
[CHG] Device 30:E2:83:05:4B:E9 UUIDs: 00001101-0000-1000-8000-00805f9b34fb
[CHG] Device 30:E2:83:05:4B:E9 UUIDs: 00001200-0000-1000-8000-00805f9b34fb
[CHG] Device 30:E2:83:05:4B:E9 ServicesResolved: yes
[CHG] Device 30:E2:83:05:4B:E9 Paired: yes
Pairing successful
[CHG] Device 30:E2:83:05:4B:E9 ServicesResolved: no
[CHG] Device 30:E2:83:05:4B:E9 Connected: no
[CHG] Device D5:93:D7:56:A3:21 RSSI: -84

bluetoothctl
scan on
pair 30:E2:83:05:4B:E9
trust 30:E2:83:05:4B:E9
connect 30:E2:83:05:4B:E9
pair 30:E2:83:05:4B:E9


rfcomm bind rfcomm0 30:E2:83:05:4B:E9
screen /dev/rfcomm0 9600,cs8,-parenb,-cstopb,-hupcl  #  use ^a, k to exit.



bluetooth]# trust 30:E2:83:05:4B:E9
[CHG] Device 30:E2:83:05:4B:E9 Trusted: yes
Changing 30:E2:83:05:4B:E9 trust succeeded
[CHG] Device 18:93:D7:56:A3:21 RSSI: -71
[bluetooth]# connect 30:E2:83:05:4B:E9
Attempting to connect to 30:E2:83:05:4B:E9
[CHG] Device 30:E2:83:05:4B:E9 Connected: yes
[CHG] Device 30:E2:83:05:4B:E9 ServicesResolved: yes
Failed to connect: org.bluez.Error.NotAvailable
[CHG] Device D0:62:2C:25:46:74 RSSI: -67
[CHG] Device 30:E2:83:05:4B:E9 ServicesResolved: no
[CHG] Device 30:E2:83:05:4B:E9 Connected: no
[CHG] Device D5:93:D7:56:A3:21 RSSI: -80
[CHG] Device 18:93:D7:56:A3:21 RSSI: -80

sudo service bluetooth status
● bluetooth.service - Bluetooth service
     Loaded: loaded (/lib/systemd/system/bluetooth.service; enabled; vendor pre>
     Active: active (running) since Sat 2024-11-02 08:26:14 GMT; 19min ago
       Docs: man:bluetoothd(8)
   Main PID: 1688 (bluetoothd)
     Status: "Running"
      Tasks: 1 (limit: 4915)
        CPU: 432ms
     CGroup: /system.slice/bluetooth.service
             └─1688 /usr/libexec/bluetooth/bluetoothd --compat

Nov 02 08:26:14 raspberrypi systemd[1]: Starting Bluetooth service...
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Bluetooth daemon 5.55
Nov 02 08:26:14 raspberrypi systemd[1]: Started Bluetooth service.
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Starting SDP server
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Bluetooth management interface 1.>
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: profiles/sap/server.c:sap_server_>
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: sap-server: Operation not permitt>
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Failed to set privacy: Rejected (>
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Endpoint registered: sender=:1.45>
Nov 02 08:26:14 raspberrypi bluetoothd[1688]: Endpoint registered: sender=:1.45>
lines 1-21/21 (END)

sap-server: Operation not permitted (1)が表示された場合
/etc/systemd/system/bluetooth.target.wants/bluetooth.serviceを変更

// ExecStart=/usr/lib/bluetooth/bluetoothd 変更前
ExecStart=/usr/lib/bluetooth/bluetoothd --noplugin=sap //変更後

systemdを再起動

sudo systemctl daemon-reload

Bluetoothを再起動

sudo service bluetooth restart


