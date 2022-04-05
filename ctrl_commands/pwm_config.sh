#!/bin/bash
DEVICE="/dev/ttyACM0"

cmd_send() {
	printf "$*\n" >> ${DEVICE}
}

config_channel() {
	CHANNEL="$1"
	[ -z $CHANNEL ] && CHANNEL=1
	cmd_send "0:hello"
	cmd_send "0:pwm${CHANNEL}/freq/set 500.0"
	cmd_send "0:pwm${CHANNEL}/duty/set 0.5"
	cmd_send "0:pwm${CHANNEL}/polarity/set \"+\""
	cmd_send "0:pwm${CHANNEL}/started/set yes"
}


config_channel 1
config_channel 2
config_channel 3
