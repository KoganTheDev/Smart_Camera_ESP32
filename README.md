# 🎯 Smart Camera ESP32: Motion Target Acquisition

[![Cppcheck Analysis](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/static_analysis.yml/badge.svg)](https://github.com/KoganTheDev/Smart_Camera_ESP32/actions/workflows/static_analysis.yml) <!-- CPP check badge--> 


This project implements an autonomous motion-tracking system using an ESP32-CAM. Instead of static edge detection, the system uses Frame Differencing to isolate moving targets, calculate their centroids, and lock onto them in real-time.

---

## 🚀 Core Features
* Temporal Frame Differencing: Compares the current frame against a stored reference frame to identify pixels that have changed.Target Centroid Calculation: Automatically calculates the X, Y center of the detected motion "blob" to provide coordinates for tracking.
* Adaptive Thresholding: Dynamically filters out sensor noise to prevent "ghost" motion triggers.
* Target Locking: Maintains a stable lock on moving objects while ignoring the static background.
