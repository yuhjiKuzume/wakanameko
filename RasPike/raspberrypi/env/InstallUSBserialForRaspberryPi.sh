#!/bin/bash

# 引数が -MANUAL であるかどうかをチェック
if [[ "$1" == "-MANUAL" ]]; then
    # lsusbの結果をリストに取得
    devices=$(lsusb)
    echo "接続されたUSBデバイスの一覧:"

    # デバイスを番号付きで表示
    index=1
    declare -A device_map
    while IFS= read -r line; do
        echo "[$index] $line"
        device_map[$index]="$line"
        index=$((index+1))
    done <<< "$devices"

    # ユーザーにデバイスの番号を入力してもらう
    read -p "番号を選んでください: " selected_index

    # インデックスが有効かチェック
    if [[ -z "${device_map[$selected_index]}" ]]; then
        echo "エラー: 有効な番号を選択してください。"
        exit 1
    fi

    # 選択されたデバイスの情報を取得
    selected_device=${device_map[$selected_index]}
    echo "選択されたデバイス: $selected_device"

    # デバイスのID部分を抽出し、スペースで分割された4番目の要素を取得
    id_string=$(echo $selected_device | awk '{print $6}')

    # IDを:で区切り、ベンダーIDとプロダクトIDをそれぞれ抽出
    VENDOR_ID=$(echo $id_string | cut -d':' -f1)
    PRODUCT_ID=$(echo $id_string | cut -d':' -f2)

    # 結果を表示
    echo "Vendor ID: $VENDOR_ID"
    echo "Product ID: $PRODUCT_ID"
else
    VENDOR_ID="0694"
    PRODUCT_ID="0009"
fi


# Python仮想環境の作成とpyusbのインストール
python -m venv ../tools/python
../tools/python/bin/pip install pyusb

# 必要なパッケージのインストール
sudo apt install -y minicom

# udevルールの設定
RULE="SUBSYSTEM==\"tty\", ATTRS{idVendor}==\"$VENDOR_ID\", ATTRS{idProduct}==\"$PRODUCT_ID\", SYMLINK+=\"USB_SPIKE\", MODE=\"0666\""
echo $RULE > /tmp/serial_temp_rule

# ルールを移動してリロード
sudo mv /tmp/serial_temp_rule /etc/udev/rules.d/99-serial.rules
sudo udevadm control --reload

echo "USBシリアル設定が完了しました。"