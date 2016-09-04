module.exports = [
  {
    "type": "heading",
    "defaultValue": "Serial Whatchface Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BACKGROUND_COLOR",
        "defaultValue": "0xFFFFFF",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "TEXT_COLOR",
        "defaultValue": "0x000000",
        "label": "Text Color"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "OpenWeatherMap"
      },
      {
        "type": "input",
        "label": "ApiKey",
        "messageKey":"WEATHER_APIKEY",
        "defaultValue": false
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save"
  }
];