# MQTT Reverse Geocoding

## Publishing Client
- The publishing client connects to an MQTT broker running on the local machine and publishes a random latitude and longitude in JSON to __globe-topic__.
- The information is published continually on a three second delay for five minutes.

## Subscribing Client
- The subscribing client recieves each publication from the broker and displays it to the console window.
- An API call will be made for each publication to provide a the information availale from the Bing Maps API.

## *Dependencies*
- For the clients to communicate, a Mosquitto broker must be runnnig on the host machine. If you do not have Mosquitto installed, you can download it here: https://mosquitto.org/download/
- The publishing client reqruies that the Mosquitto library is installed using the package manager of your choice.
- You will need to run the Subscribing client (mqtt_geocoding_sub.exe) in the same directory as a file named "bing-maps-key.txt" that contains your Bing Maps key. If you do not have a Bing Maps key, you can register for one here: https://docs.microsoft.com/en-us/bingmaps/getting-started/bing-maps-dev-center-help/getting-a-bing-maps-key