#ifndef ESTACOES_H
#define ESTACOES_H

#include <Arduino.h>
#include <cstdint>
#include <vector>

struct Estacao {
  uint32_t freq_khz;
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
bool   isRDSAtivo(uint32_t freq_khz);
String buscarPS(uint32_t freq_khz);
String buscarRT(uint32_t freq_khz);
int8_t buscarPTY(uint32_t freq_khz);
void   avancarScroll(uint32_t freq_khz);
String buscarMusica(uint32_t freq_khz);
String buscarGenero(uint32_t freq_khz);
size_t totalEstacoes();
Estacao& getEstacao(size_t i);

#endif // ESTACOES_H
