#ifndef CONTEUDO_H
#define CONTEUDO_H

#include <Arduino.h>
#include <cstdint>

String      generateRandomSong();
String      getSongForPTY(uint8_t pty);
String      getRandomGenreByPTY(uint8_t pty_code);
String      getRandomArtistByPTY(uint8_t pty_code);
String      getRandomYearByPTY(uint8_t pty_code);
const char* getPTYName(uint8_t pty_code);
String      rotateSongString(const String& s, int pos);
String      formatSongWithYear(const String& song, const String& year);
String      generateRandomWeather();
String      generateRandomTemperature();
void        generateRandomTime(String& hora, String& minuto, String& segundo);
void        generateRandomDate(String& dia, String& mes, String& ano);

#endif // CONTEUDO_H
