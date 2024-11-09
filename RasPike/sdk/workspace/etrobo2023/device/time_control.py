import time

MAX_LEFT_TIME = 120
start_time = time.time()

def set_start_time():
    global start_time
    start_time = time.time()

def get_diff_time():
    global start_time
    return time.time() - start_time

def get_left_time():
    return MAX_LEFT_TIME - get_diff_time()
    
def get_current_time():
    return time.time()
