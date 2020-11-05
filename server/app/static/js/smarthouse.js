var xhr = new XMLHttpRequest();
var base_url = "http://localhost:5000/";
var checkboxItems = document.querySelectorAll("input[class=onoffswitch2-checkbox]");

for (let i = 0; i < checkboxItems.length; i++) {
    checkboxItems[i].addEventListener("change", function () {
        if (this.checked) {
            turnOnOff(this.id, 255);
        } else {
            turnOnOff(this.id, 0);
        }
    });
}

function turnOnOff(id, value) {
    var data = {};
    data.value = value;
    var json = JSON.stringify(data);
    var cmd = "set_channel_value/";
    var url = base_url.concat(cmd);
    url = url.concat(id);
    xhr.open("PUT", url, true);
    xhr.setRequestHeader("Content-type", "application/json; charset=utf-8");
    xhr.onreadystatechange = function () {
        if (this.readyState == XMLHttpRequest.DONE && this.status == 200)
            console.log("WORZONATA");
    }
    xhr.send(json);
}
