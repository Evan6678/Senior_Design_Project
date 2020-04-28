#!/bin/bash
sudo /opt/tomcat6/apache-tomcat-6.0.53/bin/catalina.sh start > /dev/null
sudo python3 ~/Documents/signalReader.py &
firefox "localhost:9090/ScadaBR" &
