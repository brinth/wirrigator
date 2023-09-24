#!/bin/sh

# Create secret.h 
export CONFIG_WIFI_SSID
export CONFIG_WIFI_PASS

read -p "Enter your WiFi SSID: " CONFIG_WIFI_SSID
read -p "Enter password: " CONFIG_WIFI_PASS

cat << EOF > secret.h
#ifndef __SECRET_H__
#define __SECRET_H__

#define CONFIG_WIFI_SSID "${CONFIG_WIFI_SSID}"
#define CONFIG_WIFI_PASS "${CONFIG_WIFI_PASS}"

#endif // __SECRET_H__
EOF

echo "My Lips are sealed ;)"

