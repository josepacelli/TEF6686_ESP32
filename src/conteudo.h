#ifndef CONTEUDO_H
#define CONTEUDO_H

#include <Arduino.h>
#include <cstdint>

String      generateRandomSong();
String      getRandomGenreByPTY(uint8_t pty_code);
String      getRandomArtistByPTY(uint8_t pty_code);
String      getRandomYearByPTY(uint8_t pty_code);
const char* getPTYName(uint8_t pty_code);
String      rotateSongString(const String& s, int pos);
String      formatSongWithYear(const String& song, const String& year);

#endif // CONTEUDO_H
