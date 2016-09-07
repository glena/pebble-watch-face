var myAPIKey = null;

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationError(err) {
  console.log("Location error", err);
}

function locationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  xhrRequest(url, 'GET', 
    function(responseText) {
      var json = JSON.parse(responseText);
      var temperature = Math.round(json.main.temp - 273.15);
      var conditions = json.weather[0].main;      
      
      var dictionary = {
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions
      };
      
      Pebble.sendAppMessage(dictionary,
        function(e) {

        },
        function(e) {

        }
      );
    }      
  );
}

module.exports.getWeather = function () {
  if (myAPIKey === null) {
    return;
  }
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

module.exports.setAPIKey = function (APIKey) {
  myAPIKey = APIKey;
}