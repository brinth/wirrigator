# AutoIrrigation

Wemos Mini (ESP8266) Project for Automatic Irrigation of Home Garden

## Component Required
1. Wemos D1 Mini (ESP8266)
2. 12V DC Solenoid Valve
3. Flow Meter
4. Power supply (as per the setup)

## HLD
- Wemos board operates the Solenoid Valve every day for 2 mins (which could be configured)
    morning and evening
- It uses the flow meter to control the Amount of water to be discharged and also to validate the 
  Valve operations
- It gets the Time online and updates every time on bootup
- It should also connect to OpenHAB local server through MQTT for override

## LLD
- Uses Solid State Relay for Solenoid operations
- Uses NTP for Time Sync
- Uses MQTT for OpenHAB communication
