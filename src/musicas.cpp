#include "musicas.h"

const char* brazilianArtists[] = {
  // Brasileiros Classicos
  "Caetano Veloso", "Gilberto Gil", "Tom Jobim", "Joao Gilberto",
  "Gal Costa", "Ivete Sangalo", "Jorge Aragao", "Beth Carvalho",
  "Alcione", "Cartola", "Elza Soares", "Marisa Monte",
  "Adriana Calcanhotto", "Chico Buarque", "Ivan Lins", "Djavan",
  "Ze Ramalho", "Lenine", "Criolo", "Arlindo Cruz",
  "Paulinho da Viola", "Martinho da Vila", "Clara Nunes", "Milton Nascimento",
  "Jorge Ben Jor", "Luiz Gonzaga", "Jackson do Pandeiro", "Dominguinhos",
  "Elba Ramalho", "Gusttavo Lima", "Jorge e Mateus", "Bruno e Marrone",
  "Zeze di Camargo", "Ana Castela", "Anitta", "Pabllo Vittar",
  "Ludmilla", "Emicida", "Legiao Urbana", "Titas",
  "Capital Inicial", "Sepultura", "Rita Lee", "Paralamas do Sucesso",
  "Os Mutantes", "Sorriso Maroto", "Dilsinho", "Ferrugem",
  "Dona Ivone Lara", "Nando Reis",
  // Brasileiros Contemporaneos
  "Luisa Sonza", "Selena Gomez feat. Tiesto", "Alok", "Tropkillaz",
  "MC Zaac", "Maejor", "DJ Snake", "Kevin O", "Kevinho",
  "Ludmilla", "Pocah", "Tony Tornado", "Projota", "Rael", "BK",
  "Tarcisio Meira", "Pedro Scooby", "Biel", "Leo Santana", "Filipe Ret",
  // Internacionais
  "Taylor Swift", "Ariana Grande", "Billie Eilish", "Dua Lipa",
  "The Weeknd", "Ed Sheeran", "Bruno Mars", "Justin Bieber",
  "The Beatles", "Rolling Stones", "Led Zeppelin", "Pink Floyd",
  "Queen", "David Bowie", "Michael Jackson", "Madonna",
  "Whitney Houston", "Aretha Franklin", "Frank Sinatra", "Miles Davis",
  "Ella Fitzgerald", "Elvis Presley", "Bob Dylan", "Jimi Hendrix",
  "Bob Marley", "Drake", "Beyonce", "Alicia Keys",
  "Eminem", "Metallica", "Guns N Roses", "The Doors",
  "Aerosmith", "Deep Purple", "Black Sabbath", "Iron Maiden",
  "Nirvana", "Pearl Jam", "U2", "Coldplay",
  "Daft Punk", "Calvin Harris", "David Guetta", "Adele",
  "Amy Winehouse", "Radiohead", "Prince", "Tina Turner",
  "Mariah Carey", "Stevie Wonder"
};
const int ARTISTS_COUNT = sizeof(brazilianArtists) / sizeof(brazilianArtists[0]);

const char* musicGenres[] = {
  "Samba", "Bossa Nova", "Forro", "Sertanejo", "Funk Carioca",
  "Rock Brasileiro", "MPB", "Pagode", "Axe", "Tropicalia",
  "Pop", "Hip-Hop", "Jazz", "Blues", "Soul", "Reggae", "Disco", "EDM"
};
const int GENRES_COUNT = sizeof(musicGenres) / sizeof(musicGenres[0]);

const char* brazilianYears[] = {
  "2024", "2023", "2022", "2021", "2020", "2019", "2018", "2017",
  "2016", "2015", "2014", "2013", "2012", "2011", "2010", "2009",
  "2008", "2007", "2006", "2005", "2004", "2003", "2002", "2001",
  "2000", "1999", "1998", "1997", "1996", "1995", "1990", "1985",
  "1980", "1975", "1970", "1965", "1960"
};
const int YEARS_COUNT = sizeof(brazilianYears) / sizeof(brazilianYears[0]);

const char* brazilianSongs[] = {
  // Brasileiras Classicas
  "Garota de Ipanema", "Aguas de Marco", "Aquele Abraco", "Toda Menina Baiana",
  "Tropicalia", "Asa Branca", "Saudade", "Noite de Samba",
  "Amor Infinito", "Alegria Infinita", "Carnaval Brasileiro", "Praia Brasileira",
  "Rio de Janeiro", "Samba Eterno", "Paixao Tropical", "Som da Alma",
  "Ritmo do Coracao", "Energia Pura", "Danca da Paixao", "Noite Magica",
  "Amor Verdadeiro", "Noite Carioca", "Bateria Certeira", "Samba Enredo",
  "Brasilidade", "Melodia Tropical", "Batida Certa", "Som do Brasil",
  "Raizes do Samba", "Alma Brasileira", "Balada Inteligente", "Flor de Lis",
  "Levada Brasileira", "Festa", "Cadencia Brasileira", "Pura Energia",
  "Beleza Tropical", "Floresta Amazonica", "Bahia Minha", "Choro Tradicional",
  "Samba Rock Carioca", "Forró Cearense", "Baiao do Sertao", "Coco de Raiz",
  "Xote Autentico", "Nordeste Musical", "Carioca Paixao", "Noite Brasileira",
  "Amor Sagrado", "Ritmo Perfeito",
  // Brasileiras Contemporaneas
  "Envolver", "Vai Malandra", "Show das Poderosas", "Corpo Perfeito",
  "Noticia Boa", "Danca da Paixao", "Levada Brasileira", "Melody",
  "Solte o Frevo", "Coreografias", "De Janeiro a Janeiro", "Danca Comigo",
  "Meu Carnaval", "Ritmo Cearense", "Fortaleza e Coracoes", "Praia Vermelha",
  "Saudade Tropical", "Brisa do Mar", "Luz da Madrugada", "Noites de Luar",
  "Musica Cearense", "Sertanejo Puro", "Alma Nordestina", "Grito Tropical",
  // Internacionais
  "Stairway to Heaven", "Bohemian Rhapsody", "Hotel California", "Smells Like Teen Spirit",
  "Yesterday", "Let It Be", "Hey Jude", "Imagine",
  "Purple Rain", "Thriller", "Billie Jean", "Beat It",
  "Like a Prayer", "Respect", "I Will Always Love You", "Nothing Else Matters",
  "Enter Sandman", "One More Time", "Get Lucky", "Losing My Religion",
  "One", "With or Without You", "Don't Stop Believin", "Eye of the Tiger",
  "We Will Rock You", "Under Pressure", "Shake It Off", "Bad Guy",
  "Blinding Lights", "Levitating", "Shape of You", "Thinking Out Loud",
  "Someone Like You", "Rolling in the Deep", "Back to Black", "Creep",
  "Karma Police", "Paranoid", "Smoke on the Water", "Black Dog",
  "Back in Black", "Highway to Hell", "Born to Run", "Like a Rolling Stone",
  "What's Going On", "Sweet Home Chicago", "Johnny B Goode", "Good Vibrations",
  "No Woman No Cry", "Three Little Birds"
};
const int SONGS_COUNT = sizeof(brazilianSongs) / sizeof(brazilianSongs[0]);
