var Clay = require('pebble-clay');
var clayConfig = require('./config.js');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var myAPIKey = null;

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
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
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {

        },
        function(e) {

        }
      );
    }      
  );
}

function locationError(err) {

}

function getWeather() {
  if (myAPIKey === null) {
    return;
  }
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  // Get the keys and values from each config item
  var dict = clay.getSettings(e.response);
  var data = JSON.parse(e.response);

  myAPIKey = data.WeatherApiKey.value;
  
  // Send settings values to watch side
  Pebble.sendAppMessage(dict, function(e) {
    getWeather();
  }, function(e) {

  });
});

// // Listen for when an AppMessage is received
// Pebble.addEventListener('appmessage',
//   function(e) {
//     getWeather();
//   }                     
// );

Pebble.addEventListener('appmessage', function(event) {    
    getWeather();
});

function restoreSettings() {
  // Restore settings from localStorage and send to watch
  var json_settings = localStorage.getItem('clay-settings');
  var settings = JSON.parse(json_settings);
  
  if (settings) {
    console.log(JSON.stringify(clay.getSettings(json_settings, true)));
    Pebble.sendAppMessage(clay.getSettings(json_settings, true),
        function(e) {
          console.log('SENT');
        },
        function(e) {
          console.log('ERROR');
        }
      );
    
    myAPIKey = settings.WeatherApiKey;
    getWeather();
  }
}

Pebble.addEventListener('ready', function(event) {    
    restoreSettings();
});