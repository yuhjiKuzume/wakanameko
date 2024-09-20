#!/usr/bin/bash
cd RasPike/sdk/workspace/etrobo2023/

# フォルダの名前を設定
folder="flagFolder"

# フォルダが存在するか確認
if [ ! -d "$folder" ]; then
  # フォルダが存在しない場合は作成
  mkdir "$folder"
fi

python wait_capture.py