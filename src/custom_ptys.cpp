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
  e.ps = "RADIO METROPOLITANA FM 79.7MHZ - RADIO A RADIO DA COMUNIDADE";
  e.rt = "RADIO METROPOLITANA FM 79.7MHZ - RADIO A RADIO DA COMUNIDADE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 86700;
  e.pty_code = 10;
  e.ps = "RADIO EDUCATIVA FM 86.7 - RADIO IFCE FM 86.7 - RADIO EDUCACAO E CULTURA NO AR";
  e.rt = "RADIO EDUCATIVA FM 86.7 - RADIO IFCE FM 86.7 - RADIO EDUCACAO E CULTURA NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 87100;
  e.pty_code = 10;
  e.ps = "RADIO CEARA FM 87.1MHZ - RADIO O SOM DO CEARA";
  e.rt = "RADIO CEARA FM 87.1MHZ - RADIO O SOM DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88300;
  e.pty_code = 20;
  e.ps = "RADIO JERUSALEM FM 88.3 - RADIO A VOZ DE DEUS NO AR";
  e.rt = "RADIO JERUSALEM FM 88.3 - RADIO A VOZ DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88900;
  e.pty_code = 10;
  e.ps = "RADIO JANGADEIRO FM 88.9 - RADIO A RADIO QUE TODO MUNDO AMA";
  e.rt = "RADIO JANGADEIRO FM 88.9 - RADIO A RADIO QUE TODO MUNDO AMA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 89900;
  e.pty_code = 10;
  e.ps = "RADIO 89 FM 89.9 FM - RADIO A MAIS TOCADA DE FORTALEZA";
  e.rt = "RADIO 89 FM 89.9 FM - RADIO A MAIS TOCADA DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90700;
  e.pty_code = 10;
  e.ps = "RADIO FORTALEZA FM 90.7 - RADIO A RADIO DE FORTALEZA";
  e.rt = "RADIO FORTALEZA FM 90.7 - RADIO A RADIO DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90300;
  e.pty_code = 20;
  e.ps = "RADIO UIRAPURU FM 90.3 - RADIO REDE ALELUIA FM 90.3 - RADIO REDE ALELUIA FM 90.3 - GLORIA A DEUS";
  e.rt = "RADIO UIRAPURU FM 90.3 - RADIO REDE ALELUIA FM 90.3 - RADIO REDE ALELUIA FM 90.3 - GLORIA A DEUS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 91700;
  e.pty_code = 20;
  e.ps = "RADIO SHALOM FM 91.7MHZ - RADIO COMUNIDADE SHALOM NO AR";
  e.rt = "RADIO SHALOM FM 91.7MHZ - RADIO COMUNIDADE SHALOM NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92100;
  e.pty_code = 20;
  e.ps = "RADIO EFRAIM FM 92.1 - RADIO A RADIO GOSPEL DO CEARA";
  e.rt = "RADIO EFRAIM FM 92.1 - RADIO A RADIO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92500;
  e.pty_code = 10;
  e.ps = "RADIO VERDINHA FM 92.5 - RADIO JORNALISMO QUE FALA A NOSSA LINGUA";
  e.rt = "RADIO VERDINHA FM 92.5 - RADIO JORNALISMO QUE FALA A NOSSA LINGUA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92900;
  e.pty_code = 10;
  e.ps = "RADIO JOVEM PAN NEWS FORTALEZA FM 92.9 - RADIO A RADIO QUE TOCA NOTICIA";
  e.rt = "RADIO JOVEM PAN NEWS FORTALEZA FM 92.9 - RADIO A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93500;
  e.pty_code = 20;
  e.ps = "RADIO CANAA FM 93.5 - RADIO A TERRA PROMETIDA DO RADIO";
  e.rt = "RADIO CANAA FM 93.5 - RADIO A TERRA PROMETIDA DO RADIO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93900;
  e.pty_code = 10;
  e.ps = "RADIO FM 93 SEMPRE AO SEU LADO - RADIO SEMPRE AO SEU LADO";
  e.rt = "RADIO FM 93 SEMPRE AO SEU LADO - RADIO SEMPRE AO SEU LADO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94300;
  e.pty_code = 10;
  e.ps = "RADIO SOL FM 94.3 OFICIAL - RADIO NOSSO BRILHO E VOCE";
  e.rt = "RADIO SOL FM 94.3 OFICIAL - RADIO NOSSO BRILHO E VOCE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94700;
  e.pty_code = 10;
  e.ps = "RADIO JOVEM PAN FORTALEZA FM 94.7 - RADIO A MELHOR RADIO DO BRASIL";
  e.rt = "RADIO JOVEM PAN FORTALEZA FM 94.7 - RADIO A MELHOR RADIO DO BRASIL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95100;
  e.pty_code = 20;
  e.ps = "RADIO LOGOS FM 95.1 - RADIO A PALAVRA DE DEUS NO AR";
  e.rt = "RADIO LOGOS FM 95.1 - RADIO A PALAVRA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95500;
  e.pty_code = 10;
  e.ps = "RADIO CBN O POVO FM 95.5 - RADIO A RADIO QUE TOCA NOTICIA";
  e.rt = "RADIO CBN O POVO FM 95.5 - RADIO A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96100;
  e.pty_code = 20;
  e.ps = "RADIO DOMBOSCO FM 96,1 - RADIO A RADIO SALESIANA DO CEARA";
  e.rt = "RADIO DOMBOSCO FM 96,1 - RADIO A RADIO SALESIANA DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96700;
  e.pty_code = 10;
  e.ps = "RADIO ALECE FM 96.7MHZ - RADIO A RADIO DA ASSEMBLEIA LEGISLATIVA";
  e.rt = "RADIO ALECE FM 96.7MHZ - RADIO A RADIO DA ASSEMBLEIA LEGISLATIVA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97100;
  e.pty_code = 20;
  e.ps = "RADIO MARIA BRASIL FM 97.1 - RADIO AVE MARIA CHEIA DE GRACA";
  e.rt = "RADIO MARIA BRASIL FM 97.1 - RADIO AVE MARIA CHEIA DE GRACA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97700;
  e.pty_code = 10;
  e.ps = "RADIO ANTENA 1 FM 97.7 - RADIO SOM INTERNACIONAL SINTONIA LOCAL";
  e.rt = "RADIO ANTENA 1 FM 97.7 - RADIO SOM INTERNACIONAL SINTONIA LOCAL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 98300;
  e.pty_code = 20;
  e.ps = "RADIO LIDER FM GOSPEL 98.3 - RADIO A LIDER DO GOSPEL DO CEARA";
  e.rt = "RADIO LIDER FM GOSPEL 98.3 - RADIO A LIDER DO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99100;
  e.pty_code = 10;
  e.ps = "RADIO CIDADE FM 99.1 - RADIO A RADIO DA CIDADE DE FORTALEZA";
  e.rt = "RADIO CIDADE FM 99.1 - RADIO A RADIO DA CIDADE DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99900;
  e.pty_code = 20;
  e.ps = "RADIO REDE ALELUIA FM 99.9 - RADIO GLORIA A DEUS NAS ALTURAS";
  e.rt = "RADIO REDE ALELUIA FM 99.9 - RADIO GLORIA A DEUS NAS ALTURAS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 100900;
  e.pty_code = 20;
  e.ps = "RADIO DEUS E AMOR FM 100.9 - RADIO DEUS E AMOR - A RADIO DA CURA";
  e.rt = "RADIO DEUS E AMOR FM 100.9 - RADIO DEUS E AMOR - A RADIO DA CURA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101300;
  e.pty_code = 20;
  e.ps = "RADIO NOVA RADIO CRISTA FM 101.3 - RADIO A NOVA VOZ E RADIO DO EVANGELHO";
  e.rt = "RADIO NOVA RADIO CRISTA FM 101.3 - RADIO A NOVA VOZ E RADIO DO EVANGELHO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101700;
  e.pty_code = 10;
  e.ps = "RADIO BANDNEWS FM 101.7 - RADIO TODA HORA TODA NOTICIA";
  e.rt = "RADIO BANDNEWS FM 101.7 - RADIO TODA HORA TODA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102300;
  e.pty_code = 20;
  e.ps = "RADIO TEMPLO CENTRAL FM 102.3 - RADIO O TEMPLO DE DEUS NO AR";
  e.rt = "RADIO TEMPLO CENTRAL FM 102.3 - RADIO O TEMPLO DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102700;
  e.pty_code = 10;
  e.ps = "RADIO BEACH PARK FM 102.7 - RADIO A CADA MUSICA UMA NOVA DESCOBERTA";
  e.rt = "RADIO BEACH PARK FM 102.7 - RADIO A CADA MUSICA UMA NOVA DESCOBERTA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103300;
  e.pty_code = 10;
  e.ps = "RADIO SENADO FM 103.3 - RADIO O SOM DA DEMOCRACIA";
  e.rt = "RADIO SENADO FM 103.3 - RADIO O SOM DA DEMOCRACIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103500;
  e.pty_code = 20;
  e.ps = "RADIO REDE SHALOM DE RADIO FM 103.5 - RADIO COMUNIDADE SHALOM - PAZ E BEM";
  e.rt = "RADIO REDE SHALOM DE RADIO FM 103.5 - RADIO COMUNIDADE SHALOM - PAZ E BEM";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103900;
  e.pty_code = 10;
  e.ps = "RADIO TEMPO FM 103.9 A SUA MELHOR ESTACAO - RADIO O TEMPO TODO A MELHOR MUSICA";
  e.rt = "RADIO TEMPO FM 103.9 A SUA MELHOR ESTACAO - RADIO O TEMPO TODO A MELHOR MUSICA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = "RADIO REDE METROPOLITANA FM 104.3 - RADIO METROPOLITANA YES FM 104.3 - MUSICA POP - MUSICA SERTANEJA = TOCA TODAS";
  e.rt = "RADIO REDE METROPOLITANA FM 104.3 - RADIO METROPOLITANA YES FM 104.3 = MUSICA POP - MUSICA SERTANEJA = TOCA TODAS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105100;
  e.pty_code = 20;
  e.ps = "RADIO AD CIDADE FM 105.1MHZ - RADIO ASSEMBLEIA DE DEUS NO AR";
  e.rt = "RADIO AD CIDADE FM 105.1MHZ - RADIO ASSEMBLEIA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105700;
  e.pty_code = 10;
  e.ps = "RADIO ATLANTICO SUL FM 105.7 - RADIO SUA VIDA NA MELHOR TRILHA";
  e.rt = "RADIO ATLANTICO SUL FM 105.7 - RADIO SUA VIDA NA MELHOR TRILHA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 106500;
  e.pty_code = 10;
  e.ps = "RADIO NOVABRASIL FM 106.5 - RADIO A MAIOR AUDIENCIA NO PUBLICO ADULTO";
  e.rt = "RADIO NOVABRASIL FM 106.5 - RADIO A MAIOR AUDIENCIA NO PUBLICO ADULTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107500;
  e.pty_code = 10;
  e.ps = "RADIO MIX FM 107.5 - RADIO OS MAIORES HITS DO MOMENTO";
  e.rt = "RADIO MIX FM 107.5 - RADIO OS MAIORES HITS DO MOMENTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107900;
  e.pty_code = 10;
  e.ps = "107.9MHZ RADIO UNIVERSITARIA FM 107.9MHZ - RADIO A RADIO DA UFC - MUSICA POP E SABER";
  e.rt = "107.9MHZ RADIO UNIVERSITARIA FM 107.9MHZ - RADIO A RADIO DA UFC - MUSICA POP E SABER";
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
