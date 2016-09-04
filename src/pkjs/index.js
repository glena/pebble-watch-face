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
  
  Pebble.sendAppMessage(dict, function(e) {
    location.getWeather();
  }, function(e) {

  });
});

Pebble.addEventListener('appmessage', function(event) {
  console.log('appmessage',JSON.stringify(event));    
  location.getWeather();
});

function restoreSettings() {
  var json_settings = localStorage.getItem('clay-settings');
  var settings = JSON.parse(json_settings);
  
  if (settings) {

    Pebble.sendAppMessage(clay.getSettings(json_settings, true),
        function(e) {
          console.log('SENT');
        },
        function(e) {
          console.log('ERROR');
        }
      );
    

    location.setAPIKey(settings.WEATHER_APIKEY);
    location.getWeather();
  }
}

Pebble.addEventListener('ready', function(event) {    
  Pebble.sendAppMessage({COMMAND:'init'},
        function(e) {
          console.log('SENT init');
        },
        function(e) {
          console.log('ERROR init');
        }
      );
});