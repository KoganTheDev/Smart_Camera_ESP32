#! This is a proof of concept
# TODO: implement the code in c++

import cv2
import numpy as np

def roberts_cross_poc(image_path):
    # 1. Load image and convert to Grayscale (just like the ESP32-CAM buffer)
    img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print("Image not found")
        return

    # Create an empty output buffer
    height, width = img.shape
    output = np.zeros((height, width), dtype=np.uint8)

    # 2. Iterate through pixels (stopping at width-1 and height-1 to avoid overflow)
    for y in range(height - 1):
        for x in range(width - 1):
            # The 2x2 neighborhood
            # P1 P2
            # P3 P4
            p1 = int(img[y, x])
            p2 = int(img[y, x + 1])
            p3 = int(img[y + 1, x])
            p4 = int(img[y + 1, x + 1])

            # Roberts Cross Kernels
            gx = p1 - p4
            gy = p2 - p3

            # Calculate Gradient Magnitude
            # We use absolute sum (L1 norm) because it's faster for ESP32
            magnitude = abs(gx) + abs(gy)

            # Clamp the value to 0-255
            output[y, x] = min(255, magnitude)

    return output

# Run POC
result = roberts_cross_poc('input.jpg')
cv2.imshow('Roberts Cross Edge Detection', result)
cv2.waitKey(0)