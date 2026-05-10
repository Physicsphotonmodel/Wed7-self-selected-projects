// 綁定 DOM
var sliderPitch = document.getElementById('slider-pitch');
var sliderRoll = document.getElementById('slider-roll');
var pitchValDisplay = document.getElementById('pitch-val');
var rollValDisplay = document.getElementById('roll-val');
var rotDisplay = document.getElementById('rot-display');

// 滑桿事件
sliderPitch.addEventListener('input', function(e) {
    var p = parseInt(e.target.value);
    pitchValDisplay.innerText = p + "°";
    window.updateNeckRotation(p, parseInt(sliderRoll.value));
});

sliderRoll.addEventListener('input', function(e) {
    var r = parseInt(e.target.value);
    rollValDisplay.innerText = r + "°";
    window.updateNeckRotation(parseInt(sliderPitch.value), r);
});

// 全域更新函數
window.updateNeckRotation = function(pitch, roll) {
    if (headGroup) {
        // 設定 3D 模型旋轉 (將角度轉弧度)
        headGroup.rotation.x = pitch * Math.PI / 180;
        headGroup.rotation.z = roll * Math.PI / 180;
        
        // 更新文字
        rotDisplay.innerText = "角度：P " + pitch + "° | R " + roll + "°";
        
        // 同步滑桿
        sliderPitch.value = pitch;
        sliderRoll.value = roll;
    }
};

window.updateTemperature = function(celsius) {
    if (scene) {
        var head = scene.getObjectByName("UserHead");
        if (head) {
            var p = (Math.max(30, Math.min(42, celsius)) - 30) / 12;
            head.material.color.setHSL((1 - p) * 0.66, 1, 0.5);
            document.getElementById('temp-display').innerText = "溫度：" + celsius + "°C";
        }
    }
};

// 延遲一下確保模型蓋好
setTimeout(function() {
    window.updateNeckRotation(0, 0);
    window.updateTemperature(36.5);
}, 200);