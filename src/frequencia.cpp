#include "frequencia.h"

const uint32_t allFrequencies[] = {
  79700,   // 00 Radio Metropolitana FM 79.7
  86700,   // 01 Radio Educativa IFCE FM 86.7
  87100,   // 02 Radio Ceara FM 87.1
  87500,   // 41 Radio Poti FM 87.5
  87900,   // 42 Radio Novo Tempo FM 87.9
  88100,   // 43 Radio Nordeste FM 88.1
  88300,   // 03 Radio Jerusalem FM 88.3
  88900,   // 04 Radio Jangadeiro FM 88.9
  89100,   // 44 Radio Record News FM 89.1
  89500,   // 45 Radio Maranata FM 89.5
  89900,   // 05 Radio 89 FM 89.9
  90300,   // 06 Radio Uirapuru 90.3
  90700,   // 07 Radio Fortaleza FM 90.7
  91100,   // 46 Radio Impacto FM 91.1
  91300,   // 08 Radio Metropolitana FM 91.3
  91700,   // 09 Radio Shalom FM 91.7
  92100,   // 10 Radio Efraim 92.1
  92500,   // 11 Radio Verdinha FM 92.5
  92900,   // 12 Radio Jovem Pan News 92.9
  93100,   // 47 Radio Globo Fortaleza 93.1
  93500,   // 13 Radio Canaa FM 93.5
  93900,   // 14 Radio FM 93 93.9
  94100,   // 48 Radio Rede Vida FM 94.1
  94300,   // 15 Radio Sol FM 94.3
  94700,   // 16 Radio Jovem Pan FM 94.7
  95100,   // 17 Radio Logos FM 95.1
  95300,   // 49 Radio Cultura CE FM 95.3
  95500,   // 18 Radio O Povo CBN 95.5
  96100,   // 19 Radio Dom Bosco FM 96.1
  96300,   // 50 Radio Cristo Vive FM 96.3
  96700,   // 20 Radio Alece FM 96.7
  97100,   // 21 Radio Maria Brasil 97.1
  97300,   // 51 Radio Tupi Fortaleza 97.3
  97700,   // 22 Radio Antena 1 97.7
  98300,   // 23 Radio Lider FM 98.3
  98700,   // 52 Radio Iguatemi FM 98.7
  99100,   // 24 Radio Cidade FM 99.1
  99500,   // 25 Radio Plus FM 99.5
  99900,   // 26 Radio Rede Aleluia 99.9
  100100,  // 53 Radio Novo Tempo FM 100.1
  100500,  // 54 Radio Forro 100 FM 100.5
  100900,  // 27 Radio Deus E Amor 100.9
  101100,  // 55 Radio Paz FM 101.1
  101300,  // 40 Nova Radio Crista FM 101.3
  101700,  // 28 Radio BandNews FM 101.7
  102300,  // 29 Radio Templo Central FM 102.3
  102500,  // 56 Radio Difusora FM 102.5
  102700,  // 30 Radio Beach Park FM 102.7
  103300,  // 31 Radio Senado 103.3
  103500,  // 32 Radio Shalom FM 103.5
  103900,  // 33 Radio Tempo FM 103.9
  104300,  // 34 Radio Metropolitana FM 104.3
  104700,  // 57 Radio Litoral FM 104.7
  105100,  // 35 Radio AD Cidade FM 105.1
  105700,  // 36 Radio Atlantico Sul FM 105.7
  106100,  // 58 Radio Evangelica FM 106.1
  106500,  // 37 Radio Nova Brasil FM 106.5
  106900,  // 59 Radio Sertao FM 106.9
  107300,  // 60 Radio Total FM 107.3
  107500,  // 38 Radio Mix FM 107.5
  107900,  // 39 Radio Universitaria FM 107.9
};

// PTY codes parallel to allFrequencies[].
// 1=Current Affairs  2=Information  4=Education  6=Culture
// 8=Varied Speech    10=Pop Music   12=Easy Listening
// 20=Religion/Gospel 25=Country/Forro
const int8_t allPTYCodes[] = {
  10,  // 79700  Radio Metropolitana 79.7       Pop Music
   4,  // 86700  Radio Educativa IFCE           Education
  10,  // 87100  Radio Ceara FM                 Pop Music
  10,  // 87500  Radio Poti FM                  Pop Music
  20,  // 87900  Radio Novo Tempo FM            Gospel/Religion
  25,  // 88100  Radio Nordeste FM              Country/Forro
  20,  // 88300  Radio Jerusalem FM             Gospel/Religion
  10,  // 88900  Radio Jangadeiro FM            Pop Music
   1,  // 89100  Radio Record News FM           Current Affairs
  20,  // 89500  Radio Maranata FM              Gospel/Religion
  10,  // 89900  Radio 89 FM                    Pop Music
  20,  // 90300  Radio Uirapuru/Rede Aleluia    Gospel/Religion
  10,  // 90700  Radio Fortaleza FM             Pop Music
  10,  // 91100  Radio Impacto FM               Pop Music
  10,  // 91300  Radio Metropolitana 91.3       Pop Music
  20,  // 91700  Radio Shalom FM                Gospel/Religion
  20,  // 92100  Radio Efraim                   Gospel/Religion
   1,  // 92500  Radio Verdinha FM              Current Affairs
   1,  // 92900  Radio Jovem Pan News           Current Affairs
   8,  // 93100  Radio Globo Fortaleza          Varied Speech
  20,  // 93500  Radio Canaa FM                 Gospel/Religion
  10,  // 93900  Radio FM 93                    Pop Music
  20,  // 94100  Radio Rede Vida FM             Gospel/Religion
  10,  // 94300  Radio Sol FM                   Pop Music
  10,  // 94700  Radio Jovem Pan FM             Pop Music
  20,  // 95100  Radio Logos FM                 Gospel/Religion
   6,  // 95300  Radio Cultura CE FM            Culture
   1,  // 95500  Radio O Povo CBN               Current Affairs
  20,  // 96100  Radio Dom Bosco FM             Gospel/Religion
  20,  // 96300  Radio Cristo Vive FM           Gospel/Religion
   2,  // 96700  Radio Alece FM                 Information
  20,  // 97100  Radio Maria Brasil             Gospel/Religion
   8,  // 97300  Radio Tupi Fortaleza           Varied Speech
  12,  // 97700  Radio Antena 1                 Easy Listening
  20,  // 98300  Radio Lider FM                 Gospel/Religion
  10,  // 98700  Radio Iguatemi FM              Pop Music
  10,  // 99100  Radio Cidade FM                Pop Music
  10,  // 99500  Radio Plus FM                  Pop Music
  20,  // 99900  Radio Rede Aleluia             Gospel/Religion
  20,  // 100100 Radio Novo Tempo FM 100.1      Gospel/Religion
  25,  // 100500 Radio Forro 100 FM             Country/Forro
  20,  // 100900 Radio Deus E Amor              Gospel/Religion
  20,  // 101100 Radio Paz FM                   Gospel/Religion
  20,  // 101300 Nova Radio Crista FM           Gospel/Religion
   1,  // 101700 Radio BandNews FM              Current Affairs
  20,  // 102300 Radio Templo Central FM        Gospel/Religion
   8,  // 102500 Radio Difusora FM              Varied Speech
  10,  // 102700 Radio Beach Park FM            Pop Music
   2,  // 103300 Radio Senado                   Information
  20,  // 103500 Radio Shalom FM 103.5          Gospel/Religion
  10,  // 103900 Radio Tempo FM                 Pop Music
  10,  // 104300 Radio Metropolitana 104.3      Pop Music
  10,  // 104700 Radio Litoral FM               Pop Music
  20,  // 105100 Radio AD Cidade FM             Gospel/Religion
  10,  // 105700 Radio Atlantico Sul FM         Pop Music
  20,  // 106100 Radio Evangelica FM            Gospel/Religion
  10,  // 106500 Radio Nova Brasil FM           Pop Music
  25,  // 106900 Radio Sertao FM                Country/Forro
  10,  // 107300 Radio Total FM                 Pop Music
  10,  // 107500 Radio Mix FM                   Pop Music
   4,  // 107900 Radio Universitaria FM         Education
};

const int FREQ_COUNT = sizeof(allFrequencies) / sizeof(allFrequencies[0]);

int8_t getPTYCodeForFreq(uint32_t freq_khz) {
  for (int i = 0; i < FREQ_COUNT; i++) {
    if (allFrequencies[i] == freq_khz) return allPTYCodes[i];
  }
  return -1;
}
