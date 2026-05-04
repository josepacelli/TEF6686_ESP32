#include "weather.h"
#include "custom_ptys.h"
#include <WiFi.h>
#include <WiFiClient.h>

const char* WEATHER_HOST = "api.open-meteo.com";
const char* WEATHER_PATH = "/v1/forecast?latitude=-3.73&longitude=-38.53&current=temperature_2m,weather_code,relative_humidity_2m&timezone=America/Fortaleza";

const char* getWeatherDesc(int code) {
  switch (code) {
    case 0: return "Céu Limpo";
    case 1:
    case 2: return "Pouco Nublado";
    case 3: return "Nublado";
    case 45:
    case 48: return "Nevoeiro";
    case 51:
    case 53:
    case 55: return "Chuvisco";
    case 61:
    case 63:
    case 65: return "Chuva";
    case 71:
    case 73:
    case 75: return "Neve";
    case 77: return "Flocos Neve";
    case 80:
    case 81:
    case 82: return "Chuva Forte";
    case 85:
    case 86: return "Chuva Neve";
    case 95:
    case 96:
    case 99: return "Tempestade";
    default: return "Desconhecido";
  }
}

void updateWeatherOnRDS() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  WiFiClient client;
  if (!client.connect(WEATHER_HOST, 80)) {
    return;
  }

  String request = String("GET ") + WEATHER_PATH + " HTTP/1.1\r\n";
  request += "Host: " + String(WEATHER_HOST) + "\r\n";
  request += "Connection: close\r\n\r\n";

  client.print(request);

  // Wait for response
  unsigned long timeout = millis() + 5000;
  while (client.connected() && millis() < timeout) {
    delay(1);
  }

  String response = "";
  while (client.available()) {
    response += (char)client.read();
  }
  client.stop();

  // Find JSON start
  int jsonStart = response.indexOf('{');
  if (jsonStart == -1) return;

  String jsonStr = response.substring(jsonStart);

  // Parse temperature_2m
  int tempIdx = jsonStr.indexOf("\"temperature_2m\":");
  if (tempIdx == -1) return;
  int tempEnd = jsonStr.indexOf(',', tempIdx);
  String tempStr = jsonStr.substring(tempIdx + 17, tempEnd);
  float temp = tempStr.toFloat();

  // Parse weather_code
  int codeIdx = jsonStr.indexOf("\"weather_code\":");
  if (codeIdx == -1) return;
  int codeEnd = jsonStr.indexOf(',', codeIdx);
  String codeStr = jsonStr.substring(codeIdx + 15, codeEnd);
  int wmo = codeStr.toInt();

  // Parse relative_humidity_2m
  int humIdx = jsonStr.indexOf("\"relative_humidity_2m\":");
  if (humIdx == -1) return;
  int humEnd = jsonStr.indexOf(',', humIdx);
  if (humEnd == -1) humEnd = jsonStr.indexOf('}', humIdx);
  String humStr = jsonStr.substring(humIdx + 23, humEnd);
  int humidity = humStr.toInt();

  String weatherDesc = getWeatherDesc(wmo);

  // Format: "25°C | Céu Limpo" (max 8 chars for PS in RDS)
  char psText[32];
  snprintf(psText, sizeof(psText), "%.0f°C %s", temp, weatherDesc);

  String rtText = String(psText) + " | Umid:" + String(humidity) + "%";

  // Get current frequency
  extern uint32_t frequency;
  extern byte band;
  extern uint32_t frequency_OIRT;

  uint32_t currentFreqKhz = 0;
  if (band == 1) { // BAND_FM
    currentFreqKhz = frequency * 10;
  } else if (band == 0) { // BAND_OIRT
    currentFreqKhz = frequency_OIRT * 10;
  }

  if (currentFreqKhz > 0) {
    addCustomPTY(currentFreqKhz, 0, String(psText), rtText);
  }
}
