//
// Created by shan on 15/05/24.
//

#ifndef CONFIG_H
#define CONFIG_H

#ifndef WIFI_SSID
#define WIFI_SSID "default_ssid"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "default_password"
#endif

#ifndef HOST
#define HOST "host_base64"
#endif

#ifndef TOKEN
#define TOKEN "token_base64"
#endif

#ifndef DEVICE_ID
#define DEVICE_ID "device_id_base64"
#endif

#ifndef INSECURE
#define INSECURE 0  // Default to secure (wss://) for backward compatibility
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 443  // Default port for wss://
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* host = HOST;
const char* token = TOKEN;
const char* device_id = DEVICE_ID;

#endif //CONFIG_H
