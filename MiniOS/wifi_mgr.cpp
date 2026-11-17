#include "web_server.h"
#include "wifi_mgr.h"
#include "fs_mgr.h"
#include "camera.h"
#include "mic.h"
#include "config.h"
#include <WebServer.h>
#include <Update.h>
#include <LittleFS.h>

static WebServer server(80);

static const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>MiniOS</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#000;color:#fff;font-family:-apple-system,BlinkMacSystemFont,sans-serif;overflow-x:hidden}
section{min-height:100vh;display:flex;flex-direction:column;align-items:center;justify-content:center;padding:60px 20px;position:relative}
h1{font-size:clamp(48px,10vw,96px);font-weight:700;letter-spacing:-2px;text-align:center}
h2{font-size:clamp(32px,6vw,64px);font-weight:600;margin:20px 0}
p{font-size:clamp(16px,2vw,24px);color:#86868b;max-width:600px;text-align:center;line-height:1.5}
.hero{background:radial-gradient(ellipse at center,#1a1a2e 0%,#000 70%)}
.hero h1{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent}
.device{width:280px;height:200px;background:#111;border-radius:20px;border:3px solid #333;margin:40px 0;display:flex;align-items:center;justify-content:center;position:relative;box-shadow:0 20px 60px rgba(102,126,234,0.3)}
.device::before{content:'';position:absolute;top:10px;left:50%;transform:translateX(-50%);width:80px;height:20px;background:#000;border-radius:10px}
.screen{width:240px;height:160px;background:#000;border-radius:8px;display:flex;flex-direction:column;align-items:center;justify-content:center;font-family:monospace}
.screen-time{font-size:48px;font-weight:700;color:#fff}
.screen-date{font-size:14px;color:#86868b;margin-top:8px}
.screen-status{font-size:12px;color:#667eea;margin-top:12px}
.btn{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#fff;border:none;padding:16px 32px;border-radius:30px;font-size:18px;font-weight:600;cursor:pointer;margin:10px;transition:transform 0.2s}
.btn:hover{transform:scale(1.05)}
.btn:active{transform:scale(0.95)}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(280px,1fr));gap:30px;max-width:1200px;width:100%;margin:40px 0}
.card{background:#111;border-radius:20px;padding:30px;border:1px solid #333}
.card h3{font-size:24px;margin-bottom:16px}
.video-feed{width:100%;max-width:640px;border-radius:12px;border:2px solid #333;margin:20px 0;background:#000}
.audio-viz{width:100%;height:100px;background:#111;border-radius:12px;margin:20px 0}
.vbtn-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:15px;max-width:400px;margin:30px 0}
.vbtn{background:#222;color:#fff;border:2px solid #444;border-radius:12px;padding:20px;font-size:16px;font-weight:600;cursor:pointer;transition:all 0.15s}
.vbtn:hover{background:#333;border-color:#667eea}
.vbtn:active{background:#667eea;transform:scale(0.95)}
.file-list{width:100%;max-width:800px;margin:20px 0}
.file-item{display:flex;justify-content:space-between;align-items:center;padding:12px;background:#111;border-radius:8px;margin:8px 0;border:1px solid #333}
.file-item button{background:#ff4444;color:#fff;border:none;padding:8px 16px;border-radius:6px;cursor:pointer}
input[type="file"]{background:#222;color:#fff;padding:12px;border-radius:8px;border:1px solid #444;margin:10px 0}
input[type="text"],input[type="password"]{background:#222;color:#fff;padding:12px;border-radius:8px;border:1px solid #444;margin:8px 0;width:100%;max-width:400px}
form{display:flex;flex-direction:column;align-items:center;margin:20px 0}
.status{color:#667eea;font-size:14px;margin-top:10px}
</style>
</head>
<body>

<section class="hero">
<h1>MiniOS</h1>
<p>Умная плата с дисплеем, камерой и микрофоном. Управляй через браузер.</p>
<div class="device">
<div class="screen">
<div class="screen-time" id="dev-time">--:--</div>
<div class="screen-date" id="dev-date">---</div>
<div class="screen-status" id="dev-status">Загрузка...</div>
</div>
</div>
<button class="btn" onclick="document.getElementById('camera').scrollIntoView({behavior:'smooth'})">Начать</button>
</section>

<section id="camera">
<h2>Камера</h2>
<p>Live-стрим с камеры OV2640 в реальном времени</p>
<img class="video-feed" id="cam-feed" src="/stream" alt="Camera stream">
<p class="status" id="cam-status">Подключение к камере...</p>
</section>

<section id="mic">
<h2>Микрофон</h2>
<p>Цифровой микрофон I2S — слушай звук в реальном времени</p>
<canvas class="audio-viz" id="audio-viz"></canvas>
<button class="btn" id="mic-btn" onclick="toggleMic()">Включить микрофон</button>
<p class="status" id="mic-status">Микрофон выключен</p>
</section>

<section id="controls">
<h2>Виртуальные кнопки</h2>
<p>Управляй платой как настоящими кнопками</p>
<div class="vbtn-grid">
<button class="vbtn" onclick="sendCmd('up')">▲ ВВЕРХ</button>
<button class="vbtn" onclick="sendCmd('ok')">● ОК</button>
<button class="vbtn" onclick="sendCmd('home')">⌂ НАЗАД</button>
<button class="vbtn" onclick="sendCmd('left')">◄ ВЛЕВО</button>
<button class="vbtn" onclick="sendCmd('down')">▼ ВНИЗ</button>
<button class="vbtn" onclick="sendCmd('right')">► ВПРАВО</button>
</div>
<p class="status" id="cmd-status">Нажми кнопку</p>
</section>

<section id="files">
<h2>Файлы</h2>
<p>Управление файловой системой платы</p>
<div class="file-list" id="file-list">Загрузка...</div>
<h3 style="margin-top:30px">Загрузить файл</h3>
<form action="/upload" method="POST" enctype="multipart/form-data">
<input type="file" name="file" required>
<button class="btn" type="submit">Загрузить</button>
</form>
</section>

<section id="wifi">
<h2>Wi-Fi</h2>
<p id="wifi-info">Загрузка...</p>
<form action="/setup" method="POST">
<input type="text" name="ssid" placeholder="SSID сети" required>
<input type="password" name="pass" placeholder="Пароль" required>
<button class="btn" type="submit">Подключить</button>
</form>
</section>

<section id="update">
<h2>Обновление прошивки</h2>
<p>Загрузи firmware-ota.bin для обновления по воздуху</p>
<form action="/update" method="POST" enctype="multipart/form-data">
<input type="file" name="firmware" accept=".bin" required>
<button class="btn" type="submit">Прошить</button>
</form>
</section>

<script>
// Обновление информации с платы
async function updateInfo(){
  try{
    const r=await fetch('/api/info');
    const d=await r.json();
    document.getElementById('dev-time').textContent=d.time||'--:--';
    document.getElementById('dev-date').textContent=d.date||'---';
    document.getElementById('dev-status').textContent=d.status||'';
    document.getElementById('wifi-info').textContent='IP: '+(d.ip||'нет')+' | Сеть: '+(d.ssid||'нет');
  }catch(e){}
}
setInterval(updateInfo,2000);
updateInfo();

// Список файлов
async function loadFiles(){
  try{
    const r=await fetch('/api/files');
    const d=await r.json();
    const el=document.getElementById('file-list');
    if(d.files.length===0){el.innerHTML='<p>Нет файлов</p>';return}
    el.innerHTML=d.files.map(f=>`<div class="file-item"><span>${f.name} (${f.size} Б)</span><button onclick="delFile('${f.name}')">Удалить</button></div>`).join('');
  }catch(e){}
}
async function delFile(name){
  await fetch('/delete?path='+name);
  loadFiles();
}
loadFiles();
setInterval(loadFiles,5000);

// Виртуальные кнопки
async function sendCmd(btn){
  try{
    await fetch('/cmd?btn='+btn);
    document.getElementById('cmd-status').textContent='Отправлено: '+btn;
  }catch(e){
    document.getElementById('cmd-status').textContent='Ошибка';
  }
}

// Микрофон
let audioCtx,analyser,source,streaming=false;
async function toggleMic(){
  const btn=document.getElementById('mic-btn');
  const status=document.getElementById('mic-status');
  if(!streaming){
    try{
      const resp=await fetch('/audio');
      const reader=resp.body.getReader();
      audioCtx=new(window.AudioContext||window.webkitAudioContext)();
      analyser=audioCtx.createAnalyser();
      analyser.fftSize=256;
      const bufferLength=analyser.frequencyBinCount;
      const dataArray=new Uint8Array(bufferLength);
      const canvas=document.getElementById('audio-viz');
      const ctx=canvas.getContext('2d');
      canvas.width=canvas.offsetWidth;
      canvas.height=canvas.offsetHeight;
      
      function processAudio(){
        reader.read().then(({done,value})=>{
          if(done){status.textContent='Стрим остановлен';streaming=false;btn.textContent='Включить микрофон';return}
          const int16=new Int16Array(value.buffer);
          const float32=new Float32Array(int16.length);
          for(let i=0;i<int16.length;i++)float32[i]=int16[i]/32768;
          const ab=audioCtx.createBuffer(1,float32.length,16000);
          ab.getChannelData(0).set(float32);
          const src=audioCtx.createBufferSource();
          src.buffer=ab;
          src.connect(analyser);
          analyser.connect(audioCtx.destination);
          src.start();
          
          analyser.getByteFrequencyData(dataArray);
          ctx.fillStyle='#111';
          ctx.fillRect(0,0,canvas.width,canvas.height);
          const barWidth=(canvas.width/bufferLength)*2.5;
          let x=0;
          for(let i=0;i<bufferLength;i++){
            const barHeight=(dataArray[i]/255)*canvas.height;
            ctx.fillStyle=`rgb(${dataArray[i]+100},100,200)`;
            ctx.fillRect(x,canvas.height-barHeight,barWidth,barHeight);
            x+=barWidth+1;
          }
          processAudio();
        });
      }
      processAudio();
      streaming=true;
      btn.textContent='Выключить микрофон';
      status.textContent='Слушаю...';
    }catch(e){
      status.textContent='Ошибка: '+e.message;
    }
  }else{
    streaming=false;
    if(audioCtx)audioCtx.close();
    btn.textContent='Включить микрофон';
    status.textContent='Микрофон выключен';
  }
}

// Камера статус
document.getElementById('cam-feed').onload=()=>{
  document.getElementById('cam-status').textContent='Live-стрим активен';
};
document.getElementById('cam-feed').onerror=()=>{
  document.getElementById('cam-status').textContent='Камера недоступна';
};
</script>
</body>
</html>
)rawliteral";

static void handle_root() {
    server.send_P(200, "text/html; charset=utf-8", MAIN_PAGE);
}

static void handle_setup() {
    if (server.hasArg("ssid")) {
        wifi_save(server.arg("ssid"), server.arg("pass"));
        server.send(200, "text/html; charset=utf-8", "<h1>Сохранено. Перезагрузка...</h1>");
        delay(500);
        ESP.restart();
    } else {
        server.send(400, "text/plain", "no ssid");
    }
}

static void handle_update_done() {
    bool ok = !Update.hasError();
    server.sendHeader("Connection", "close");
    server.send(200, "text/html; charset=utf-8",
        ok ? "<h1>Прошито! Перезагрузка...</h1>" : "<h1>Ошибка обновления</h1>");
    delay(500);
    ESP.restart();
}

static void handle_update_upload() {
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("OTA start: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
            Update.printError(Serial);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (!Update.end(true)) Update.printError(Serial);
        else Serial.printf("OTA done: %u bytes\n", upload.totalSize);
    }
}

static void handle_api_info() {
    struct tm timeinfo;
    String time_str = "--:--", date_str = "---";
    if (getLocalTime(&timeinfo, 100)) {
        char t[16], d[32];
        strftime(t, sizeof(t), "%H:%M", &timeinfo);
        strftime(d, sizeof(d), "%d %b %Y", &timeinfo);
        time_str = t;
        date_str = d;
    }
    
    String status;
    NetState s = wifi_state();
    if (s == NET_CLIENT) status = "Wi-Fi: " + wifi_ip();
    else if (s == NET_AP) status = "AP: " + String(AP_SSID);
    else status = "Подключение...";
    
    String json = "{";
    json += "\"time\":\"" + time_str + "\",";
    json += "\"date\":\"" + date_str + "\",";
    json += "\"status\":\"" + status + "\",";
    json += "\"ip\":\"" + wifi_ip() + "\",";
    json += "\"ssid\":\"" + (s == NET_CLIENT ? WiFi.SSID() : String(AP_SSID)) + "\"";
    json += "}";
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}

static void handle_api_files() {
    String json = "{\"files\":[";
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    bool first = true;
    while (file) {
        if (!first) json += ",";
        json += "{\"name\":\"" + String(file.name()) + "\",\"size\":" + String(file.size()) + "}";
        first = false;
        file = root.openNextFile();
    }
    json += "]}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}

static void handle_delete() {
    if (server.hasArg("path")) {
        String path = server.arg("path");
        if (fs_delete(path)) {
            server.send(200, "text/plain", "ok");
        } else {
            server.send(500, "text/plain", "error");
        }
    } else {
        server.send(400, "text/plain", "no path");
    }
}

static void handle_upload_done() {
    server.send(200, "text/html; charset=utf-8", "<h1>Файл загружен</h1><a href='/'>Назад</a>");
}

static void handle_upload_upload() {
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String path = "/" + upload.filename;
        Serial.printf("Upload start: %s\n", path.c_str());
        File f = LittleFS.open(path, "w");
        if (f) f.close();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        String path = "/" + upload.filename;
        File f = LittleFS.open(path, "a");
        if (f) {
            f.write(upload.buf, upload.currentSize);
            f.close();
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("Upload done: %u bytes\n", upload.totalSize);
    }
}

// MJPEG стрим камеры
static void handle_stream() {
    if (!camera_is_ready()) {
        server.send(503, "text/plain", "Camera not ready");
        return;
    }
    
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Pragma", "no-cache");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "multipart/x-mixed-replace; boundary=frame");
    
    uint8_t *buf = (uint8_t *)malloc(32768);
    if (!buf) {
        server.client().stop();
        return;
    }
    
    while (server.client().connected()) {
        size_t len = camera_capture(buf, 32768);
        if (len > 0) {
            String header = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " + String(len) + "\r\n\r\n";
            server.client().write(header.c_str(), header.length());
            server.client().write(buf, len);
            server.client().write("\r\n", 2);
        }
        delay(100); // 10 FPS
    }
    free(buf);
}

// Аудио стрим микрофона
static void handle_audio() {
    if (!mic_is_ready()) {
        server.send(503, "text/plain", "Mic not ready");
        return;
    }
    
    server.sendHeader("Cache-Control", "no-cache");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "audio/wav");
    
    uint8_t *buf = (uint8_t *)malloc(4096);
    if (!buf) {
        server.client().stop();
        return;
    }
    
    while (server.client().connected()) {
        size_t len;
        if (mic_record(buf, 4096)) {
            server.client().write(buf, 4096);
        }
        delay(10);
    }
    free(buf);
}

// Команды от виртуальных кнопок
static void handle_cmd() {
    if (server.hasArg("btn")) {
        String btn = server.arg("btn");
        Serial.printf("CMD: %s\n", btn.c_str());
        // Сохраняем команду для обработки в loop()
        last_cmd = btn;
        server.send(200, "text/plain", "ok");
    } else {
        server.send(400, "text/plain", "no btn");
    }
}

String last_cmd = "";

void web_init() {
    server.on("/", handle_root);
    server.on("/setup", HTTP_POST, handle_setup);
    server.on("/update", HTTP_POST, handle_update_done, handle_update_upload);
    server.on("/api/info", handle_api_info);
    server.on("/api/files", handle_api_files);
    server.on("/delete", handle_delete);
    server.on("/upload", HTTP_POST, handle_upload_done, handle_upload_upload);
    server.on("/stream", handle_stream);
    server.on("/audio", handle_audio);
    server.on("/cmd", handle_cmd);
    server.begin();
    Serial.println("Web server started");
}

void web_loop() { 
    server.handleClient(); 
}

String get_last_cmd() {
    String cmd = last_cmd;
    last_cmd = "";
    return cmd;
}
