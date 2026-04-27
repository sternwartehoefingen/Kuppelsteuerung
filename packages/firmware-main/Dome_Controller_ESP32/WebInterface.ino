// WebInterface.ino - HTML/CSS/JavaScript for Web UI

// ==================== Main Page (Azimuth Display) ====================
String getMainPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Kuppelsteuerung - Azimut</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <div class="container">
    <header>
      <h1>🔭 Sternwarte Hüfingen</h1>
      <h2>Kuppelsteuerung</h2>
      <nav>
        <a href="/" class="active">Azimut</a>
        <a href="/debug">Debug</a>
      </nav>
    </header>
    
    <main class="azimuth-view">
      <div class="status-bar">
        <div class="status-item">
          <span class="label">Verbindung:</span>
          <span id="connected" class="value">--</span>
        </div>
        <div class="status-item">
          <span class="label">Motor:</span>
          <span id="motor-state" class="value">--</span>
        </div>
        <div class="status-item">
          <span class="label">Slewing:</span>
          <span id="slewing" class="value">--</span>
        </div>
      </div>
      
      <div class="compass-container">
        <svg id="compass" viewBox="0 0 400 400" xmlns="http://www.w3.org/2000/svg">
          <!-- Outer circle -->
          <circle cx="200" cy="200" r="180" fill="none" stroke="#34495e" stroke-width="2"/>
          
          <!-- Cardinal directions -->
          <text x="200" y="40" text-anchor="middle" class="cardinal">N</text>
          <text x="360" y="210" text-anchor="middle" class="cardinal">E</text>
          <text x="200" y="375" text-anchor="middle" class="cardinal">S</text>
          <text x="40" y="210" text-anchor="middle" class="cardinal">W</text>
          
          <!-- Degree markings -->
          <g id="degree-marks"></g>
          
          <!-- Target azimuth indicator -->
          <line id="target-line" x1="200" y1="200" x2="200" y2="60" 
                stroke="#27ae60" stroke-width="3" stroke-dasharray="5,5" 
                transform="rotate(0 200 200)" opacity="0.7"/>
          
          <!-- Current azimuth needle -->
          <g id="needle" transform="rotate(0 200 200)">
            <polygon points="200,50 195,200 205,200" fill="#e74c3c"/>
            <circle cx="200" cy="200" r="10" fill="#c0392b"/>
          </g>
          
          <!-- Center display -->
          <circle cx="200" cy="200" r="80" fill="#2c3e50" opacity="0.9"/>
          <text x="200" y="190" text-anchor="middle" class="azimuth-label">Azimut</text>
          <text id="azimuth-value" x="200" y="220" text-anchor="middle" class="azimuth-value">0.0°</text>
        </svg>
      </div>
      
      <div class="info-panel">
        <div class="info-row">
          <span class="info-label">Aktuell:</span>
          <span id="current-az" class="info-value">0.0°</span>
        </div>
        <div class="info-row">
          <span class="info-label">Ziel:</span>
          <span id="target-az" class="info-value">0.0°</span>
        </div>
        <div class="info-row">
          <span class="info-label">Encoder:</span>
          <span id="encoder-value" class="info-value">0</span>
        </div>
        <div class="info-row">
          <span class="info-label">Uptime:</span>
          <span id="uptime" class="info-value">0s</span>
        </div>
      </div>
    </main>
    
    <footer>
      <p>ESP32 ASCOM Alpaca Dome Controller v1.0</p>
      <p>WiFi RSSI: <span id="wifi-rssi">--</span> dBm</p>
    </footer>
  </div>
  
  <script src="/script.js"></script>
</body>
</html>
)rawliteral";
}

// ==================== Debug Page ====================
String getDebugPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Kuppelsteuerung - Debug</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <div class="container">
    <header>
      <h1>🔧 Debug-Informationen</h1>
      <nav>
        <a href="/">Azimut</a>
        <a href="/debug" class="active">Debug</a>
      </nav>
    </header>
    
    <main class="debug-view">
      <section class="debug-section">
        <h3>📡 Verbindung</h3>
        <table class="debug-table">
          <tr>
            <td>ASCOM Connected:</td>
            <td id="debug-connected">--</td>
          </tr>
          <tr>
            <td>WiFi RSSI:</td>
            <td><span id="debug-wifi-rssi">--</span> dBm</td>
          </tr>
          <tr>
            <td>Uptime:</td>
            <td id="debug-uptime">--</td>
          </tr>
        </table>
      </section>
      
      <section class="debug-section">
        <h3>🧭 Position</h3>
        <table class="debug-table">
          <tr>
            <td>Azimuth (°):</td>
            <td id="debug-azimuth">--</td>
          </tr>
          <tr>
            <td>Target Azimuth (°):</td>
            <td id="debug-target">--</td>
          </tr>
          <tr>
            <td>Encoder Value:</td>
            <td id="debug-encoder">--</td>
          </tr>
          <tr>
            <td>Encoder Max:</td>
            <td>2452 ppr</td>
          </tr>
        </table>
      </section>
      
      <section class="debug-section">
        <h3>🔧 Motor</h3>
        <table class="debug-table">
          <tr>
            <td>Motor State:</td>
            <td id="debug-motor">--</td>
          </tr>
          <tr>
            <td>Slewing:</td>
            <td id="debug-slewing">--</td>
          </tr>
          <tr>
            <td>Watchdog Timeout:</td>
            <td>5000 ms</td>
          </tr>
        </table>
      </section>
      
      <section class="debug-section">
        <h3>🧲 Reed Kontakte</h3>
        <table class="debug-table">
          <tr>
            <td>Reed Contact 1 (0°):</td>
            <td id="debug-reed1">--</td>
          </tr>
          <tr>
            <td>Reed Contact 2 (120°):</td>
            <td id="debug-reed2">--</td>
          </tr>
          <tr>
            <td>Sync Count:</td>
            <td id="debug-sync-count">--</td>
          </tr>
        </table>
      </section>
      
      <section class="debug-section">
        <h3>📊 System</h3>
        <table class="debug-table">
          <tr>
            <td>Device Name:</td>
            <td>ESP32 Dome Controller</td>
          </tr>
          <tr>
            <td>Firmware Version:</td>
            <td>1.0.0</td>
          </tr>
          <tr>
            <td>API Version:</td>
            <td>ASCOM Alpaca v1</td>
          </tr>
          <tr>
            <td>Update Rate:</td>
            <td id="update-rate">--</td>
          </tr>
        </table>
      </section>
      
      <section class="debug-section">
        <h3>📝 Raw Status JSON</h3>
        <pre id="raw-json" class="json-display">{}</pre>
      </section>
    </main>
    
    <footer>
      <p>Last Update: <span id="last-update">--</span></p>
    </footer>
  </div>
  
  <script>
    let lastUpdateTime = 0;
    let updateCount = 0;
    
    async function updateDebug() {
      try {
        const response = await fetch('/api/status');
        const data = await response.json();
        
        // Connection
        document.getElementById('debug-connected').textContent = data.connected ? '✅ Ja' : '❌ Nein';
        document.getElementById('debug-wifi-rssi').textContent = data.wifiRSSI;
        document.getElementById('debug-uptime').textContent = formatUptime(data.uptime);
        
        // Position
        document.getElementById('debug-azimuth').textContent = data.azimuth.toFixed(2);
        document.getElementById('debug-target').textContent = data.targetAzimuth.toFixed(2);
        document.getElementById('debug-encoder').textContent = data.encoderValue;
        
        // Motor
        document.getElementById('debug-motor').textContent = data.motorState;
        document.getElementById('debug-slewing').textContent = data.slewing ? '✅ Ja' : '❌ Nein';
        
        // Reed Contacts
        document.getElementById('debug-reed1').textContent = data.reedContact1 === 0 ? '🔴 Aktiv' : '⚫ Inaktiv';
        document.getElementById('debug-reed2').textContent = data.reedContact2 === 0 ? '🔴 Aktiv' : '⚫ Inaktiv';
        document.getElementById('debug-sync-count').textContent = data.syncCount;
        
        // Raw JSON
        document.getElementById('raw-json').textContent = JSON.stringify(data, null, 2);
        
        // Update rate
        const now = Date.now();
        if (lastUpdateTime > 0) {
          const interval = now - lastUpdateTime;
          document.getElementById('update-rate').textContent = `${interval} ms (${(1000/interval).toFixed(1)} Hz)`;
        }
        lastUpdateTime = now;
        updateCount++;
        
        document.getElementById('last-update').textContent = new Date().toLocaleTimeString('de-DE');
        
      } catch (error) {
        console.error('Update failed:', error);
      }
    }
    
    function formatUptime(seconds) {
      const days = Math.floor(seconds / 86400);
      const hours = Math.floor((seconds % 86400) / 3600);
      const mins = Math.floor((seconds % 3600) / 60);
      const secs = seconds % 60;
      
      let result = '';
      if (days > 0) result += `${days}d `;
      if (hours > 0) result += `${hours}h `;
      if (mins > 0) result += `${mins}m `;
      result += `${secs}s`;
      
      return result;
    }
    
    // Update every 200ms
    setInterval(updateDebug, 200);
    updateDebug();
  </script>
</body>
</html>
)rawliteral";
}

// ==================== CSS ====================
String getCSS() {
  return R"rawliteral(
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
  color: #ecf0f1;
  min-height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 20px;
}

.container {
  width: 100%;
  max-width: 900px;
  background: rgba(44, 62, 80, 0.95);
  border-radius: 20px;
  box-shadow: 0 10px 40px rgba(0, 0, 0, 0.3);
  overflow: hidden;
}

header {
  background: linear-gradient(135deg, #2c3e50 0%, #34495e 100%);
  padding: 30px;
  text-align: center;
  border-bottom: 3px solid #3498db;
}

header h1 {
  font-size: 2.5em;
  margin-bottom: 10px;
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
}

header h2 {
  font-size: 1.3em;
  color: #3498db;
  font-weight: normal;
}

nav {
  margin-top: 20px;
  display: flex;
  justify-content: center;
  gap: 20px;
}

nav a {
  color: #ecf0f1;
  text-decoration: none;
  padding: 10px 25px;
  border-radius: 25px;
  background: rgba(52, 152, 219, 0.2);
  transition: all 0.3s;
  font-weight: 500;
}

nav a:hover {
  background: rgba(52, 152, 219, 0.4);
  transform: translateY(-2px);
}

nav a.active {
  background: #3498db;
}

main {
  padding: 30px;
}

.status-bar {
  display: flex;
  justify-content: space-around;
  margin-bottom: 30px;
  padding: 20px;
  background: rgba(52, 73, 94, 0.5);
  border-radius: 15px;
}

.status-item {
  text-align: center;
}

.status-item .label {
  display: block;
  font-size: 0.9em;
  color: #95a5a6;
  margin-bottom: 5px;
}

.status-item .value {
  display: block;
  font-size: 1.3em;
  font-weight: bold;
  color: #3498db;
}

.compass-container {
  display: flex;
  justify-content: center;
  margin: 30px 0;
}

#compass {
  max-width: 400px;
  width: 100%;
  height: auto;
}

.cardinal {
  font-size: 24px;
  font-weight: bold;
  fill: #3498db;
}

.azimuth-label {
  font-size: 14px;
  fill: #95a5a6;
}

.azimuth-value {
  font-size: 28px;
  font-weight: bold;
  fill: #e74c3c;
}

.info-panel {
  background: rgba(52, 73, 94, 0.5);
  border-radius: 15px;
  padding: 25px;
  margin-top: 30px;
}

.info-row {
  display: flex;
  justify-content: space-between;
  padding: 12px 0;
  border-bottom: 1px solid rgba(149, 165, 166, 0.2);
}

.info-row:last-child {
  border-bottom: none;
}

.info-label {
  color: #95a5a6;
  font-weight: 500;
}

.info-value {
  color: #3498db;
  font-weight: bold;
  font-size: 1.1em;
}

.debug-view {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 20px;
}

.debug-section {
  background: rgba(52, 73, 94, 0.5);
  border-radius: 15px;
  padding: 20px;
}

.debug-section h3 {
  color: #3498db;
  margin-bottom: 15px;
  font-size: 1.2em;
  border-bottom: 2px solid rgba(52, 152, 219, 0.3);
  padding-bottom: 10px;
}

.debug-table {
  width: 100%;
  border-collapse: collapse;
}

.debug-table tr {
  border-bottom: 1px solid rgba(149, 165, 166, 0.1);
}

.debug-table td {
  padding: 10px 5px;
}

.debug-table td:first-child {
  color: #95a5a6;
  width: 60%;
}

.debug-table td:last-child {
  color: #3498db;
  font-weight: bold;
  text-align: right;
}

.json-display {
  background: #1e272e;
  color: #00d2d3;
  padding: 15px;
  border-radius: 10px;
  overflow-x: auto;
  font-size: 0.85em;
  line-height: 1.5;
}

footer {
  background: rgba(44, 62, 80, 0.7);
  padding: 20px;
  text-align: center;
  color: #95a5a6;
  font-size: 0.9em;
  border-top: 1px solid rgba(52, 152, 219, 0.3);
}

footer p {
  margin: 5px 0;
}

@media (max-width: 600px) {
  header h1 {
    font-size: 1.8em;
  }
  
  .status-bar {
    flex-direction: column;
    gap: 15px;
  }
  
  .debug-view {
    grid-template-columns: 1fr;
  }
}
)rawliteral";
}

// ==================== JavaScript ====================
String getJavaScript() {
  return R"rawliteral(
// Update interval in milliseconds
const UPDATE_INTERVAL = 500;

// Create degree markings on compass
function createDegreeMarks() {
  const svg = document.getElementById('degree-marks');
  for (let i = 0; i < 360; i += 10) {
    const angle = i - 90; // SVG rotation offset
    const length = i % 30 === 0 ? 15 : 8;
    const x1 = 200 + 165 * Math.cos(angle * Math.PI / 180);
    const y1 = 200 + 165 * Math.sin(angle * Math.PI / 180);
    const x2 = 200 + (165 - length) * Math.cos(angle * Math.PI / 180);
    const y2 = 200 + (165 - length) * Math.sin(angle * Math.PI / 180);
    
    const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
    line.setAttribute('x1', x1);
    line.setAttribute('y1', y1);
    line.setAttribute('x2', x2);
    line.setAttribute('y2', y2);
    line.setAttribute('stroke', i % 30 === 0 ? '#3498db' : '#7f8c8d');
    line.setAttribute('stroke-width', i % 30 === 0 ? '2' : '1');
    
    svg.appendChild(line);
  }
}

// Update status data
async function updateStatus() {
  try {
    const response = await fetch('/api/status');
    const data = await response.json();
    
    // Update compass needle
    const needle = document.getElementById('needle');
    needle.setAttribute('transform', `rotate(${data.azimuth} 200 200)`);
    
    // Update target line
    const targetLine = document.getElementById('target-line');
    targetLine.setAttribute('transform', `rotate(${data.targetAzimuth} 200 200)`);
    
    // Update center display
    document.getElementById('azimuth-value').textContent = `${data.azimuth.toFixed(1)}°`;
    
    // Update status bar
    document.getElementById('connected').textContent = data.connected ? '✅ Verbunden' : '❌ Getrennt';
    document.getElementById('motor-state').textContent = data.motorState;
    document.getElementById('slewing').textContent = data.slewing ? '🔄 Aktiv' : '⏸️ Inaktiv';
    
    // Update info panel
    document.getElementById('current-az').textContent = `${data.azimuth.toFixed(2)}°`;
    document.getElementById('target-az').textContent = `${data.targetAzimuth.toFixed(2)}°`;
    document.getElementById('encoder-value').textContent = data.encoderValue;
    document.getElementById('uptime').textContent = formatUptime(data.uptime);
    
    // Update footer
    document.getElementById('wifi-rssi').textContent = data.wifiRSSI;
    
  } catch (error) {
    console.error('Failed to fetch status:', error);
  }
}

// Format uptime
function formatUptime(seconds) {
  const days = Math.floor(seconds / 86400);
  const hours = Math.floor((seconds % 86400) / 3600);
  const mins = Math.floor((seconds % 3600) / 60);
  const secs = seconds % 60;
  
  let result = '';
  if (days > 0) result += `${days}d `;
  if (hours > 0) result += `${hours}h `;
  if (mins > 0) result += `${mins}m `;
  result += `${secs}s`;
  
  return result;
}

// Initialize
if (document.getElementById('compass')) {
  createDegreeMarks();
  setInterval(updateStatus, UPDATE_INTERVAL);
  updateStatus();
}
)rawliteral";
}
