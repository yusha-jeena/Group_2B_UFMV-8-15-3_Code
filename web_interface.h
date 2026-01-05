#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

// The following frontend code has been developed by AI 

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>AL5D 2-Slot Master</title>
  <style>
    body { font-family: sans-serif; background: #121212; color: #ddd; text-align: center; padding: 10px; margin: 0; }
    .container { max-width: 700px; margin: auto; background: #1e1e1e; padding: 15px; border-radius: 10px; min-height: 90vh; }
    
    .nav { display: flex; border-bottom: 2px solid #333; margin-bottom: 20px; }
    .nav-btn { flex: 1; padding: 15px; background: #1a1a1a; color: #888; border: none; font-weight: bold; cursor: pointer; font-size: 1.1em; }
    .nav-btn.active { background: #1e1e1e; color: #00E676; border-bottom: 3px solid #00E676; }
    
    .page { display: none; }
    .page.active { display: block; }

    .control-row { display: flex; align-items: center; margin: 15px 0; }
    label { width: 70px; font-weight: bold; text-align: left; font-size: 0.9em; color: #ccc;}
    input[type=range] { flex-grow: 1; margin: 0 15px; cursor: pointer; }
    .val { width: 40px; text-align: right; font-family: monospace; color: #00E676; }
    .speed-box { background: #263238; padding: 10px; border-radius: 5px; margin-bottom: 20px; border-left: 4px solid #FF9100; }
    
    .presets { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 20px; border-top: 1px solid #333; padding-top: 20px; }
    .slot { background: #333; padding: 8px; border-radius: 5px; display: flex; flex-direction: column; align-items: center;}
    .slot-label { font-weight: bold; color: #00E676; font-size: 0.8em; margin-bottom: 5px; }
    .slot-btns { display: flex; width: 100%; gap: 5px; }
    button { border: none; padding: 8px 5px; border-radius: 4px; font-weight: bold; cursor: pointer; flex-grow: 1; font-size: 0.8em;}
    .btn-save { background: #555; color: #fff; max-width: 50px;}
    .btn-go { background: #00E676; color: #000; }
    
    .matrix-area { margin-top: 20px; padding: 10px; background: #222; border-radius: 10px; border: 1px solid #444; }
    .matrix-table { width: 100%; border-collapse: collapse; }
    .matrix-table th { color: #888; padding: 10px; font-size: 0.9em; border-bottom: 1px solid #444; }
    .matrix-table td { padding: 10px 5px; border-bottom: 1px solid #333; }
    
    .btn-matrix { width: 100%; padding: 12px; background: #29b6f6; color: #000; font-weight: bold; border-radius: 5px; font-size: 0.9em; }
    .btn-matrix:active { background: #0288d1; }
    .btn-batch { width: 100%; padding: 15px; background: #ffeb3b; color: #000; font-weight: bold; font-size: 1em; border-radius: 5px; margin-top: 5px;}
    .b2 { background: #ba68c8; } 
    .batch-b2 { background: #9c27b0; color: #fff; }

    .loop-control { margin-bottom: 15px; display:flex; align-items:center; justify-content:center; gap:10px; background:#333; padding:10px; border-radius:8px;}
    input[type=checkbox] { transform: scale(1.5); cursor: pointer; }
    .loop-label { color: #fff; font-weight: bold; font-size: 1.1em; cursor: pointer; }

    .btn-stop { width: 100%; background: #f44336; color: #fff; padding: 20px; font-size: 1.2em; margin-top: 20px; border-radius: 8px; font-weight: bold;}
    .btn-home-lg { width: 100%; background: #444; color: #fff; padding: 15px; font-size: 1.1em; margin-top: 10px; border-radius: 5px; border: 1px solid #666;}

    .sequencer { margin-top: 30px; border: 1px solid #555; border-radius: 8px; padding: 15px; background: #1a262e; }
    .seq-selector { margin-bottom: 15px; padding: 10px; background: #000; color: #fff; border: 1px solid #555; width: 100%; }
    .btn-add { background: #4caf50; color: #fff; width: 60%; padding: 10px; }
    .btn-clear { background: #d32f2f; color: #fff; width: 35%; }
    #seq-list { list-style: none; padding: 0; text-align: left; max-height: 150px; overflow-y: auto; background: #000; border: 1px solid #333; font-size: 0.8em; color: #aaa;}
    #seq-list li { padding: 5px; border-bottom: 1px solid #222; }

    .status { margin-top: 20px; padding: 10px; background: #000; color: #00E676; font-family: monospace; border-radius: 4px; }
  </style>
</head>
<body>
  <div class="container">
    
    <div class="nav">
        <button class="nav-btn active" onclick="showPage('operation')">OPERATION</button>
        <button class="nav-btn" onclick="showPage('training')">TRAINING</button>
    </div>

    <div id="operation" class="page active">
        <div class="status" id="stat-op">STATUS: READY</div>

        <div class="matrix-area">
            <h3 style="margin-top:0; color:#fff;">COMMAND CENTER</h3>
            
            <div class="loop-control">
                <input type="checkbox" id="loop-check">
                <label for="loop-check" class="loop-label">ENABLE LOOP MODE</label>
            </div>

            <table class="matrix-table">
                <thead>
                    <tr>
                        <th style="text-align:left;">SOURCE</th>
                        <th>TO BIN 1</th>
                        <th>TO BIN 2</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td style="text-align:left; font-weight:bold;">SLOT 1</td>
                        <td><button class="btn-matrix" onclick="runSequence(1)">MOVE &#9658;</button></td>
                        <td><button class="btn-matrix b2" onclick="runSequence(3)">MOVE &#9658;</button></td>
                    </tr>
                    <tr>
                        <td style="text-align:left; font-weight:bold;">SLOT 2</td>
                        <td><button class="btn-matrix" onclick="runSequence(2)">MOVE &#9658;</button></td>
                        <td><button class="btn-matrix b2" onclick="runSequence(4)">MOVE &#9658;</button></td>
                    </tr>
                </tbody>
            </table>
            
            <div style="margin-top:20px; display:grid; grid-template-columns: 1fr 1fr; gap:10px;">
                 <button class="btn-batch" onclick="runBatch(1)">MOVE ALL<br>TO BIN 1</button>
                 <button class="btn-batch batch-b2" onclick="runBatch(2)">MOVE ALL<br>TO BIN 2</button>
            </div>

            <button class="btn-home-lg" onclick="load(4)">GO HOME</button>
            <button class="btn-stop" onclick="stop()">STOP ROBOT</button>
        </div>
    </div>

    <div id="training" class="page">
        <div class="status" id="stat-tr">STATUS: TRAINING MODE</div>
        
        <div style="margin-top:20px;">
            <div class="control-row speed-box">
            <label style="color:#FF9100">Speed</label>
            <input type="range" min="1" max="100" value="20" id="S" oninput="send()">
            <span id="vS" class="val" style="color:#FF9100">20</span>
            </div>

            <div class="control-row"><label>Rotate</label><input type="range" min="-90" max="90" value="0" id="ROT" oninput="send()"><span id="vROT" class="val">0</span></div>
            <div class="control-row"><label>Reach</label><input type="range" min="50" max="320" value="150" id="RCH" oninput="send()"><span id="vRCH" class="val">150</span></div>
            <div class="control-row"><label>Height</label><input type="range" min="-50" max="400" value="100" id="Z" oninput="send()"><span id="vZ" class="val">100</span></div>
            <div class="control-row"><label>Pitch</label><input type="range" min="-90" max="90" value="0" id="P" oninput="send()"><span id="vP" class="val">0</span></div>
            <div class="control-row"><label>Roll</label><input type="range" min="-100" max="100" value="0" id="R" oninput="send()"><span id="vR" class="val">0</span></div>
            <div class="control-row"><label>Grip</label><input type="range" min="0" max="18.1" step="0.1" value="18.1" id="G" oninput="send()"><span id="vG" class="val">18.1</span></div>
        </div>

        <div class="presets">
          <div style="grid-column: 1;">
            <div class="slot"><span class="slot-label">REC SLOT 1</span><div class="slot-btns"><button class="btn-save" onclick="save(1)">SAVE</button><button class="btn-go" onclick="load(1)">GO</button></div></div>
            <div class="slot" style="margin-top:5px"><span class="slot-label">REC SLOT 2</span><div class="slot-btns"><button class="btn-save" onclick="save(5)">SAVE</button><button class="btn-go" onclick="load(5)">GO</button></div></div>
          </div>
          
          <div style="grid-column: 2;">
            <div class="slot"><span class="slot-label">BIN 1</span><div class="slot-btns"><button class="btn-save" onclick="save(2)">SAVE</button><button class="btn-go" onclick="load(2)">GO</button></div></div>
            <div class="slot" style="margin-top:5px"><span class="slot-label">BIN 2</span><div class="slot-btns"><button class="btn-save" onclick="save(3)">SAVE</button><button class="btn-go" onclick="load(3)">GO</button></div></div>
            <div class="slot" style="margin-top:5px"><span class="slot-label">HOME</span><div class="slot-btns"><button class="btn-save" onclick="save(4)">SAVE</button><button class="btn-go" onclick="load(4)">GO</button></div></div>
          </div>
        </div>

        <div class="sequencer">
            <div class="seq-header" style="color:#fff; font-weight:bold; margin-bottom:5px;">SEQUENCE BUILDER</div>
            <select id="seq-select" class="seq-selector" onchange="updateSeqUI()">
                <option value="1">PATTERN 1: Slot 1 -> Bin 1</option>
                <option value="2">PATTERN 2: Slot 2 -> Bin 1</option>
                <option value="3">PATTERN 3: Slot 1 -> Bin 2</option>
                <option value="4">PATTERN 4: Slot 2 -> Bin 2</option>
            </select>
            <div style="display:flex; justify-content:center; gap:10px;">
                <button class="btn-add" onclick="addStep()">+ ADD STEP</button>
                <button class="btn-clear" onclick="clearSeq()">CLEAR</button>
            </div>
            <ul id="seq-list"></ul>
        </div>
    </div>
  </div>

  <script>
    let busy = false;
    let stopFlag = false;
    const keys = ['ROT','RCH','Z','P','R','G','S'];
    
    let sequences = { 1:[], 2:[], 3:[], 4:[] };

    function showPage(pageId) {
        document.querySelectorAll('.page').forEach(el => el.classList.remove('active'));
        document.querySelectorAll('.nav-btn').forEach(el => el.classList.remove('active'));
        document.getElementById(pageId).classList.add('active');
        let index = (pageId === 'operation') ? 0 : 1;
        document.querySelectorAll('.nav-btn')[index].classList.add('active');
    }

    function setStatus(msg) {
        document.getElementById('stat-op').innerText = msg;
        document.getElementById('stat-tr').innerText = msg;
    }

    function send(override = false) {
      keys.forEach(k => document.getElementById('v'+k).innerText = document.getElementById(k).value);
      
      if(busy && !override) return Promise.resolve(); 
      if(!override) busy = true;    
      
      let q = keys.map(k => k.toLowerCase() + '=' + document.getElementById(k).value).join('&');
      
      return fetch('/set?' + q)
        .then(() => { if(!override) busy = false; })
        .catch(e => { if(!override) busy = false; });
    }

    function stop() {
        stopFlag = true;
        setStatus("STATUS: STOPPED");
    }

    function save(id) {
      let data = {};
      keys.forEach(k => data[k] = document.getElementById(k).value);
      localStorage.setItem('al5d_cyl_slot_'+id, JSON.stringify(data));
      alert("Position Saved!");
    }
    
    function load(id) {
      let json = localStorage.getItem('al5d_cyl_slot_'+id);
      if(!json) return;
      let data = JSON.parse(json);
      keys.forEach(k => { if(data[k]) document.getElementById(k).value = data[k]; });
      send();
    }

    window.onload = function() {
        for(let i=1; i<=4; i++){
            let s = localStorage.getItem('al5d_seq_'+i);
            if(s) sequences[i] = JSON.parse(s);
        }
        updateSeqUI();
    }

    function addStep() {
        let id = document.getElementById('seq-select').value;
        let step = {};
        keys.forEach(k => step[k] = document.getElementById(k).value);
        sequences[id].push(step);
        localStorage.setItem('al5d_seq_' + id, JSON.stringify(sequences[id]));
        updateSeqUI();
    }

    function clearSeq() {
        let id = document.getElementById('seq-select').value;
        if(confirm("Clear Pattern " + id + "?")) {
            sequences[id] = [];
            localStorage.setItem('al5d_seq_' + id, JSON.stringify([]));
            updateSeqUI();
        }
    }

    function updateSeqUI() {
        let id = document.getElementById('seq-select').value;
        let list = document.getElementById('seq-list');
        list.innerHTML = "";
        sequences[id].forEach((step, index) => {
            let li = document.createElement('li');
            li.innerText = `#${index+1}: Rot:${step.ROT} Rch:${step.RCH} Z:${step.Z} Grip:${step.G}`;
            list.appendChild(li);
        });
    }

    async function runSequence(id, isBatch = false) {
        if(busy && !isBatch) return; 
        
        if(!isBatch) stopFlag = false; 
        
        let seq = sequences[id];
        let loopEnabled = document.getElementById('loop-check').checked;

        if(!seq || seq.length === 0) {
            if(!isBatch) alert("Pattern " + id + " empty! Use Training Page.");
            return;
        }

        if(!isBatch) setStatus(loopEnabled ? "LOOPING PATTERN " + id : "RUNNING PATTERN " + id);
        
        busy = true; 

        do {
            for (let i = 0; i < seq.length; i++) {
                if(stopFlag) break; 
                let step = seq[i];
                keys.forEach(k => { if(step[k]) document.getElementById(k).value = step[k]; });
                
                await send(true); 
                
                await new Promise(r => setTimeout(r, 2000));
            }
            if(stopFlag) break;
            if(isBatch) break; 

        } while(loopEnabled && !stopFlag);

        if(!isBatch) {
            busy = false; 
            setStatus(stopFlag ? "STATUS: STOPPED" : "STATUS: READY");
        }
    }

    async function runBatch(binTarget) {
        if(busy) return;
        stopFlag = false;
        busy = true;
        
        let batchList = [];
        if(binTarget === 1) batchList = [1, 2]; 
        if(binTarget === 2) batchList = [3, 4]; 
        
        let loopEnabled = document.getElementById('loop-check').checked;

        setStatus(loopEnabled ? "LOOPING BATCH TO BIN " + binTarget : "RUNNING BATCH TO BIN " + binTarget);

        do {
            for (let id of batchList) {
                if(stopFlag) break;
                await runSequence(id, true); 
                await new Promise(r => setTimeout(r, 1000));
            }
        } while(loopEnabled && !stopFlag);

        busy = false;
        setStatus(stopFlag ? "STATUS: STOPPED" : "STATUS: READY");
    }
  </script>
</body>
</html>
)rawliteral";

#endif