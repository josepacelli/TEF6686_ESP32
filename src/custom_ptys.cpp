#include "custom_ptys.h"
#include <SPIFFS.h>
#include <vector>
#include <cmath>
#include "logbook.h"

static std::vector<PTYEntry> customPtys;
static const char * CUSTOM_PTY_PATH = "/custom_ptys.csv";

// Função para buscar o PS pelo customPtys
String findCustomPSForFreq(uint32_t freq_khz) {
  // Busca exata primeiro
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.ps;
  }
  // Busca com tolerância de 100 kHz
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.ps;
  }
  return String("");
}

String findCustomRTForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.rt;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.rt;
  }
  return String("");
}

String findCustomSongForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.song;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.song;
  }
  return String("");
}

void loadIsaacPTYs() {
  // load default PTYs from provided list (MHz -> kHz)
  log_info("Loading default Isaac PTYs");
  customPtys.clear();
  PTYEntry e;
  // Exemplo: Pop Music = 10, Religion = 20 (de acordo com PTY_EU)
  e.freq_khz = 79700;
  e.pty_code = 20;
  e.ps = "RADIO METROPOLITANA 79.7MHZ - A RADIO DA COMUNIDADE";
  e.rt = "RADIO METROPOLITANA 79.7MHZ - A RADIO DA COMUNIDADE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 86700;
  e.pty_code = 10;
  e.ps = "RADIO EDUCATIVA - IFCE FM - EDUCACAO E CULTURA NO AR";
  e.rt = "RADIO EDUCATIVA - IFCE FM - EDUCACAO E CULTURA NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 87100;
  e.pty_code = 10;
  e.ps = "CEARA FM 87.1MHZ - O SOM DO CEARA";
  e.rt = "CEARA FM 87.1MHZ - O SOM DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88300;
  e.pty_code = 20;
  e.ps = "RADIO JERUSALEM FM - A VOZ DE DEUS NO AR";
  e.rt = "RADIO JERUSALEM FM - A VOZ DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88900;
  e.pty_code = 10;
  e.ps = "JANGADEIRO FM - A RADIO QUE TODO MUNDO AMA";
  e.rt = "JANGADEIRO FM - A RADIO QUE TODO MUNDO AMA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 89900;
  e.pty_code = 10;
  e.ps = "89 FM 89.9 FM - A MAIS TOCADA DE FORTALEZA";
  e.rt = "89 FM 89.9 FM - A MAIS TOCADA DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90700;
  e.pty_code = 10;
  e.ps = "FORTALEZA FM - A RADIO DE FORTALEZA";
  e.rt = "FORTALEZA FM - A RADIO DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90300;
  e.pty_code = 20;
  e.ps = "RADIO UIRAPURU - REDE ALELUIA - REDE ALELUIA - GLORIA A DEUS";
  e.rt = "RADIO UIRAPURU - REDE ALELUIA - REDE ALELUIA - GLORIA A DEUS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 91700;
  e.pty_code = 20;
  e.ps = "SHALOM FM 91.7MHZ - COMUNIDADE SHALOM NO AR";
  e.rt = "SHALOM FM 91.7MHZ - COMUNIDADE SHALOM NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92100;
  e.pty_code = 20;
  e.ps = "RADIO EFRAIM - A RADIO GOSPEL DO CEARA";
  e.rt = "RADIO EFRAIM - A RADIO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92500;
  e.pty_code = 10;
  e.ps = "VERDINHA FM 92.5 - JORNALISMO QUE FALA A NOSSA LINGUA";
  e.rt = "VERDINHA FM 92.5 - JORNALISMO QUE FALA A NOSSA LINGUA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92900;
  e.pty_code = 10;
  e.ps = "JOVEM PAN NEWS FORTALEZA - A RADIO QUE TOCA NOTICIA";
  e.rt = "JOVEM PAN NEWS FORTALEZA - A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93500;
  e.pty_code = 20;
  e.ps = "CANAA FM 93.5 - A TERRA PROMETIDA DO RADIO";
  e.rt = "CANAA FM 93.5 - A TERRA PROMETIDA DO RADIO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93900;
  e.pty_code = 10;
  e.ps = "FM 93 SEMPRE AO SEU LADO - SEMPRE AO SEU LADO";
  e.rt = "FM 93 SEMPRE AO SEU LADO - SEMPRE AO SEU LADO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94300;
  e.pty_code = 10;
  e.ps = "SOL FM 94.3 OFICIAL - NOSSO BRILHO E VOCE";
  e.rt = "SOL FM 94.3 OFICIAL - NOSSO BRILHO E VOCE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94700;
  e.pty_code = 10;
  e.ps = "JOVEM PAN FORTALEZA FM 94.7 - A MELHOR RADIO DO BRASIL";
  e.rt = "JOVEM PAN FORTALEZA FM 94.7 - A MELHOR RADIO DO BRASIL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95100;
  e.pty_code = 20;
  e.ps = "LOGOS FM - A PALAVRA DE DEUS NO AR";
  e.rt = "LOGOS FM - A PALAVRA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95500;
  e.pty_code = 10;
  e.ps = "CBN O POVO - A RADIO QUE TOCA NOTICIA";
  e.rt = "CBN O POVO - A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96100;
  e.pty_code = 20;
  e.ps = "DOMBOSCO FM 96,1 - A RADIO SALESIANA DO CEARA";
  e.rt = "DOMBOSCO FM 96,1 - A RADIO SALESIANA DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96700;
  e.pty_code = 10;
  e.ps = "ALECE FM 96.7MHZ - A RADIO DA ASSEMBLEIA LEGISLATIVA";
  e.rt = "ALECE FM 96.7MHZ - A RADIO DA ASSEMBLEIA LEGISLATIVA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97100;
  e.pty_code = 20;
  e.ps = "RADIO MARIA BRASIL - AVE MARIA CHEIA DE GRACA";
  e.rt = "RADIO MARIA BRASIL - AVE MARIA CHEIA DE GRACA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97700;
  e.pty_code = 10;
  e.ps = "ANTENA 1 FM 97.7 - SOM INTERNACIONAL SINTONIA LOCAL";
  e.rt = "ANTENA 1 FM 97.7 - SOM INTERNACIONAL SINTONIA LOCAL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 98300;
  e.pty_code = 20;
  e.ps = "RADIO LIDER FM GOSPEL 98.3 - A LIDER DO GOSPEL DO CEARA";
  e.rt = "RADIO LIDER FM GOSPEL 98.3 - A LIDER DO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99100;
  e.pty_code = 10;
  e.ps = "CIDADE FM 99.1 - A RADIO DA CIDADE DE FORTALEZA";
  e.rt = "CIDADE FM 99.1 - A RADIO DA CIDADE DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99900;
  e.pty_code = 20;
  e.ps = "REDE ALELUIA FM 99.9 - GLORIA A DEUS NAS ALTURAS";
  e.rt = "REDE ALELUIA FM 99.9 - GLORIA A DEUS NAS ALTURAS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 100900;
  e.pty_code = 20;
  e.ps = "DEUS E AMOR FM 100.9 - DEUS E AMOR - A RADIO DA CURA";
  e.rt = "DEUS E AMOR FM 100.9 - DEUS E AMOR - A RADIO DA CURA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101300;
  e.pty_code = 20;
  e.ps = "NOVA RADIO CRISTA - A NOVA VOZ DO EVANGELHO";
  e.rt = "NOVA RADIO CRISTA - A NOVA VOZ DO EVANGELHO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101700;
  e.pty_code = 10;
  e.ps = "BANDNEWS FM 101.7 - TODA HORA TODA NOTICIA";
  e.rt = "BANDNEWS FM 101.7 - TODA HORA TODA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102300;
  e.pty_code = 20;
  e.ps = "TEMPLO CENTRAL FM 102.3 - O TEMPLO DE DEUS NO AR";
  e.rt = "TEMPLO CENTRAL FM 102.3 - O TEMPLO DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102700;
  e.pty_code = 10;
  e.ps = "RADIO BEACH PARK FM 102.7 - A CADA MUSICA UMA NOVA DESCOBERTA";
  e.rt = "RADIO BEACH PARK FM 102.7 - A CADA MUSICA UMA NOVA DESCOBERTA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103300;
  e.pty_code = 10;
  e.ps = "RADIO SENADO - O SOM DA DEMOCRACIA";
  e.rt = "RADIO SENADO - O SOM DA DEMOCRACIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103500;
  e.pty_code = 20;
  e.ps = "REDE SHALOM DE RADIO - COMUNIDADE SHALOM - PAZ E BEM";
  e.rt = "REDE SHALOM DE RADIO - COMUNIDADE SHALOM - PAZ E BEM";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103900;
  e.pty_code = 10;
  e.ps = "TEMPO FM 103.9 A SUA MELHOR ESTACAO. - O TEMPO TODO COM VOCE";
  e.rt = "TEMPO FM 103.9 A SUA MELHOR ESTACAO. - O TEMPO TODO COM VOCE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = "REDE METROPOLITANA FM 104.3 - YES!!!";
  e.rt = "REDE METROPOLITANA FM 104.3 - YES!!!";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105100;
  e.pty_code = 20;
  e.ps = "AD CIDADE FM 105.1MHZ - ASSEMBLEIA DE DEUS NO AR";
  e.rt = "AD CIDADE FM 105.1MHZ - ASSEMBLEIA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105700;
  e.pty_code = 10;
  e.ps = "ATLANTICO SUL FM 105.7 - SUA VIDA NA MELHOR TRILHA";
  e.rt = "ATLANTICO SUL FM 105.7 - SUA VIDA NA MELHOR TRILHA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 106500;
  e.pty_code = 10;
  e.ps = "NOVABRASIL FM 106.5 - A MAIOR AUDIENCIA NO PUBLICO ADULTO";
  e.rt = "NOVABRASIL FM 106.5 - A MAIOR AUDIENCIA NO PUBLICO ADULTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107500;
  e.pty_code = 10;
  e.ps = "MIX FM 107.5 - OS MAIORES HITS DO MOMENTO";
  e.rt = "MIX FM 107.5 - OS MAIORES HITS DO MOMENTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107900;
  e.pty_code = 10;
  e.ps = "107.9MHZ UNIVERSITARIA FM 107.9MHZ - A RADIO DA UFC - CULTURA E SABER";
  e.rt = "107.9MHZ UNIVERSITARIA FM 107.9MHZ - A RADIO DA UFC - CULTURA E SABER";
  e.song = "";
  customPtys.push_back(e);
  log_info("Default Isaac PTYs loaded.");
}

void loadCustomPTYS() {
  customPtys.clear();
  log_info("Iniciando leitura do CSV de PTYs personalizados");
  if (!SPIFFS.exists(CUSTOM_PTY_PATH)) {
    log_info("Arquivo de PTYs personalizados nao existe.");
    loadIsaacPTYs();
    return;
  };
  fs::File f = SPIFFS.open(CUSTOM_PTY_PATH, "r");
  if (!f) {
    log_info("Erro ao abrir o arquivo de PTYs personalizados");
    return;
  }
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    log_info("Read line: " + line + "\n");
    if (line.length() == 0) continue;

    // Parse CSV: freq,ptycode,ps,rt,song
    int comma1 = line.indexOf(',');
    if (comma1 == -1) continue;

    String freqs = line.substring(0, comma1);
    freqs.trim();

    int comma2 = line.indexOf(',', comma1 + 1);
    String ptycodeStr = (comma2 == -1) ? line.substring(comma1 + 1) : line.substring(comma1 + 1, comma2);
    ptycodeStr.trim();

    String ps = "";
    String rt = "";
    String song = "";

    if (comma2 != -1) {
      int comma3 = line.indexOf(',', comma2 + 1);
      ps = (comma3 == -1) ? line.substring(comma2 + 1) : line.substring(comma2 + 1, comma3);
      ps.trim();

      if (comma3 != -1) {
        int comma4 = line.indexOf(',', comma3 + 1);
        rt = (comma4 == -1) ? line.substring(comma3 + 1) : line.substring(comma3 + 1, comma4);
        rt.trim();

        if (comma4 != -1) {
          song = line.substring(comma4 + 1);
          song.trim();
        }
      }
    }

    // accept either kHz integer or MHz with decimal
    uint32_t freq_khz = 0;
    if (freqs.indexOf('.') != -1) {
      // MHz like 102.7 -> kHz *1000 = 102700
      float mhz = freqs.toFloat();
      freq_khz = (uint32_t)round(mhz * 1000.0);
    } else {
      freq_khz = (uint32_t)freqs.toInt();
      // if it looks like MHz without decimal but small, assume MHz
      if (freq_khz > 0 && freq_khz < 2000) freq_khz = freq_khz * 1000;
    }
    uint8_t pty_code = (uint8_t)ptycodeStr.toInt();
    PTYEntry e; e.freq_khz = freq_khz; e.pty_code = pty_code; e.ps = ps; e.rt = rt; e.song = song;
    log_info("PTY: freq_khz=" + String(e.freq_khz) + " pty_code=" + String(e.pty_code) + " ps=" + e.ps + " rt=" + e.rt + " song=" + e.song + "\n");
    customPtys.push_back(e);
  }
  f.close();
}

void saveCustomPTYS() {
  fs::File f = SPIFFS.open(CUSTOM_PTY_PATH, "w");
  if (!f) return;
  for (auto &e : customPtys) {
    // write frequency as MHz with one decimal
    // freq_khz is in kHz (e.g., 102700 = 102.7 MHz)
    String freqStr = String((float)e.freq_khz / 1000.0, 1);
    f.print(freqStr + "," + String(e.pty_code) + "," + e.ps + "," + e.rt + "," + e.song + "\n");
  }
  f.flush();
  f.close();
}

size_t getCustomPTYSCount() {
  return customPtys.size();
}

PTYEntry getCustomPTYEntry(size_t idx) {
  PTYEntry e; e.freq_khz = 0; e.pty_code = 0; e.ps = ""; e.rt = ""; e.song = "";
  if (idx < customPtys.size()) return customPtys[idx];
  return e;
}

int8_t findCustomPTYCodeForFreq(uint32_t freq_khz) {
  // exact match first
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.pty_code;
  }
  // try match within 100 kHz tolerance for rounding differences
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.pty_code;
  }
  return -1;
}

void addCustomPTY(uint32_t freq_khz, uint8_t pty_code, const String &ps, const String &rt, const String &song) {
  PTYEntry e; e.freq_khz = freq_khz; e.pty_code = pty_code; e.ps = ps; e.rt = rt; e.song = song;
  customPtys.push_back(e);
  saveCustomPTYS();
}

void removeCustomPTY(size_t idx) {
  if (idx < customPtys.size()) {
    customPtys.erase(customPtys.begin() + idx);
    saveCustomPTYS();
  }
}
