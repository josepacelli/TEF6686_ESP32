#ifndef NTP_H
#define NTP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Time.h>

static const char ntpServerName[] = "0.pool.ntp.org";
static const int localPort = 8944;
const int NTP_PACKET_SIZE = 48;

extern ESP32Time rtc;
extern WiFiUDP Udp;
extern bool wifi;
extern bool NTPupdated;
extern int8_t NTPoffset;

void sendNTPpacket(IPAddress &address);
void NTPupdate();
time_t getNtpTime();

#endif
