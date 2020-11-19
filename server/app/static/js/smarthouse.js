var xhr = new XMLHttpRequest();
var base_url = "http://localhost:5000/";

window.onload = function () {
    var switchItems = document.querySelectorAll("input[name=switch]");
    for (let i = 0; i < switchItems.length; i++) {
        switchItems[i].addEventListener("change", function () {
            if (this.checked) {
                var brightness = document.getElementById("brightness_" + this.id);
                if (brightness != null) {
                    var value = Math.round((255 / 100) * brightness.value);
                    turnOnOff(this.id, value);
                } else {
                    turnOnOff(this.id, 255);
                }
            } else {
                turnOnOff(this.id, 0);
            }
        });
    }

    var sensItems = document.querySelectorAll("input[name=sens]");
    for (let i = 0; i < sensItems.length; i++) {
        sensItems[i].addEventListener("change", function () {
            if (this.checked) {
                turnOnOff(this.id, 1);
            } else {
                turnOnOff(this.id, 0);
            }
        });
    }


    setLocation();

    var time = document.getElementById("time");
    setInterval(refreshTime, 1000);

    var tmp_button = document.getElementById("btn-tmp");
    tmp_button.addEventListener("click", get_temperature);

    var brightnessItems = document.querySelectorAll("select[name=brightness]");
    for (let i = 0; i < brightnessItems.length; i++) {
        brightnessItems[i].addEventListener("change", function () {
            var channel = brightnessItems[i].id.substring(11);
            var value = Math.round((255 / 100) * brightnessItems[i].value);
            var ch_switch = document.getElementById(channel);
            if (ch_switch.checked) {
                turnOnOff(channel, value);
            }
        });
    }
};

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
            console.log("Request complete");
    };
    xhr.send(json);
}

function get_temperature() {
    var cmd = "get_temperature";
    var url = base_url.concat(cmd);
    xhr.open("GET", url, true);
    xhr.onreadystatechange = function () {
        if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {
            json_res = JSON.parse(xhr.response);
            document.getElementById("temperature").innerHTML = json_res.temperature + "°";
            document.getElementById("humidity").innerHTML = "Umidità: " + json_res.humidity + "%";
            console.log("Request complete");
        }
    };
    xhr.send();
}

function setLocation() {
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(showPosition);
    } else {
        console.log("Error");
    }
}

function showPosition(position) {
    var lat = position.coords.latitude;
    var long = position.coords.longitude;
    xhr.open("GET", "https://maps.googleapis.com/maps/api/geocode/json?latlng=" + lat + "," + long + "&key=AIzaSyAUKVWhf**************", true);
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var address = JSON.parse(this.responseText);
            var city = address.results[0].address_components[2].long_name;
            document.getElementById("city").innerHTML += " " + city;
        }
    };
    xhr.send();
}

function refreshTime() {
    var dateString = new Date().toLocaleString("it-IT", { timeZone: "Europe/Rome" });
    var formattedString = dateString.replace(", ", " - ");
    time.innerHTML = formattedString;
}