#include "custom_ptys.h"
#include <vector>
#include <cmath>
#include "logbook.h"

static std::vector<PTYEntry> customPtys;

const char* brazilianArtists[] = {
  "Caetano Veloso", "Gilberto Gil", "Tom Jobim", "João Gilberto",
  "Gal Costa", "Anitta", "Ivete Sangalo", "Pabllo Vittar",
  "Ludmilla", "Pocah", "Jorge Aragão", "Beth Carvalho",
  "Alcione", "Cartola", "Nelson Cavaquinho", "Elza Soares",
  "Marisa Monte", "Adriana Calcanhotto", "Paralamas do Sucesso", "Legião Urbana",
  "Titãs", "Capital Inicial", "CPM 22", "NX Zero",
  "Sepultura", "Barão Vermelho", "Os Mutantes", "Os Originais do Samba"
};
const int ARTISTS_COUNT = sizeof(brazilianArtists) / sizeof(brazilianArtists[0]);

const char* brazilianSongs[] = {
  "Aquele Abraço", "Toda Menina Baiana", "Garota de Ipanema", "Águas de Março",
  "Carcará", "Saudade", "O Canto da Cidade", "Festa", "Levada Brasileira",
  "Samba Enredo", "Bateria Certeira", "Sou Samba", "Vou Ficar",
  "Alma Brasileira", "Ritmo Perfeito", "Noite de Luar", "Som do Brasil",
  "Ritmo Que Toca", "Melodia Tropical", "Batida Certa", "Samba do Brasil",
  "Toque Autêntico", "Brasilidade", "Pura Energia", "Cadência Brasileira",
  "Força da Tradição", "Raízes do Samba", "Grito da Alma"
};
const int SONGS_COUNT = sizeof(brazilianSongs) / sizeof(brazilianSongs[0]);


String generateRandomSong() {
  int artistIdx = random(0, ARTISTS_COUNT);
  int songIdx = random(0, SONGS_COUNT);
  return String(brazilianArtists[artistIdx]) + " - " + String(brazilianSongs[songIdx]);
}

String rotateSongString(const String& song, int scrollPos) {
  if (song.length() == 0) return song;
  int pos = scrollPos % song.length();
  return song.substring(pos) + song.substring(0, pos);
}

void advanceSongScrollPos(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      if (e.song.length() > 0) {
        e.songScrollPos = (e.songScrollPos + 1) % e.song.length();
      }
      return;
    }
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
      if (e.song.length() > 0) {
        e.songScrollPos = (e.songScrollPos + 1) % e.song.length();
      }
      return;
    }
  }
}

// Função para buscar o PS pelo customPtys
String findCustomPSForFreq(uint32_t freq_khz) {
  String ps = String("");
  int scrollPos = 0;
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      ps = e.ps;
      scrollPos = e.songScrollPos;
      break;
    }
  }
  if (ps.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        ps = e.ps;
        scrollPos = e.songScrollPos;
        break;
      }
    }
  }

  String song = findCustomSongForFreq(freq_khz);
  if (ps.length() > 0 && song.length() > 0) {
    String rotatedSong = rotateSongString(song, scrollPos);
    ps += " | " + rotatedSong;
  }
  return ps;
}

String findCustomRTForFreq(uint32_t freq_khz) {
  String rt = String("");
  int scrollPos = 0;
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      rt = e.rt;
      scrollPos = e.songScrollPos;
      break;
    }
  }
  if (rt.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        rt = e.rt;
        scrollPos = e.songScrollPos;
        break;
      }
    }
  }

  String song = findCustomSongForFreq(freq_khz);
  if (rt.length() > 0 && song.length() > 0) {
    String rotatedSong = rotateSongString(song, scrollPos);
    rt += " - " + rotatedSong;
  }
  return rt;
}

void loadIsaacPTYs() {
  // load default PTYs from provided list (MHz -> kHz)
  log_info("Loading default Isaac PTYs");
  customPtys.clear();
  PTYEntry e;
  // Exemplo: Pop Music = 10, Religion = 20 (de acordo com PTY_EU)
  e.freq_khz = 79700;
  e.pty_code = 20;
  e.ps = "RADIO METROPOLITANA 79.7MHZ";
  e.rt = "A RADIO DA CONUNIDADE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 86700;
  e.pty_code = 10;
  e.ps = "RADIO EDUCATIVA - IFCE FM";
  e.rt = "EDUCACAO E CULTURA NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 87100;
  e.pty_code = 10;
  e.ps = "CEARA FM 87.1MHZ";
  e.rt = "O SOM DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88300;
  e.pty_code = 20;
  e.ps = "RADIO JERUSALEM FM";
  e.rt = "A VOZ DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 88900;
  e.pty_code = 10;
  e.ps = "JANGADEIRO FM";
  e.rt = "A RADIO QUE TODO MUNDO AMA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 89900;
  e.pty_code = 10;
  e.ps = "89 FM 89.9 FM";
  e.rt = "A MAIS TOCADA DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90700;
  e.pty_code = 10;
  e.ps = "FORTALEZA FM";
  e.rt = "A RADIO DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 90300;
  e.pty_code = 20;
  e.ps = "RADIO UIRAPURU - REDE ALELUIA";
  e.rt = "GLORIA A DEUS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 91700;
  e.pty_code = 20;
  e.ps = "SHALOM FM 91.7MHZ";
  e.rt = "COMUNIDADE SHALOM NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92100;
  e.pty_code = 20;
  e.ps = "RADIO EFRAIM";
  e.rt = "A RADIO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92500;
  e.pty_code = 10;
  e.ps = "VERDINHA FM 92.5";
  e.rt = "JORNALISMO QUE FALA A NOSSA LINGUA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 92900;
  e.pty_code = 10;
  e.ps = "JOVEM PAN NEWS FORTALEZA";
  e.rt = "A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93500;
  e.pty_code = 20;
  e.ps = "CANAA FM 93.5";
  e.rt = "A TERRA PROMETIDA DO RADIO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 93900;
  e.pty_code = 10;
  e.ps = "FM 93 SEMPRE AO SEU LADO";
  e.rt = "SEMPRE AO SEU LADO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94300;
  e.pty_code = 10;
  e.ps = "SOL FM 94.3 OFICIAL";
  e.rt = "NOSSO BRILHO E VOCE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 94700;
  e.pty_code = 10;
  e.ps = "JOVEM PAN FORTALEZA FM 94.7";
  e.rt = "A MELHOR RADIO DO BRASIL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95100;
  e.pty_code = 20;
  e.ps = "LOGOS FM";
  e.rt = "A PALAVRA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 95500;
  e.pty_code = 10;
  e.ps = "CBN O POVO";
  e.rt = "A RADIO QUE TOCA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96100;
  e.pty_code = 20;
  e.ps = "DOMBOSCO FM 96,1";
  e.rt = "A RADIO SALESIANA DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 96700;
  e.pty_code = 10;
  e.ps = "ALECE FM 96.7MHZ";
  e.rt = "A RADIO DA ASSEMBLEIA LEGISLATIVA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97100;
  e.pty_code = 20;
  e.ps = "RADIO MARIA BRASIL";
  e.rt = "AVE MARIA CHEIA DE GRACA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 97700;
  e.pty_code = 10;
  e.ps = "ANTENA 1 FM 97.7";
  e.rt = "SOM INTERNACIONAL SINTONIA LOCAL";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 98300;
  e.pty_code = 20;
  e.ps = "RADIO LIDER FM GOSPEL 98.3";
  e.rt = "A LIDER DO GOSPEL DO CEARA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99100;
  e.pty_code = 10;
  e.ps = "CIDADE FM 99.1";
  e.rt = "A RADIO DA CIDADE DE FORTALEZA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 99900;
  e.pty_code = 20;
  e.ps = "REDE ALELUIA FM 99.9";
  e.rt = "GLORIA A DEUS NAS ALTURAS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 100900;
  e.pty_code = 20;
  e.ps = "DEUS E AMOR FM 100.9";
  e.rt = "DEUS E AMOR - A RADIO DA CURA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101300;
  e.pty_code = 20;
  e.ps = "NOVA RADIO CRISTA";
  e.rt = "A NOVA VOZ DO EVANGELHO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 101700;
  e.pty_code = 10;
  e.ps = "BANDNEWS FM 101.7";
  e.rt = "TODA HORA TODA NOTICIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102300;
  e.pty_code = 20;
  e.ps = "TEMPLO CENTRAL FM 102.3";
  e.rt = "O TEMPLO DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 102700;
  e.pty_code = 10;
  e.ps = "RADIO BEACH PARK FM 102.7";
  e.rt = "A CADA MUSICA UMA NOVA DESCOBERTA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103300;
  e.pty_code = 10;
  e.ps = "RADIO SENADO";
  e.rt = "O SOM DA DEMOCRACIA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103500;
  e.pty_code = 20;
  e.ps = "REDE SHALOM DE RADIO";
  e.rt = "COMUNIDADE SHALOM - PAZ E BEM";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 103900;
  e.pty_code = 10;
  e.ps = "TEMPO FM 103.9 A SUA MELHOR ESTACAO";
  e.rt = "O TEMPO TODO A MELHOR MUSICA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = "REDE METROPOLITANA FM 104.3";
  e.rt = "POP - SERTANEJA = TOCA TODAS";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105100;
  e.pty_code = 20;
  e.ps = "AD CIDADE FM 105.1MHZ";
  e.rt = "ASSEMBLEIA DE DEUS NO AR";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 105700;
  e.pty_code = 10;
  e.ps = "ATLANTICO SUL FM 105.7";
  e.rt = "SUA VIDA NA MELHOR TRILHA";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 106500;
  e.pty_code = 10;
  e.ps = "NOVABRASIL FM 106.5";
  e.rt = "A MAIOR AUDIENCIA NO PUBLICO ADULTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107500;
  e.pty_code = 10;
  e.ps = "MIX FM 107.5";
  e.rt = "OS MAIORES HITS DO MOMENTO";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 107900;
  e.pty_code = 10;
  e.ps = "107.9MHZ UNIVERSITARIA FM 107.9MHZ";
  e.rt = "A RADIO DA UFC - CULTURA E SABER";
  e.song = "";
  customPtys.push_back(e);
  log_info("Default Isaac PTYs loaded.");
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

String findCustomSongForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      if (e.song.length() == 0) {
        e.song = generateRandomSong();
      }
      return e.song;
    }
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
      if (e.song.length() == 0) {
        e.song = generateRandomSong();
      }
      return e.song;
    }
  }
  return String("");
}

String findCustomTimeForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.hour + ":" + e.minute;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.hour + ":" + e.minute;
  }
  return String("");
}

String findCustomDateForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.day + "/" + e.month + "/" + e.year;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.day + "/" + e.month + "/" + e.year;
  }
  return String("");
}

