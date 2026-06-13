#include "custom_ptys.h"
#include <vector>
#include <cmath>
#include "logbook.h"
#include "frequencia.h"
#include "ps_language.h"
#include "rt_language.h"

static std::vector<PTYEntry> customPtys;

#include "musicas.h"



// POP GENRE
const char* popArtists[] = {
  "Anitta", "Marisa Monte", "Adriana Calcanhotto", "Ivete Sangalo", "Pabllo Vittar",
  "Ludmilla", "Pocah", "Kevinho", "Silva", "Vanessa Lopes"
};
const int POP_ARTISTS = sizeof(popArtists) / sizeof(popArtists[0]);

const char* popSongs[] = {
  "Danca da Paixao", "Noite Magica", "Noticia Boa", "Festa", "Corpo Perfeito",
  "Envolver", "Levada Brasileira", "Ritmo Perfeito", "Melodia Tropical", "Pura Energia"
};
const int POP_SONGS = sizeof(popSongs) / sizeof(popSongs[0]);

// ROCK GENRE
const char* rockArtists[] = {
  "Legiao Urbana", "Titas", "Capital Inicial", "CPM 22", "Os Mutantes",
  "Rita Lee", "NX Zero", "Sepultura", "Barao Vermelho", "Paralamas do Sucesso"
};
const int ROCK_ARTISTS = sizeof(rockArtists) / sizeof(rockArtists[0]);

const char* rockSongs[] = {
  "Energia Pura", "Comida", "Fico Assim Sem Voce", "Respeito e Admira", "Tropicalia",
  "Lanca Perfume", "Jardim da Saudade", "Orgulho da Ignorancia", "Ratos e Urubus", "Ainda Escolho"
};
const int ROCK_SONGS = sizeof(rockSongs) / sizeof(rockSongs[0]);

// EASY LISTENING
const char* easyArtists[] = {
  "Tom Jobim", "Joao Gilberto", "Marisa Monte", "Gal Costa", "Djavan",
  "Ivan Lins", "Beth Carvalho", "Cartola", "Nelson Cavaquinho", "Paulinho da Viola"
};
const int EASY_ARTISTS = sizeof(easyArtists) / sizeof(easyArtists[0]);

const char* easySongs[] = {
  "Garota de Ipanema", "Desafinado", "Aguas de Marco", "Universo da Lembranca", "Festa para um Rei Negro",
  "Saudade", "Alma Brasileira", "Noite de Luar", "Som do Brasil", "Ritmo Que Toca"
};
const int EASY_SONGS = sizeof(easySongs) / sizeof(easySongs[0]);

// CLASSICAL/LIGHT
const char* classicalArtists[] = {
  "Egberto Gismonti", "Tom Jobim", "Chopin", "Beethoven", "Bach",
  "Mozart", "Debussy", "Erik Satie", "Liszt", "Vivaldi"
};
const int CLASSICAL_ARTISTS = sizeof(classicalArtists) / sizeof(classicalArtists[0]);

const char* classicalSongs[] = {
  "Agua e Vinho", "Agua de Marzo", "Nocturne Op.9", "Moonlight Sonata", "Air on G String",
  "Eine Kleine Nachtmusik", "Clair de Lune", "Gymnopédie No.1", "La Campanella", "The Four Seasons"
};
const int CLASSICAL_SONGS = sizeof(classicalSongs) / sizeof(classicalSongs[0]);

// COUNTRY/FORRÓ
const char* countryArtists[] = {
  "Luiz Gonzaga", "Jackson do Pandeiro", "Dominguinhos", "Xangai", "Amelinha",
  "Elba Ramalho", "Lula Cortes", "Marines", "Genival Lacerda", "Siqueira"
};
const int COUNTRY_ARTISTS = sizeof(countryArtists) / sizeof(countryArtists[0]);

const char* countrySongs[] = {
  "Asa Branca", "Forró do Seu Zé", "Meu Sertao", "Forró dos Namorados", "Fruto da Paixao",
  "Coco de Caio", "Danca da Vassoura", "Sonho Meu", "Xangai de Meu Coracão", "Beijaflor"
};
const int COUNTRY_SONGS = sizeof(countrySongs) / sizeof(countrySongs[0]);

// NATIONAL/SERTANEJO
const char* nationalArtists[] = {
  "Gusttavo Lima", "Jorge e Mateus", "Bruno e Marrone", "Leonardo", "Chitaozinho e Xororo",
  "Henrique e Juliano", "Maiara e Maraisa", "Ana Castela", "Zeze di Camargo", "Leao e Halim"
};
const int NATIONAL_ARTISTS = sizeof(nationalArtists) / sizeof(nationalArtists[0]);

const char* nationalSongs[] = {
  "Balada Inteligente", "Sossega", "Quem nao sonha", "Saudade de Mim", "Fio de Ouro",
  "Flores em Vida", "Calamidade", "Menino da Porteira", "Rainha do Meu Coracao", "Trem da Saudade"
};
const int NATIONAL_SONGS = sizeof(nationalSongs) / sizeof(nationalSongs[0]);

// GOSPEL
const char* gospelArtists[] = {
  "Ivete Sangalo", "Gal Costa", "Beth Carvalho", "Alcione", "Elza Soares",
  "Criolo", "Lenine", "Arlindo Cruz", "Paulinho da Viola", "Martinho da Vila",
  "Angela Ro Ro", "Benito di Paula", "Fernanda Brum", "Eyshila", "Soraya Moraes",
  "Kemilly Santos", "Priscilla Alcantara", "Midian Lima", "Gino Amen", "Thalles Roberto"
};
const int GOSPEL_ARTISTS = sizeof(gospelArtists) / sizeof(gospelArtists[0]);

const char* gospelSongs[] = {
  "Noite Sagrada", "Palavra de Deus", "Levada Brasileira", "Alma Brasileira",
  "Graca Divina", "Maria Sagrada", "Lider Celestial", "Paz e Bem",
  "Assembleia Sagrada", "Templo Sagrado", "Amor Divino", "Voz do Evangelho",
  "Gloria Eterna", "Amor Infinito", "Comunidade Shalom", "Verdade Tropical",
  "Democracia Sonora", "Noticia Sempre", "Melhor Musica", "Todas Tocadas",
  "Ritmo Nordestino", "Danca Nordestina", "Raiz do Sertao", "Cabaço e Viola",
  "Aboio do Sertao", "Forró Tradicional", "Batida Cearense", "Musica Folclorica",
  "Raizes Culturais", "Tradição Musical", "Folclore Vivo", "Cultura Popular",
  "Expressão Nordestina", "Ritmo Sagrado", "Divino Ritmo", "Eterno Evangelho",
  "Infinita Bencao", "Puro Evangelho", "Sincero Louvor", "Verdadeiro Culto",
  "Doce Meloquia", "Quente Paixao", "Sensual Adoracao", "Ardente Louvor"
};
const int GOSPEL_SONGS = sizeof(gospelSongs) / sizeof(gospelSongs[0]);

const char* gospelGenres[] = {
  "Gospel", "Gospel Tradicional", "Gospel Contemporaneo", "Gospel Urbano",
  "Gospel Rock", "Gospel Soul", "Gospel Sertanejo", "Gospel Forró",
  "Gospel Samba", "Gospel Pagode", "Gospel Jazz", "Gospel Blues",
  "Adoracao", "Louvor", "Musica Crista", "Hinario", "Esperanca Cristã"
};
const int GOSPEL_GENRES = sizeof(gospelGenres) / sizeof(gospelGenres[0]);

const char* gospelYears[] = {
  "2023", "2022", "2021", "2020", "2019", "2018", "2017", "2016", "2015", "2014",
  "2013", "2012", "2011", "2010", "2009", "2008", "2007", "2006", "2005", "2004",
  "2003", "2002", "2001", "2000", "1999", "1998", "1997", "1996", "1995", "1994",
  "1993", "1992", "1991", "1990", "1989", "1988", "1987", "1986", "1985", "1984",
  "1983", "1982", "1981", "1980"
};
const int GOSPEL_YEARS = sizeof(gospelYears) / sizeof(gospelYears[0]);

// GOSPEL PS/RT STATIONS
const char* gospelPS[] = {
  "RADIO MARIA BRASIL", "REDE ALELUIA FM", "RADIO JERUSALEM FM", "SHALOM FM",
  "RADIO EFRAIM", "DOMBOSCO FM", "LOGOS FM", "AD CIDADE FM",
  "NOVA RADIO CRISTA", "TEMPLO CENTRAL FM", "REDE SHALOM RADIO", "ASSEMBLÉIA DE DEUS",
  "DEUS E AMOR FM", "CANAA FM", "RADIO UIRAPURU", "CRISTO VIVE FM",
  "VOZ DO EVANGELHO", "BENÇÃO FM", "GRAÇA ETERNA FM", "COMUNIDADE SHALOM"
};
const int GOSPEL_PS_COUNT = sizeof(gospelPS) / sizeof(gospelPS[0]);

const char* gospelRT[] = {
  "AVE MARIA CHEIA DE GRACA", "GLORIA A DEUS NAS ALTURAS", "A VOZ DE DEUS NO AR",
  "COMUNIDADE SHALOM NO AR", "A RADIO GOSPEL DO CEARA", "A RADIO SALESIANA DO CEARA",
  "A PALAVRA DE DEUS NO AR", "ASSEMBLEIA DE DEUS NO AR", "A NOVA VOZ DO EVANGELHO",
  "O TEMPLO DE DEUS NO AR", "COMUNIDADE SHALOM - PAZ E BEM", "Cristo VIVE SEMPRE",
  "DEUS E AMOR - A RADIO DA CURA", "A TERRA PROMETIDA DO RADIO", "GLORIA A DEUS",
  "LUZ E SALVACAO", "A VOZ DA FE", "BENCAO E PAZ", "ETERNIDADE DIVINA", "LOUVOR AO SENHOR"
};
const int GOSPEL_RT_COUNT = sizeof(gospelRT) / sizeof(gospelRT[0]);

// RHYTHM/GENRE ARRAYS BY PTY
const char* popGenres[] = {
  "Pop", "Pop Brasileiro", "Pop Rock", "Electropop", "Synthpop", "Pop Soul", "Pop Dance"
};
const int POP_GENRES_COUNT = sizeof(popGenres) / sizeof(popGenres[0]);

const char* rockGenres[] = {
  "Rock", "Rock Nacional", "Hard Rock", "Rock Alternativo", "Indie Rock", "Rock Progressivo"
};
const int ROCK_GENRES_COUNT = sizeof(rockGenres) / sizeof(rockGenres[0]);

const char* easyListeningGenres[] = {
  "Bossa Nova", "Easy Listening", "Suave", "Relaxante", "Melódico", "Soft Jazz"
};
const int EASY_LISTENING_GENRES_COUNT = sizeof(easyListeningGenres) / sizeof(easyListeningGenres[0]);

const char* classicalGenres[] = {
  "Clássico", "Música Erudita", "Sinfônico", "Orquestra", "Concerto", "Ópera Leve"
};
const int CLASSICAL_GENRES_COUNT = sizeof(classicalGenres) / sizeof(classicalGenres[0]);

const char* countryGenres[] = {
  "Forró", "Forró Tradicional", "Forró Universitário", "Xote", "Baião", "Coco"
};
const int COUNTRY_GENRES_COUNT = sizeof(countryGenres) / sizeof(countryGenres[0]);

const char* sertanejoGenres[] = {
  "Sertanejo", "Sertanejo Raiz", "Sertanejo Universitário", "Sertanejo Moderno", "Dupla Sertaneja"
};
const int SERTANEJO_GENRES_COUNT = sizeof(sertanejoGenres) / sizeof(sertanejoGenres[0]);

const char* jazzGenres[] = {
  "Jazz", "Jazz Brasileiro", "Bossa Nova Jazz", "Jazz Contemporâneo", "Jazz Fusão", "Samba Jazz"
};
const int JAZZ_GENRES_COUNT = sizeof(jazzGenres) / sizeof(jazzGenres[0]);

// PS (PROGRAM SERVICE) ARRAYS BY PTY
const char* popPS[] = {
  "POP FM", "MEGA FM", "HIT FM", "RADIO POP", "PURA POP", "POP BRASIL",
  "PODER FM", "CIDADE FM", "RITMO FM", "JOVEM PAN", "KISS FM", "NOVA FM"
};
const int POP_PS_COUNT = sizeof(popPS) / sizeof(popPS[0]);

const char* rockPS[] = {
  "ROCK FM", "HEAVY FM", "POWER FM", "METAL FM", "STONE FM", "RIFF FM",
  "GUITAR FM", "AMP FM", "STAGE FM", "LIVE ROCK", "ROCKER FM", "SONIC FM"
};
const int ROCK_PS_COUNT = sizeof(rockPS) / sizeof(rockPS[0]);

const char* easyPS[] = {
  "BOSSA FM", "RELAX FM", "SMOOTH FM", "JAZZ+ FM", "CALM FM", "SOUL FM",
  "SERENITY FM", "HARMONY FM", "LOUNGE FM", "MELLOW FM", "ZEN FM", "PEACE FM"
};
const int EASY_PS_COUNT = sizeof(easyPS) / sizeof(easyPS[0]);

const char* classicalPS[] = {
  "CLASSICO FM", "SINFONIA FM", "ORQUESTRA FM", "ERUDITO FM", "MAESTRO FM",
  "OPUS FM", "CONCERTO FM", "SONATA FM", "REQUIEM FM", "ARIA FM", "CHOPIN FM"
};
const int CLASSICAL_PS_COUNT = sizeof(classicalPS) / sizeof(classicalPS[0]);

const char* countryPS[] = {
  "FORRO FM", "SERTAO FM", "NORDESTE FM", "CABAÇO FM", "VIOLA FM", "RAIZ FM",
  "XOTE FM", "BAIAO FM", "TRIANGULO FM", "ZABUMBA FM", "COCO FM", "TRADIÇÃO FM"
};
const int COUNTRY_PS_COUNT = sizeof(countryPS) / sizeof(countryPS[0]);

const char* sertanejoPS[] = {
  "SERTANEJO FM", "CAMPO FM", "RURAL FM", "DUPLA FM", "CAVALO FM", "FAZENDA FM",
  "VAQUEIRO FM", "BOIADEIRO FM", "TROPEIRO FM", "INTERIOR FM", "SOLO FM", "DUO FM"
};
const int SERTANEJO_PS_COUNT = sizeof(sertanejoPS) / sizeof(sertanejoPS[0]);

const char* jazzPS[] = {
  "JAZZ FM", "IMPROVISO FM", "SAX FM", "BLUE NOTE FM", "SWING FM", "COOL FM",
  "BEBOP FM", "LATIN JAZZ", "FUSION FM", "GROOVE FM", "MODAL FM", "STANDARDS FM"
};
const int JAZZ_PS_COUNT = sizeof(jazzPS) / sizeof(jazzPS[0]);

const char* sportPS[] = {
  "SPORT FM", "MATCH FM", "GOAL FM", "ARENA FM", "CAMPO FM", "PLACAR FM",
  "CAMPEAO FM", "VITORIA FM", "GOL FM", "LANCE FM", "JOGADA FM", "CRAQUE FM"
};
const int SPORT_PS_COUNT = sizeof(sportPS) / sizeof(sportPS[0]);

const char* talkPS[] = {
  "TALK FM", "NEWS TALK", "DEBATE FM", "VOICE FM", "CHAT FM", "FORUM FM",
  "PALAVRA FM", "CONVERSA FM", "ENTREVISTA FM", "DIALOGO FM", "FALA FM", "AUDIO FM"
};
const int TALK_PS_COUNT = sizeof(talkPS) / sizeof(talkPS[0]);

// RT (RADIO TEXT) ARRAYS BY PTY
const char* popRT[] = {
  "OS MAIORES HITS POP", "MUSICA POP NO AR", "SUCESSOS ATUAIS", "HITS DO MOMENTO",
  "O MELHOR DO POP", "POP PARA VOCE", "RITMO POP SEMPRE", "POP 24 HORAS",
  "PURA ENERGIA POP", "CHART TOPPER", "NUMERO UM NO AR", "TOP 10 SEMPRE"
};
const int POP_RT_COUNT = sizeof(popRT) / sizeof(popRT[0]);

const char* rockRT[] = {
  "ROCK EM ALTA VOLTAGEM", "ENERGIA ROCK", "GUITARRA NO AR", "ROCK PURO",
  "PODER DO ROCK", "HEAVY SOUND", "AMP MAXIMA", "RIFF POWER", "STAGE VIVO",
  "ROCK 24 HORAS", "GUITAR LEGENDS", "AMPLIFICADO AO MAXIMO"
};
const int ROCK_RT_COUNT = sizeof(rockRT) / sizeof(rockRT[0]);

const char* easyRT[] = {
  "MELODIAS SUAVES", "BOSSA NOVA ETERNA", "RELAXAMENTO TOTAL", "MUSICA CALMA",
  "HARMONIA E PAZ", "NOITES TRANQUILAS", "SUAVIDADE NO AR", "JAZZ SUAVE",
  "CORPO E MENTE", "ZEN MUSIC", "SERENIDADE", "SMOOTH VIBES"
};
const int EASY_RT_COUNT = sizeof(easyRT) / sizeof(easyRT[0]);

const char* classicalRT[] = {
  "CLASSICOS SINFONIA", "MUSICA REFINADA", "ORQUESTRA VIVA", "ERUDITO PURO",
  "MAESTRIA SONORA", "ARTE NO AR", "SINFONIA GRANDEZA", "ÓPERA E CONCERTO",
  "ELEGANCIA MUSICAL", "CULTURA CLASSICA", "SINFONIA SUPREMA", "OPUS PERFEITO"
};
const int CLASSICAL_RT_COUNT = sizeof(classicalRT) / sizeof(classicalRT[0]);

const char* countryRT[] = {
  "FORRÓ CEARENSE", "RITMO NORDESTINO", "DANCA NORDESTINA", "RAIZ DO SERTAO",
  "FORRÓ TRADICIONAL", "BATIDA CEARENSE", "TRIANGULO E ZABUMBA", "XOTE AUTÊNTICO",
  "BAIÃO DO SERTAO", "COCO DE RAIZ", "NORDESTE MUSICAL", "TRADIÇÃO VIVA"
};
const int COUNTRY_RT_COUNT = sizeof(countryRT) / sizeof(countryRT[0]);

const char* sertanejoRT[] = {
  "SERTANEJO RAIZ", "MUSICA SERTANEJA", "SERTAO SONORO", "CABAÇO E VIOLA",
  "ABOIO DO SERTAO", "DUOS SERTANEJOS", "CAMPO MELODIA", "VAQUEIRO CANTADOR",
  "BOIADEIRO SONORO", "CAVALGADA MUSICAL", "DUPLA TRADICIONAL", "INTERIOR VIVO"
};
const int SERTANEJO_RT_COUNT = sizeof(sertanejoRT) / sizeof(sertanejoRT[0]);

const char* jazzRT[] = {
  "JAZZ E IMPROVISO", "IMPROVISO PURO", "JAZZ CLASSICO", "SAXOFONE E HARMONIA",
  "JAZZ BRASILEIRO", "RITMO SINCOPADO", "SWING TIME", "BLUE NOTE",
  "JAZZ STANDARDS", "IMPROVISO CRIATIVO", "BEBOP AUTÊNTICO", "LATIN JAZZ VIVO"
};
const int JAZZ_RT_COUNT = sizeof(jazzRT) / sizeof(jazzRT[0]);

const char* sportRT[] = {
  "NOTICIA E ESPORTE", "ESPORTE EM TEMPO REAL", "PLACAR VIVO", "COBERTURA ESPORTIVA",
  "JOGOS E NOTICIAS", "ACAO NO ESPORTE", "ARENA EM DIRETO", "GOL E EMOÇÃO",
  "LANCE PERFEITO", "VITORIA NO AR", "CRAQUE DO MOMENTO", "CAMPEONATO VIVO"
};
const int SPORT_RT_COUNT = sizeof(sportRT) / sizeof(sportRT[0]);

const char* talkRT[] = {
  "CONVERSA E MUSICA", "MISTURA PERFEITA", "VARIEDADE SEMPRE", "DIALOGO MUSICAL",
  "TROCA DE IDEIAS", "PALAVRA NO AR", "FORUM DE VOZ", "ENTREVISTA VIVA",
  "DEBATE ABERTO", "VOZES DA CIDADE", "AUDIO CRIATIVO", "FALA INTELIGENTE"
};
const int TALK_RT_COUNT = sizeof(talkRT) / sizeof(talkRT[0]);

// SPORT
const char* sportArtists[] = {
  "Galvao Bueno", "Tiago Leifert", "Vania Neves", "Jota A", "Casio",
  "Mauro Beting", "Gustavo Villani", "Nilson Cesar", "Everaldo Marques", "Luizao",
  "Sormani", "Romario", "Ronaldo", "Pele", "Zico", "Ronaldinho", "Neymar", "Vinicius Jr"
};
const int SPORT_ARTISTS = sizeof(sportArtists) / sizeof(sportArtists[0]);

const char* sportSongs[] = {
  "Gol do Brasil", "Cobertura Esportiva", "Placar Ao Vivo", "Noticia Esporte",
  "Campeonato Brasileiro", "Copa do Brasil", "Champions League", "Libertadores",
  "Futebol Cearense", "Jogos do Nordeste", "Atletismo Brasileiro", "Volei Brasil",
  "Basket Nacional", "Tenis Ranking", "Natacao Olimpica", "Desporto Radical",
  "Acao e Tensao", "Momentos Gloriosos", "Vitoria Impressionante", "Derrota Inesperada"
};
const int SPORT_SONGS = sizeof(sportSongs) / sizeof(sportSongs[0]);

const char* sportGenres[] = {
  "Esporte", "Futebol", "Volei", "Basquete", "Tenis", "Natacao",
  "Atletismo", "Ciclismo", "MMA", "Boxe", "Esporte Radical", "Esporte Aquatico",
  "Cobertura Ao Vivo", "Analise Esportiva", "Noticia Esportiva", "Comentario Esportivo"
};
const int SPORT_GENRES = sizeof(sportGenres) / sizeof(sportGenres[0]);

const char* sportYears[] = {
  "2024", "2023", "2022", "2021", "2020", "2019", "2018", "2017", "2016", "2015",
  "2014", "2013", "2012", "2011", "2010", "2009", "2008", "2007", "2006", "2005",
  "2004", "2003", "2002", "2001", "2000", "1999", "1998", "1997", "1996", "1995"
};
const int SPORT_YEARS = sizeof(sportYears) / sizeof(sportYears[0]);

// NEWS (PTY 0)
const char* newsArtists[] = {
  "Jornal da Globo", "Jornal Nacional", "RBS Noticias", "Jornal do Ceara", "Bom Dia Brasil",
  "Jornal da Record", "SBT Brasil", "Band News", "CQC", "Diario de Noticias"
};
const int NEWS_ARTISTS = sizeof(newsArtists) / sizeof(newsArtists[0]);

const char* newsSongs[] = {
  "Manchete do Dia", "Noticias Atualizadas", "Boletim Informativo", "Destaques Noticias",
  "Brasil em Destaque", "Mundo em Pauta", "Economia Brasileira", "Politica Nacional",
  "Seguranca Publica", "Saude em Foco", "Educacao Brasil", "Tecnologia Atual"
};
const int NEWS_SONGS = sizeof(newsSongs) / sizeof(newsSongs[0]);

const char* newsGenres[] = {
  "Noticias", "Jornalismo", "Boletim Informativo", "Analise", "Debate Noticias", "Reportagem"
};
const int NEWS_GENRES = sizeof(newsGenres) / sizeof(newsGenres[0]);

const char* newsPS[] = {
  "NEWS FM", "BAND NEWS", "CBN BRASIL", "NOTICIA FM", "JORNAL FM",
  "RADIO NOTICIAS", "BROADCAST FM", "INFO FM", "ALERTA FM", "PAUTA FM"
};
const int NEWS_PS_COUNT = sizeof(newsPS) / sizeof(newsPS[0]);

const char* newsRT[] = {
  "NOTICIAS 24 HORAS", "JORNALISMO EM TEMPO REAL", "INFORMACAO SEMPRE",
  "BRASIL ATUALIZADO", "MANCHETES DO DIA", "MUNDO EM PAUTA"
};
const int NEWS_RT_COUNT = sizeof(newsRT) / sizeof(newsRT[0]);

// CURRENT AFFAIRS (PTY 1)
const char* currentAffairsGenres[] = {
  "Politica", "Assuntos Atuais", "Debate Publico", "Forum de Discussao", "Analise Politica",
  "Questoes Sociais", "Direitos Cidadanios", "Governa Nacional", "Legislacao", "Civismo"
};
const int CURRENT_AFFAIRS_GENRES_COUNT = sizeof(currentAffairsGenres) / sizeof(currentAffairsGenres[0]);

const char* currentAffairsPS[] = {
  "DEBATE FM", "ASSUNTOS FM", "POLITICA FM", "FORUM FM", "ANALISE FM",
  "PAUTA DIARIA FM", "VOZES BRASIL", "REDE DEBATE", "CONVERSA PUBLICA", "FORO FM"
};
const int CURRENT_AFFAIRS_PS_COUNT = sizeof(currentAffairsPS) / sizeof(currentAffairsPS[0]);

const char* currentAffairsRT[] = {
  "ASSUNTOS DO MOMENTO", "POLITICA E SOCIEDADE", "FORUM DE DEBATES",
  "QUESTOES ATUAIS", "BRASIL DISCUTE", "VOZES DO POVO"
};
const int CURRENT_AFFAIRS_RT_COUNT = sizeof(currentAffairsRT) / sizeof(currentAffairsRT[0]);

// INFORMATION (PTY 2)
const char* informationGenres[] = {
  "Informacao", "Boletim Informativo", "Guia Pratico", "Orientacao", "Consulta",
  "Referencia", "Dados Uteis", "Tutorial", "Manual", "Instrucoes"
};
const int INFORMATION_GENRES_COUNT = sizeof(informationGenres) / sizeof(informationGenres[0]);

const char* informationPS[] = {
  "INFO FM", "GUIA FM", "SERVICOS FM", "ORIENTACAO FM", "DICA FM",
  "BRASIL INFORMA", "DADOS FM", "CONHECIMENTO FM", "REFERENCIA FM", "SAIBA FM"
};
const int INFORMATION_PS_COUNT = sizeof(informationPS) / sizeof(informationPS[0]);

const char* informationRT[] = {
  "INFORMACAO UTIL", "GUIA PRATICO", "ORIENTACOES DIARIAS",
  "SERVICOS AO PUBLICO", "DICAS VALIOSAS", "CONHECIMENTO NO AR"
};
const int INFORMATION_RT_COUNT = sizeof(informationRT) / sizeof(informationRT[0]);

// EDUCATION (PTY 4)
const char* educationArtists[] = {
  "Professor Nilson", "Dra. Maria Silva", "Prof. Joao Santos", "Especialista Ana", "Mestre Paulo"
};
const int EDUCATION_ARTISTS = sizeof(educationArtists) / sizeof(educationArtists[0]);

const char* educationSongs[] = {
  "Aula de Matematica", "Historia do Brasil", "Fisica Explicada", "Quimica Facil",
  "Biologia Viva", "Literatura Classica", "Lingua Portuguesa", "Artes Visuais"
};
const int EDUCATION_SONGS = sizeof(educationSongs) / sizeof(educationSongs[0]);

const char* educationPS[] = {
  "EDUCACAO FM", "APRENDIZADO FM", "ESCOLA NO AR", "SABER FM", "UNIVERSIDADE FM",
  "CULTURA EDUCATIVA", "PEDAGOGIA FM", "CONHECER FM", "AULA FM", "ESTUDO FM"
};
const int EDUCATION_PS_COUNT = sizeof(educationPS) / sizeof(educationPS[0]);

const char* educationRT[] = {
  "APRENDIZADO CONTINUO", "EDUCACAO SEMPRE", "CONHECIMENTO COMPARTILHADO",
  "AULAS E ENSINAMENTOS", "SABEDORIA NO AR", "FORMACAO INTEGRAL"
};
const int EDUCATION_RT_COUNT = sizeof(educationRT) / sizeof(educationRT[0]);

const char* educationGenres[] = {
  "Educacao", "Aprendizado", "Aula", "Ensino", "Academico", "Pedagogia",
  "Instrucao", "Formacao", "Conhecimento", "Ensino Superior"
};
const int EDUCATION_GENRES = sizeof(educationGenres) / sizeof(educationGenres[0]);

// DRAMA (PTY 5)
const char* dramaArtists[] = {
  "Ator Classico", "Atriz Tradicional", "Diretor Drama", "Compositor Dramatico", "Roteirista Cenas"
};
const int DRAMA_ARTISTS = sizeof(dramaArtists) / sizeof(dramaArtists[0]);

const char* dramaSongs[] = {
  "Tragedia Sonora", "Romance Dramatico", "Conflito Emocional", "Tensao Narrativa",
  "Historia Comovente", "Drama Intenso", "Momento Critico", "Resolucao Dramatica"
};
const int DRAMA_SONGS = sizeof(dramaSongs) / sizeof(dramaSongs[0]);

const char* dramaPS[] = {
  "DRAMA FM", "TEATRO FM", "CENAS FM", "NARRATIVA FM", "EMOCAO FM",
  "HISTORIA FM", "PALCO NO AR", "TENSAO FM", "ATOR FM", "ENCENA FM"
};
const int DRAMA_PS_COUNT = sizeof(dramaPS) / sizeof(dramaPS[0]);

const char* dramaRT[] = {
  "DRAMAS E HISTORIAS", "TEATRO NO AR", "NARRATIVAS COMOVENTES",
  "EMOCOES DRAMATICAS", "CENAS INTENSAS", "PALCO SONORO"
};
const int DRAMA_RT_COUNT = sizeof(dramaRT) / sizeof(dramaRT[0]);

const char* dramaGenres[] = {
  "Drama", "Teatro", "Tragedia", "Comedia Dramatica", "Narrativa", "Producao Teatral",
  "Encenacao", "Historia Dramatica", "Monodrama", "Peça Teatral"
};
const int DRAMA_GENRES = sizeof(dramaGenres) / sizeof(dramaGenres[0]);

// CULTURE (PTY 6)
const char* cultureArtists[] = {
  "Gilberto Gil", "Caetano Veloso", "Clara Nunes", "Beth Carvalho", "Tom Jobim"
};
const int CULTURE_ARTISTS = sizeof(cultureArtists) / sizeof(cultureArtists[0]);

const char* cultureSongs[] = {
  "Tropicalia", "Tropicalismo", "Expresao Cultural", "Heranca Brasileira", "Arte Popular",
  "Manifestacao Artistica", "Patrimonio Cultural", "Identidade Nacional", "Expressao Criativa"
};
const int CULTURE_SONGS = sizeof(cultureSongs) / sizeof(cultureSongs[0]);

const char* culturePS[] = {
  "CULTURA FM", "ARTE FM", "PATRIMONIO FM", "BRASIL CULTURAL", "EXPRESSAO FM",
  "MANIFESTACAO FM", "CRIATIVIDADE FM", "TRADICAO FM", "IDENTIDADE FM", "MUSA FM"
};
const int CULTURE_PS_COUNT = sizeof(culturePS) / sizeof(culturePS[0]);

const char* cultureRT[] = {
  "CULTURA VIVA", "ARTE E TRADICION", "EXPRESSAO CULTURAL BRASILEIRA",
  "PATRIMONIO NO AR", "CRIATIVIDADE SEM LIMITE", "BRASIL CULTURAL"
};
const int CULTURE_RT_COUNT = sizeof(cultureRT) / sizeof(cultureRT[0]);

const char* cultureGenres[] = {
  "Cultura", "Arte Brasileira", "Patrimonio Cultural", "Manifestacao Cultural", "Tropicalismo",
  "Musica Tradicional", "Folclore", "Heranca Brasileira", "Identidade Nacional", "Expressao Artistica"
};
const int CULTURE_GENRES = sizeof(cultureGenres) / sizeof(cultureGenres[0]);

// SCIENCE (PTY 7)
const char* scienceArtists[] = {
  "Dr. Astronomia", "Prof. Fisica", "Dra. Biologia", "Prof. Quimica", "Dr. Tecnologia"
};
const int SCIENCE_ARTISTS = sizeof(scienceArtists) / sizeof(scienceArtists[0]);

const char* scienceSongs[] = {
  "Descoberta Cientifica", "Universo Expandido", "Inovacao Tecnologica", "Teoria Explicada",
  "Experimento Fascinante", "Conhecimento Profundo", "Avanco Cientifico", "Pesquisa Atual"
};
const int SCIENCE_SONGS = sizeof(scienceSongs) / sizeof(scienceSongs[0]);

const char* sciencePS[] = {
  "CIENCIA FM", "LABORATORIO FM", "INOVACAO FM", "TECNOLOGIA FM", "SABEDORIA FM",
  "DESCOBERTA FM", "PESQUISA FM", "CONHECIMENTO FM", "FUTURO FM", "EVOLUCAO FM"
};
const int SCIENCE_PS_COUNT = sizeof(sciencePS) / sizeof(sciencePS[0]);

const char* scienceRT[] = {
  "CIENCIA E INOVACAO", "DESCOBERTAS CIENTIFICAS", "TECNOLOGIA NO AR",
  "PESQUISA AVANCADA", "CONHECIMENTO CIENTIFICO", "FUTURO DA CIENCIA"
};
const int SCIENCE_RT_COUNT = sizeof(scienceRT) / sizeof(scienceRT[0]);

const char* scienceGenres[] = {
  "Ciencia", "Astronomia", "Fisica", "Biologia", "Quimica", "Tecnologia",
  "Inovacao", "Pesquisa", "Descoberta", "Conhecimento Cientifico"
};
const int SCIENCE_GENRES = sizeof(scienceGenres) / sizeof(scienceGenres[0]);

// OTHER MUSIC (PTY 9)
const char* otherMusicGenres[] = {
  "Fusao Musical", "Experimental", "Eclético", "Criativo", "Alternativo",
  "Inovador", "Hibrido", "Contemporaneo", "Vanguarda", "Sintetico"
};
const int OTHER_MUSIC_GENRES_COUNT = sizeof(otherMusicGenres) / sizeof(otherMusicGenres[0]);

const char* otherMusicPS[] = {
  "FUSAO FM", "MISTURA FM", "RITMO LIVRE FM", "ESTILO FM", "DIVERSIDADE FM",
  "ECLÉTICO FM", "FUSION FM", "CRIATIVO FM", "INDIE FM", "ALTERNATIVO FM"
};
const int OTHER_MUSIC_PS_COUNT = sizeof(otherMusicPS) / sizeof(otherMusicPS[0]);

const char* otherMusicRT[] = {
  "MUSICA SEM LIMITES", "RITMOS DIVERSOS", "FUSAO SONORA",
  "CRIATIVIDADE MUSICAL", "ESTILOS VARIADOS", "RITMO LIVRE"
};
const int OTHER_MUSIC_RT_COUNT = sizeof(otherMusicRT) / sizeof(otherMusicRT[0]);

// VARIED SPEECH
const char* variedSpeechArtists[] = {
  "Marilia Mendonca", "Simone Mendes", "Tony Tornado", "Projota", "Rael",
  "Criolo", "Emicida", "BK", "DBS", "Pocah", "Ludmilla", "Kevinho",
  "Pabllo Vittar", "Anitta", "MC Zaac", "Tropkillaz", "Maejor", "DJ Snake"
};
const int VARIED_SPEECH_ARTISTS = sizeof(variedSpeechArtists) / sizeof(variedSpeechArtists[0]);

const char* variedSpeechSongs[] = {
  "Conversas do Dia", "Mistura Musical", "Conversa e Musica", "Dialogo Sonoro",
  "Bate-Papo Musical", "Troca de Ideias", "Reflexao e Ritmo", "Fala e Danca",
  "Noite de Conversa", "Variedade Total", "Surpresa Musical", "Encontro de Sons",
  "Ritmo e Fala", "Musica Falada", "Prosa Melódica", "Sentimento e Som"
};
const int VARIED_SPEECH_SONGS = sizeof(variedSpeechSongs) / sizeof(variedSpeechSongs[0]);

const char* variedSpeechGenres[] = {
  "Conversas", "Talk Music", "Musica Falada", "Hip-Hop", "Rap", "Trap",
  "Funk", "Funk Carioca", "Trap Soul", "Lo-Fi", "Indie Alternativo",
  "Podcast Musical", "Debate Sonoro", "Entrevista Musical", "Cronica Cantada"
};
const int VARIED_SPEECH_GENRES = sizeof(variedSpeechGenres) / sizeof(variedSpeechGenres[0]);

const char* variedSpeechYears[] = {
  "2024", "2023", "2022", "2021", "2020", "2019", "2018", "2017", "2016", "2015",
  "2014", "2013", "2012", "2011", "2010", "2009", "2008", "2007", "2006", "2005",
  "2004", "2003", "2002", "2001", "2000", "1999", "1998", "1997", "1996", "1995"
};
const int VARIED_SPEECH_YEARS = sizeof(variedSpeechYears) / sizeof(variedSpeechYears[0]);

// WEATHER (PTY 16)
const char* weatherGenres[] = {
  "Meteorologia", "Previsao Tempo", "Clima", "Condicoes Atmosfericas", "Boletim Meteorologico",
  "Condicoes Clima", "Temperaturas", "Umidade", "Vento", "Precipitacao"
};
const int WEATHER_GENRES_COUNT = sizeof(weatherGenres) / sizeof(weatherGenres[0]);

const char* weatherPS[] = {
  "TEMPO FM", "CLIMA FM", "METEOROLOGIA FM", "PREVISAO FM", "TEMPO HOJE FM",
  "ATMOSFERA FM", "CONDICOES FM", "FORECAST FM", "CELSIUS FM", "CLIMA BRASIL"
};
const int WEATHER_PS_COUNT = sizeof(weatherPS) / sizeof(weatherPS[0]);

const char* weatherRT[] = {
  "PREVISAO DO TEMPO", "CONDICOES ATMOSFERICAS", "TEMPO EM TEMPO REAL",
  "INFORMACOES METEOROLOGICAS", "CLIMA ATUAL", "TEMPO E TEMPERATURE"
};
const int WEATHER_RT_COUNT = sizeof(weatherRT) / sizeof(weatherRT[0]);

// FINANCE (PTY 17)
const char* financeGenres[] = {
  "Economia", "Mercado Financeiro", "Investimento", "Bolsa", "Negocios",
  "Cotacoes", "Economia Brasil", "Financa Pessoal", "Empreendimento", "Renda"
};
const int FINANCE_GENRES_COUNT = sizeof(financeGenres) / sizeof(financeGenres[0]);

const char* financePS[] = {
  "BOLSA FM", "ECONOMIA FM", "INVESTIMENTO FM", "MERCADO FM", "FINANCEIRO FM",
  "DOLAR FM", "ACAO FM", "NEGOCIOS FM", "ECONOMIA BRASIL", "COTACAO FM"
};
const int FINANCE_PS_COUNT = sizeof(financePS) / sizeof(financePS[0]);

const char* financeRT[] = {
  "COTACOES ATUALIZADAS", "MERCADO FINANCEIRO", "ECONOMIA BRASILEIRA",
  "NOTICIAS DE NEGOCIOS", "INVESTIMENTOS HOJE", "BOLSA EM TEMPO REAL"
};
const int FINANCE_RT_COUNT = sizeof(financeRT) / sizeof(financeRT[0]);

// CHILDREN'S (PTY 18)
const char* childrenGenres[] = {
  "Infantil", "Criancas", "Educativo", "Historias Infantis", "Musica Infantil",
  "Brincadeira", "Aprendizado", "Diversao", "Fantasia", "Aventura Infantil"
};
const int CHILDREN_GENRES_COUNT = sizeof(childrenGenres) / sizeof(childrenGenres[0]);

const char* childrenPS[] = {
  // Original stations
  "INFANTIL FM", "KIDS FM", "CRIANCAS FM", "TURMINHA FM", "ALEGRIA FM",
  "BRINCADEIRA FM", "SOMOS FM", "PEQUENO FM", "CIRCO FM", "FANTASIA FM",

  // Fun & Playful
  "DIVERSAO FM", "RISADA FM", "BRINCADEIRA FELIZ", "PULA PULA FM",
  "GIRA GIRA FM", "DANCA FM", "MUSICA FELIZ", "SUPER KIDS",
  "MEGA KIDS", "CRIANCADA FM", "TURMINHA UNIDA", "AMIGOS FM",

  // Educational
  "APRENDIZADO FM", "SABER FM", "ESCOLA NO AR", "CONHECIMENTO FM",
  "ESTUDO FM", "AULA FM", "PROFESSOR FM", "EDUCACAO FM",
  "INTELIGENCIA FM", "GENIO PEQUENO", "SABEDORIA FM", "CRIANCA SBIA",

  // Adventure & Fantasy
  "AVENTURA FM", "HEROI FM", "SUPER HEROI", "PODER FM",
  "MAGIA FM", "ENCANTO FM", "SONHO FM", "UNIVERSO FM",
  "PLANETA FM", "GALAXIA FM", "ESPACO FM", "ASTRONAUTA FM",

  // Animals & Nature
  "BICHO FM", "ZOO FM", "FLORESTA FM", "NATUREZA FM",
  "PASSARO FM", "PEIXE FM", "ABELHA FM", "BORBOLETA FM",
  "ANIMAL FM", "FAUNA FM", "SELVAGEM FM", "MUNDO ANIMAL",

  // Classic Characters
  "TURMA MONICA", "XUXA RADIO", "BALAO MAGICO FM", "GALINHA PINTADINHA",
  "MUNDO BITA FM", "PALAVRA CANTADA FM", "PATATI PATATA FM", "TOQUINHO FM",
  "GILBERTO GIL FM", "RITA LEE FM", "ANITTA KIDS FM", "IVETE FM",

  // Time-based
  "MANHA FELIZ FM", "TARDE LEGAL FM", "NOITE SONHO FM", "ACORDAR FM",
  "DURMIR FM", "CAFEZINHO FM", "LANCHE FM", "REFEICAO FM",

  // Emotion & Values
  "CORACAO FM", "AMIZADE FM", "AMOR FM", "RESPEITO FM",
  "HONRA FM", "VALENTIA FM", "HONESTIDADE FM", "LEALDADE FM",
  "SOLIDARIEDADE FM", "GENTILEZA FM", "CARINHO FM", "ABRACO FM",

  // Cultural & Festive
  "CARNAVAL FM", "FESTA FM", "CELEBRACAO FM", "ALEGRIA TOTAL",
  "ENCONTRO FM", "REUNIAO FM", "CONFRATERNIZACAO FM", "FESTA INFANTIL",
  "ANIVERSARIO FM", "PARABENS FM", "PRESENTE FM", "SURPRESA FM",

  // Imaginative
  "CONTE FM", "HISTORINHA FM", "FADA FM", "BRUXA FM",
  "GIGANTE FM", "ANAO FM", "PRINCESA FM", "PRINCIPE FM",
  "CASTELO FM", "REINO FM", "IMPERIO FM", "REINO MAGICO",

  // Creative
  "CORES FM", "DESENHO FM", "ARTE FM", "CRIATIVIDADE FM",
  "MUSICA CRIATIVA", "RITMO FM", "MELODIA FM", "HARMONIA FM",
  "INSTRUMENTO FM", "CANTAR FM", "DANCA CRIATIVA", "MOVIMENTO FM",

  // Active Play
  "CORRER FM", "PULAR FM", "SALTAR FM", "EQUILIBRIO FM",
  "FORCE FM", "VELOCIDADE FM", "AGILIDADE FM", "COORDENACAO FM",
  "ESPORTE FM", "CAMPEONATO FM", "VITORIA FM", "CAMPEAO FM",

  // Rest & Comfort
  "DESCANSO FM", "RELAX FM", "CALMA FM", "PAZ FM",
  "TRANQUILIDADE FM", "SERENIDADE FM", "SONO FM", "REPOUSO FM",
  "CONFORTO FM", "ACONCHEGO FM", "SEGURANCA FM", "ABRACO CALOROSO",

  // Special Topics
  "DINOSSAURO FM", "CAVERNA FM", "TESOURO FM", "PIRATA FM",
  "NAVIO FM", "VIAGEM FM", "EXPLORACAO FM", "MISTERIO FM",
  "ENIGMA FM", "DESAFIO FM", "COMPETICAO FM", "TORNEIO FM",

  // Miscellaneous
  "RADIA MAGICA", "ONDA FELIZ", "SINTONIA CRIANCA", "CANAL INFANTIL",
  "STATION KIDS", "EMISSORA PEQUENA", "RADIO MIUDA", "VOZ INFANTIL",
  "TRANSMISSAO FELIZ", "SINAL ALEGRE", "FREQUENCIA CRIANCA", "BANDA SONORA"
};
const int CHILDREN_PS_COUNT = sizeof(childrenPS) / sizeof(childrenPS[0]);

const char* childrenRT[] = {
  // Original
  "MUSICA INFANTIL", "HISTORIAS PARA CRIANCAS", "DIVERSAO E APRENDIZADO",
  "MUNDO INFANTIL", "DIVERSAO SEM PARAR", "CRIANCAS FELIZES",

  // Fun & Entertainment
  "BRINCANDO E CANTANDO", "RISADAS E MUSICA", "DIVERSAO TOTAL AQUI",
  "ALEGRIA EM CADA NOTA", "FESTA INFANTIL NO AR", "DIVERSAO SEM LIMITES",
  "CRIANCADA DANCA", "MUSICA QUE ALEGRA", "ENTRETENIMENTO PURO",
  "HUMOR E MUSICA", "GRITO DE ALEGRIA", "FESTA SEM PARAR",

  // Educational Messages
  "APRENDENDO BRINCANDO", "EDUCACAO COM ALEGRIA", "CONHECIMENTO DIVERTIDO",
  "SABER E BRINCAR", "AULA MUSICAL FELIZ", "CRIANCA APRENDE AQUI",
  "EDUCACAO INFANTIL", "MENTE CRIATIVA SEMPRE", "CONHECIMENTO CRIANCAS",
  "LETRAS E NUMEROS", "APRENDIZADO TOTAL", "INTELIGENCIA PURA",

  // Adventure & Magic
  "AVENTURAS EMOCIONANTES", "MAGIA NO AR", "MUNDO FANTASTICO",
  "HISTORIAS INCRIVEIS", "CONTOS DE FADAS", "REINOS MAGICOS",
  "HEROIS CRIANCAS", "SUPER PODERES", "MISSAO CRIANCA",
  "JORNADA FANTASIA", "TRAVESSIA MAGICA", "PORTAL ENCANTADO",

  // Values & Emotions
  "AMOR E AMIZADE", "CARINHO CRIANCA", "RESPEITO SEMPRE",
  "CORACAO FELIZ", "PAZ E HARMONIA", "GENTILEZA CRIANCA",
  "HONESTIDADE CRIANCA", "BRAVURA INFANTIL", "LEALDADE PURA",
  "SOLIDARIEDADE CRIANCA", "COMPARTILHANDO AMOR", "UNIAO CRIANCAS",

  // Nature & Animals
  "MUNDO ANIMAL AQUI", "NATUREZA CRIANCA", "ZOO MUSICAL",
  "ANIMAIS CANTAM", "FLORESTA VIVA", "PASSAROS FELIZES",
  "MUNDO SELVAGEM", "NATUREZA PURA", "AMBIENTE CRIANCA",
  "PLANTAS E CRIANCAS", "ECOSISTEMA INFANTIL", "VERDE E VIDA",

  // Action & Movement
  "DANCAS CRIANCAS", "MOVIMENTO INFANTIL", "ENERGIA PURA",
  "CRIANCA SE MEXE", "ESPORTES CRIANCAS", "ACAO INFANTIL",
  "DINAMICA PURA", "RITMO E MOVIMENTO", "CORPO FELIZ",
  "EXERCICIO DIVERTIDO", "AGILIDADE CRIANCA", "VELOCIDADE BRINCAR",

  // Time & Routine
  "BOM DIA CRIANCAS", "ACORDAR CANTANDO", "MANHA FELIZ",
  "LANCHINHOS HORA", "LANCHE CRIANCA", "REFEICAO MUSICAL",
  "BRINCADEIRA TARDE", "TARDE CRIANCA", "VESPERA FELIZ",
  "NOITE TRANQUILA", "DORMIR CANTANDO", "SONO DOCE",

  // Festive Messages
  "CARNAVAL CRIANCA", "FESTA COLORIDA", "CELEBRACAO INFANTIL",
  "ANIVERSARIO CRIANCA", "PARABENS AQUI", "PRESENTES MUSICA",
  "NATAL INFANTIL", "PASCOA CRIANCA", "FESTAS SEMPRE",
  "ENCONTRO CRIANCA", "REUNIAO FELIZ", "CONFRATERNIZACAO PURA",

  // Creative & Imagination
  "CRIATIVIDADE PURA", "IMAGINACAO CRIANCA", "FANTASIA AQUI",
  "CORES DO ARCO IRIS", "DESENHO ANIMADO", "ARTE CRIANCA",
  "MUSICA CRIATIVIDADE", "CRIACAO MUSICAL", "EXPRESSAO INFANTIL",
  "TALENTO CRIANCA", "INTELIGENCIA CRIATIVA", "GENIO PEQUENO",

  // Stories & Tales
  "HISTORIAS INCRIVEIS", "CONTOS FANTASIA", "NARRATIVAS CRIANCA",
  "LENDAS INFANTIS", "FABULAS CRIANCAS", "MITOS CRIANCA",
  "AVENTURA HISTÓRIA", "DRAMA CRIANCA", "ROMANCE INFANTIL",
  "SUSPENSE CRIANCA", "MISTÉRIO INFANTIL", "ENIGMA CRIANCA",

  // Special Themes
  "DINOSSAUROS MUSICAIS", "ERA JURASSICA", "CAVERNA SONS",
  "TESOURO CRIANCA", "PIRATAS CRIANCAS", "VIAGEM OCEANO",
  "ESPACIO CRIANCA", "ASTRONAUTAS AQUI", "PLANETA MUSICA",
  "SUBMARINA CRIANCA", "FUNDO MAR", "OCEANO FELIZ",

  // Encouragement & Motivation
  "CRIANCA PODE", "TENTA SEMPRE", "NUNCA DESISTA",
  "ACREDITA EM TI", "VOCE CONSEGUE", "PODER CRIANCA",
  "CAMPEAO SEMPRE", "VITORIA CRIANCA", "SUCESSO INFANTIL",
  "BRAVURA CRIANCA", "CORAGEM AQUI", "DETERMINACAO PURA",

  // Miscellaneous
  "SINTONIA CRIANCA", "FREQUENCIA FELIZ", "ONDA INFANTIL",
  "SINAL ALEGRE", "TRANSMISSAO CRIANCA", "VOZ CRIANCA",
  "MUSICA 24 HORAS", "SEM PARAR AQUI", "SEMPRE CRIANCA",
  "CRIANCAS UNIDAS", "TURMA FELIZ", "FAMILIA CRIANCA"
};
const int CHILDREN_RT_COUNT = sizeof(childrenRT) / sizeof(childrenRT[0]);

// CHILDREN'S ARTISTS
const char* childrenArtists[] = {
  // Brazilian Artists
  "Turma da Monica", "Xuxa", "Balao Magico", "Toquinho", "Gilberto Gil",
  "Anitta Kids", "Ivete Sangalo", "Tribu Batuera", "Palavra Cantada", "Mundo Bita",
  "Galinha Pintadinha", "Elefantinho Colorido", "Pequeno Ciencias", "Brincadeira Musical",
  "Patati Patata", "Amigas da Onça", "Rá Tim Bum", "Criancas Felizes",
  "Tatiana", "Angra dos Reis", "Chico Science", "Nando Reis",
  "Silva", "Arnaldo Antunes", "Criança Esperança", "Turma do Mickey",
  "Circo da Fantasia", "Show da Crianca", "Magico Ramirez", "Boneco Seu Lobo",
  "Aquela Crianca", "Ceu da Boca", "Meu Lindo Balao", "Brincar de Viver",
  "Rita Lee Kids", "Tom Zé Criancas", "Tomzinho", "Guilherme Arantes",
  "Zé da Viola", "Raul Seixas Infantil", "Renato Russo Kids", "Capital Inicial Kids",
  "Emilio Santiago", "Dalila Vieira", "Paulo Santana", "Ze Rodrix",
  "Silvio Santos", "Gugu Liberato", "Manoel Neves", "Pelé",
  "Ayrton Senna", "Ronaldo Fenomeno", "Ronaldinho Gaucho", "Neymar Jr",
  "Vinicius Junior", "Anderson Silva", "Oscar Schmidt", "Hortencia",
  "Soraya Moraes", "Kemilly Santos", "Priscilla Alcantara", "Leandro Forró",

  // International Artists
  "Barney", "Bob Zoom", "Sessame Street", "Mister Rogers",
  "Disney Characters", "Hello Kitty", "SpongeBob", "Mickey Mouse",
  "Minnie Mouse", "Donald Duck", "Goofy", "Pluto",
  "Toy Story Cast", "Frozen Cast", "Aladdin", "The Lion King",
  "Cars", "Finding Nemo", "Shrek", "Madagaskar",
  "Kung Fu Panda", "How To Train Your Dragon", "Moana", "Encanto",
  "Peppa Pig", "Thomas Tank Engine", "Caillou", "Dora Explorer",
  "Go Diego Go", "Boots", "Swiper", "Map",
  "Power Rangers", "Teenage Mutant Ninja Turtles", "Transformers", "Voltron",
  "He-Man", "She-Ra", "Inspector Gadget", "Scooby Doo",
  "Tom Jerry", "Looney Tunes", "Cartoon Network", "Boomerang",
  "Ben Ten", "Generator Rex", "Dexter Lab", "Samurai Jack",
  "Adventure Time", "Steven Universe", "Gravity Falls", "The Owl House",
  "Avatar Aang", "Avatar Korra", "Legend of Korra", "She Ra Princesses",
  "DC Super Hero Girls", "Marvel Kids", "Avengers", "X Men",
  "Spiderman", "Batman", "Superman", "Wonder Woman",
  "Capitao America", "Iron Man", "Thor", "Hulk",
  "Viuva Negra", "Gaviao Negro", "Pantera Negra", "Capitao Marvel",
  "Doc McStuffins", "Sofia First", "Elena of Avalor", "Tangled",
  "Cinderela", "Bela e Fera", "Rapunzel", "Ariel",
  "Jasmine", "Branca de Neve", "Chapeuzinho Vermelho", "Gato de Botas",
  "Shrek", "Puss in Boots", "Donkey", "Fiona",
  "Madagascar Penguins", "Marty Zebra", "Melman Girafa", "Gloria Hippo",
  "Kung Fu Panda Characters", "Po", "Tigresa", "Furacao",
  "Stuart Little", "Rato Mickey", "Rato Ratazana", "Tom Gato",
  "Jerry Rato", "Sylvester Gato", "Tweety Passaro", "Pernalonga",
  "Trix Coelho", "Toucan Sam", "Cap'n Crunch", "Lucky Cereal",
  "Cookie Monster", "Big Bird", "Oscar Grouch", "Bert Ernie",
  "Elmo", "Dorothy Sapato", "Coruja Noturna", "Saci Perere",
  "Curupay", "Boitatá", "Iara Sereia", "Mula Sem Cabeca",
  "Lobisomem", "Mapinguari", "Yara", "Cabra Cabriola",
  "Bruxinha Atrapalhada", "Bruxa Malvada", "Mago Magico", "Princesa Encantada",
  "Sapo Carnê", "Abelha Zuada", "Lagarta Verde", "Borboleta Magica",
  "Unicornio Arco Iris", "Dragao Amigavel", "Fenix Lendária", "Griffo Protetor",
  "Centauro Sábio", "Sereia Bela", "Tritão", "Nereida",
  "Sátiro Dançante", "Ninfa Florestal", "Duende Travesso", "Gnomo Barudo",
  "Troll Amigável", "Ogro Inteligente", "Gigante Bom", "Titã Protetor",
  "Kraken Amigável", "Leviata Paz", "Fênix Renascida", "Hidra Calmosa"
};
const int CHILDREN_ARTISTS = sizeof(childrenArtists) / sizeof(childrenArtists[0]);

// CHILDREN'S SONGS
const char* childrenSongs[] = {
  // Classic Children's
  "Turma da Monica", "A Xuxa So Para Criancas", "Balao Magico", "Aquarela",
  "A Casa", "Canta Crianca", "Mundo Infantil", "Brincadeira de Crianca",
  "Historias Infantis", "Musica e Danca", "Ritmo Infantil", "Cantiggas de Roda",
  "Roda Gigante", "Carroussel", "Diversao Total", "Alegria das Criancas",
  "Aprendizado Feliz", "Educacao e Musica", "Fantasia Infantil", "Viagem Imaginaria",
  "Contos de Fadas", "Princesas e Principes", "Aventuras Magicas", "Tempo de Brincar",
  "Hora da Historinha", "Cantiga de Ninar", "Acordar Cantando", "Dormir Sorrindo",
  "Sol e Lua", "Noite Estrelada", "Brincadeira de Circo", "Pista de Danca",

  // Nursery Rhymes & Counting
  "Atirei o Pau no Gato", "Borboletinha", "Indiozinho", "Marcha Soldado",
  "Meu Chapeu", "Seu Jacó", "A Baratinha", "Uma Pulga na Careca",
  "Olha a Onca", "A Girafa", "Sapo Cururu", "Caixinha de Surpresa",
  "O Macaco Subiu", "Cinco Macaquinhos", "Um Dois Tres", "Quatro Cinco Seis",
  "Sete Oito Nove", "Dez Numerinhos", "Contando Ate Dez", "Numeros Cantados",

  // Disney & Animated
  "Let It Go", "Frozen Forte", "Mundo Novo", "A Distancia",
  "Quanto Basta", "Deixa Eu Sonhar", "Reino Magico", "Castelo Encantado",
  "Princesa de Ouro", "Sapato de Cristal", "Pequeno Mundo", "Cidade Dourada",
  "Floresta Profunda", "Savana Selvagem", "Oceano Azul", "Reino Submarino",

  // Learning Songs
  "ABC Alfabeto", "Letras Cantadas", "Palavras Novas", "Aprendo Cantando",
  "Cores do Arco Iris", "Formas Geometricas", "Tamanhos Diferentes", "Conceitos Basicos",
  "Corpo Humano", "Cinco Sentidos", "Higiene Pessoal", "Habitos Saudaveis",
  "Frutas e Legumes", "Alimentacao Saudavel", "Exercicio Fisico", "Sono Repousante",

  // Action & Movement
  "Danca da Vassoura", "Danca do Sapato", "Danca da Boneca", "Danca Maluca",
  "Pula Pula Pulando", "Pisa Forte", "Gira Que Gira", "Salta Que Salta",
  "Pula Salta Rola", "Estica e Encolhe", "Mexe o Braco", "Levanta a Perna",
  "Cabeca Ombro Joelho", "Dedo do Pe", "Mexe Tudo", "Danca Criativa",

  // Animal Songs
  "A Vaca Muu", "O Pato Qua", "O Gato Miau", "O Cachorro Au",
  "O Passaro Piu", "O Leao Rugindo", "O Elefante Trompando", "O Macaco Pula",
  "O Sapo Croa", "A Galinha Cloca", "O Galo Canta", "A Abelha Zunindo",
  "A Borboleta Voando", "O Peixe Nadando", "A Tartaruga Lenta", "O Cavalo Galopando",

  // Bedtime & Relaxing
  "Tutu Marambaia", "Bois da Cara Preta", "Nana Nenê", "Durme Durme Filhinho",
  "Luar da Noite", "Silencio na Noite", "Repouso Doce", "Sonho Tranquilo",
  "Embalalacô", "Carcara", "Noite Feliz", "Durme Nenem",
  "Quieta a Noite", "Dorme Querida", "Sono Profundo", "Repouso Eterno",

  // Seasonal & Festive
  "Natal Chegou", "Papai Noel", "Arvore de Natal", "Velas Acesas",
  "Carnaval Alegre", "Fantasias Coloridas", "Mascaras Bonitas", "Confete Caindo",
  "Ovo de Pascoa", "Coelho Feliz", "Flores Coloridas", "Primavera Chegou",
  "Sao Joao Festa", "Baloes Voando", "Fogueira Acesa", "Quadrilha Danca",

  // Disney Characters Themed
  "Mickey Aventura", "Minnie Danca", "Donald Travessura", "Goofy Gracioso",
  "Cinderela Magica", "Bela Encantada", "Ariel Subaquatica", "Jasmine Sonhadora",
  "Aladdin Heroe", "Rapunzel Liberta", "Moana Corajosa", "Frozen Irmaes",
  "Toy Story Amigos", "Nemo Perdido", "Shrek Ogro", "Madagascar Festa",

  // Nursery Games
  "Chicotinho Queimado", "Cabra Cabriola", "Esconde Esconde", "Batata Quente",
  "Passa Passa Gaveta", "Seu Lobo", "Cabra Cabriola", "Eu Conheço Um Jogo",
  "Pedra Papel Tesoura", "Eu Sou Pequenino", "Fui Morar Numa Casinha", "Lá Vem o Pato",
  "Sapo Carnê", "Tangolomango", "Navio Negreiro", "Carrossel Magico",

  // Sound & Onomatopoeia
  "Som do Vento", "Som da Chuva", "Som do Trovao", "Som do Raio",
  "Som do Carro", "Som do Trem", "Som do Aviao", "Som da Sirene",
  "Som do Sino", "Som do Sino Largo", "Som da Sino Agudo", "Tiquetaque do Relogio",
  "Tic Tac Hora", "Pingue Pongue", "Plim Plim", "Trim Trim",

  // Educational Concepts
  "Grande Pequeno Medio", "Cima Baixo Meio", "Dentro Fora Perto", "Longe Perto Aqui",
  "Dia Noite Madrugada", "Manha Tarde Noite", "Segunda Terca Quarta", "Quinta Sexta",
  "Sabado Domingo", "Meses do Ano", "Estacoes Natureza", "Clima Tempo Previsao"
};
const int CHILDREN_SONGS = sizeof(childrenSongs) / sizeof(childrenSongs[0]);

// CHILDREN'S YEARS
const char* childrenYears[] = {
  // Recent Years
  "2024", "2023", "2022", "2021", "2020", "2019", "2018", "2017", "2016", "2015",
  "2014", "2013", "2012", "2011", "2010", "2009", "2008", "2007", "2006", "2005",
  "2004", "2003", "2002", "2001", "2000",

  // 1990s
  "1999", "1998", "1997", "1996", "1995", "1994", "1993", "1992", "1991", "1990",

  // 1980s
  "1989", "1988", "1987", "1986", "1985", "1984", "1983", "1982", "1981", "1980",

  // 1970s
  "1979", "1978", "1977", "1976", "1975", "1974", "1973", "1972", "1971", "1970",

  // 1960s
  "1969", "1968", "1967", "1966", "1965", "1964", "1963", "1962", "1961", "1960",

  // 1950s
  "1959", "1958", "1957", "1956", "1955", "1954", "1953", "1952", "1951", "1950",

  // Classic Era (1940s and earlier for golden age animation)
  "1949", "1948", "1947", "1946", "1945", "1944", "1943", "1942", "1941", "1940"
};
const int CHILDREN_YEARS = sizeof(childrenYears) / sizeof(childrenYears[0]);

// SOCIAL AFFAIRS (PTY 19)
const char* socialGenres[] = {
  "Questoes Sociais", "Direitos Humanos", "Comunidade", "Solidariedade", "Cidadania",
  "Bem Comum", "Justica Social", "Inclusao", "Igualdade", "Responsabilidade"
};
const int SOCIAL_GENRES_COUNT = sizeof(socialGenres) / sizeof(socialGenres[0]);

const char* socialPS[] = {
  "SOCIAL FM", "SOCIEDADE FM", "COMUNIDADE FM", "SOLIDARIEDADE FM", "BEM FM",
  "AJUDA FM", "INCLUSAO FM", "DIREITOS FM", "CIDADANIA FM", "JUSTICA FM"
};
const int SOCIAL_PS_COUNT = sizeof(socialPS) / sizeof(socialPS[0]);

const char* socialRT[] = {
  "QUESTOES SOCIAIS", "COMUNIDADE EM FOCO", "DIREITOS HUMANOS",
  "SOLIDARIEDADE SOCIAL", "CIDADANIA ATIVA", "BEM COMUM NO AR"
};
const int SOCIAL_RT_COUNT = sizeof(socialRT) / sizeof(socialRT[0]);

// TRAVEL (PTY 21)
const char* travelGenres[] = {
  "Viagem", "Turismo", "Aventura", "Explorador", "Roteiro",
  "Destino", "Caminhada", "Experiencia", "Mochileiro", "Turista"
};
const int TRAVEL_GENRES_COUNT = sizeof(travelGenres) / sizeof(travelGenres[0]);

const char* travelPS[] = {
  "VIAGEM FM", "TURISMO FM", "PASSAGEM FM", "DESTINO FM", "EXPLORE FM",
  "ROTEIRO FM", "AVENTURA FM", "MAPA FM", "TRILHA FM", "MUNDO FM"
};
const int TRAVEL_PS_COUNT = sizeof(travelPS) / sizeof(travelPS[0]);

const char* travelRT[] = {
  "DICAS DE VIAGEM", "DESTINOS TURISTICOS", "EXPLORE O MUNDO",
  "ROTEIROS INCRIVEIS", "AVENTURAS GLOBAIS", "VIAGEM NO AR"
};
const int TRAVEL_RT_COUNT = sizeof(travelRT) / sizeof(travelRT[0]);

// LEISURE (PTY 22)
const char* leisureGenres[] = {
  "Lazer", "Entretenimento", "Relaxamento", "Estilo de Vida", "Moda",
  "Gastronomia", "Hobby", "Esporte", "Diversao", "Prazer"
};
const int LEISURE_GENRES_COUNT = sizeof(leisureGenres) / sizeof(leisureGenres[0]);

const char* leisurePS[] = {
  "LAZER FM", "DIVERSAO FM", "ENTRETENIMENTO FM", "RELAXAMENTO FM", "ESTILO FM",
  "MODA FM", "GASTRONOMIA FM", "VITROLA FM", "DIVERSAO 24H FM", "PRAZER FM"
};
const int LEISURE_PS_COUNT = sizeof(leisurePS) / sizeof(leisurePS[0]);

const char* leisureRT[] = {
  "ENTRETENIMENTO TOTAL", "DIVERSAO E LAZER", "ESTILO DE VIDA",
  "RELAXAMENTO COMPLETO", "TEMPO DE LAZER", "DIVERSAO SEM PARAR"
};
const int LEISURE_RT_COUNT = sizeof(leisureRT) / sizeof(leisureRT[0]);

// DOCUMENTARY (PTY 23)
const char* documentaryGenres[] = {
  "Documentario", "Historia", "Narrativa", "Investigacao", "Realidade",
  "Pesquisa", "Prova", "Fatos", "Registro", "Relato"
};
const int DOCUMENTARY_GENRES_COUNT = sizeof(documentaryGenres) / sizeof(documentaryGenres[0]);

const char* documentaryPS[] = {
  "DOCUMENTARIO FM", "HISTORIA FM", "NARRATIVA FM", "INVESTIGACAO FM", "PROVA FM",
  "REALIDADE FM", "FATOS FM", "PESQUISA FM", "VERDADE FM", "REGISTRO FM"
};
const int DOCUMENTARY_PS_COUNT = sizeof(documentaryPS) / sizeof(documentaryPS[0]);

const char* documentaryRT[] = {
  "DOCUMENTARIOS REAIS", "HISTORIAS VERDADEIRAS", "INVESTIGACAO PROFUNDA",
  "FATOS COMPROVADOS", "NARRATIVAS REAIS", "CONHECIMENTO DOCUMENTADO"
};
const int DOCUMENTARY_RT_COUNT = sizeof(documentaryRT) / sizeof(documentaryRT[0]);

// OLDIES (PTY 27)
const char* oldiesArtists[] = {
  "Tom Jobim", "Joao Gilberto", "Frank Sinatra", "Dean Martin", "Nat King Cole"
};
const int OLDIES_ARTISTS = sizeof(oldiesArtists) / sizeof(oldiesArtists[0]);

const char* oldiesSongs[] = {
  "Musicas Classicas", "Sucessos Atemporais", "Ouro do Passado", "Nostalgia Pura",
  "Memoria Musical", "Eternos Classicos", "Melhorias Passadas", "Recordacoes Musicais"
};
const int OLDIES_SONGS = sizeof(oldiesSongs) / sizeof(oldiesSongs[0]);

const char* oldiesGenres[] = {
  "Oldies", "Classicos", "Nostalgia", "Retro", "Vintage", "Successo Atemporal",
  "Musica Eterna", "Memorabilia", "Ouro da Musica", "Lendas do Passado"
};
const int OLDIES_GENRES = sizeof(oldiesGenres) / sizeof(oldiesGenres[0]);

const char* oldiesPS[] = {
  "OLDIES FM", "RETRO FM", "CLASSICOS FM", "MEMORIA FM", "NOSTALGIA FM",
  "OURO FM", "VINTAGE FM", "SAUDADE FM", "PASSADO FM", "TRADICAO FM"
};
const int OLDIES_PS_COUNT = sizeof(oldiesPS) / sizeof(oldiesPS[0]);

const char* oldiesRT[] = {
  "MUSICAS DO PASSADO", "SUCESSOS IMORTAIS", "NOSTALGIA PURA",
  "MEMORIA MUSICAL", "OURO DO RADIO", "ETERNOS SUCESSOS"
};
const int OLDIES_RT_COUNT = sizeof(oldiesRT) / sizeof(oldiesRT[0]);

// FOLK (PTY 28)
const char* folkGenres[] = {
  "Folclorica", "Musica Regional", "Tradicao Popular", "Heranca Cultural", "Raizes",
  "Arte Folclorica", "Expressao Regional", "Cultura Nordestina", "Musica Tradicional", "Patrimonio"
};
const int FOLK_GENRES_COUNT = sizeof(folkGenres) / sizeof(folkGenres[0]);

const char* folkArtists[] = {
  "Luiz Gonzaga", "Jackson do Pandeiro", "Elba Ramalho", "Emilio Santiago", "Marisa Monte"
};
const int FOLK_ARTISTS = sizeof(folkArtists) / sizeof(folkArtists[0]);

const char* folkSongs[] = {
  "Musica Folclorica", "Raizes Culturais", "Tradiao Popular", "Expressao Folclorica",
  "Heranca Regional", "Cultura Folclorica", "Ritmo Regional", "Arte Folclorica"
};
const int FOLK_SONGS = sizeof(folkSongs) / sizeof(folkSongs[0]);

const char* folkPS[] = {
  "FOLCLORE FM", "RAIZES FM", "CULTURA POPULAR FM", "TRADICAO FM", "REGIONAL FM",
  "FOLCLORICA FM", "NORDESTE FM", "HERANCA FM", "EXPRESSAO FM", "PATRIMONIO FM"
};
const int FOLK_PS_COUNT = sizeof(folkPS) / sizeof(folkPS[0]);

const char* folkRT[] = {
  "MUSICA FOLCLORICA BRASILEIRA", "RAIZES CULTURAIS", "TRADIÇÃO MUSICAL",
  "FOLCLORE VIVO", "CULTURA POPULAR", "EXPRESSÃO NORDESTINA", "HERANÇA REGIONAL",
  "ARTE FOLCLORICA", "RITMO TRADICIONAL", "PATRIMONIO CULTURAL"
};
const int FOLK_RT_COUNT = sizeof(folkRT) / sizeof(folkRT[0]);

// ALARM TEST (PTY 30)
const char* alarmTestGenres[] = {
  "Teste de Alerta", "Sistema de Emergencia", "Verificacao", "Teste Operacional"
};
const int ALARM_TEST_GENRES_COUNT = sizeof(alarmTestGenres) / sizeof(alarmTestGenres[0]);

const char* alarmTestPS[] = {
  "ALERTA FM", "TESTE FM", "EMERGENCIA FM", "AVISO FM", "SEGURANCA FM"
};
const int ALARM_TEST_PS_COUNT = sizeof(alarmTestPS) / sizeof(alarmTestPS[0]);

const char* alarmTestRT[] = {
  "TESTE DE ALERTA", "SISTEMA DE EMERGENCIA", "VERIFICACAO ATIVA"
};
const int ALARM_TEST_RT_COUNT = sizeof(alarmTestRT) / sizeof(alarmTestRT[0]);

// ALARM (PTY 31)
const char* alarmGenres[] = {
  "Alerta de Emergencia", "Aviso de Perigo", "Notificacao Critica", "Protecao"
};
const int ALARM_GENRES_COUNT = sizeof(alarmGenres) / sizeof(alarmGenres[0]);

const char* alarmPS[] = {
  "EMERGENCIA FM", "SOCORRO FM", "ALERTA ATIVO FM", "PROTECAO FM", "SEGURANCA FM"
};
const int ALARM_PS_COUNT = sizeof(alarmPS) / sizeof(alarmPS[0]);

const char* alarmRT[] = {
  "ALERTA ATIVO", "EMERGENCIA NO AR", "PROTECAO EM TEMPO REAL"
};
const int ALARM_RT_COUNT = sizeof(alarmRT) / sizeof(alarmRT[0]);

String generateRandomSong() {
  int artistIdx = random(0, ARTISTS_COUNT);
  int songIdx = random(0, SONGS_COUNT);
  int genreIdx = random(0, GENRES_COUNT);
  int yearIdx = random(0, YEARS_COUNT);
  return String(musicGenres[genreIdx]) + " | " + String(brazilianArtists[artistIdx]) + " - " + String(brazilianSongs[songIdx]) + " (" + String(brazilianYears[yearIdx]) + ")";
}

String getSongForPTY(uint8_t pty) {
  switch (pty) {
    case 0: {  // News
      int a = random(0, NEWS_ARTISTS);
      int s = random(0, NEWS_SONGS);
      return String(newsArtists[a]) + " - " + String(newsSongs[s]);
    }
    case 3: {  // Sport
      int a = random(0, SPORT_ARTISTS);
      int s = random(0, SPORT_SONGS);
      int g = random(0, SPORT_GENRES);
      int y = random(0, SPORT_YEARS);
      return String(sportGenres[g]) + " | " + String(sportArtists[a]) + " - " + String(sportSongs[s]) + " (" + String(sportYears[y]) + ")";
    }
    case 4: {  // Education
      int a = random(0, EDUCATION_ARTISTS);
      int s = random(0, EDUCATION_SONGS);
      return String(educationArtists[a]) + " - " + String(educationSongs[s]);
    }
    case 5: {  // Drama
      int a = random(0, DRAMA_ARTISTS);
      int s = random(0, DRAMA_SONGS);
      return String(dramaArtists[a]) + " - " + String(dramaSongs[s]);
    }
    case 6: {  // Culture
      int a = random(0, CULTURE_ARTISTS);
      int s = random(0, CULTURE_SONGS);
      return String(cultureArtists[a]) + " - " + String(cultureSongs[s]);
    }
    case 7: {  // Science
      int a = random(0, SCIENCE_ARTISTS);
      int s = random(0, SCIENCE_SONGS);
      return String(scienceArtists[a]) + " - " + String(scienceSongs[s]);
    }
    case 8:   // Varied Speech
    case 15: {  // Varied Speech
      int a = random(0, VARIED_SPEECH_ARTISTS);
      int s = random(0, VARIED_SPEECH_SONGS);
      int g = random(0, VARIED_SPEECH_GENRES);
      int y = random(0, VARIED_SPEECH_YEARS);
      return String(variedSpeechGenres[g]) + " | " + String(variedSpeechArtists[a]) + " - " + String(variedSpeechSongs[s]) + " (" + String(variedSpeechYears[y]) + ")";
    }
    case 10: {  // Pop Music
      int g = random(0, POP_GENRES_COUNT);
      int a = random(0, POP_ARTISTS);
      int s = random(0, POP_SONGS);
      return String(popGenres[g]) + " | " + String(popArtists[a]) + " - " + String(popSongs[s]);
    }
    case 11: {  // Rock Music
      int g = random(0, ROCK_GENRES_COUNT);
      int a = random(0, ROCK_ARTISTS);
      int s = random(0, ROCK_SONGS);
      return String(rockGenres[g]) + " | " + String(rockArtists[a]) + " - " + String(rockSongs[s]);
    }
    case 12: {  // Easy Listening
      int g = random(0, EASY_LISTENING_GENRES_COUNT);
      int a = random(0, EASY_ARTISTS);
      int s = random(0, EASY_SONGS);
      return String(easyListeningGenres[g]) + " | " + String(easyArtists[a]) + " - " + String(easySongs[s]);
    }
    case 13: {  // Light Classical
      int g = random(0, CLASSICAL_GENRES_COUNT);
      int a = random(0, CLASSICAL_ARTISTS);
      int s = random(0, CLASSICAL_SONGS);
      return String(classicalGenres[g]) + " | " + String(classicalArtists[a]) + " - " + String(classicalSongs[s]);
    }
    case 14: {  // Serious Classical
      int g = random(0, CLASSICAL_GENRES_COUNT);
      int a = random(0, CLASSICAL_ARTISTS);
      int s = random(0, CLASSICAL_SONGS);
      return String(classicalGenres[g]) + " | " + String(classicalArtists[a]) + " - " + String(classicalSongs[s]);
    }
    case 20: {  // Religion/Gospel
      int a = random(0, GOSPEL_ARTISTS);
      int s = random(0, GOSPEL_SONGS);
      int g = random(0, GOSPEL_GENRES);
      int y = random(0, GOSPEL_YEARS);
      return String(gospelGenres[g]) + " | " + String(gospelArtists[a]) + " - " + String(gospelSongs[s]) + " (" + String(gospelYears[y]) + ")";
    }
    case 24: {  // Jazz
      int g = random(0, JAZZ_GENRES_COUNT);
      int a = random(0, ROCK_ARTISTS);
      int s = random(0, ROCK_SONGS);
      return String(jazzGenres[g]) + " | " + String(rockArtists[a]) + " - " + String(rockSongs[s]);
    }
    case 25: {  // Country
      int g = random(0, COUNTRY_GENRES_COUNT);
      int a = random(0, COUNTRY_ARTISTS);
      int s = random(0, COUNTRY_SONGS);
      return String(countryGenres[g]) + " | " + String(countryArtists[a]) + " - " + String(countrySongs[s]);
    }
    case 26: {  // National Music
      int g = random(0, SERTANEJO_GENRES_COUNT);
      int a = random(0, NATIONAL_ARTISTS);
      int s = random(0, NATIONAL_SONGS);
      return String(sertanejoGenres[g]) + " | " + String(nationalArtists[a]) + " - " + String(nationalSongs[s]);
    }
    case 27: {  // Oldies
      int g = random(0, EASY_LISTENING_GENRES_COUNT);
      int a = random(0, OLDIES_ARTISTS);
      int s = random(0, OLDIES_SONGS);
      return String(easyListeningGenres[g]) + " | " + String(oldiesArtists[a]) + " - " + String(oldiesSongs[s]);
    }
    case 28: {  // Folk Music
      int g = random(0, COUNTRY_GENRES_COUNT);
      int a = random(0, FOLK_ARTISTS);
      int s = random(0, FOLK_SONGS);
      return String(countryGenres[g]) + " | " + String(folkArtists[a]) + " - " + String(folkSongs[s]);
    }
    case 18: {  // Children's Music
      int g = random(0, CHILDREN_GENRES_COUNT);
      int a = random(0, CHILDREN_ARTISTS);
      int s = random(0, CHILDREN_SONGS);
      int y = random(0, CHILDREN_YEARS);
      return String(childrenGenres[g]) + " | " + String(childrenArtists[a]) + " - " + String(childrenSongs[s]) + " (" + String(childrenYears[y]) + ")";
    }
    default: return generateRandomSong();
  }
}

String rotateSongString(const String& song, int scrollPos) {
  if (song.length() == 0) return song;
  int pos = scrollPos % song.length();
  return song.substring(pos) + song.substring(0, pos);
}

String formatSongWithYear(const String& song, const String& year) {
  if (song.length() == 0) return String("");
  if (year.length() == 0) return song;
  // Check if year already in song format
  if (song.indexOf("(") != -1 && song.indexOf(")") != -1) {
    return song;  // Year already included
  }
  return song + " (" + year + ")";
}

String getRandomGospelPS() {
  return String(gospelPS[random(0, GOSPEL_PS_COUNT)]);
}

String getRandomGospelRT() {
  return String(gospelRT[random(0, GOSPEL_RT_COUNT)]);
}

String getRandomGenreByPTY(uint8_t pty_code) {
  switch (pty_code) {
    case 0:  return String(newsGenres[random(0, NEWS_GENRES)]);
    case 1:  return String(currentAffairsGenres[random(0, CURRENT_AFFAIRS_GENRES_COUNT)]);
    case 2:  return String(informationGenres[random(0, INFORMATION_GENRES_COUNT)]);
    case 3:  return String(sportGenres[random(0, SPORT_GENRES)]);
    case 4:  return String(educationGenres[random(0, EDUCATION_GENRES)]);
    case 5:  return String(dramaGenres[random(0, DRAMA_GENRES)]);
    case 6:  return String(cultureGenres[random(0, CULTURE_GENRES)]);
    case 7:  return String(scienceGenres[random(0, SCIENCE_GENRES)]);
    case 8:
    case 15: return String(variedSpeechGenres[random(0, VARIED_SPEECH_GENRES)]);
    case 9:  return String(otherMusicGenres[random(0, OTHER_MUSIC_GENRES_COUNT)]);
    case 10: return String(popGenres[random(0, POP_GENRES_COUNT)]);
    case 11: return String(rockGenres[random(0, ROCK_GENRES_COUNT)]);
    case 12: return String(easyListeningGenres[random(0, EASY_LISTENING_GENRES_COUNT)]);
    case 13:
    case 14: return String(classicalGenres[random(0, CLASSICAL_GENRES_COUNT)]);
    case 16: return String(weatherGenres[random(0, WEATHER_GENRES_COUNT)]);
    case 17: return String(financeGenres[random(0, FINANCE_GENRES_COUNT)]);
    case 18: return String(childrenGenres[random(0, CHILDREN_GENRES_COUNT)]);
    case 19: return String(socialGenres[random(0, SOCIAL_GENRES_COUNT)]);
    case 20: return String(gospelGenres[random(0, GOSPEL_GENRES)]);
    case 21: return String(travelGenres[random(0, TRAVEL_GENRES_COUNT)]);
    case 22: return String(leisureGenres[random(0, LEISURE_GENRES_COUNT)]);
    case 23: return String(documentaryGenres[random(0, DOCUMENTARY_GENRES_COUNT)]);
    case 24: return String(jazzGenres[random(0, JAZZ_GENRES_COUNT)]);
    case 25: return String(countryGenres[random(0, COUNTRY_GENRES_COUNT)]);
    case 26: return String(sertanejoGenres[random(0, SERTANEJO_GENRES_COUNT)]);
    case 27: return String(oldiesGenres[random(0, OLDIES_GENRES)]);
    case 28: return String(folkGenres[random(0, FOLK_GENRES_COUNT)]);
    case 30: return String(alarmTestGenres[random(0, ALARM_TEST_GENRES_COUNT)]);
    case 31: return String(alarmGenres[random(0, ALARM_GENRES_COUNT)]);
    default: return String(musicGenres[random(0, GENRES_COUNT)]);
  }
}

String getRandomArtistByPTY(uint8_t pty_code) {
  switch (pty_code) {
    case 0:  return String(newsArtists[random(0, NEWS_ARTISTS)]);
    case 3:  return String(sportArtists[random(0, SPORT_ARTISTS)]);
    case 4:  return String(educationArtists[random(0, EDUCATION_ARTISTS)]);
    case 5:  return String(dramaArtists[random(0, DRAMA_ARTISTS)]);
    case 6:  return String(cultureArtists[random(0, CULTURE_ARTISTS)]);
    case 7:  return String(scienceArtists[random(0, SCIENCE_ARTISTS)]);
    case 10: return String(popArtists[random(0, POP_ARTISTS)]);
    case 11: return String(rockArtists[random(0, ROCK_ARTISTS)]);
    case 12: return String(easyArtists[random(0, EASY_ARTISTS)]);
    case 13:
    case 14: return String(classicalArtists[random(0, CLASSICAL_ARTISTS)]);
    case 18: return String(childrenArtists[random(0, CHILDREN_ARTISTS)]);
    case 20: return String(gospelArtists[random(0, GOSPEL_ARTISTS)]);
    case 24: return String(brazilianArtists[random(0, ARTISTS_COUNT)]);
    case 25: return String(countryArtists[random(0, COUNTRY_ARTISTS)]);
    case 26: return String(nationalArtists[random(0, NATIONAL_ARTISTS)]);
    case 27: return String(oldiesArtists[random(0, OLDIES_ARTISTS)]);
    case 28: return String(folkArtists[random(0, FOLK_ARTISTS)]);
    case 15: return String(variedSpeechArtists[random(0, VARIED_SPEECH_ARTISTS)]);
    default: return String(brazilianArtists[random(0, ARTISTS_COUNT)]);
  }
}

String getRandomYearByPTY(uint8_t pty_code) {
  switch (pty_code) {
    case 3:  return String(sportYears[random(0, SPORT_YEARS)]);
    case 8:
    case 15: return String(variedSpeechYears[random(0, VARIED_SPEECH_YEARS)]);
    case 18: return String(childrenYears[random(0, CHILDREN_YEARS)]);
    case 20: return String(gospelYears[random(0, GOSPEL_YEARS)]);
    default: return String(brazilianYears[random(0, YEARS_COUNT)]);
  }
}

String getRandomPSByPTY(uint8_t pty_code) {
  switch (pty_code) {
    case 0:  return String(newsPS[random(0, NEWS_PS_COUNT)]);
    case 1:  return String(currentAffairsPS[random(0, CURRENT_AFFAIRS_PS_COUNT)]);
    case 2:  return String(informationPS[random(0, INFORMATION_PS_COUNT)]);
    case 3:  return String(sportPS[random(0, SPORT_PS_COUNT)]);
    case 4:  return String(educationPS[random(0, EDUCATION_PS_COUNT)]);
    case 5:  return String(dramaPS[random(0, DRAMA_PS_COUNT)]);
    case 6:  return String(culturePS[random(0, CULTURE_PS_COUNT)]);
    case 7:  return String(sciencePS[random(0, SCIENCE_PS_COUNT)]);
    case 8:
    case 15: return String(talkPS[random(0, TALK_PS_COUNT)]);
    case 9:  return String(otherMusicPS[random(0, OTHER_MUSIC_PS_COUNT)]);
    case 10: return String(popPS[random(0, POP_PS_COUNT)]);
    case 11: return String(rockPS[random(0, ROCK_PS_COUNT)]);
    case 12: return String(easyPS[random(0, EASY_PS_COUNT)]);
    case 13:
    case 14: return String(classicalPS[random(0, CLASSICAL_PS_COUNT)]);
    case 16: return String(weatherPS[random(0, WEATHER_PS_COUNT)]);
    case 17: return String(financePS[random(0, FINANCE_PS_COUNT)]);
    case 18: return String(childrenPS[random(0, CHILDREN_PS_COUNT)]);
    case 19: return String(socialPS[random(0, SOCIAL_PS_COUNT)]);
    case 20: return String(gospelPS[random(0, GOSPEL_PS_COUNT)]);
    case 21: return String(travelPS[random(0, TRAVEL_PS_COUNT)]);
    case 22: return String(leisurePS[random(0, LEISURE_PS_COUNT)]);
    case 23: return String(documentaryPS[random(0, DOCUMENTARY_PS_COUNT)]);
    case 24: return String(jazzPS[random(0, JAZZ_PS_COUNT)]);
    case 25: return String(countryPS[random(0, COUNTRY_PS_COUNT)]);
    case 26: return String(sertanejoPS[random(0, SERTANEJO_PS_COUNT)]);
    case 27: return String(oldiesPS[random(0, OLDIES_PS_COUNT)]);
    case 28: return String(folkPS[random(0, FOLK_PS_COUNT)]);
    case 30: return String(alarmTestPS[random(0, ALARM_TEST_PS_COUNT)]);
    case 31: return String(alarmPS[random(0, ALARM_PS_COUNT)]);
    default: return String("RADIO FM");
  }
}

String getRandomRTByPTY(uint8_t pty_code) {
  switch (pty_code) {
    case 0:  return String(newsRT[random(0, NEWS_RT_COUNT)]);
    case 1:  return String(currentAffairsRT[random(0, CURRENT_AFFAIRS_RT_COUNT)]);
    case 2:  return String(informationRT[random(0, INFORMATION_RT_COUNT)]);
    case 3:  return String(sportRT[random(0, SPORT_RT_COUNT)]);
    case 4:  return String(educationRT[random(0, EDUCATION_RT_COUNT)]);
    case 5:  return String(dramaRT[random(0, DRAMA_RT_COUNT)]);
    case 6:  return String(cultureRT[random(0, CULTURE_RT_COUNT)]);
    case 7:  return String(scienceRT[random(0, SCIENCE_RT_COUNT)]);
    case 8:
    case 15: return String(talkRT[random(0, TALK_RT_COUNT)]);
    case 9:  return String(otherMusicRT[random(0, OTHER_MUSIC_RT_COUNT)]);
    case 10: return String(popRT[random(0, POP_RT_COUNT)]);
    case 11: return String(rockRT[random(0, ROCK_RT_COUNT)]);
    case 12: return String(easyRT[random(0, EASY_RT_COUNT)]);
    case 13:
    case 14: return String(classicalRT[random(0, CLASSICAL_RT_COUNT)]);
    case 16: return String(weatherRT[random(0, WEATHER_RT_COUNT)]);
    case 17: return String(financeRT[random(0, FINANCE_RT_COUNT)]);
    case 18: return String(childrenRT[random(0, CHILDREN_RT_COUNT)]);
    case 19: return String(socialRT[random(0, SOCIAL_RT_COUNT)]);
    case 20: return String(gospelRT[random(0, GOSPEL_RT_COUNT)]);
    case 21: return String(travelRT[random(0, TRAVEL_RT_COUNT)]);
    case 22: return String(leisureRT[random(0, LEISURE_RT_COUNT)]);
    case 23: return String(documentaryRT[random(0, DOCUMENTARY_RT_COUNT)]);
    case 24: return String(jazzRT[random(0, JAZZ_RT_COUNT)]);
    case 25: return String(countryRT[random(0, COUNTRY_RT_COUNT)]);
    case 26: return String(sertanejoRT[random(0, SERTANEJO_RT_COUNT)]);
    case 27: return String(oldiesRT[random(0, OLDIES_RT_COUNT)]);
    case 28: return String(folkRT[random(0, FOLK_RT_COUNT)]);
    case 30: return String(alarmTestRT[random(0, ALARM_TEST_RT_COUNT)]);
    case 31: return String(alarmRT[random(0, ALARM_RT_COUNT)]);
    default: return String("MUSICA 24 HORAS");
  }
}

const char* getPTYName(uint8_t pty_code) {
  static const char* pty_names[] = {
    "News", "Current Affairs", "Information", "Sport", "Education",
    "Drama", "Culture", "Science", "Varied Speech", "Pop Music",
    "Rock Music", "Easy Listening", "Light Classical", "Serious Classical", "Other Music",
    "Weather", "Finance", "Children's", "Social Affairs", "Religion",
    "Gospel", "Travel", "Leisure", "Jazz Music", "Country Music",
    "National Music", "Oldies", "Folk Music", "Documentary", "Alarm Test",
    "Alarm", "Talk"
  };
  if (pty_code >= 0 && pty_code <= 31) {
    return pty_names[pty_code];
  }
  return "Unknown";
}

String getGenreByPS(const String& ps) {
  for (auto &e : customPtys) {
    if (e.ps == ps) {
      return getRandomGenreByPTY(e.pty_code);
    }
  }
  for (auto &e : customPtys) {
    if (e.ps.indexOf(ps) != -1) {
      return getRandomGenreByPTY(e.pty_code);
    }
  }
  return String("");
}

String getGenreByRT(const String& rt) {
  for (auto &e : customPtys) {
    if (e.rt == rt) {
      return getRandomGenreByPTY(e.pty_code);
    }
  }
  for (auto &e : customPtys) {
    if (e.rt.indexOf(rt) != -1) {
      return getRandomGenreByPTY(e.pty_code);
    }
  }
  return String("");
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

String getPSForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.ps;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.ps;
  }
  return String("");
}

String findCustomPSForFreq(uint32_t freq_khz) {
  String ps = String("");
  String songYear = String("");
  String weather = String("");
  String temperature = String("");
  String hour = String("");
  String minute = String("");
  String second = String("");
  String day = String("");
  String month = String("");
  String year = String("");
  int scrollPos = 0;
  uint8_t pty_code = 0;

  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      ps = e.ps;
      pty_code = e.pty_code;
      songYear = e.songYear;
      weather = e.weather;
      temperature = e.temperature;
      hour = e.hour;
      minute = e.minute;
      second = e.second;
      day = e.day;
      month = e.month;
      year = e.year;
      scrollPos = e.songScrollPos;
      break;
    }
  }
  if (ps.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        ps = e.ps;
        pty_code = e.pty_code;
        songYear = e.songYear;
        weather = e.weather;
        temperature = e.temperature;
        hour = e.hour;
        minute = e.minute;
        second = e.second;
        day = e.day;
        month = e.month;
        year = e.year;
        scrollPos = e.songScrollPos;
        break;
      }
    }
  }

  String result = String("[") + String(pty_code) + "] " + String(getPTYName(pty_code));
  if (ps.length() > 0) {
    result += " | " + ps;
  }

  String song = findCustomSongForFreq(freq_khz);
  if (song.length() > 0) {
    String rotatedSong = rotateSongString(song, scrollPos);
    String formattedSong = formatSongWithYear(rotatedSong, songYear);
    result += " | " + formattedSong;
  }

  if (day.length() > 0 && month.length() > 0 && year.length() > 0) {
    result += " | " + day + "/" + month + "/" + year;
  }
  if (hour.length() > 0 && minute.length() > 0) {
    result += " " + hour + ":" + minute;
    if (second.length() > 0) {
      result += ":" + second;
    }
  }
  if (weather.length() > 0) {
    result += " | " + weather;
    if (temperature.length() > 0) {
      result += " " + temperature;
    }
  }

  return result;
}

String getRTForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.rt;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.rt;
  }
  return String("");
}

String findCustomRTForFreq(uint32_t freq_khz) {
  String rt = String("");
  String songYear = String("");
  String weather = String("");
  String temperature = String("");
  String hour = String("");
  String minute = String("");
  String second = String("");
  String day = String("");
  String month = String("");
  String year = String("");
  int scrollPos = 0;
  uint8_t pty_code = 0;

  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      rt = e.rt;
      pty_code = e.pty_code;
      songYear = e.songYear;
      weather = e.weather;
      temperature = e.temperature;
      hour = e.hour;
      minute = e.minute;
      second = e.second;
      day = e.day;
      month = e.month;
      year = e.year;
      scrollPos = e.songScrollPos;
      break;
    }
  }
  if (rt.length() == 0) {
    for (auto &e : customPtys) {
      if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
        rt = e.rt;
        pty_code = e.pty_code;
        songYear = e.songYear;
        weather = e.weather;
        temperature = e.temperature;
        hour = e.hour;
        minute = e.minute;
        second = e.second;
        day = e.day;
        month = e.month;
        year = e.year;
        scrollPos = e.songScrollPos;
        break;
      }
    }
  }

  String result = String("[") + String(pty_code) + "] " + String(getPTYName(pty_code));
  if (rt.length() > 0) {
    result += " | " + rt;
  }

  String song = findCustomSongForFreq(freq_khz);
  if (song.length() > 0) {
    String rotatedSong = rotateSongString(song, scrollPos);
    String formattedSong = formatSongWithYear(rotatedSong, songYear);
    result += " - " + formattedSong;
  }

  if (day.length() > 0 && month.length() > 0 && year.length() > 0) {
    result += " | " + day + "/" + month + "/" + year;
  }
  if (hour.length() > 0 && minute.length() > 0) {
    result += " " + hour + ":" + minute;
    if (second.length() > 0) {
      result += ":" + second;
    }
  }
  if (weather.length() > 0) {
    result += " | " + weather;
    if (temperature.length() > 0) {
      result += " " + temperature;
    }
  }

  return result;
}

void loadIsaacPTYs() {
  log_info("Loading default Isaac PTYs");
  customPtys.clear();
  PTYEntry e;
  int si = 0;

  e.freq_khz = 79700;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Samba | Jorge Aragao - Samba Eternidade";
  e.songYear = "1995";
  e.hour = "14";
  e.minute = "35";
  e.second = "00";
  e.day = "26";
  e.month = "05";
  e.year = "2026";
  e.weather = "Sunny";
  e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 86700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "MPB | Gilberto Gil - Aquele Abraco";
  e.songYear = "1969";
  e.hour = "14"; e.minute = "38"; e.second = "03";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 87100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Forró | Luiz Gonzaga - Sertanejo Roots";
  e.songYear = "1952";
  e.hour = "14"; e.minute = "40"; e.second = "06";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 88300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pagode | Sorriso Maroto - Amor Infinito";
  e.songYear = "2003";
  e.hour = "14"; e.minute = "42"; e.second = "09";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 88900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Sertanejo | Gusttavo Lima - Festa";
  e.songYear = "2014";
  e.hour = "14"; e.minute = "44"; e.second = "12";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 89900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Funk Carioca | Anitta - Danca da Paixao";
  e.songYear = "2019";
  e.hour = "14"; e.minute = "46"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 90700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Bossa Nova | Tom Jobim - Garota de Ipanema";
  e.songYear = "1962";
  e.hour = "14"; e.minute = "48"; e.second = "18";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 90300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Ivete Sangalo - Noite Sagrada";
  e.songYear = "2007";
  e.hour = "14"; e.minute = "50"; e.second = "21";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 91700;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Gal Costa - Levada Brasileira";
  e.songYear = "1980";
  e.hour = "14"; e.minute = "52"; e.second = "24";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "30°C";
  customPtys.push_back(e);

  e.freq_khz = 92100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Beth Carvalho - Alma Brasileira";
  e.songYear = "1985";
  e.hour = "14"; e.minute = "54"; e.second = "27";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 92500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "MPB | Caetano Veloso - Verdade Tropical";
  e.songYear = "1968";
  e.hour = "14"; e.minute = "56"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 92900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop | Marisa Monte - Noite Magica";
  e.songYear = "1998";
  e.hour = "14"; e.minute = "58"; e.second = "33";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 93500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pagode | Dilsinho - Comunidade Shalom";
  e.songYear = "2004";
  e.hour = "15"; e.minute = "00"; e.second = "36";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 93900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Samba | Cartola - Saudade Profunda";
  e.songYear = "1978";
  e.hour = "15"; e.minute = "02"; e.second = "39";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 94300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Rock Brasileiro | Legiao Urbana - Energia Pura";
  e.songYear = "1987";
  e.hour = "15"; e.minute = "04"; e.second = "42";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 94700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Sertanejo | Jorge e Mateus - Melhor Trilha";
  e.songYear = "2010";
  e.hour = "15"; e.minute = "06"; e.second = "45";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 95100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Alcione - Palavra de Deus";
  e.songYear = "1990";
  e.hour = "15"; e.minute = "08"; e.second = "48";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 95500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop | Adriana Calcanhotto - Noticia Boa";
  e.songYear = "2002";
  e.hour = "15"; e.minute = "10"; e.second = "51";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 96100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Chico Buarque - Graca Divina";
  e.songYear = "1979";
  e.hour = "15"; e.minute = "12"; e.second = "54";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 96700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "MPB | Ivan Lins - Democracia Sonora";
  e.songYear = "1986";
  e.hour = "15"; e.minute = "14"; e.second = "57";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "30°C";
  customPtys.push_back(e);

  e.freq_khz = 97100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Elza Soares - Maria Sagrada";
  e.songYear = "1988";
  e.hour = "15"; e.minute = "16"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 97700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop | Djavan - Som Internacional";
  e.songYear = "1994";
  e.hour = "15"; e.minute = "18"; e.second = "05";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 98300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Belchior - Lider Celestial";
  e.songYear = "1992";
  e.hour = "15"; e.minute = "20"; e.second = "08";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 99100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Samba | Ze Ramalho - Cidade Maravilha";
  e.songYear = "1984";
  e.hour = "15"; e.minute = "22"; e.second = "11";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 99900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Geraldo Azevedo - Gloria Eterna";
  e.songYear = "2001";
  e.hour = "15"; e.minute = "24"; e.second = "14";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 100900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Lenine - Amor Divino";
  e.songYear = "2005";
  e.hour = "15"; e.minute = "26"; e.second = "17";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "30°C";
  customPtys.push_back(e);

  e.freq_khz = 101300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Criolo - Voz do Evangelho";
  e.songYear = "2011";
  e.hour = "15"; e.minute = "28"; e.second = "20";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "27°C";
  customPtys.push_back(e);

  e.freq_khz = 101700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop | Arlindo Cruz - Noticia Sempre";
  e.songYear = "2008";
  e.hour = "15"; e.minute = "30"; e.second = "23";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 102300;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Paulinho da Viola - Templo Sagrado";
  e.songYear = "1989";
  e.hour = "15"; e.minute = "32"; e.second = "26";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 102700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Axé | Martinho da Vila - Descoberta Nova";
  e.songYear = "2006";
  e.hour = "15"; e.minute = "34"; e.second = "29";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 103300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "MPB | Clara Nunes - Democracia Sonora";
  e.songYear = "1981";
  e.hour = "15"; e.minute = "36"; e.second = "32";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 103500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Angela Ro Ro - Paz e Bem";
  e.songYear = "2000";
  e.hour = "15"; e.minute = "38"; e.second = "35";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 103900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Samba | Benito di Paula - Melhor Musica";
  e.songYear = "1983";
  e.hour = "15"; e.minute = "40"; e.second = "38";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 104300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Sertanejo | Silas de Oliveira - Todas Tocadas";
  e.songYear = "2009";
  e.hour = "15"; e.minute = "42"; e.second = "41";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 105100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Jorge Ben Jor - Assembleia Sagrada";
  e.songYear = "1997";
  e.hour = "15"; e.minute = "44"; e.second = "44";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 105700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Bossa Nova | Egberto Gismonti - Vida Melhor";
  e.songYear = "1975";
  e.hour = "15"; e.minute = "46"; e.second = "47";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 106500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "MPB | Milton Nascimento - Brasil Novo";
  e.songYear = "1982";
  e.hour = "15"; e.minute = "48"; e.second = "50";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 107500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop | Anitta - Hits Atuais";
  e.songYear = "2020";
  e.hour = "15"; e.minute = "50"; e.second = "53";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 107900;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Tropicália | Gilberto Gil - Cultura Infinita";
  e.songYear = "1967";
  e.hour = "15"; e.minute = "52"; e.second = "56";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "30°C";
  customPtys.push_back(e);

  // === GOSPEL STATIONS (PTY 20) ===
  e.freq_khz = 79500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel | Gino Amen - Graça Divina";
  e.songYear = "2022";
  e.hour = "16"; e.minute = "00"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 83400;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Tradicional | Thalles Roberto - Palavra de Deus";
  e.songYear = "2020";
  e.hour = "16"; e.minute = "05"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "31°C";
  customPtys.push_back(e);

  e.freq_khz = 84900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Contemporaneo | Priscilla Alcantara - Eternidade Divina";
  e.songYear = "2023";
  e.hour = "16"; e.minute = "10"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 85600;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Urbano | Kemilly Santos - Verdade Tropical";
  e.songYear = "2021";
  e.hour = "16"; e.minute = "15"; e.second = "45";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "34°C";
  customPtys.push_back(e);

  e.freq_khz = 100100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Rock | Fernanda Brum - Noite Sagrada";
  e.songYear = "2019";
  e.hour = "16"; e.minute = "20"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 100500;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Soul | Eyshila - Voz do Evangelho";
  e.songYear = "2022";
  e.hour = "16"; e.minute = "25"; e.second = "20";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 102900;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Adoracao | Midian Lima - Paz e Bem";
  e.songYear = "2023";
  e.hour = "16"; e.minute = "30"; e.second = "35";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 104100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Sertanejo | Soraya Moraes - Raizes Culturais";
  e.songYear = "2020";
  e.hour = "16"; e.minute = "35"; e.second = "50";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 106100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Forró | Gal Costa - Levada Brasileira";
  e.songYear = "1980";
  e.hour = "16"; e.minute = "40"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "33°C";
  customPtys.push_back(e);

  e.freq_khz = 107100;
  e.pty_code = 20;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Gospel Samba | Beth Carvalho - Alma Brasileira";
  e.songYear = "1985";
  e.hour = "16"; e.minute = "45"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "31°C";
  customPtys.push_back(e);

  // === POP STATIONS (PTY 10) ===
  e.freq_khz = 87500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop Brasileiro | Anitta - Festa";
  e.songYear = "2019";
  e.hour = "16"; e.minute = "50"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "32°C";
  customPtys.push_back(e);

  e.freq_khz = 99300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Pop Rock | Marisa Monte - Noite Magica";
  e.songYear = "1998";
  e.hour = "16"; e.minute = "55"; e.second = "45";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "33°C";
  customPtys.push_back(e);

  // === ROCK STATIONS (PTY 11) ===
  e.freq_khz = 88700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Rock Nacional | Legiao Urbana - Comida";
  e.songYear = "1987";
  e.hour = "17"; e.minute = "00"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "35°C";
  customPtys.push_back(e);

  e.freq_khz = 94500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Hard Rock | Sepultura - Roots Bloody Roots";
  e.songYear = "1996";
  e.hour = "17"; e.minute = "05"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "31°C";
  customPtys.push_back(e);

  // === EASY LISTENING (PTY 12) ===
  e.freq_khz = 91500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Bossa Nova | Tom Jobim - Garota de Ipanema";
  e.songYear = "1962";
  e.hour = "17"; e.minute = "10"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "30°C";
  customPtys.push_back(e);

  e.freq_khz = 97300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Easy Listening | Joao Gilberto - Desafinado";
  e.songYear = "1959";
  e.hour = "17"; e.minute = "15"; e.second = "45";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  // === LIGHT CLASSICAL (PTY 13) ===
  e.freq_khz = 93100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Clássico | Egberto Gismonti - Agua e Vinho";
  e.songYear = "1974";
  e.hour = "17"; e.minute = "20"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "33°C";
  customPtys.push_back(e);

  // === SERIOUS CLASSICAL (PTY 14) ===
  e.freq_khz = 95700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Sinfônico | Mozart - Eine Kleine Nachtmusik";
  e.songYear = "1787";
  e.hour = "17"; e.minute = "25"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Windy"; e.temperature = "29°C";
  customPtys.push_back(e);

  // === VARIED SPEECH (PTY 15) ===
  e.freq_khz = 89700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Conversa | Criolo - Reflexao e Ritmo";
  e.songYear = "2014";
  e.hour = "17"; e.minute = "30"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Cloudy"; e.temperature = "28°C";
  customPtys.push_back(e);

  // === SPORT (PTY 3) ===
  e.freq_khz = 98100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Futebol | Galvao Bueno - Gol do Brasil";
  e.songYear = "2022";
  e.hour = "17"; e.minute = "35"; e.second = "45";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "34°C";
  customPtys.push_back(e);

  // === JAZZ (PTY 24) ===
  e.freq_khz = 101500;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Jazz Brasileiro | Miles Davis - So What";
  e.songYear = "1959";
  e.hour = "17"; e.minute = "40"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "31°C";
  customPtys.push_back(e);

  // === COUNTRY/FORRÓ (PTY 25) ===
  e.freq_khz = 103700;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Forró Tradicional | Jackson do Pandeiro - Asa Branca";
  e.songYear = "1947";
  e.hour = "17"; e.minute = "45"; e.second = "15";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Hot & Humid"; e.temperature = "35°C";
  customPtys.push_back(e);

  // === NATIONAL/SERTANEJO (PTY 26) ===
  e.freq_khz = 105300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Sertanejo | Gusttavo Lima - Balada Inteligente";
  e.songYear = "2013";
  e.hour = "17"; e.minute = "50"; e.second = "30";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "32°C";
  customPtys.push_back(e);

  // === CHILDREN'S (PTY 18) ===
  e.freq_khz = 84300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Turma da Monica - Aquarela Magica";
  e.songYear = "2015";
  e.hour = "10"; e.minute = "30"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 85300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Xuxa - Iluminando Criancas";
  e.songYear = "1995";
  e.hour = "14"; e.minute = "15"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "30°C";
  customPtys.push_back(e);

  e.freq_khz = 86200;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Galinha Pintadinha - Brincando Cantando";
  e.songYear = "2008";
  e.hour = "09"; e.minute = "45"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "27°C";
  customPtys.push_back(e);

  e.freq_khz = 87300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Balao Magico - Roda Gigante Feliz";
  e.songYear = "1982";
  e.hour = "15"; e.minute = "00"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 90100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Mundo Bita - Aventura Musical";
  e.songYear = "2010";
  e.hour = "11"; e.minute = "00"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "26°C";
  customPtys.push_back(e);

  e.freq_khz = 91100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Patati Patata - Malabarismo Sonoro";
  e.songYear = "2005";
  e.hour = "13"; e.minute = "30"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "29°C";
  customPtys.push_back(e);

  e.freq_khz = 92200;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Toquinho - Cantar Aprender";
  e.songYear = "1978";
  e.hour = "10"; e.minute = "00"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Clear"; e.temperature = "28°C";
  customPtys.push_back(e);

  e.freq_khz = 93300;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Gilberto Gil Kids - Mundo Colorido";
  e.songYear = "1985";
  e.hour = "14"; e.minute = "45"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Partly Cloudy"; e.temperature = "27°C";
  customPtys.push_back(e);

  e.freq_khz = 94100;
  e.pty_code = 10;
  e.ps = getPSByLanguage(si, currentPTYLanguage);
  e.rt = getRTByLanguage(si, currentPTYLanguage);
  si++;
  e.song = "Infantil | Magico Ramirez - Prestidigitacao Sonora";
  e.songYear = "1990";
  e.hour = "15"; e.minute = "15"; e.second = "00";
  e.day = "26"; e.month = "05"; e.year = "2026";
  e.weather = "Sunny"; e.temperature = "32°C";
  customPtys.push_back(e);

  // === Fill all remaining Fortaleza FM frequencies ===
  struct StationProfile {
    const char* ps;
    uint8_t pty;
  };

  const StationProfile stationProfiles[] = {
    // Pop stations (PTY 10)
    {"JANGADEIRO", 10}, {"RADIO CITY", 10}, {"FORTALEZA", 10},
    {"CULTURA FM", 10}, {"CIDADE FM", 10}, {"TROPICAL", 10},
    // Rock stations (PTY 11)
    {"ANTENA 1", 10}, {"PULSO FM", 10}, {"LIDER FM", 10}, {"BOOM FM", 10},
    // Jazz (PTY 24)
    {"MIX FM", 10}, {"JAZZ FM", 10},
    // Easy Listening (PTY 12)
    {"TEMPO FM", 10}, {"CLASSICO", 12},
    // Light Classical (PTY 13)
    {"ELITE FM", 10}, {"ALMA FM", 10},
    // Serious Classical (PTY 14)
    {"DELTA FM", 10}, {"OPERA FM", 10},
    // Varied Speech (PTY 8, 15)
    {"RITMO FM", 10}, {"SUPER FM", 10},
    // Sport (PTY 3)
    {"ONDAS FM", 10}, {"NEWS FM", 10},
    // Country/Forró (PTY 25)
    {"SERTAO FM", 10}, {"FORRO FM", 10}, 
    // National/Sertanejo (PTY 26)
    {"NOVA BRASIL", 10}, {"BRASIL FM", 10}, {"GAUCHO FM", 10},
    // Oldies (PTY 27)
    {"SOL FM", 10},
    // Folk (PTY 28)
    {"MUSICA FM", 10}, {"ESTILO FM", 10}
  };
  const int NAMES_COUNT = sizeof(stationProfiles) / sizeof(stationProfiles[0]);

  const char* rtMessages[] = {
    "O MELHOR DO RADIO FORTALEZA",
    "MUSICA 24 HORAS NO AR",
    "SUA ESTACAO PREFERIDA SEMPRE",
    "QUALIDADE E EMOCAO EM CADA NOTA",
    "FORTALEZA MUSICAL NO AR",
    "RITMO E HARMONIA TODO DIA",
    "VOCE SEMPRE CONOSCO NA SINTONIA",
    "PURA ENERGIA MUSICAL",
    "SINTONIA PERFEITA PARA VOCE",
    "TRANSMITINDO CULTURA BRASILEIRA",
    "O SOM QUE VOCE AMA AGORA",
    "RADIO COM PAIXAO NORDESTINA",
    "SEMPRE CONECTADOS AO MELHOR",
    "DIVERSIDADE MUSICAL SEM PARAR",
    "ENCONTRO DE SONS E EMOCOES",
    "MELHOR PROGRAMACAO DO CEARA",
    "SUA MUSICA PREFERIDA AQUI",
    "RITMO DO FORTALEZA",
    "ONDAS DE MUSICA PURA",
    "DANCA E SENTIMENTO NO AR"
  };
  const int RT_COUNT = sizeof(rtMessages) / sizeof(rtMessages[0]);

  for (int i = 0; i < FREQ_COUNT; i++) {
    bool exists = false;
    for (auto &check : customPtys) {
      if (check.freq_khz == allFrequencies[i]) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      e.freq_khz = allFrequencies[i];

      // Get station profile (PS + PTY)
      StationProfile profile = stationProfiles[i % NAMES_COUNT];
      e.pty_code = profile.pty;
      e.ps = String(getRandomPSByPTY(profile.pty)) + " " + String(allFrequencies[i] / 1000.0, 1);

      // Assign RT based on PTY type with variety
      const char* rtPop[] = {
        "MUSICA POP NO AR", "HITS DO MOMENTO", "O MELHOR DO POP",
        "POP PARA VOCE", "SUCESSOS ATUAIS", "RITMO POP SEMPRE"
      };
      const char* rtRock[] = {
        "ROCK EM ALTA VOLTAGEM", "ENERGIA ROCK", "GUITARRA NO AR",
        "ROCK NACIONAL E INTERNACIONAL", "RIFF EM ALTA VOLTAGE", "ROCK PURO"
      };
      const char* rtEasyListening[] = {
        "MELODIAS SUAVES E RELAXANTES", "BOSSA NOVA ETERNA", "SOM SUAVE",
        "MUSICA PARA RELAXAR", "HARMONIA E PAZ", "NOITES TRANQUILAS"
      };
      const char* rtClassicalLight[] = {
        "CLASSICOS LEVES PARA VOCE", "MUSICA REFINADA", "CLASSICOS DOCES",
        "SINFONIA LEVE", "ELEGANCIA MUSICAL", "ARTE NO AR"
      };
      const char* rtClassicalSerious[] = {
        "CLASSICOS SINFONIA ETERNA", "ÓPERA E ORQUESTRA", "GRANDEZA MUSICAL",
        "MUSICA ERUDITA", "SINFONIA SUPREMA", "MAESTRIA SONORA"
      };
      const char* rtVariedSpeech[] = {
        "MUSICA COM CONVERSAS", "MISTURA PERFEITA", "VARIEDADE SEMPRE",
        "CONVERSA E MUSICA", "DIALOGO MUSICAL", "TROCA DE IDEIAS"
      };
      const char* rtSport[] = {
        "NOTICIA E ESPORTE NO AR", "ESPORTE EM TEMPO REAL", "PLACAR VIVO",
        "COBERTURA ESPORTIVA", "JOGOS E NOTICIAS", "ACAO NO ESPORTE"
      };
      const char* rtJazz[] = {
        "JAZZ E IMPROVISO NO AR", "IMPROVISO PURO", "JAZZ CLASSICO",
        "SAXOFONE E HARMONIA", "JAZZ BRASILEIRO", "RITMO SINCOPADO"
      };
      const char* rtCountry[] = {
        "RITMO NORDESTINO AUTENTICO", "FORRÓ CEARENSE", "DANCA NORDESTINA",
        "RAIZ DO SERTAO", "FORRÓ TRADICIONAL", "BATIDA CEARENSE"
      };
      const char* rtNational[] = {
        "SERTANEJO DO SERTAO CEARENSE", "SERTANEJO RAIZ", "MUSICA SERTANEJA",
        "SERTAO SONORO", "CABAÇO E VIOLA", "ABOIO DO SERTAO"
      };
      const char* rtOldies[] = {
        "SUCESSOS QUE NAO ENVELHECEM", "CLASSICOS DO PASSADO", "NOSTALGIA MUSICAL",
        "OURO DO RADIO", "MEMORIAS SONORAS", "ETERNOS SUCESSOS"
      };
      const char* rtFolk[] = {
        "MUSICA FOLCLORICA BRASILEIRA", "RAIZES CULTURAIS", "TRADIÇÃO MUSICAL",
        "FOLCLORE VIVO", "CULTURA POPULAR", "EXPRESSÃO NORDESTINA"
      };

      e.rt = getRandomRTByPTY(e.pty_code);
      e.song = getSongForPTY(e.pty_code);
      e.songYear = String(2010 + (i % 16));

      // Time variation: spread across 24 hours
      int totalMins = (i * 23) % 1440;  // 1440 mins in a day
      int hrs = totalMins / 60;
      int mins = totalMins % 60;
      int secs = (i * 7) % 60;

      char timeStr[3];
      snprintf(timeStr, sizeof(timeStr), "%02d", hrs);
      e.hour = String(timeStr);
      snprintf(timeStr, sizeof(timeStr), "%02d", mins);
      e.minute = String(timeStr);
      snprintf(timeStr, sizeof(timeStr), "%02d", secs);
      e.second = String(timeStr);

      // Date variation: spread across month
      // Vary month across the year
      const char* months[] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
      const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

      int monthIdx = (i * 5) % 12;
      int monthNum = monthIdx + 1;
      int maxDaysInMonth = daysInMonth[monthIdx];
      int dayOfMonth = 1 + ((i * 3) % maxDaysInMonth);

      snprintf(timeStr, sizeof(timeStr), "%02d", dayOfMonth);
      e.day = String(timeStr);
      e.month = String(months[monthIdx]);

      // Year variation: 2024-2026
      int yearVar = 2024 + ((i * 2) % 3);
      e.year = String(yearVar);

      // Realistic Fortaleza weather (tropical)
      const char* weatherConditions[] = {
        "Sunny", "Clear", "Partly Cloudy", "Cloudy", "Overcast",
        "Light Rain", "Scattered Showers", "Rainy", "Tropical Storm", "Humid",
        "Hot & Sunny", "Hot & Humid", "Breezy", "Windy", "Sea Breeze"
      };
      const int WEATHER_COUNT = sizeof(weatherConditions) / sizeof(weatherConditions[0]);

      int tempBase = 26 + ((i * 3) % 12);  // 26-37°C range (tropical)
      int weatherIdx = (i * 7) % WEATHER_COUNT;

      e.weather = String(weatherConditions[weatherIdx]);
      e.temperature = String(tempBase) + "°C";

      customPtys.push_back(e);
    }
  }

  log_info("Default Isaac PTYs loaded.");
}

bool isCustomRDSEnabled(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return e.custom_rds_enabled;
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return e.custom_rds_enabled;
  }
  return true;
}

int8_t findCustomPTYCodeForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      if (!e.custom_rds_enabled) return -1;
      return e.pty_code;
    }
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
      if (!e.custom_rds_enabled) continue;
      return e.pty_code;
    }
  }
  return -1;
}

String getPTYInfoForFreq(uint32_t freq_khz) {
  int8_t pty = findCustomPTYCodeForFreq(freq_khz);
  if (pty == -1) return String("");

  String result = String("[") + String(pty) + "] " + String(getPTYName(pty));
  String genre = findCustomGenreForFreq(freq_khz);
  if (genre.length() > 0) {
    result += " | Genre: " + genre;
  }
  return result;
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

String findCustomGenreForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return getRandomGenreByPTY(e.pty_code);
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return getRandomGenreByPTY(e.pty_code);
  }
  return String("");
}

size_t getCustomPTYCount() {
  return customPtys.size();
}

PTYEntry& getCustomPTYAt(size_t i) {
  return customPtys[i];
}

const char** getPSListForPTY(int8_t pty_code, int* count) {
  switch (pty_code) {
    case  1: *count = NEWS_PS_COUNT;             return (const char**)newsPS;
    case  2: *count = CURRENT_AFFAIRS_PS_COUNT;  return (const char**)currentAffairsPS;
    case  3: *count = INFORMATION_PS_COUNT;       return (const char**)informationPS;
    case  4: *count = SPORT_PS_COUNT;             return (const char**)sportPS;
    case  5: *count = EDUCATION_PS_COUNT;         return (const char**)educationPS;
    case  6: *count = DRAMA_PS_COUNT;             return (const char**)dramaPS;
    case  7: *count = CULTURE_PS_COUNT;           return (const char**)culturePS;
    case  8: *count = SCIENCE_PS_COUNT;           return (const char**)sciencePS;
    case  9: *count = OTHER_MUSIC_PS_COUNT;       return (const char**)otherMusicPS;
    case 10: *count = POP_PS_COUNT;               return (const char**)popPS;
    case 11: *count = ROCK_PS_COUNT;              return (const char**)rockPS;
    case 12: *count = EASY_PS_COUNT;              return (const char**)easyPS;
    case 13:
    case 14: *count = CLASSICAL_PS_COUNT;         return (const char**)classicalPS;
    case 15: *count = OTHER_MUSIC_PS_COUNT;       return (const char**)otherMusicPS;
    case 16: *count = WEATHER_PS_COUNT;           return (const char**)weatherPS;
    case 17: *count = FINANCE_PS_COUNT;           return (const char**)financePS;
    case 18: *count = CHILDREN_PS_COUNT;          return (const char**)childrenPS;
    case 19: *count = SOCIAL_PS_COUNT;            return (const char**)socialPS;
    case 20: *count = GOSPEL_PS_COUNT;            return (const char**)gospelPS;
    case 21: *count = TALK_PS_COUNT;              return (const char**)talkPS;
    case 22: *count = TRAVEL_PS_COUNT;            return (const char**)travelPS;
    case 23: *count = LEISURE_PS_COUNT;           return (const char**)leisurePS;
    case 24: *count = JAZZ_PS_COUNT;              return (const char**)jazzPS;
    case 25: *count = COUNTRY_PS_COUNT;           return (const char**)countryPS;
    case 26: *count = SERTANEJO_PS_COUNT;         return (const char**)sertanejoPS;
    case 27: *count = OLDIES_PS_COUNT;            return (const char**)oldiesPS;
    case 28: *count = FOLK_PS_COUNT;              return (const char**)folkPS;
    case 29: *count = DOCUMENTARY_PS_COUNT;       return (const char**)documentaryPS;
    case 30: *count = ALARM_TEST_PS_COUNT;        return (const char**)alarmTestPS;
    case 31: *count = ALARM_PS_COUNT;             return (const char**)alarmPS;
    default: *count = TALK_PS_COUNT;              return (const char**)talkPS;
  }
}

String findCustomArtistForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) return getRandomArtistByPTY(e.pty_code);
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) return getRandomArtistByPTY(e.pty_code);
  }
  return String("");
}

String findCustomYearForFreq(uint32_t freq_khz) {
  for (auto &e : customPtys) {
    if (e.freq_khz == freq_khz) {
      if (e.songYear.length() == 0) {
        return getRandomYearByPTY(e.pty_code);
      }
      return e.songYear;
    }
  }
  for (auto &e : customPtys) {
    if (abs((int32_t)e.freq_khz - (int32_t)freq_khz) <= 100) {
      if (e.songYear.length() == 0) {
        return getRandomYearByPTY(e.pty_code);
      }
      return e.songYear;
    }
  }
  return String("");
}







































































































