var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var location = require('./location')
var myAPIKey = null;

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  var dict = clay.getSettings(e.response);
  var data = JSON.parse(e.response);

  location.setAPIKey(data.WEATHER_APIKEY.value);
  localStorage.setItem('weather_api_key', data.WEATHER_APIKEY.value);
  
  Pebble.sendAppMessage(dict, function(e) {
    location.getWeather();
  }, function(e) {

  });
});

Pebble.addEventListener('appmessage', function(event) {
  location.getWeather();
});

function restoreSettings() {
  var weather_api_key = localStorage.getItem('weather_api_key');
  location.setAPIKey(weather_api_key);
  location.getWeather();
}

Pebble.addEventListener('ready', function(event) {    
  restoreSettings();
});