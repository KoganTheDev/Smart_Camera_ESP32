/**
 * @file index_html.h
 * @brief Web Interface for the ESP32-CAM Turret system.
 * This file contains the raw HTML, CSS, and JavaScript stored in Flash memory (PROGMEM).
 * The interface provides a live MJPEG stream, a directional D-Pad for motor control,
 * and a firing trigger.
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Raw string containing the full web interface.
 * Stored in PROGMEM to conserve SRAM. This page is served by the HttpServer 
 * index_handler to the client's browser.
 */
static const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport"
      content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">

<title>ESP32 Turret Control</title>

<style>
:root {
    --bg: #1a1a1a;
    --panel: #2d2d2d;
    --accent: #ff6b00;
    --text: #ffffff;
    --safe: #444;
    --locked: #ff0000;
    --ai: #00d4ff;
}

* { box-sizing: border-box; }

body {
    margin: 0;
    padding: 16px;
    background: var(--bg);
    color: var(--text);
    font-family: system-ui, sans-serif;
    overscroll-behavior: none;
    touch-action: manipulation;
}

.container {
    max-width: 1000px;
    margin: auto;
    display: flex;
    flex-direction: column;
    gap: 16px;
}

@media (min-width: 768px) {
    .container { flex-direction: row; }
}

.video-box {
    flex: 2;
    background: black;
    border-radius: 12px;
    overflow: hidden;
    position: relative;
    border: 2px solid var(--panel);
}

.video-box img {
    width: 100%;
    display: block;
}

.lock-indicator {
    position: absolute;
    top: 12px;
    right: 12px;
    padding: 8px 14px;
    border-radius: 8px;
    font-weight: bold;
    background: rgba(0,0,0,0.6);
    border: 2px solid var(--safe);
}

.lock-indicator.locked {
    color: var(--locked);
    border-color: var(--locked);
    box-shadow: 0 0 12px var(--locked);
    animation: blink 0.5s infinite;
}

@keyframes blink {
    50% { opacity: 0.5; }
}

.controls {
    flex: 1;
    background: var(--panel);
    padding: 16px;
    border-radius: 12px;
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 16px;
}

.mode-badge {
    width: 100%;
    padding: 8px;
    text-align: center;
    border-radius: 6px;
    background: #444;
    font-size: 0.85rem;
}

.mode-badge.ai {
    color: var(--ai);
    border: 1px solid var(--ai);
    background: rgba(0,212,255,0.15);
}

.d-pad {
    display: grid;
    grid-template-columns: repeat(3, 64px);
    grid-template-rows: repeat(3, 64px);
    gap: 10px;
}

.btn {
    background: #444;
    border-radius: 10px;
    display: flex;
    align-items: center;
    justify-content: center;
    user-select: none;
}

.btn svg {
    width: 26px;
    height: 26px;
    fill: white;
}

.btn.active {
    background: var(--accent);
    box-shadow: 0 0 12px var(--accent);
}

.up    { grid-column: 2; grid-row: 1; }
.left  { grid-column: 1; grid-row: 2; }
.right { grid-column: 3; grid-row: 2; }
.down  { grid-column: 2; grid-row: 3; }

.shoot-btn {
    width: 100%;
    padding: 16px;
    font-size: 1.1rem;
    font-weight: bold;
    color: white;
    border: none;
    border-radius: 40px;
    background: linear-gradient(135deg, #ff4500, #b22222);
}

.shoot-btn:disabled {
    background: #555;
    opacity: 0.4;
    box-shadow: none;
}

#status {
    font-size: 0.75rem;
    opacity: 0.6;
}
</style>
</head>

<body>
<div class="container">

<div class="video-box">
    <div id="lockLabel" class="lock-indicator">NO TARGET</div>
    <img id="stream" src="/stream" alt="Camera Stream">
</div>

<div class="controls">
    <div id="modeBadge" class="mode-badge">MANUAL MODE</div>

    <div class="d-pad">
        <div class="btn up"    data-dir="up"><svg viewBox="0 0 24 24"><path d="M12 4l-9 10h18z"/></svg></div>
        <div class="btn left"  data-dir="left"><svg viewBox="0 0 24 24"><path d="M4 12l10 9v-18z"/></svg></div>
        <div class="btn right" data-dir="right"><svg viewBox="0 0 24 24"><path d="M20 12l-10-9v18z"/></svg></div>
        <div class="btn down"  data-dir="down"><svg viewBox="0 0 24 24"><path d="M12 20l9-10h-18z"/></svg></div>
    </div>

    <button id="fireBtn" class="shoot-btn" onclick="fire()">FIRE</button>
    <div id="status">SYSTEM: ONLINE</div>
</div>

</div>

<script>
let moveInterval = null;
let turretLocked = false;
let turretAI = false;

function sendCmd(cmd, dir="") {
    fetch(`/cmd?action=${cmd}${dir ? "&dir="+dir : ""}`, {
        cache: "no-store",
        keepalive: true
    }).catch(()=>{});
}

function startMove(btn) {
    const dir = btn.dataset.dir;
    btn.classList.add("active");
    sendCmd("move", dir);
    moveInterval = setInterval(() => sendCmd("move", dir), 120);
}

function stopMove(btn) {
    btn.classList.remove("active");
    clearInterval(moveInterval);
    moveInterval = null;
    sendCmd("stop");
}

document.querySelectorAll(".btn").forEach(btn => {
    btn.addEventListener("mousedown", () => startMove(btn));
    btn.addEventListener("touchstart", e => { e.preventDefault(); startMove(btn); });

    ["mouseup","mouseleave","touchend","touchcancel"]
        .forEach(ev => btn.addEventListener(ev, () => stopMove(btn)));
});

function fire() {
    if (turretAI && !turretLocked) return;
    sendCmd("fire");
}

function updateTurretStatus(locked, ai) {
    turretLocked = locked;
    turretAI = ai;

    const l = document.getElementById("lockLabel");
    const m = document.getElementById("modeBadge");
    const f = document.getElementById("fireBtn");

    l.textContent = locked ? "LOCKED" : "NO TARGET";
    l.classList.toggle("locked", locked);

    m.textContent = ai ? "AI AUTO-TRACK" : "MANUAL MODE";
    m.classList.toggle("ai", ai);

    const canFire = !ai || locked;
    f.disabled = !canFire;
    f.textContent = canFire ? "FIRE" : "NO TARGET";
}

// DEMO (remove once ESP32 feeds real status)
setTimeout(() => updateTurretStatus(true, true), 2000);
</script>

</body>
</html>
)rawliteral";
