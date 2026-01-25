# 🎯 Smart Camera ESP32: Motion Target Acquisition

[![Cppcheck Analysis](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/static_analysis.yml/badge.svg)](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/static_analysis.yml) <!-- CPP check badge--> 
[![Doxygen - Documentation Generator](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/doxygen_docs_generator.yml/badge.svg)](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/doxygen_docs_generator.yml)

This project implements an autonomous motion-tracking system using an ESP32-CAM. Instead of static edge detection, the system uses Frame Differencing to isolate moving targets, calculate their centroids, and lock onto them in real-time. **Motion detection is visualized in real-time on the HTTP video stream with a green target overlay.**

---

## 🚀 Core Features
* **Frame Differencing**: Compares the current frame against a stored reference frame to identify pixels that have changed
* **Target Centroid Calculation**: Automatically calculates the X, Y center of the detected motion "blob" for precise tracking
* **Adaptive Thresholding**: Dynamically filters out sensor noise to prevent false motion triggers
* **Target Locking**: Maintains stable lock on moving objects while ignoring static backgrounds
* **Motion Visualization**: Real-time green crosshair + circle overlay on HTTP video stream showing detected motion
* **Servo Control**: Pan and tilt servos automatically track the detected target
* **Web Interface**: Live video streaming with motion detection visualization accessible via browser


