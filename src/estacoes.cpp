#include "estacoes.h"
#include "conteudo.h"
#include "logbook.h"
#include "ps_language.h"
#include "rt_language.h"
#include "TEF6686.h"
#include <vector>
#include <algorithm>
#include <cmath>

extern TEF6686 radio;

static std::vector<Estacao> estacoes;

static Estacao* findEstacao(uint32_t freq_khz) {
  for (auto &e : estacoes) if (e.freq_khz == freq_khz) return &e;
  for (auto &e : estacoes) if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return &e;
  return nullptr;
}

void avancarScroll(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (e && e->musica.length() > 0)
    e->posScroll = (e->posScroll + 1) % e->musica.length();
}

String buscarMusica(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (!e) return String("");
  if (e->musica.length() == 0) e->musica = generateRandomSong();
  return e->musica;
}

static String montarPS(Estacao* e, uint32_t freq_khz) {
  String result = "[" + String(e->pty_code) + "] " + getPTYName(e->pty_code);
  if (e->ps.length() > 0) result += " | " + e->ps;
  String m = buscarMusica(freq_khz);
  if (m.length() > 0)
    result += " | " + formatSongWithYear(rotateSongString(m, e->posScroll), e->anoMusica);
  if (e->dia.length() > 0) result += " | " + e->dia + "/" + e->mes + "/" + e->ano;
  if (e->hora.length() > 0) {
    result += " " + e->hora + ":" + e->minuto;
    if (e->segundo.length() > 0) result += ":" + e->segundo;
  }
  if (e->tempo.length() > 0) result += " | " + e->tempo + " " + e->temperatura;
  return result;
}

String buscarPS(uint32_t freq_khz) {
  if (radio.rds.stationName[0] != '\0') {
    String result = "[" + String(radio.rds.stationTypeCode) + "] " + getPTYName(radio.rds.stationTypeCode);
    result += " | " + String(radio.rds.stationName);
    String m = buscarMusica(freq_khz);
    if (m.length() > 0) {
      Estacao* e = findEstacao(freq_khz);
      if (e) result += " | " + formatSongWithYear(rotateSongString(m, e->posScroll), e->anoMusica);
    }
    return result;
  }
  Estacao* e = findEstacao(freq_khz);
  return e ? montarPS(e, freq_khz) : String("");
}

String buscarRT(uint32_t freq_khz) {
  if (radio.rds.rtRadio[0] != '\0') {
    String result = "[" + String(radio.rds.stationTypeCode) + "] " + getPTYName(radio.rds.stationTypeCode);
    result += " | " + String(radio.rds.rtRadio);
    String m = buscarMusica(freq_khz);
    if (m.length() > 0) {
      Estacao* e = findEstacao(freq_khz);
      if (e) result += " - " + formatSongWithYear(rotateSongString(m, e->posScroll), e->anoMusica);
    }
    return result;
  }
  Estacao* e = findEstacao(freq_khz);
  if (!e) return String("");
  String result = "[" + String(e->pty_code) + "] " + getPTYName(e->pty_code);
  if (e->rt.length() > 0) result += " | " + e->rt;
  String m = buscarMusica(freq_khz);
  if (m.length() > 0)
    result += " - " + formatSongWithYear(rotateSongString(m, e->posScroll), e->anoMusica);
  if (e->dia.length() > 0) result += " | " + e->dia + "/" + e->mes + "/" + e->ano;
  if (e->hora.length() > 0) {
    result += " " + e->hora + ":" + e->minuto;
    if (e->segundo.length() > 0) result += ":" + e->segundo;
  }
  if (e->tempo.length() > 0) result += " | " + e->tempo + " " + e->temperatura;
  return result;
}

bool isRDSAtivo(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  return e ? e->rds_ativo : true;
}

int8_t buscarPTY(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (!e || !e->rds_ativo) return -1;
  return e->pty_code;
}

String buscarPI(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (!e || !e->rds_ativo || e->pi_code == 0) return String("");
  String s = String(e->pi_code, HEX);
  s.toUpperCase();
  while (s.length() < 4) s = "0" + s;
  return s;
}

String buscarGenero(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  return e ? getRandomGenreByPTY(e->pty_code) : String("");
}

void rotateStationMusic(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (e) {
    e->musica = getSongForPTY(e->pty_code);
    e->anoMusica = getRandomYearByPTY(e->pty_code);
    e->posScroll = 0;
  }
}

void rotateStationWeather(uint32_t freq_khz) {
  Estacao* e = findEstacao(freq_khz);
  if (e) {
    generateRandomTime(e->hora, e->minuto, e->segundo);
    generateRandomDate(e->dia, e->mes, e->ano);
    e->tempo = generateRandomWeather();
    e->temperatura = generateRandomTemperature();
  }
}

size_t totalEstacoes() { return estacoes.size(); }

Estacao& getEstacao(size_t i) { return estacoes[i]; }

void setAllRDS(bool ativo) {
  for (auto& e : estacoes) e.rds_ativo = ativo;
}

void carregarEstacoes() {
  log_info("Carregando estacoes...");
  estacoes.clear();
  Estacao e;
  e.rds_ativo = true;

  // 39 estações em frequência crescente, com índices 0-38 para arrays de idiomas

#define STATION(freq, pty, idx) \
  e.freq_khz = freq; e.pty_code = pty; \
  e.ps = getPSByLanguage(idx, currentPTYLanguage); \
  e.rt = getRTByLanguage(idx, currentPTYLanguage); \
  e.musica = getSongForPTY(pty); e.anoMusica = getRandomYearByPTY(pty); \
  generateRandomTime(e.hora, e.minuto, e.segundo); \
  generateRandomDate(e.dia, e.mes, e.ano); \
  e.tempo = generateRandomWeather(); e.temperatura = generateRandomTemperature(); \
  estacoes.push_back(e);

  STATION(79700, 20, 0);
  STATION(86700, 10, 1);
  STATION(87100, 10, 2);
  STATION(88300, 20, 3);
  STATION(88900, 10, 4);
  STATION(89900, 10, 5);
  STATION(90700, 10, 6);
  STATION(90300, 20, 7);
  STATION(91700, 20, 8);
  STATION(92100, 20, 9);
  STATION(92500, 10, 10);
  STATION(92900, 10, 11);
  STATION(93500, 20, 12);
  STATION(93900, 10, 13);
  STATION(94300, 10, 14);
  STATION(94700, 10, 15);
  STATION(95100, 20, 16);
  STATION(95500, 10, 17);
  STATION(96100, 20, 18);
  STATION(96700, 10, 19);
  STATION(97100, 20, 20);
  STATION(97700, 10, 21);
  STATION(98300, 20, 22);
  STATION(99100, 10, 23);
  STATION(99900, 20, 24);
  STATION(100900, 20, 25);
  STATION(101300, 20, 26);
  STATION(101700, 10, 27);
  STATION(102300, 20, 28);
  STATION(102700, 10, 29);
  STATION(103300, 10, 30);
  STATION(103500, 20, 31);
  STATION(103900, 10, 32);
  STATION(104300, 10, 33);
  STATION(105100, 20, 34);
  STATION(105700, 10, 35);
  STATION(106500, 10, 36);
  STATION(107500, 10, 37);
  STATION(107900, 10, 38);

#undef STATION

  std::sort(estacoes.begin(), estacoes.end(), [](const Estacao& a, const Estacao& b) {
    return a.freq_khz < b.freq_khz;
  });

  log_info("Estacoes carregadas.");
}
