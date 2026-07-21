#include <ps5Controller.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// ---------------- WiFi streaming (ESP32 as its own Access Point) ----------------
const char* apSSID     = "RC_CAR_ESP32";   // network name you'll see on your PC/phone
const char* apPassword = "rccar1234";      // must be at least 8 characters, or use "" for open network
const uint16_t streamPort = 8080;   // raw TCP stream, still used by PuTTY/telnet
const uint16_t httpPort   = 80;     // serves the telemetry.html dashboard page
const uint16_t wsPort     = 81;     // WebSocket feed the dashboard page connects to

// Fixed IP config for the AP (ESP32 always assigns itself this address)
IPAddress apIP(192, 168, 4, 1);
IPAddress apGateway(192, 168, 4, 1);
IPAddress apSubnet(255, 255, 255, 0);

WiFiServer wifiServer(streamPort);
WiFiClient wifiClient;
WebServer httpServer(httpPort);
WebSocketsServer webSocket(wsPort);

// The dashboard page, stored in flash (PROGMEM) so it doesn't use RAM.
// This is served at http://192.168.4.1/ when you open a browser.
const char DASHBOARD_HTML[] PROGMEM = R"HTMLPAGE(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>RC TELEMETRY</title>
<style>
  :root{
    --bg:#0B0E11;
    --panel:#161B20;
    --panel-border:#262E36;
    --amber:#F2A03D;
    --cyan:#4FD1C5;
    --red:#E5484D;
    --green:#3FCF7F;
    --steel:#8A94A0;
    --steel-dim:#4B535C;
    --white:#E9EDF1;
  }
  *{ margin:0; padding:0; box-sizing:border-box; -webkit-tap-highlight-color:transparent; }
  html, body{
    background:var(--bg);
    color:var(--white);
    font-family: 'TelemetryMono', 'Courier New', monospace;
    height:100%;
    overflow-x:hidden;
  }
  /* Monospace stack that works with zero external font loading (AP has no internet) */
  .num{
    font-family: 'Courier New', Courier, monospace;
    font-variant-numeric: tabular-nums;
    letter-spacing: 0.5px;
  }
  .label{
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    font-size: 10px;
    letter-spacing: 1.6px;
    text-transform: uppercase;
    color: var(--steel);
    font-weight: 600;
  }
  /* ---------------- STATUS BAR ---------------- */
  .statusbar{
    position: sticky;
    top: 0;
    z-index: 10;
    display:flex;
    align-items:center;
    justify-content:space-between;
    padding: 12px 16px;
    background: rgba(11,14,17,0.92);
    backdrop-filter: blur(6px);
    border-bottom: 1px solid var(--panel-border);
  }
  .brand{
    display:flex;
    align-items:center;
    gap:10px;
  }
  .brand-title{
    font-size: 13px;
    font-weight: 700;
    letter-spacing: 2px;
    color: var(--white);
  }
  .brand-sub{
    font-size: 9px;
    color: var(--steel-dim);
    letter-spacing: 1.5px;
  }
  .conn{
    display:flex;
    align-items:center;
    gap:8px;
    padding: 6px 12px;
    background: var(--panel);
    border: 1px solid var(--panel-border);
    border-radius: 3px;
  }
  .conn-dot{
    width:8px; height:8px;
    border-radius:50%;
    background: var(--red);
    box-shadow: 0 0 0 0 rgba(229,72,77,0.6);
  }
  .conn-dot.live{
    background: var(--green);
    animation: pulse-live 1.6s infinite;
  }
  @keyframes pulse-live{
    0%{ box-shadow: 0 0 0 0 rgba(63,207,127,0.55); }
    70%{ box-shadow: 0 0 0 7px rgba(63,207,127,0); }
    100%{ box-shadow: 0 0 0 0 rgba(63,207,127,0); }
  }
  .conn-text{
    font-size: 10px;
    letter-spacing: 1.2px;
    color: var(--steel);
    text-transform: uppercase;
  }
  .conn-text.live{ color: var(--green); }
  .conn-text.dead{ color: var(--red); }
  /* ---------------- LAYOUT ---------------- */
  .wrap{
    padding: 16px;
    max-width: 720px;
    margin: 0 auto;
    padding-bottom: 40px;
  }
  .grid{
    display:grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
    margin-bottom: 12px;
  }
  .panel{
    background: var(--panel);
    border: 1px solid var(--panel-border);
    border-radius: 4px;
    padding: 14px;
    position: relative;
    overflow: hidden;
  }
  .panel::before{
    content:'';
    position:absolute;
    top:0; left:0;
    width: 100%;
    height: 2px;
    background: var(--panel-border);
  }
  .panel.accent-amber::before{ background: var(--amber); }
  .panel.accent-cyan::before{ background: var(--cyan); }
  .panel-full{ grid-column: 1 / -1; }
  .panel-head{
    display:flex;
    justify-content: space-between;
    align-items: baseline;
    margin-bottom: 10px;
  }
  .readout{
    font-size: 30px;
    font-weight: 700;
    line-height: 1;
    color: var(--white);
  }
  .readout .unit{
    font-size: 13px;
    color: var(--steel);
    font-weight: 400;
    margin-left: 4px;
  }
  .readout.amber{ color: var(--amber); }
  .readout.cyan{ color: var(--cyan); }
  /* ---------------- COMPASS ---------------- */
  .compass-panel{
    display:flex;
    align-items:center;
    gap: 18px;
  }
  .compass-wrap{
    position: relative;
    width: 96px;
    height: 96px;
    flex-shrink: 0;
  }
  .compass-face{
    width:100%;
    height:100%;
  }
  .needle{
    transform-origin: 50px 50px;
    transition: transform 0.35s cubic-bezier(0.4,0,0.2,1);
  }
  .compass-readout{
    flex:1;
  }
  .compass-deg{
    font-size: 34px;
    font-weight: 700;
    color: var(--cyan);
    line-height: 1;
  }
  .compass-dir{
    font-size: 11px;
    color: var(--steel);
    letter-spacing: 2px;
    margin-top: 4px;
    text-transform: uppercase;
  }
  /* ---------------- GPS BLOCK ---------------- */
  .gps-rows{
    display:flex;
    flex-direction:column;
    gap: 8px;
  }
  .gps-row{
    display:flex;
    justify-content: space-between;
    align-items: center;
    padding: 6px 0;
    border-bottom: 1px solid var(--panel-border);
  }
  .gps-row:last-child{ border-bottom: none; padding-bottom:0; }
  .gps-row .label{ margin-bottom:0; }
  .gps-val{
    font-size: 14px;
    color: var(--white);
    font-weight: 600;
  }
  .sat-badge{
    display:inline-flex;
    align-items:center;
    gap:5px;
    padding: 3px 8px;
    background: rgba(79,209,197,0.1);
    border: 1px solid rgba(79,209,197,0.3);
    border-radius: 20px;
    font-size: 11px;
    color: var(--cyan);
    font-weight: 700;
  }
  .sat-badge.no-fix{
    background: rgba(229,72,77,0.1);
    border-color: rgba(229,72,77,0.3);
    color: var(--red);
  }
  /* ---------------- IMU BARS ---------------- */
  .imu-grid{
    display:grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 10px;
  }
  .axis-cell{
    text-align:center;
    padding: 10px 4px;
    background: rgba(255,255,255,0.02);
    border: 1px solid var(--panel-border);
    border-radius: 3px;
  }
  .axis-name{
    font-size: 10px;
    color: var(--steel-dim);
    letter-spacing: 1px;
    margin-bottom: 6px;
  }
  .axis-val{
    font-size: 16px;
    font-weight: 700;
    color: var(--white);
  }
  .bar-track{
    height: 3px;
    background: var(--panel-border);
    border-radius: 2px;
    margin-top: 8px;
    overflow: hidden;
    position: relative;
  }
  .bar-fill{
    position:absolute;
    top:0; bottom:0;
    left:50%;
    background: var(--amber);
    transition: all 0.2s ease-out;
  }
  /* ---------------- LOG PANEL ---------------- */
  .log-panel{
    margin-top: 12px;
  }
  .log-box{
    background: #05070A;
    border: 1px solid var(--panel-border);
    border-radius: 3px;
    padding: 10px 12px;
    height: 140px;
    overflow-y: auto;
    font-size: 11px;
    line-height: 1.5;
    color: var(--steel);
  }
  .log-box::-webkit-scrollbar{ width: 4px; }
  .log-box::-webkit-scrollbar-thumb{ background: var(--panel-border); }
  .log-line{
    white-space: pre-wrap;
    word-break: break-all;
  }
  .log-line.hdr{ color: var(--cyan); font-weight:700; margin-top:4px; }
  /* ---------------- CONFIG BAR ---------------- */
  .cfg{
    display:flex;
    gap: 8px;
    margin-bottom: 12px;
  }
  .cfg input{
    flex:1;
    background: var(--panel);
    border: 1px solid var(--panel-border);
    color: var(--white);
    padding: 10px 12px;
    border-radius: 3px;
    font-size: 13px;
    font-family: 'Courier New', monospace;
  }
  .cfg input:focus{ outline: 1px solid var(--amber); }
  .cfg button{
    background: var(--amber);
    border: none;
    color: #1A1300;
    font-weight: 700;
    padding: 10px 16px;
    border-radius: 3px;
    font-size: 12px;
    letter-spacing: 1px;
    text-transform: uppercase;
    cursor: pointer;
  }
  .cfg button:active{ opacity: 0.8; }
  .footer-note{
    text-align:center;
    color: var(--steel-dim);
    font-size: 10px;
    letter-spacing: 1px;
    margin-top: 18px;
    padding-bottom: 10px;
  }
  @media (max-width: 400px){
    .grid{ grid-template-columns: 1fr; }
    .readout{ font-size: 26px; }
  }
</style>
</head>
<body>
  <div class="statusbar">
    <div class="brand">
      <div>
        <div class="brand-title">RC TELEMETRY</div>
        <div class="brand-sub">GPS &middot; IMU &middot; LIVE LINK</div>
      </div>
    </div>
    <div class="conn">
      <div class="conn-dot" id="connDot"></div>
      <div class="conn-text dead" id="connText">DISCONNECTED</div>
    </div>
  </div>
  <div class="wrap">
    <div class="cfg">
      <input type="text" id="hostInput" value="192.168.4.1" placeholder="ESP32 IP">
      <input type="text" id="portInput" value="81" placeholder="Port" style="max-width:70px;"><!-- ws port, matches wsPort in .ino -->
      <button id="connectBtn">Connect</button>
    </div>
    <div class="grid">
      <div class="panel accent-amber">
        <div class="panel-head">
          <div class="label">Speed</div>
        </div>
        <div class="readout amber num" id="speedVal">--<span class="unit">km/h</span></div>
      </div>
      <div class="panel accent-cyan">
        <div class="panel-head">
          <div class="label">Altitude</div>
        </div>
        <div class="readout cyan num" id="altVal">--<span class="unit">m</span></div>
      </div>
      <div class="panel panel-full compass-panel">
        <div class="compass-wrap">
          <svg class="compass-face" viewBox="0 0 100 100">
            <circle cx="50" cy="50" r="46" fill="none" stroke="#262E36" stroke-width="1.5"/>
            <circle cx="50" cy="50" r="38" fill="none" stroke="#262E36" stroke-width="1"/>
            <text x="50" y="12" fill="#8A94A0" font-size="9" text-anchor="middle" font-family="monospace">N</text>
            <text x="50" y="94" fill="#4B535C" font-size="8" text-anchor="middle" font-family="monospace">S</text>
            <text x="90" y="53" fill="#4B535C" font-size="8" text-anchor="middle" font-family="monospace">E</text>
            <text x="10" y="53" fill="#4B535C" font-size="8" text-anchor="middle" font-family="monospace">W</text>
            <g class="needle" id="needle">
              <polygon points="50,16 45,54 50,48 55,54" fill="#4FD1C5"/>
              <polygon points="50,84 45,54 50,60 55,54" fill="#2B3238"/>
              <circle cx="50" cy="50" r="4" fill="#0B0E11" stroke="#4FD1C5" stroke-width="1.5"/>
            </g>
          </svg>
        </div>
        <div class="compass-readout">
          <div class="compass-deg num" id="headingVal">---&deg;</div>
          <div class="compass-dir" id="headingDir">NO HEADING</div>
        </div>
      </div>
      <div class="panel panel-full">
        <div class="panel-head">
          <div class="label">GPS Fix</div>
          <div class="sat-badge no-fix" id="satBadge">0 SAT</div>
        </div>
        <div class="gps-rows">
          <div class="gps-row">
            <div class="label">Latitude</div>
            <div class="gps-val num" id="latVal">--</div>
          </div>
          <div class="gps-row">
            <div class="label">Longitude</div>
            <div class="gps-val num" id="lngVal">--</div>
          </div>
          <div class="gps-row">
            <div class="label">UTC Time</div>
            <div class="gps-val num" id="utcVal">--:--:--</div>
          </div>
        </div>
      </div>
      <div class="panel panel-full">
        <div class="panel-head">
          <div class="label">Accelerometer (raw)</div>
        </div>
        <div class="imu-grid">
          <div class="axis-cell">
            <div class="axis-name">X</div>
            <div class="axis-val num" id="axVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="axBar" style="width:0%;"></div></div>
          </div>
          <div class="axis-cell">
            <div class="axis-name">Y</div>
            <div class="axis-val num" id="ayVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="ayBar" style="width:0%;"></div></div>
          </div>
          <div class="axis-cell">
            <div class="axis-name">Z</div>
            <div class="axis-val num" id="azVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="azBar" style="width:0%;"></div></div>
          </div>
        </div>
      </div>
      <div class="panel panel-full">
        <div class="panel-head">
          <div class="label">Gyroscope (raw)</div>
        </div>
        <div class="imu-grid">
          <div class="axis-cell">
            <div class="axis-name">X</div>
            <div class="axis-val num" id="gxVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="gxBar" style="width:0%; background:var(--cyan);"></div></div>
          </div>
          <div class="axis-cell">
            <div class="axis-name">Y</div>
            <div class="axis-val num" id="gyVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="gyBar" style="width:0%; background:var(--cyan);"></div></div>
          </div>
          <div class="axis-cell">
            <div class="axis-name">Z</div>
            <div class="axis-val num" id="gzVal">0</div>
            <div class="bar-track"><div class="bar-fill" id="gzBar" style="width:0%; background:var(--cyan);"></div></div>
          </div>
        </div>
      </div>
    </div>
    <div class="panel log-panel">
      <div class="panel-head">
        <div class="label">Raw Stream</div>
      </div>
      <div class="log-box" id="logBox"></div>
    </div>
    <div class="footer-note">RC_CAR_ESP32 &middot; WEBSOCKET LINK &middot; PORT <span id="footerPort">81</span></div>
  </div>
<script>
  let ws = null;
  let reconnectTimer = null;
  const connDot = document.getElementById('connDot');
  const connText = document.getElementById('connText');
  const logBox = document.getElementById('logBox');
  const hostInput = document.getElementById('hostInput');
  const portInput = document.getElementById('portInput');
  const connectBtn = document.getElementById('connectBtn');

  function setConnected(isLive){
    if(isLive){
      connDot.classList.add('live');
      connText.classList.remove('dead');
      connText.classList.add('live');
      connText.textContent = 'LIVE';
    } else {
      connDot.classList.remove('live');
      connText.classList.remove('live');
      connText.classList.add('dead');
      connText.textContent = 'DISCONNECTED';
    }
  }

  function logLine(text){
    const line = document.createElement('div');
    line.className = 'log-line' + (text.includes('DATA') ? ' hdr' : '');
    line.textContent = text;
    logBox.appendChild(line);
    while(logBox.childNodes.length > 60){
      logBox.removeChild(logBox.firstChild);
    }
    logBox.scrollTop = logBox.scrollHeight;
  }

  function clamp(v, lo, hi){ return Math.max(lo, Math.min(hi, v)); }

  // Maps a raw MPU6050 accel/gyro reading to a -50%..50% bar fill.
  // Accel full-scale default +-2g maps roughly to +-16384 raw.
  // Gyro full-scale default +-250deg/s maps roughly to +-16384 raw too.
  function rawToBarPercent(raw){
    const pct = (raw / 16384) * 50;
    return clamp(pct, -50, 50);
  }

  function updateAxisCell(prefix, raw, isGyro){
    document.getElementById(prefix + 'Val').textContent = raw;
    const pct = rawToBarPercent(raw);
    const bar = document.getElementById(prefix + 'Bar');
    if(pct >= 0){
      bar.style.left = '50%';
      bar.style.width = pct + '%';
    } else {
      bar.style.left = (50 + pct) + '%';
      bar.style.width = (-pct) + '%';
    }
  }

  function headingToCompass(deg){
    const dirs = ['N','NNE','NE','ENE','E','ESE','SE','SSE','S','SSW','SW','WSW','W','WNW','NW','NNW'];
    const idx = Math.round(deg / 22.5) % 16;
    return dirs[idx];
  }

  // Parses one line of the existing streamPrintln() text output.
  function parseLine(line){
    line = line.trim();
    if(line.startsWith('Latitude')){
      const v = line.split(':')[1].trim();
      document.getElementById('latVal').textContent = v;
    } else if(line.startsWith('Longitude')){
      const v = line.split(':')[1].trim();
      document.getElementById('lngVal').textContent = v;
    } else if(line.startsWith('Altitude')){
      const v = parseFloat(line.split(':')[1]);
      document.getElementById('altVal').innerHTML = v.toFixed(1) + '<span class="unit">m</span>';
    } else if(line.startsWith('Speed')){
      const v = parseFloat(line.split(':')[1]);
      document.getElementById('speedVal').innerHTML = v.toFixed(1) + '<span class="unit">km/h</span>';
    } else if(line.startsWith('Course')){
      const v = parseFloat(line.split(':')[1]);
      document.getElementById('headingVal').innerHTML = v.toFixed(0) + '&deg;';
      document.getElementById('headingDir').textContent = headingToCompass(v);
      document.getElementById('needle').setAttribute('transform', `rotate(${v} 50 50)`);
    } else if(line.startsWith('Satellites')){
      const v = parseInt(line.split(':')[1]);
      const badge = document.getElementById('satBadge');
      badge.textContent = v + ' SAT';
      if(v > 0){ badge.classList.remove('no-fix'); } else { badge.classList.add('no-fix'); }
    } else if(line.startsWith('UTC Time')){
      const v = line.split(':').slice(1).join(':').trim();
      document.getElementById('utcVal').textContent = v;
    } else if(line.startsWith('Accel X')){
      updateAxisCell('ax', parseInt(line.split(':')[1]));
    } else if(line.startsWith('Accel Y')){
      updateAxisCell('ay', parseInt(line.split(':')[1]));
    } else if(line.startsWith('Accel Z')){
      updateAxisCell('az', parseInt(line.split(':')[1]));
    } else if(line.startsWith('Gyro X')){
      updateAxisCell('gx', parseInt(line.split(':')[1]), true);
    } else if(line.startsWith('Gyro Y')){
      updateAxisCell('gy', parseInt(line.split(':')[1]), true);
    } else if(line.startsWith('Gyro Z')){
      updateAxisCell('gz', parseInt(line.split(':')[1]), true);
    }
    if(line.length > 0){
      logLine(line);
    }
  }

  function connect(){
    const host = hostInput.value.trim();
    const port = portInput.value.trim();
    document.getElementById('footerPort').textContent = port;
    if(ws){ ws.close(); }
    if(reconnectTimer){ clearTimeout(reconnectTimer); }
    ws = new WebSocket(`ws://${host}:${port}/`);
    ws.onopen = () => {
      setConnected(true);
      logLine('--- link established ---');
    };
    ws.onmessage = (evt) => {
      parseLine(evt.data);
    };
    ws.onclose = () => {
      setConnected(false);
      logLine('--- link lost, retrying in 3s ---');
      reconnectTimer = setTimeout(connect, 3000);
    };
    ws.onerror = () => {
      ws.close();
    };
  }

  connectBtn.addEventListener('click', connect);

  // Auto-connect on load using default values
  window.addEventListener('load', () => {
    connect();
  });
</script>
</body>
</html>)HTMLPAGE";

// ---------------- Motors ----------------
int enableRightMotor = 22;
int rightMotorPin1 = 27;
int rightMotorPin2 = 26;
int enableLeftMotor = 23;
int leftMotorPin1 = 18;
int leftMotorPin2 = 19;

const int PWMFreq = 1000;
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

const int JOYSTICK_DEADZONE = 20;
const int TRIGGER_DEADZONE = 10;
const int CREEP_SPEED = 90;

// ---------------- Sensors ----------------
TinyGPSPlus gps;
MPU6050 mpu;
#define gpsSerial Serial2

unsigned long lastSensorPrint = 0;
const unsigned long sensorPrintInterval = 1000;

// ================= STREAM HELPER =================
// Mirrors output to USB serial, the raw TCP client (PuTTY/telnet),
// AND every connected WebSocket client (the browser dashboard).
void streamPrintln(const String &s){
  Serial.println(s);
  if (wifiClient && wifiClient.connected())
  {
    wifiClient.println(s);
  }
  // Broadcast the same line to any browser tabs connected via WebSocket.
  // The dashboard JS parses these lines the same way PuTTY just displays them.
  // broadcastTXT() takes a non-const String&, so pass a local mutable copy.
  String wsPayload = s;
  webSocket.broadcastTXT(wsPayload);
}

// ================= HTTP HANDLER =================
// Serves the telemetry dashboard page at http://192.168.4.1/
void handleRoot(){
  httpServer.send_P(200, "text/html", DASHBOARD_HTML);
}

// ================= WEBSOCKET HANDLER =================
void onWebSocketEvent(uint8_t clientNum, WStype_t type, uint8_t *payload, size_t length){
  switch (type)
  {
    case WStype_CONNECTED:
      Serial.printf("WebSocket client #%u connected.\n", clientNum);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client #%u disconnected.\n", clientNum);
      break;
    default:
      break; // dashboard only receives data, doesn't send commands (yet)
  }
}

// ================= PS5 =================
void notify(){
  int yAxisValue = -(ps5.data.analog.stick.ly);
  int xAxisValue = -(ps5.data.analog.stick.rx);
  int r2Value = ps5.data.analog.button.r2;
  int l2Value = ps5.data.analog.button.l2;

  int direction = 0;
  if (yAxisValue > JOYSTICK_DEADZONE) direction = 1;
  else if (yAxisValue < -JOYSTICK_DEADZONE) direction = -1;

  int throttle = 0;
  int steering = 0;

  if (direction != 0)
  {
    steering = map(xAxisValue, -127, 127, -255, 255);
    if (r2Value > TRIGGER_DEADZONE)
    {
      int magnitude = map(r2Value, 0, 255, 0, 255);
      throttle = magnitude * direction;
    }
    else
    {
      throttle = CREEP_SPEED * direction;
    }
  }

  if (l2Value > TRIGGER_DEADZONE)
  {
    float brakeFactor = 1.0 - (l2Value / 255.0);
    throttle = (int)(throttle * brakeFactor);
  }

  int motorDirection = (throttle < 0) ? -1 : 1;
  throttle = abs(throttle);

  int rightMotorSpeed, leftMotorSpeed;
  rightMotorSpeed = throttle - steering;
  leftMotorSpeed = throttle + steering;

  rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);

  rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
}

void onConnect() {
  Serial.println("PS5 Controller Connected!");
}

void onDisConnect() {
  rotateMotor(0, 0);
  Serial.println("PS5 Controller Disconnected!");
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }

  ledcWrite(rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorPWMSpeedChannel, abs(leftMotorSpeed));
}

void setUpPinModes() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(enableRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, leftMotorPWMSpeedChannel);

  rotateMotor(0, 0);
}

// ================= SENSOR DISPLAY (streamed to USB + WiFi) =================
void displayGPS() {
  streamPrintln("GPS DATA");
  if (gps.location.isValid()) {
    char buf[80];
    snprintf(buf, sizeof(buf), "Latitude : %.6f", gps.location.lat());
    streamPrintln(buf);
    snprintf(buf, sizeof(buf), "Longitude: %.6f", gps.location.lng());
    streamPrintln(buf);
    snprintf(buf, sizeof(buf), "Altitude : %.2f m", gps.altitude.meters());
    streamPrintln(buf);
    snprintf(buf, sizeof(buf), "Speed    : %.2f km/h", gps.speed.kmph());
    streamPrintln(buf);
    snprintf(buf, sizeof(buf), "Course   : %.2f", gps.course.deg());
    streamPrintln(buf);
    snprintf(buf, sizeof(buf), "Satellites: %d", gps.satellites.value());
    streamPrintln(buf);
    if (gps.time.isValid()) {
      snprintf(buf, sizeof(buf), "UTC Time : %02d:%02d:%02d", 
               gps.time.hour(), gps.time.minute(), gps.time.second());
      streamPrintln(buf);
    }
  } else {
    streamPrintln("Waiting for GPS Fix...");
  }
}

void displayMPU() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  streamPrintln("");
  streamPrintln("MPU6500 DATA");
  char buf[40];
  snprintf(buf, sizeof(buf), "Accel X : %d", ax); streamPrintln(buf);
  snprintf(buf, sizeof(buf), "Accel Y : %d", ay); streamPrintln(buf);
  snprintf(buf, sizeof(buf), "Accel Z : %d", az); streamPrintln(buf);
  snprintf(buf, sizeof(buf), "Gyro X  : %d", gx); streamPrintln(buf);
  snprintf(buf, sizeof(buf), "Gyro Y  : %d", gy); streamPrintln(buf);
  snprintf(buf, sizeof(buf), "Gyro Z  : %d", gz); streamPrintln(buf);
}

// ================= WIFI SETUP (Access Point mode) =================
void setUpWiFi(){
  WiFi.mode(WIFI_AP);
  // Configure the AP's own IP before starting it
  WiFi.softAPConfig(apIP, apGateway, apSubnet);
  bool apStarted = WiFi.softAP(apSSID, apPassword);
  if (apStarted) {
    Serial.println("Access Point started successfully.");
  } else {
    Serial.println("Failed to start Access Point!");
  }
  Serial.print("AP SSID: ");
  Serial.println(apSSID);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());   // will print 192.168.4.1

  wifiServer.begin();
  Serial.print("TCP stream server started on port ");
  Serial.println(streamPort);

  httpServer.on("/", handleRoot);
  httpServer.begin();
  Serial.print("HTTP dashboard server started on port ");
  Serial.println(httpPort);

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.print("WebSocket server started on port ");
  Serial.println(wsPort);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  setUpPinModes();
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Wire.begin(21, 25);
  Serial.println("Initializing MPU6500...");
  mpu.initialize();
  if (mpu.testConnection())
    Serial.println("MPU6500 Connected!");
  else
    Serial.println("MPU6500 NOT Found!");

  setUpWiFi();

  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisConnect);
  ps5.begin("14:3A:9A:F1:C6:16");
  while (ps5.isConnected() == false) {
    Serial.println("PS5 controller not found");
    delay(300);
  }

  Serial.println("Ready.");
  Serial.println("Waiting for GPS fix...");
}

// ================= LOOP =================
void loop() {
  // Service the HTTP dashboard page and WebSocket telemetry feed.
  httpServer.handleClient();
  webSocket.loop();

  // Accept a new WiFi client if one connects (replaces old one if needed)
  if (wifiServer.hasClient())
  {
    if (!wifiClient || !wifiClient.connected())
    {
      wifiClient = wifiServer.available();
      Serial.println("WiFi stream client connected.");
    }
    else
    {
      wifiServer.available().stop(); // reject extra connection, only 1 client supported for now
    }
  }

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  unsigned long now = millis();
  if (now - lastSensorPrint >= sensorPrintInterval) {
    lastSensorPrint = now;
    displayGPS();
    displayMPU();
    streamPrintln("----------------------------------------");
  }
}
