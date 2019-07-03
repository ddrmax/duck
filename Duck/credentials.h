// Please do not commit credentials to public GH

#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

#define SSID        "MySpectrumWiFi6e-2G" // Type your SSID
#define PASSWORD    "famousocean778" // Type your Password

//#define MQTT_MAX_PACKET_SIZE 1000;

#define ORG         "9c6nfo"                  // "quickstart" or use your organisation
#define DEVICE_ID   "SolarCT"
#define DEVICE_TYPE "PAPA"                // your device type or not used for "quickstart"
#define TOKEN       "rA7tGJNrLC8UA3S50?"      // your device token or not used for "quickstart"

char server[]           = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[]            = "iot-2/evt/sensorup/fmt/json";
char authMethod[]       = "use-token-auth";
char token[]            = TOKEN;
char clientId[]         = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

#endif
