/*
  Clock/Calendar TG1508D5V5 retrofit
  with Rapsberry Pi Pico W

  by Rodrigo Feliciano
  https://www.github.com/pakequis

*/

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "LED_display.h"

#define TIMEZONE_OFFSET -3 * 3600 /* Define the time zone */

/* Define your Wi-Fi credentials */
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

/* Define the NTP servers */
const char* ntp_server1 = "a.ntp.br";
const char* ntp_server2 = "time.nist.gov";

/* Initialize Wi-Fi and the NTP client */
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

info_struct all_info;

unsigned long previous_millis = 0; /* Stores the last time the function was executed */
const long interval = 1000; /* Interval of 1 second (1000 milliseconds) */

// Function to check and maintain Wi-Fi connection
void check_wifi_connection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected, trying to reconnect...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Reconnected to Wi-Fi.");
  }
}

/* Function to update the internal RTC */
void update_RTC() 
{ 
  time_t raw_time = timeClient.getEpochTime(); 
  struct timeval now = { .tv_sec = raw_time }; 
  settimeofday(&now, NULL);
}

/* Function to update the time using NTP */
void print_date_time() 
{
  time_t raw_time = timeClient.getEpochTime();
  struct tm * time_info = localtime(&raw_time);

  char buffer[50];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);
  Serial.println(buffer);
}

/* Function to update the current date and time */
bool update_time() 
{
  if (!timeClient.update()) {
    Serial.println("Trying second NTP server...");
    timeClient.setPoolServerName(ntp_server2);
    if (!timeClient.update()) {
      return false;
    }
  }
  return true;
}

/***********************************************
  Core 0 - Wifi and NTP
 ***********************************************/
void setup() 
{
  /* Initialize serial communication */
  Serial.begin(115200);

  /* Conect to WiFi */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi.");
  delay(2000);

  /* Initialize the NTP client with the first server */
  timeClient.begin();
  timeClient.setPoolServerName(ntp_server1);
  timeClient.setTimeOffset(TIMEZONE_OFFSET); /* Adjust according to the time zone */
  update_time();
}

void loop() 
{
  // Check Wi-Fi connection
  check_wifi_connection();
  /* Update the time */
  if (update_time()) {
    /* Print the current date and time */
    print_date_time();
    update_RTC();
  } else {
    Serial.println("Failed to get time from NTP.");
  }

  delay(60000); /* Wait 60 seconds before the next update */
}

/***********************************************
  Core 1 - Display control
 ***********************************************/
void setup1()
{
  analogReadResolution(12);
  (void)display_init();
}

void loop1()
{
  unsigned long current_millis = millis(); /* Get the current time */
  /* Check if 1 second has passed */
  if (current_millis - previous_millis >= interval) 
  { 
    previous_millis = current_millis; 
    time_t rawtime = timeClient.getEpochTime();
    struct tm* timeinfo = localtime(&rawtime);
    all_info.year = timeinfo->tm_year + 1900;
    all_info.month = timeinfo->tm_mon + 1;
    all_info.day = timeinfo->tm_mday;
    all_info.hour = timeinfo->tm_hour;
    all_info.minute = timeinfo->tm_min;
    all_info.second = timeinfo->tm_sec;
    all_info.day_of_week = timeinfo->tm_wday;

    /* Temperature sensor NTC pins COM_T1 e COM_T2 */
    all_info.temp = read_temperature();
  }
  (void)display_ctrl(all_info);
}