#!/bin/sh
# $Id: rc.conf.pl,v 1.3 1998/08/10 19:07:53 abial Exp $
###  Network configuration sub-section  ######################
### Basic network options: ###
hostname="pico.mydomain.org.pl"	# Set this!
tcp_extensions="NO"		# Allow RFC1323 & RFC1644 extensions (or NO).
network_interfaces="lo0"	# List of network interfaces (lo0 is loopback).
ifconfig_lo0="inet 127.0.0.1"	# default loopback device configuration.
#ifconfig_lo0_alias0="inet 127.0.0.254 netmask 0xffffffff" # Sample alias entry.
### Network routing options: ###
defaultrouter="NO"		# Set to default gateway (or NO).
static_routes=""		# Set to static route list (or leave empty).
gateway_enable="NO"		# Set to YES if this host will be a gateway.
arpproxy_all=""			# replaces obsolete kernel option ARP_PROXYALL.
###  System console options  #################################
keymap="pl_PL.ISO_8859-2.kbd"		# keymap in /usr/share/syscons/* (or NO).
keyrate="NO"		# keyboard rate to: slow, normal, fast (or NO).
keybell="NO"		# bell to duration.pitch or normal or visual (or NO).
keychange="NO"		# function keys default values (or NO).
cursor="NO"		# cursor type {normal|blink|destructive} (or NO).
font8x16="iso02-8x16.fnt"		# font 8x16 from /usr/share/syscons/* (or NO).
font8x14="NO"		# font 8x14 from /usr/share/syscons/* (or NO).
font8x8="NO"		# font 8x8 from /usr/share/syscons/* (or NO).
blanktime="NO"		# blank time (in seconds) or "NO" to turn it off.
moused_enable="YES"	# Run the mouse daemon
moused_type="microsoft"
moused_port="/dev/cuaa0"
moused_flags="-3"
### Allow local configuration override at the very end here ##
if [ -f /etc/rc.conf.local ]; then
	. /etc/rc.conf.local
fi
