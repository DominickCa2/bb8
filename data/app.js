const speedBox = document.getElementById("speed");
const speedButton = document.getElementById("setSpeedButton");
const accLabel = document.getElementById("acc");
const enButton = document.getElementById("en_btn");
const speedSlider = document.getElementById("speedSlider");
const speedValue = document.getElementById("speedValue");
const fwSlider = document.getElementById("fwSlider");
const fwValue = document.getElementById("fwValue");
const kpInput = document.getElementById("kpInput");
const kdInput = document.getElementById("kdInput");

let enabled = false;
let drv_speed = 0;
const FW_MID_THR = 128;
let fw_speed = FW_MID_THR;

function sendSocketMessage(cmd, value) {
    if (socket && socket.readyState == WebSocket.OPEN) {
        socket.send(JSON.stringify({ cmd, value }));
    }
}

function releaseSlider(slider, valueBox, cmd, resetValue, stateSetter) {
    const resetToValue = () => {
        slider.value = resetValue;
        valueBox.textContent = String(resetValue);
        if (stateSetter) {
            stateSetter(resetValue);
        }
        sendSocketMessage(cmd, resetValue);
    };

    slider.addEventListener("pointerup", resetToValue);
    slider.addEventListener("pointercancel", resetToValue);
    slider.addEventListener("blur", resetToValue);
}

speedSlider.addEventListener("input", () => {
    speedValue.textContent = speedSlider.value;
    drv_speed = Number(speedSlider.value);
    sendSocketMessage("drv_speed", drv_speed);
});

fwSlider.addEventListener("input", () => {
    fwValue.textContent = fwSlider.value;
    fw_speed = Number(fwSlider.value);
    sendSocketMessage("fw_speed", fw_speed);
});

releaseSlider(speedSlider, speedValue, "drv_speed", 0, (value) => {
    drv_speed = value;
});

releaseSlider(fwSlider, fwValue, "fw_speed", 90, (value) => {
    fw_speed = value;
});

kpInput.addEventListener("input", () => {
    const kp = Number(kpInput.value);
    sendSocketMessage("kp", kp);
});

kdInput.addEventListener("input", () => {
    const kd = Number(kdInput.value);
    sendSocketMessage("kd", kd);
});

enButton.addEventListener("click", async () => {
    enabled = !enabled;
    if (enabled) {
        enButton.classList.remove("stopped");
        enButton.classList.add("enabled");
        enButton.textContent = "STOP";
    }
    else {
        enButton.classList.remove("enabled");
        enButton.classList.add("stopped");
        enButton.textContent = "Enable";
    }
    try {
        if (enabled) {
            const response = await fetch(`/api/enable`);
            const text = await response.text();
        }
        else {
            const response = await fetch(`/api/stop`);
            const text = await response.text();
        }
    }
    catch (err) {
        console.error(err);
    }
})

let socket;

function connectWebSocket() {
    socket = new WebSocket("ws://" + window.location.host + "/ws");
    socket.onopen = () => {
        console.log("Opened WebSocket");
    };
    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        accLabel.textContent = data.angle;
    }

    socket.onclose = () => {
        console.log("Closed WebSocket");
        setTimeout(connectWebSocket, 1000);
    }

    socket.onerror = (error) => {
        console.log("WebSocket error: ", error);
    }
}

connectWebSocket();


