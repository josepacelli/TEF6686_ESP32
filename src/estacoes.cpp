#include "estacoes.h"
#include "conteudo.h"
#include "logbook.h"
#include "ps_language.h"
#include "rt_language.h"
#include <vector>
#include <algorithm>
#include <cmath>

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
  Estacao* e = findEstacao(freq_khz);
  return e ? montarPS(e, freq_khz) : String("");
}

String buscarRT(uint32_t freq_khz) {
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

  // === FORTALEZA FM STATIONS - SORTED BY FREQUENCY ===

  e.freq_khz = 79700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(0, currentPTYLanguage);
  e.rt = getRTByLanguage(0, currentPTYLanguage);
  e.musica = "Pop | Anitta - Envolver";
  e.anoMusica = "2021";
  e.hora = "08"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 86700;
  e.pty_code = 4;
  e.ps = getPSByLanguage(1, currentPTYLanguage);
  e.rt = getRTByLanguage(1, currentPTYLanguage);
  e.musica = "Educativo | Programa de Radio - Cultura e Informacao";
  e.anoMusica = "2026";
  e.hora = "08"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 87100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(2, currentPTYLanguage);
  e.rt = getRTByLanguage(2, currentPTYLanguage);
  e.musica = "Pop | Fortaleza FM - O Som do Ceara";
  e.anoMusica = "2024";
  e.hora = "09"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "29°C";
  estacoes.push_back(e);

  e.freq_khz = 87300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(61, currentPTYLanguage);
  e.rt = getRTByLanguage(61, currentPTYLanguage);
  e.musica = "Pop | Anitta - Vai Malandra";
  e.anoMusica = "2017";
  e.hora = "08"; e.minuto = "02"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "27°C";
  estacoes.push_back(e);

  e.freq_khz = 87500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(41, currentPTYLanguage);
  e.rt = getRTByLanguage(41, currentPTYLanguage);
  e.musica = "Pop | Anitta - Envolver";
  e.anoMusica = "2021";
  e.hora = "08"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 87700;
  e.pty_code = 20;
  e.ps = getPSByLanguage(62, currentPTYLanguage);
  e.rt = getRTByLanguage(62, currentPTYLanguage);
  e.musica = "Gospel | Ministerio Ipiranga - Tu es Fiel";
  e.anoMusica = "2019";
  e.hora = "08"; e.minuto = "08"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "27°C";
  estacoes.push_back(e);

  e.freq_khz = 87900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(42, currentPTYLanguage);
  e.rt = getRTByLanguage(42, currentPTYLanguage);
  e.musica = "Gospel | Novo Tempo - Segura na Mao de Deus";
  e.anoMusica = "2019";
  e.hora = "08"; e.minuto = "10"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 88100;
  e.pty_code = 25;
  e.ps = getPSByLanguage(43, currentPTYLanguage);
  e.rt = getRTByLanguage(43, currentPTYLanguage);
  e.musica = "Forro | Luiz Gonzaga - Asa Branca";
  e.anoMusica = "1947";
  e.hora = "08"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 88300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(3, currentPTYLanguage);
  e.rt = getRTByLanguage(3, currentPTYLanguage);
  e.musica = "Gospel | Eyshila - Voz do Evangelho";
  e.anoMusica = "2018";
  e.hora = "08"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "29°C";
  estacoes.push_back(e);

  e.freq_khz = 88500;
  e.pty_code = 1;
  e.ps = getPSByLanguage(63, currentPTYLanguage);
  e.rt = getRTByLanguage(63, currentPTYLanguage);
  e.musica = "Noticias | Panorama - Debate Politico Cearense";
  e.anoMusica = "2026";
  e.hora = "08"; e.minuto = "25"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "28°C";
  estacoes.push_back(e);

  e.freq_khz = 88900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(4, currentPTYLanguage);
  e.rt = getRTByLanguage(4, currentPTYLanguage);
  e.musica = "Sertanejo | Gusttavo Lima - Balada";
  e.anoMusica = "2014";
  e.hora = "08"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 89100;
  e.pty_code = 1;
  e.ps = getPSByLanguage(44, currentPTYLanguage);
  e.rt = getRTByLanguage(44, currentPTYLanguage);
  e.musica = "Noticias | Record News - Edicao da Tarde";
  e.anoMusica = "2026";
  e.hora = "09"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 89500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(45, currentPTYLanguage);
  e.rt = getRTByLanguage(45, currentPTYLanguage);
  e.musica = "Gospel | Grupo Logos - Maranata";
  e.anoMusica = "2018";
  e.hora = "09"; e.minuto = "10"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 89900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(5, currentPTYLanguage);
  e.rt = getRTByLanguage(5, currentPTYLanguage);
  e.musica = "Pop | Anitta - Show das Poderosas";
  e.anoMusica = "2013";
  e.hora = "09"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 90100;
  e.pty_code = 12;
  e.ps = getPSByLanguage(64, currentPTYLanguage);
  e.rt = getRTByLanguage(64, currentPTYLanguage);
  e.musica = "MPB | Djavan - Se";
  e.anoMusica = "1987";
  e.hora = "08"; e.minuto = "55"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "29°C";
  estacoes.push_back(e);

  e.freq_khz = 90300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(6, currentPTYLanguage);
  e.rt = getRTByLanguage(6, currentPTYLanguage);
  e.musica = "Pop | Ivete Sangalo - Festa";
  e.anoMusica = "2006";
  e.hora = "09"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 90500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(65, currentPTYLanguage);
  e.rt = getRTByLanguage(65, currentPTYLanguage);
  e.musica = "Pop | Ivete Sangalo - A Lua Que Eu Te Dei";
  e.anoMusica = "2008";
  e.hora = "09"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 90700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(7, currentPTYLanguage);
  e.rt = getRTByLanguage(7, currentPTYLanguage);
  e.musica = "Pop | Fortaleza FM - A Mais Tocada";
  e.anoMusica = "2024";
  e.hora = "09"; e.minuto = "10"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 91100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(46, currentPTYLanguage);
  e.rt = getRTByLanguage(46, currentPTYLanguage);
  e.musica = "Pop | Anitta - Envolver";
  e.anoMusica = "2020";
  e.hora = "09"; e.minuto = "25"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 91300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(8, currentPTYLanguage);
  e.rt = getRTByLanguage(8, currentPTYLanguage);
  e.musica = "Pop | Marisa Monte - Amor I Love You";
  e.anoMusica = "1995";
  e.hora = "09"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 91500;
  e.pty_code = 6;
  e.ps = getPSByLanguage(66, currentPTYLanguage);
  e.rt = getRTByLanguage(66, currentPTYLanguage);
  e.musica = "Cultura | Programa Cultural - Arte Cearense";
  e.anoMusica = "2026";
  e.hora = "09"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 91700;
  e.pty_code = 20;
  e.ps = getPSByLanguage(9, currentPTYLanguage);
  e.rt = getRTByLanguage(9, currentPTYLanguage);
  e.musica = "Gospel | Comunidade Shalom - Ave Maria";
  e.anoMusica = "2019";
  e.hora = "09"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 91900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(67, currentPTYLanguage);
  e.rt = getRTByLanguage(67, currentPTYLanguage);
  e.musica = "Gospel | Eyshila - Identidade";
  e.anoMusica = "2016";
  e.hora = "09"; e.minuto = "35"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 92100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(10, currentPTYLanguage);
  e.rt = getRTByLanguage(10, currentPTYLanguage);
  e.musica = "Gospel | Midian Lima - Paz e Bem";
  e.anoMusica = "2020";
  e.hora = "09"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 92300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(68, currentPTYLanguage);
  e.rt = getRTByLanguage(68, currentPTYLanguage);
  e.musica = "Dance | Alok - On & On";
  e.anoMusica = "2018";
  e.hora = "09"; e.minuto = "48"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 92500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(11, currentPTYLanguage);
  e.rt = getRTByLanguage(11, currentPTYLanguage);
  e.musica = "Pop | Anitta - Envolver";
  e.anoMusica = "2021";
  e.hora = "10"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 92700;
  e.pty_code = 1;
  e.ps = getPSByLanguage(69, currentPTYLanguage);
  e.rt = getRTByLanguage(69, currentPTYLanguage);
  e.musica = "Noticias | Verdes Mares - Jornal da Manha";
  e.anoMusica = "2026";
  e.hora = "10"; e.minuto = "02"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 92900;
  e.pty_code = 1;
  e.ps = getPSByLanguage(12, currentPTYLanguage);
  e.rt = getRTByLanguage(12, currentPTYLanguage);
  e.musica = "Noticias | Jornal - Atualidades";
  e.anoMusica = "2026";
  e.hora = "10"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 93100;
  e.pty_code = 8;
  e.ps = getPSByLanguage(47, currentPTYLanguage);
  e.rt = getRTByLanguage(47, currentPTYLanguage);
  e.musica = "Talk | Radio Globo - Programa de Variedades";
  e.anoMusica = "2026";
  e.hora = "10"; e.minuto = "40"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 93300;
  e.pty_code = 1;
  e.ps = getPSByLanguage(70, currentPTYLanguage);
  e.rt = getRTByLanguage(70, currentPTYLanguage);
  e.musica = "Noticias | Pajucara - Jornalismo Nordestino";
  e.anoMusica = "2026";
  e.hora = "10"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 93500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(13, currentPTYLanguage);
  e.rt = getRTByLanguage(13, currentPTYLanguage);
  e.musica = "Gospel | Kemilly Santos - Tua Graca";
  e.anoMusica = "2022";
  e.hora = "10"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 93700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(71, currentPTYLanguage);
  e.rt = getRTByLanguage(71, currentPTYLanguage);
  e.musica = "Dance | Alok - Piece of Me";
  e.anoMusica = "2020";
  e.hora = "10"; e.minuto = "50"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 93900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(14, currentPTYLanguage);
  e.rt = getRTByLanguage(14, currentPTYLanguage);
  e.musica = "Pop | Luisa Sonza - Cachorrina";
  e.anoMusica = "2022";
  e.hora = "10"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 94100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(48, currentPTYLanguage);
  e.rt = getRTByLanguage(48, currentPTYLanguage);
  e.musica = "Gospel | Rede Vida - Louvor e Adoracao";
  e.anoMusica = "2022";
  e.hora = "11"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 94300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(15, currentPTYLanguage);
  e.rt = getRTByLanguage(15, currentPTYLanguage);
  e.musica = "Pop | Claudia Leitte - Largadinho";
  e.anoMusica = "2013";
  e.hora = "11"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 94700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(16, currentPTYLanguage);
  e.rt = getRTByLanguage(16, currentPTYLanguage);
  e.musica = "Pop | Alok - Never Let Me Go";
  e.anoMusica = "2020";
  e.hora = "11"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 95100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(17, currentPTYLanguage);
  e.rt = getRTByLanguage(17, currentPTYLanguage);
  e.musica = "Gospel | Fernanda Brum - Eternamente";
  e.anoMusica = "2019";
  e.hora = "11"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 95300;
  e.pty_code = 6;
  e.ps = getPSByLanguage(49, currentPTYLanguage);
  e.rt = getRTByLanguage(49, currentPTYLanguage);
  e.musica = "Cultura | Programa Cultural - Arte Cearense";
  e.anoMusica = "2026";
  e.hora = "11"; e.minuto = "38"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 95500;
  e.pty_code = 1;
  e.ps = getPSByLanguage(18, currentPTYLanguage);
  e.rt = getRTByLanguage(18, currentPTYLanguage);
  e.musica = "Noticias | CBN - Jornal da Manha";
  e.anoMusica = "2026";
  e.hora = "11"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 96100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(19, currentPTYLanguage);
  e.rt = getRTByLanguage(19, currentPTYLanguage);
  e.musica = "Gospel | Padre Zezinho - Presenca de Deus";
  e.anoMusica = "2010";
  e.hora = "12"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 96300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(50, currentPTYLanguage);
  e.rt = getRTByLanguage(50, currentPTYLanguage);
  e.musica = "Gospel | Thalles Roberto - Sou Livre";
  e.anoMusica = "2021";
  e.hora = "12"; e.minuto = "08"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 96500;
  e.pty_code = 8;
  e.ps = getPSByLanguage(72, currentPTYLanguage);
  e.rt = getRTByLanguage(72, currentPTYLanguage);
  e.musica = "Variedades | Programa Comunitario - Sociedade em Pauta";
  e.anoMusica = "2026";
  e.hora = "12"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 96700;
  e.pty_code = 2;
  e.ps = getPSByLanguage(20, currentPTYLanguage);
  e.rt = getRTByLanguage(20, currentPTYLanguage);
  e.musica = "Informativo | Assembleia Legislativa - Cidadania";
  e.anoMusica = "2026";
  e.hora = "12"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 96900;
  e.pty_code = 1;
  e.ps = getPSByLanguage(73, currentPTYLanguage);
  e.rt = getRTByLanguage(73, currentPTYLanguage);
  e.musica = "Noticias | Jornal FM - Edicao da Tarde";
  e.anoMusica = "2026";
  e.hora = "12"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 97100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(21, currentPTYLanguage);
  e.rt = getRTByLanguage(21, currentPTYLanguage);
  e.musica = "Gospel | Radio Maria - Terco e Oracao";
  e.anoMusica = "2024";
  e.hora = "12"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 97300;
  e.pty_code = 8;
  e.ps = getPSByLanguage(51, currentPTYLanguage);
  e.rt = getRTByLanguage(51, currentPTYLanguage);
  e.musica = "Talk | Radio Tupi - Programa de Variedades";
  e.anoMusica = "2026";
  e.hora = "12"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 97500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(74, currentPTYLanguage);
  e.rt = getRTByLanguage(74, currentPTYLanguage);
  e.musica = "Pop | Henrique e Juliano - Liberdade Provisoria";
  e.anoMusica = "2019";
  e.hora = "12"; e.minuto = "55"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 97700;
  e.pty_code = 12;
  e.ps = getPSByLanguage(22, currentPTYLanguage);
  e.rt = getRTByLanguage(22, currentPTYLanguage);
  e.musica = "Easy Listening | Tom Jobim - Garota de Ipanema";
  e.anoMusica = "1962";
  e.hora = "13"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 97900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(75, currentPTYLanguage);
  e.rt = getRTByLanguage(75, currentPTYLanguage);
  e.musica = "Pop | Ludmilla - Verdinha";
  e.anoMusica = "2020";
  e.hora = "13"; e.minuto = "08"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 98100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(76, currentPTYLanguage);
  e.rt = getRTByLanguage(76, currentPTYLanguage);
  e.musica = "International | Ed Sheeran - Shape of You";
  e.anoMusica = "2017";
  e.hora = "13"; e.minuto = "18"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 98300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(23, currentPTYLanguage);
  e.rt = getRTByLanguage(23, currentPTYLanguage);
  e.musica = "Gospel | Thalles Roberto - Lugar Secreto";
  e.anoMusica = "2021";
  e.hora = "13"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 98500;
  e.pty_code = 25;
  e.ps = getPSByLanguage(77, currentPTYLanguage);
  e.rt = getRTByLanguage(77, currentPTYLanguage);
  e.musica = "Forro | Xand Aviao - Nordeste Bom";
  e.anoMusica = "2017";
  e.hora = "13"; e.minuto = "28"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 98700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(52, currentPTYLanguage);
  e.rt = getRTByLanguage(52, currentPTYLanguage);
  e.musica = "Pop | Hits do Momento";
  e.anoMusica = "2018";
  e.hora = "13"; e.minuto = "22"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 98900;
  e.pty_code = 26;
  e.ps = getPSByLanguage(78, currentPTYLanguage);
  e.rt = getRTByLanguage(78, currentPTYLanguage);
  e.musica = "Sertanejo | Jorge e Mateus - Sossega";
  e.anoMusica = "2016";
  e.hora = "13"; e.minuto = "38"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 99100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(24, currentPTYLanguage);
  e.rt = getRTByLanguage(24, currentPTYLanguage);
  e.musica = "Pop | Gusttavo Lima - Apaixonado por Voce";
  e.anoMusica = "2019";
  e.hora = "13"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 99300;
  e.pty_code = 13;
  e.ps = getPSByLanguage(79, currentPTYLanguage);
  e.rt = getRTByLanguage(79, currentPTYLanguage);
  e.musica = "Classico | Tom Jobim - Aguas de Marco";
  e.anoMusica = "1972";
  e.hora = "14"; e.minuto = "05"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 99500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(25, currentPTYLanguage);
  e.rt = getRTByLanguage(25, currentPTYLanguage);
  e.musica = "Pop | Hits do Momento";
  e.anoMusica = "2026";
  e.hora = "13"; e.minuto = "50"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 99700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(80, currentPTYLanguage);
  e.rt = getRTByLanguage(80, currentPTYLanguage);
  e.musica = "Pop | Wesley Safadao - Camarote";
  e.anoMusica = "2015";
  e.hora = "14"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 99900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(26, currentPTYLanguage);
  e.rt = getRTByLanguage(26, currentPTYLanguage);
  e.musica = "Gospel | Ministério Ipiranga - Com Licenca";
  e.anoMusica = "2023";
  e.hora = "14"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 100100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(53, currentPTYLanguage);
  e.rt = getRTByLanguage(53, currentPTYLanguage);
  e.musica = "Gospel | Novo Tempo - Esperanca Para a Vida";
  e.anoMusica = "2023";
  e.hora = "13"; e.minuto = "42"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 100500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(54, currentPTYLanguage);
  e.rt = getRTByLanguage(54, currentPTYLanguage);
  e.musica = "Gospel | IPDA - Deus e Amor";
  e.anoMusica = "2019";
  e.hora = "13"; e.minuto = "55"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 100900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(27, currentPTYLanguage);
  e.rt = getRTByLanguage(27, currentPTYLanguage);
  e.musica = "Gospel | IPDA - Deus e Amor";
  e.anoMusica = "2022";
  e.hora = "14"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 101100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(55, currentPTYLanguage);
  e.rt = getRTByLanguage(55, currentPTYLanguage);
  e.musica = "Gospel | Fernandinho - Lugar Secreto";
  e.anoMusica = "2017";
  e.hora = "14"; e.minuto = "12"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 101300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(40, currentPTYLanguage);
  e.rt = getRTByLanguage(40, currentPTYLanguage);
  e.musica = "Gospel | Fernandinho - Grande e Bom";
  e.anoMusica = "2016";
  e.hora = "14"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 101700;
  e.pty_code = 1;
  e.ps = getPSByLanguage(28, currentPTYLanguage);
  e.rt = getRTByLanguage(28, currentPTYLanguage);
  e.musica = "Noticias | BandNews - Edicao do Meio-Dia";
  e.anoMusica = "2026";
  e.hora = "14"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 102300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(29, currentPTYLanguage);
  e.rt = getRTByLanguage(29, currentPTYLanguage);
  e.musica = "Gospel | Priscilla Alcantara - Deus Cuida de Mim";
  e.anoMusica = "2022";
  e.hora = "14"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 102500;
  e.pty_code = 8;
  e.ps = getPSByLanguage(56, currentPTYLanguage);
  e.rt = getRTByLanguage(56, currentPTYLanguage);
  e.musica = "Variedades | Programa de Entretenimento";
  e.anoMusica = "2026";
  e.hora = "14"; e.minuto = "38"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 102700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(30, currentPTYLanguage);
  e.rt = getRTByLanguage(30, currentPTYLanguage);
  e.musica = "Pop | Wesley Safadao - Camarote";
  e.anoMusica = "2015";
  e.hora = "15"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 103300;
  e.pty_code = 2;
  e.ps = getPSByLanguage(31, currentPTYLanguage);
  e.rt = getRTByLanguage(31, currentPTYLanguage);
  e.musica = "Informativo | Senado Federal - Plenario";
  e.anoMusica = "2026";
  e.hora = "15"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 103500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(32, currentPTYLanguage);
  e.rt = getRTByLanguage(32, currentPTYLanguage);
  e.musica = "Gospel | Comunidade Shalom - Paz e Bem";
  e.anoMusica = "2023";
  e.hora = "15"; e.minuto = "22"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 103900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(33, currentPTYLanguage);
  e.rt = getRTByLanguage(33, currentPTYLanguage);
  e.musica = "Pop | Kevinho - Olha a Explosao";
  e.anoMusica = "2017";
  e.hora = "15"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(34, currentPTYLanguage);
  e.rt = getRTByLanguage(34, currentPTYLanguage);
  e.musica = "Pop | Xand Aviao - Ao Vivo em Fortaleza";
  e.anoMusica = "2018";
  e.hora = "15"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 104700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(57, currentPTYLanguage);
  e.rt = getRTByLanguage(57, currentPTYLanguage);
  e.musica = "Pop | Mc Kevinho - Olha a Explosao";
  e.anoMusica = "2017";
  e.hora = "16"; e.minuto = "02"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Breezy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 105100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(35, currentPTYLanguage);
  e.rt = getRTByLanguage(35, currentPTYLanguage);
  e.musica = "Pop | Wesley Safadao - Camarote";
  e.anoMusica = "2018";
  e.hora = "16"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 105700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(36, currentPTYLanguage);
  e.rt = getRTByLanguage(36, currentPTYLanguage);
  e.musica = "Pop | Matheus e Kauan - Na Praia";
  e.anoMusica = "2016";
  e.hora = "16"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Breezy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 106100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(58, currentPTYLanguage);
  e.rt = getRTByLanguage(58, currentPTYLanguage);
  e.musica = "Pop | Hits do Momento";
  e.anoMusica = "2026";
  e.hora = "16"; e.minuto = "48"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 106500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(37, currentPTYLanguage);
  e.rt = getRTByLanguage(37, currentPTYLanguage);
  e.musica = "MPB | Maria Gadu - Shimbalaiê";
  e.anoMusica = "2009";
  e.hora = "16"; e.minuto = "30"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 106900;
  e.pty_code = 25;
  e.ps = getPSByLanguage(59, currentPTYLanguage);
  e.rt = getRTByLanguage(59, currentPTYLanguage);
  e.musica = "Forro | Dominguinhos - Forro Nordestino";
  e.anoMusica = "1985";
  e.hora = "17"; e.minuto = "10"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 107100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(81, currentPTYLanguage);
  e.rt = getRTByLanguage(81, currentPTYLanguage);
  e.musica = "Gospel | Fernandinho - Nao Existe Lugar";
  e.anoMusica = "2020";
  e.hora = "17"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 107300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(60, currentPTYLanguage);
  e.rt = getRTByLanguage(60, currentPTYLanguage);
  e.musica = "Pop | Alok - Never Let Me Go";
  e.anoMusica = "2020";
  e.hora = "17"; e.minuto = "25"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 107500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(38, currentPTYLanguage);
  e.rt = getRTByLanguage(38, currentPTYLanguage);
  e.musica = "Pop | Anitta - Girl From Rio";
  e.anoMusica = "2021";
  e.hora = "16"; e.minuto = "45"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "32°C";
  estacoes.push_back(e);

  e.freq_khz = 107900;
  e.pty_code = 4;
  e.ps = getPSByLanguage(39, currentPTYLanguage);
  e.rt = getRTByLanguage(39, currentPTYLanguage);
  e.musica = "Educativo | UFC Radio - Cultura e Ciencia";
  e.anoMusica = "2026";
  e.hora = "17"; e.minuto = "00"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 94500;
  e.pty_code = 26;
  e.ps = getPSByLanguage(82, currentPTYLanguage);
  e.rt = getRTByLanguage(82, currentPTYLanguage);
  e.musica = "Sertanejo | Gusttavo Lima - Cem Mil";
  e.anoMusica = "2020";
  e.hora = "11"; e.minuto = "10"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 95700;
  e.pty_code = 25;
  e.ps = getPSByLanguage(83, currentPTYLanguage);
  e.rt = getRTByLanguage(83, currentPTYLanguage);
  e.musica = "Forro | Falamansa - Xote dos Milagres";
  e.anoMusica = "2001";
  e.hora = "11"; e.minuto = "35"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 100300;
  e.pty_code = 1;
  e.ps = getPSByLanguage(84, currentPTYLanguage);
  e.rt = getRTByLanguage(84, currentPTYLanguage);
  e.musica = "Noticias | Nacao FM - Edicao da Tarde";
  e.anoMusica = "2026";
  e.hora = "13"; e.minuto = "48"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "36°C";
  estacoes.push_back(e);

  e.freq_khz = 101500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(85, currentPTYLanguage);
  e.rt = getRTByLanguage(85, currentPTYLanguage);
  e.musica = "Gospel | Eyshila - Jesus e Tudo";
  e.anoMusica = "2023";
  e.hora = "14"; e.minuto = "08"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 101900;
  e.pty_code = 4;
  e.ps = getPSByLanguage(86, currentPTYLanguage);
  e.rt = getRTByLanguage(86, currentPTYLanguage);
  e.musica = "Educativo | UFC FM - Ciencia e Cultura";
  e.anoMusica = "2026";
  e.hora = "14"; e.minuto = "25"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 103700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(87, currentPTYLanguage);
  e.rt = getRTByLanguage(87, currentPTYLanguage);
  e.musica = "Pop | Alok - Hear Me Now";
  e.anoMusica = "2016";
  e.hora = "15"; e.minuto = "25"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "34°C";
  estacoes.push_back(e);

  e.freq_khz = 104100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(88, currentPTYLanguage);
  e.rt = getRTByLanguage(88, currentPTYLanguage);
  e.musica = "Pop | Anitta - Funk Rave";
  e.anoMusica = "2023";
  e.hora = "15"; e.minuto = "38"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Hot & Humid"; e.temperatura = "35°C";
  estacoes.push_back(e);

  e.freq_khz = 104900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(89, currentPTYLanguage);
  e.rt = getRTByLanguage(89, currentPTYLanguage);
  e.musica = "Dance | Kevin O Chris - Coisa de Ex";
  e.anoMusica = "2020";
  e.hora = "16"; e.minuto = "08"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Breezy"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 106300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(90, currentPTYLanguage);
  e.rt = getRTByLanguage(90, currentPTYLanguage);
  e.musica = "Pop | Claudia Leitte - Caranguejo";
  e.anoMusica = "2012";
  e.hora = "16"; e.minuto = "38"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Clear"; e.temperatura = "33°C";
  estacoes.push_back(e);

  e.freq_khz = 107700;
  e.pty_code = 20;
  e.ps = getPSByLanguage(91, currentPTYLanguage);
  e.rt = getRTByLanguage(91, currentPTYLanguage);
  e.musica = "Gospel | Fernandinho - Tu Me Envolves";
  e.anoMusica = "2015";
  e.hora = "17"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Partly Cloudy"; e.temperatura = "31°C";
  estacoes.push_back(e);

  e.freq_khz = 88700;
  e.pty_code = 25;
  e.ps = getPSByLanguage(92, currentPTYLanguage);
  e.rt = getRTByLanguage(92, currentPTYLanguage);
  e.musica = "Forro | Luiz Gonzaga - Forró de Mané Vito";
  e.anoMusica = "1989";
  e.hora = "08"; e.minuto = "35"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "29°C";
  estacoes.push_back(e);

  e.freq_khz = 90900;
  e.pty_code = 12;
  e.ps = getPSByLanguage(93, currentPTYLanguage);
  e.rt = getRTByLanguage(93, currentPTYLanguage);
  e.musica = "MPB | Zeca Baleiro - Por Onde Andar";
  e.anoMusica = "1997";
  e.hora = "09"; e.minuto = "15"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "30°C";
  estacoes.push_back(e);

  e.freq_khz = 89300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(94, currentPTYLanguage);
  e.rt = getRTByLanguage(94, currentPTYLanguage);
  e.musica = "Axe | Ivete Sangalo - Sorte Grande";
  e.anoMusica = "2013";
  e.hora = "08"; e.minuto = "40"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Sunny"; e.temperatura = "29°C";
  estacoes.push_back(e);

  e.freq_khz = 89700;
  e.pty_code = 38;
  e.ps = getPSByLanguage(95, currentPTYLanguage);
  e.rt = getRTByLanguage(95, currentPTYLanguage);
  e.musica = "Rock Nacional | Legiao Urbana - Tempo Perdido";
  e.anoMusica = "1987";
  e.hora = "10"; e.minuto = "20"; e.segundo = "00";
  e.dia = "13"; e.mes = "06"; e.ano = "2026";
  e.tempo = "Cloudy"; e.temperatura = "27°C";
  estacoes.push_back(e);

  for (auto& s : estacoes)
    s.pi_code = (uint16_t)(0x4000 + (s.freq_khz / 100 - 797));

  std::sort(estacoes.begin(), estacoes.end(), [](const Estacao& a, const Estacao& b) {
    return a.freq_khz < b.freq_khz;
  });

  log_info("Estacoes carregadas.");
}
