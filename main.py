import os
import requests
import time
import threading

ESP32_IP = "http://192.168.217.184" 
CAMERA_URL = f"{ESP32_IP}/camera"
MOVE_URL = f"{ESP32_IP}/move?dir="
SAVE_DIRECTORY = "/Users/badalkr.sharma/Documents/try_fetch_image/get_img"
IMAGE_PATH = os.path.join(SAVE_DIRECTORY, "latest.jpg")

# Persistent session for faster API calls
session = requests.Session()

# Shared variable for latest command
current_command = "stop"
command_lock = threading.Lock()
turning = False  # Flag to prevent continuous turning

def fetch_image():
    """Fetches an image from the ESP32-CAM and saves it as 'latest.jpg'."""
    try:
        response = session.get(CAMERA_URL, timeout=2)
        if response.status_code == 200:
            with open(IMAGE_PATH, "wb") as file:
                file.write(response.content)
            print("Image updated.")
        else:
            print(f"Failed to fetch image, status code: {response.status_code}")
    except requests.exceptions.RequestException as e:
        print(f"Error fetching image: {e}")
        

        
        
if __name__ == "__main__":
    fetch_image()