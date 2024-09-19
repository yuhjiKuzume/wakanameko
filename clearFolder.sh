#!/usr/bin/bash
cd RasPike/sdk/workspace/etrobo2023/

# フォルダの名前を設定
folder="flagFolder"
# フォルダが存在するか確認
if [ -d "$folder" ]; then
  # フォルダが存在する場合は削除
  rm -r "$folder"
fi

mp4File="video.mp4"
# ファイルが存在するか確認
if [ -f "$mp4File" ]; then
  # ファイルが存在する場合は削除
  rm "$mp4File"
fi

trainFolder="train_ditect"
# フォルダが存在するか確認
if [ -d "$trainFolder" ]; then
  # フォルダが存在する場合は削除
  rm -r "$trainFolder"
fi

minifigFolder="minifig_photo"
# フォルダが存在するか確認
if [ -d "$minifigFolder" ]; then
  # フォルダが存在する場合は削除
  rm -r "$minifigFolder"
fi
