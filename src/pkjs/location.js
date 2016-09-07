var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationError(err) {

}

function locationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);

      // Conditions
      var conditions = json.weather[0].main;      
      
      // Assemble dictionary using our keys
      var dictionary = {
        'COMMAND':'weather',
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('WEATHER SENT', JSON.stringify(dictionary));
        },
        function(e) {
          console.log('WEATHER ERROR', JSON.stringify(dictionary));
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