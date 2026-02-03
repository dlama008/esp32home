#ifndef DASHBOARD_H
#define DASHBOARD_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="fi"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>KotiHub</title><link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
<style>
:root { 
  --bg: #0f172a; 
  --glass: rgba(255, 255, 255, 0.04); 
  --glass-border: rgba(255,255,255,0.08); 
  --accent: #0ea5e9; 
  --accent-glow: rgba(14, 165, 233, 0.5);
  --text: #f8fafc; 
  --muted: #94a3b8; 
  --success: #10b981; 
  --danger: #ef4444; 
}
body { 
  font-family: 'Segoe UI', Roboto, -apple-system, sans-serif; 
  background: radial-gradient(circle at 60% -20%, #1e293b, #0f172a); 
  color: var(--text); 
  margin: 0; 
  display: flex; flex-direction: column; align-items: center; 
  min-height: 100vh; padding: 20px; box-sizing: border-box; 
}

/* --- UUSI HEADER TYYLI --- */
header {
  margin-bottom: 40px;
  text-align: center;
  position: relative;
}

h1 { 
  font-weight: 800; 
  letter-spacing: 2px; 
  font-size: 2.2rem; 
  margin: 0;
  text-transform: uppercase; 
  
  /* Moderni Gradient-teksti */
  background: linear-gradient(135deg, #ffffff 30%, var(--accent) 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  
  /* Neon-hehku */
  filter: drop-shadow(0 0 15px var(--accent-glow));
  
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 12px;
}

/* Pieni alaviiva otsikon alle */
h1::after {
  content: '';
  position: absolute;
  bottom: -12px;
  left: 50%;
  transform: translateX(-50%);
  width: 40px;
  height: 3px;
  background: var(--accent);
  border-radius: 2px;
  box-shadow: 0 0 10px var(--accent);
}

.header-icon { font-size: 1.8rem; color: var(--accent); }

/* --- PÄÄSÄILIÖ --- */
.main-content {
  width: 100%; max-width: 850px; display: flex; flex-direction: column; gap: 30px;
}

/* --- OSIOT --- */
.section { display: flex; flex-direction: column; gap: 15px; }
h2.section-title { 
  font-size: 0.85rem; color: var(--muted); text-transform: uppercase; 
  letter-spacing: 2px; margin: 0 0 5px 5px; font-weight: 600;
}

/* --- SENSORI GRID (Yläosa) --- */
.grid-sensors { 
  display: grid; 
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); 
  gap: 15px; 
}

/* --- KRYPTO SKROLLER (Liukuhihna) --- */
.crypto-scroll {
  display: flex;
  gap: 15px;
  overflow-x: auto; 
  padding-bottom: 10px; 
  scroll-behavior: smooth;
  -webkit-overflow-scrolling: touch; 
  mask-image: linear-gradient(to right, black 95%, transparent 100%); 
}

.crypto-scroll::-webkit-scrollbar { height: 6px; }
.crypto-scroll::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.1); border-radius: 10px; }
.crypto-scroll::-webkit-scrollbar-track { background: transparent; }

/* --- YLEISET KORTTI TYYLIT --- */
.card { 
  background: var(--glass); backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px);
  border: 1px solid var(--glass-border); border-radius: 24px; padding: 25px; 
  display: flex; flex-direction: column; align-items: center; justify-content: center;
  text-align: center; transition: all 0.3s ease;
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
}
.card:hover { transform: translateY(-3px); border-color: rgba(255,255,255,0.15); }

/* --- SENSORIKORTTIEN SISÄLTÖ --- */
.card i.main-icon { color: var(--accent); font-size: 1.6rem; margin-bottom: 15px; filter: drop-shadow(0 0 10px rgba(14, 165, 233, 0.4)); }
.label { font-size: 0.75rem; color: var(--muted); text-transform: uppercase; letter-spacing: 1.5px; margin-bottom: 5px; font-weight: 700; }
.value-primary { font-size: 2.2rem; font-weight: 300; margin: 5px 0; color: #fff; }
.unit { font-size: 1rem; color: var(--muted); font-weight: 300; margin-left: 4px; }
.value-secondary { font-size: 0.95rem; color: var(--text); margin-top: 10px; opacity: 0.9; display: flex; align-items: center; gap: 8px; background: rgba(255,255,255,0.07); padding: 6px 14px; border-radius: 20px; font-weight: 500; }

/* --- KRYPTO KORTTI --- */
.crypto-card {
  min-width: 140px; 
  flex: 0 0 auto;   
  padding: 20px 15px;
  align-items: stretch; 
}
.crypto-head { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }
.coin-symbol { font-weight: 800; color: var(--muted); font-size: 1rem; letter-spacing: 1px; }
.coin-price { font-size: 1.4rem; color: #fff; font-weight: 400; margin: 0; text-align: left;}
.coin-change { font-size: 0.8rem; font-weight: 700; display: flex; align-items: center; gap: 4px; justify-content: flex-end;}
.trend-up { color: var(--success); }
.trend-down { color: var(--danger); }

/* --- FOOTER --- */
.footer-controls { margin-top: 20px; display: flex; justify-content: center; width: 100%; }
.refresh-btn { 
  background: rgba(255,255,255,0.02); border: 1px solid var(--glass-border); 
  color: var(--muted); border-radius: 30px; padding: 12px 30px; 
  cursor: pointer; transition: all 0.3s ease; font-size: 0.85rem; 
  letter-spacing: 2px; text-transform: uppercase; display: flex; align-items: center; gap: 12px; font-weight: 600;
}
.refresh-btn:hover { background: rgba(255,255,255,0.08); border-color: var(--accent); color: var(--accent); box-shadow: 0 0 20px rgba(14, 165, 233, 0.2); }
.refresh-btn.success { border-color: var(--success); color: var(--success); background: rgba(16, 185, 129, 0.05); box-shadow: 0 0 20px rgba(16, 185, 129, 0.2); }
@keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
.spinning { animation: spin 1s infinite linear; }

</style></head>
<body>

<header>
  <h1><i class="fa-solid fa-layer-group header-icon"></i> KOTIHUB</h1>
</header>

<div class="main-content">

  <div class="section">
    <div class="grid-sensors">
      <div class="card">
        <i class="fa-solid fa-house main-icon"></i><div class="label">Sisäilma</div>
        <div class="value-primary"><span id="temp">--</span><span class="unit">°C</span></div>
        <div class="value-secondary"><i class="fa-solid fa-droplet" style="font-size:0.8rem; color:#0ea5e9;"></i> <span id="humi">--</span> %</div>
      </div>
      <div class="card">
        <i class="fa-solid fa-cloud-sun main-icon"></i><div class="label">Mikkeli</div>
        <div class="value-primary"><span id="m_temp">--</span><span class="unit">°C</span></div>
      </div>
      <div class="card">
        <i class="fa-solid fa-bolt main-icon"></i><div class="label">Sähkö</div>
        <div class="value-primary"><span id="elec">--</span><span class="unit">snt</span></div>
      </div>
    </div>
  </div>

  <div class="section">
    <h2 class="section-title">Markkina (24h)</h2>
    <div class="crypto-scroll" id="crypto-list">
      <div class="card crypto-card" style="opacity:0.6; width:100%; align-items:center; justify-content:center;">Ladataan...</div>
    </div>
  </div>

  <div class="footer-controls">
    <button id="refBtn" class="refresh-btn" onclick="refreshClick()">
      <i class="fa-solid fa-rotate-right" id="refIcon"></i> <span id="refText">Päivitä</span>
    </button>
  </div>

</div>

<script>
var gateway = `ws://${window.location.hostname}:81/`; 
var websocket;
var isManualRefresh = false;
var resetTimer = null; 

function initWebSocket() { 
  websocket = new WebSocket(gateway);
  websocket.onmessage = function(event) { 
    var data = JSON.parse(event.data);
    
    if(data.t) document.getElementById('temp').innerHTML = data.t;
    if(data.h) document.getElementById('humi').innerHTML = data.h;
    if(data.mt) document.getElementById('m_temp').innerHTML = data.mt;
    if(data.ep) document.getElementById('elec').innerHTML = parseFloat(data.ep) < 0 ? "--" : data.ep; 

    if(data.c && data.c.length > 0) {
      var html = "";
      data.c.forEach(function(coin) {
        var isUp = coin.ch >= 0;
        var trendClass = isUp ? "trend-up" : "trend-down";
        var trendIcon = isUp ? "fa-arrow-trend-up" : "fa-arrow-trend-down";
        var sign = isUp ? "+" : "";
        
        html += `<div class="card crypto-card">
          <div class="crypto-head">
            <span class="coin-symbol">${coin.s.toUpperCase()}</span>
            <i class="fa-solid ${trendIcon} ${trendClass}"></i>
          </div>
          <div class="coin-price">€${coin.p}</div>
          <div class="coin-change ${trendClass}">${sign}${coin.ch}%</div>
        </div>`;
      });
      document.getElementById('crypto-list').innerHTML = html;
    }

    if (isManualRefresh) { showSuccessState(); isManualRefresh = false; }
  };
  websocket.onclose = function() { setTimeout(initWebSocket, 2000); }; 
}

function sendCmd(cmd) { if(websocket.readyState === WebSocket.OPEN) websocket.send(cmd); }

function refreshClick() {
  if (resetTimer) clearTimeout(resetTimer);
  var btn = document.getElementById('refBtn');
  var icon = document.getElementById('refIcon');
  var text = document.getElementById('refText');

  btn.classList.remove('success');       
  icon.classList.remove('fa-check');     
  icon.classList.add('fa-rotate-right'); 
  
  isManualRefresh = true;
  sendCmd('refresh');
  
  icon.classList.add('spinning');
  text.innerHTML = "Haetaan...";
  btn.style.opacity = "0.7";
}

function showSuccessState() {
  var btn = document.getElementById('refBtn');
  var icon = document.getElementById('refIcon');
  var text = document.getElementById('refText');

  icon.classList.remove('spinning');
  icon.classList.remove('fa-rotate-right');
  icon.classList.add('fa-check');
  btn.classList.add('success');
  text.innerHTML = "Valmis";
  btn.style.opacity = "1";

  resetTimer = setTimeout(function() {
    icon.classList.remove('fa-check');
    icon.classList.add('fa-rotate-right');
    btn.classList.remove('success');
    text.innerHTML = "Päivitä";
    resetTimer = null;
  }, 3000);
}

window.onload = initWebSocket;
</script></body></html>
)rawliteral";
#endif