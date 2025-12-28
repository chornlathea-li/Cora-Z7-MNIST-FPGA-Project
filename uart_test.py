import serial
import os
import random
import cv2
import numpy as np
import struct
import time
import sys


def main():
    args = sys.argv[1:]
    if len(args) == 2 and args[0] == '-port':
        port = str(args[1])
    else:
        port = "COM3"
        print("[INFO] Using default port:", port)

    dims = (10, 10)
    base_dir = os.path.expanduser(
        "~") + '/Downloads/MNIST_Dataset_JPG/MNIST_JPG_testing/'

    available_labels = [d for d in os.listdir(
        base_dir) if os.path.isdir(os.path.join(base_dir, d))]
    if not available_labels:
        print("[ERROR] No subfolders (0-9) found in MNIST_JPG_testing!")
        return

    # Open serial port once
    try:
        ser = serial.Serial(port, 115200, serial.EIGHTBITS,
                            serial.PARITY_NONE, serial.STOPBITS_ONE)
    except serial.SerialException as e:
        print(f"[ERROR] Could not open serial port {port}: {e}")
        return

    print("[INFO] Starting continuous random image sending.")

    try:
        while True:
            random_label = random.choice(available_labels)
            read_dir = os.path.join(base_dir, random_label)
            files = os.listdir(read_dir)
            if not files:
                print(f"[WARNING] No images in folder {read_dir}, skipping...")
                continue

            read_file = random.choice(files)
            img_path = os.path.join(read_dir, read_file)

            # Read and preprocess image
            img = cv2.imread(img_path, 0)
            img = cv2.resize(img, dims, interpolation=cv2.INTER_AREA)
            img = np.asarray((img / 255)).astype('float32')

            # Print info
            print("Label:", random_label, "Filename:",
                  read_file, "Serialport:", port)

            # Send image over UART
            for i in range(dims[1]):
                for j in range(dims[0]):
                    values = bytearray(struct.pack("f", img[i][j]))
                    ser.write(values)

            print("[INFO] Image sent successfully!\n")

            # Wait for next image
            time.sleep(1)

    except KeyboardInterrupt:
        print("\n[INFO] Stopped by user.")
        ser.close()


if __name__ == "__main__":
    main()
