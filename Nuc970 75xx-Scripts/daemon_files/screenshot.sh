machine_id=`cat /usr/share/status/EEPROM-data | head -n 1`
fbgrab /tmp/test.png
mosquitto_pub -h 106.51.48.231 -t $machine_id/screenshot -f /tmp/test.png -u "clanmqtt" -P "clan123"

