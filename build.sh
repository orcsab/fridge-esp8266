#!/usr/bin/bash
RED="\033[1;31m"
NOCOLOR="\033[0m"

#SERIAL="/dev/cu.usbserial-0001"
SERIAL="COM4"
FQBN="esp8266:esp8266:nodemcuv2"

echo "compile line: arduino-cli -compile --fqbn $FQBN ."
arduino-cli compile --fqbn $FQBN .
if [ $? -eq 0 ]; then
  echo "compile successful"
else
  echo "${RED}compile exited with error code${NOCOLOR}"
  exit $?
fi

arduino-cli upload -t --fqbn $FQBN -p $SERIAL .
if [ $? -eq 0 ]; then
  echo "upload successful"
else
  echo "${RED}upload exited with error code${NOCOLOR}"
  exit $?
fi
