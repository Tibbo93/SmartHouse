var xhr = new XMLHttpRequest();
var base_url = "http://localhost:5000/";

window.onload = function () {
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

    setLocation();

    var time = document.getElementById("time");
    setInterval(refreshTime, 1000);

    var tmp_button = document.getElementById("btn-tmp");
    tmp_button.addEventListener("click", get_temperature);
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
    lat = position.coords.latitude;
    long = position.coords.longitude;
    xhr.open("GET", "https://maps.googleapis.com/maps/api/geocode/json?latlng=" + lat + "," + long + "&key=AIzaSyAU***********", true);
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
    formattedString = dateString.replace(", ", " - ");
    time.innerHTML = formattedString;
}