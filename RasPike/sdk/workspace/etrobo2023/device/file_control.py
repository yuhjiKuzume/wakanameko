from time import sleep
import os

# watching folder
folder_to_watch = "./flagFolder/"

# watching file
file_start = "start.txt"
file_startSmartCarry = "startSmartCarry.txt"

# 競技開始ファイルが存在するまで待機
def wait_start():
    _wait_file(folder_to_watch + file_start)

# スマートキャリー開始ファイルが存在するまで待機
def wait_start_carry():
    _wait_file(folder_to_watch + file_startSmartCarry)
        
def _wait_file(watch_file):
    while not os.path.exists(watch_file):
        sleep(1)  # 1秒待機
