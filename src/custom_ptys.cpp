#include "custom_ptys.h"
#include <vector>
#include <cmath>
#include "logbook.h"

static std::vector<PTYEntry> customPtys;

#define ARTISTS_COUNT 30
#define SONGS_COUNT 25

const char* brazilianArtists[ARTISTS_COUNT] = {
  "Gilberto Gil", "Caetano Veloso", "Tom Jobim", "João Gilberto", "Elis Regina",
  "Gisele Bündchen", "Anitta", "Pabllo Vittar", "Ludmilla", "Maluma",
  "Raimundos", "Legião Urbana", "CPM 22", "Jota Quest", "O Terno",
  "Silva", "Thalles Roberto", "Fernandinho", "Ministério Zoe", "Kemence",
  "Alceu Valença", "Silvio Santos", "Luiz Gonzaga", "Jackson do Pandeiro", "Fagner",
  "Gal Costa", "Ivete Sangalo", "Daniela Mercury", "Olodum", "Timbalada"
};

const char* brazilianSongs[SONGS_COUNT] = {
  "Clandestino", "Saudade da Bahia", "Noite Carioca", "Samba Enredo", "Mulher Brasileira",
  "Festa Tropical", "Beijo de Chuva", "Noites de Copa", "Samba da Praia", "Amor Tropical",
  "Cidade Grande", "Coração do Brasil", "Luz do Luar", "Dança do Ritmo", "Sonho Brasileiro",
  "Toque de Mágica", "Ritmo Perfeito", "Baile da Favela", "Noite de Luar", "Saudade Infinita",
  "Batida do Coração", "Encontro de Almas", "Canção do Povo", "Toque Divino", "Espírito Livre"
};

String generateRandomSong() {
  int artistIdx = random(0, ARTISTS_COUNT);
  int songIdx = random(0, SONGS_COUNT);
  return String(brazilianArtists[artistIdx]) + " - " + String(brazilianSongs[songIdx]);
}

// Função para buscar o PS pelo customPtys
String findCustomPSForFreq(uint32_t freq_khz) {
  String ps = String("");
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      ps = e.ps;
      break;
    }
  }
  if (ps.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        ps = e.ps;
        break;
      }
    }
  }

  String song = findCustomSongForFreq(freq_khz);
  if (ps.length() > 0 && song.length() > 0) {
    ps += " | " + song;
  }
  return ps;
}

String findCustomRTForFreq(uint32_t freq_khz) {
  String rt = String("");
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      rt = e.rt;
      break;
    }
  }
  if (rt.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        rt = e.rt;
        break;
      }
    }
  }

  String song = findCustomSongForFreq(freq_khz);
  if (rt.length() > 0 && song.length() > 0) {
    rt += " - " + song;
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
  e.rt = "A RADIO DA COMUNIDADE";
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
  e.ps = "TEMPO FM 103.9";
  e.rt = "A SUA MELHOR ESTACAO. - O TEMPO TODO COM VOCE";
  e.song = "";
  customPtys.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = "REDE METROPOLITANA FM 104.3";
  e.rt = "METROPOLITANA YES!!!";
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
  e.ps = "RU107.9MHZ";
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

