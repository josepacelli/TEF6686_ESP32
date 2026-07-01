#ifndef ESTACOES_H
#define ESTACOES_H

#include <Arduino.h>
#include <cstdint>
#include <vector>

struct Estacao {
  uint32_t freq_khz;
  uint16_t pi_code = 0;
  int lang_idx = 0;
  String ps;
  String rt;
  int8_t pty_code;
  String musica;
  String anoMusica;
  size_t posScroll;
  String hora, minuto, segundo;
  String dia, mes, ano;
  String tempo, temperatura;
  bool rds_ativo = true;
};

void   carregarEstacoes();
bool     isRDSAtivo(uint32_t freq_khz);
String   buscarPS(uint32_t freq_khz);
String   buscarRT(uint32_t freq_khz);
String   buscarPSPlain(uint32_t freq_khz);
String   buscarRTPlain(uint32_t freq_khz);
int8_t   buscarPTY(uint32_t freq_khz);
String   buscarPI(uint32_t freq_khz);
void   avancarScroll(uint32_t freq_khz);
String buscarMusica(uint32_t freq_khz);
String buscarGenero(uint32_t freq_khz);
void   rotateStationMusic(uint32_t freq_khz);
void   rotateStationWeather(uint32_t freq_khz);
size_t totalEstacoes();
Estacao& getEstacao(size_t i);
void   setAllRDS(bool ativo);

#endif // ESTACOES_H
