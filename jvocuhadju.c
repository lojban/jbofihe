/***************************************
  $Header$

  Program to generate the lujvo form of a given tanru
  ***************************************/

/* COPYRIGHT */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "version.h"

static int is_consonant(char c) {
  if (strchr("bcdfgjklmnprstvxz", c)) {
    return 1;
  } else {
    return 0;
  }
}

static int is_vowel(char c) {
  if (strchr("aeiouy", c)) {
    return 1;
  } else {
    return 0;
  }
}

static int ends_in_vowel(char *s) {
  char *p;
  p = s;
  while (*p) p++;
  p--;
  return is_vowel(*p);
}

static int ends_in_consonant(char *s) {
  char *p;
  p = s;
  while (*p) p++;
  p--;
  return is_consonant(*p);
}

static void chop_last_char(char *s) {
  char *p;
  p = s;
  while (*p) p++;
  p--;
  *p = 0;
}

static int is_voiced(char c) {
  if (strchr("bdgvjz", c)) {
    return 1;
  } else {
    return 0;
  }
}

static int is_unvoiced(char c) {
  if (strchr("ptkfcsx", c)) {
    return 1;
  } else {
    return 0;
  }
}

static int is_sibilant(char c) {
  if (strchr("cjsz", c)) {
    return 1;
  } else {
    return 0;
  }
}

static int is_ccv(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_consonant(s[1]) &&
      s[2] && is_vowel(s[2])) {
    return 1;
  } else {
    return 0;
  }
}

static int is_cvv(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      ((s[2] && is_vowel(s[2])) ||
       (s[3] && is_vowel(s[3]) && s[2] == '\''))) {
    return 1;
  } else {
    return 0;
  }
}

static int is_cvc(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      s[2] && is_consonant(s[2])) {
    return 1;
  } else {
    return 0;
  }
}

static int is_cvccv(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      s[2] && is_consonant(s[2]) &&
      s[3] && is_consonant(s[3]) &&
      s[4] && is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}

static int is_ccvcv(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_consonant(s[1]) &&
      s[2] && is_vowel(s[2]) &&
      s[3] && is_consonant(s[3]) &&
      s[4] && is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}


/* Check whether a consonant pair is permissible */
static int is_pairok(char *s) {
  int test1, test2, test3, test4; /* all true if acceptable */

  test1 = (s[0] != s[1]);
  test2 = !((is_voiced(s[0]) && is_unvoiced(s[1])) ||
            (is_voiced(s[1]) && is_unvoiced(s[0])));
  test3 = !(is_sibilant(s[0]) && is_sibilant(s[1]));
  test4 = strncmp(s, "cx", 2) && strncmp(s, "kx", 2) && 
    strncmp(s, "xc", 2) && strncmp(s, "xk", 2) && strncmp(s, "mz", 2);

  return test1 && test2 && test3 && test4;

}

static int is_initialpairok(char *s) {
  if (strncmp(s, "bl", 2) &&
      strncmp(s, "br", 2) &&
      strncmp(s, "cf", 2) &&
      strncmp(s, "ck", 2) &&
      strncmp(s, "cl", 2) &&
      strncmp(s, "cm", 2) &&
      strncmp(s, "cn", 2) &&
      strncmp(s, "cp", 2) &&
      strncmp(s, "cr", 2) &&
      strncmp(s, "ct", 2) &&
      strncmp(s, "dj", 2) &&
      strncmp(s, "dr", 2) &&
      strncmp(s, "dz", 2) &&
      strncmp(s, "fl", 2) &&
      strncmp(s, "fr", 2) &&
      strncmp(s, "gl", 2) &&
      strncmp(s, "gr", 2) &&
      strncmp(s, "jb", 2) &&
      strncmp(s, "jd", 2) &&
      strncmp(s, "jg", 2) &&
      strncmp(s, "jm", 2) &&
      strncmp(s, "jv", 2) &&
      strncmp(s, "kl", 2) &&
      strncmp(s, "kr", 2) &&
      strncmp(s, "ml", 2) &&
      strncmp(s, "mr", 2) &&
      strncmp(s, "pl", 2) &&
      strncmp(s, "pr", 2) &&
      strncmp(s, "sf", 2) &&
      strncmp(s, "sk", 2) &&
      strncmp(s, "sl", 2) &&
      strncmp(s, "sm", 2) &&
      strncmp(s, "sn", 2) &&
      strncmp(s, "sp", 2) &&
      strncmp(s, "sr", 2) &&
      strncmp(s, "st", 2) &&
      strncmp(s, "tc", 2) &&
      strncmp(s, "tr", 2) &&
      strncmp(s, "ts", 2) &&
      strncmp(s, "vl", 2) &&
      strncmp(s, "vr", 2) &&
      strncmp(s, "xl", 2) &&
      strncmp(s, "xr", 2) &&
      strncmp(s, "zb", 2) &&
      strncmp(s, "zd", 2) &&
      strncmp(s, "zg", 2) &&
      strncmp(s, "zm", 2) &&
      strncmp(s, "zv", 2)) {
    return 0;
  } else {
    return 1;
  }
}

/* See whether a pair of strings can be joined within a word (i.e. do they
   have an acceptable consonant pair */
static int can_join(char *s1, char *s2) {
  char e1, f2;
  char *p;
  char trial[3];
  int test1, test2, test3;
  f2 = s2[0];
  p = s1;
  while (*p) p++;
  p--;
  e1 = *p;
  trial[0] = e1;
  trial[1] = f2;
  test1 = is_pairok(trial); /* Check pair across join are acceptable
                               middle consonant pair, this is required
                               whether or not the second string starts
                               with a consonant pair */
  if (is_consonant(s2[1])) {
    test2 = is_initialpairok(s2);
    if (e1 == 'n') {
      test3 == (strncmp(s2, "dj", 2) &&
                strncmp(s2, "dz", 2) &&
                strncmp(s2, "tc", 2) &&
                strncmp(s2, "ts", 2));
    } else {
      test3 = 1;
    }
    return test1 && test2 && test3;
  } else {
    return test1;
  }
    
}

/* See whether a pair of consonants at a string join look like an
   initial pair */
static int initial_join(char *s1, char *s2) {
  char e1, f2;
  char *p;
  char trial[3];
  int test1, test2, test3;
  f2 = s2[0];
  p = s1;
  while (*p) p++;
  p--;
  e1 = *p;
  trial[0] = e1;
  trial[1] = f2;
  test1 = is_initialpairok(trial);
  return test1;
}


/* Table of all gismu + cmavo which have associated rafsi */

static char *gismu[] = {"bacru", "badna", "badri", "bajra", "bakfu",
"bakni", "bakri", "baktu", "balji", "balni", "balre", "balvi",
"bancu", "bandu", "banfi", "bangu", "banli", "banro", "banxa",
"banzu", "bapli", "barda", "bargu", "barja", "barna", "bartu",
"basna", "basti", "batci", "batke", "bavmi", "baxso", "bebna",
"bemro", "bende", "bengo", "benji", "bersa", "berti", "besna",
"betfu", "betri", "bevri", "bi", "bi'i", "bidju", "bifce", "bikla",
"bilga", "bilma", "bilni", "bindo", "binra", "binxo", "birje",
"birka", "birti", "bisli", "bitmu", "blabi", "blaci", "blanu",
"bliku", "bloti", "bo", "bolci", "bongu", "botpi", "boxfo", "boxna",
"bradi", "bratu", "brazo", "bredi", "bridi", "brife", "briju",
"brito", "broda", "brode", "brodi", "brodo", "brodu", "bruna", "bu",
"bu'a", "budjo", "bukpu", "bumru", "bunda", "bunre", "burcu", "burna",
"ca'a", "cabna", "cabra", "cacra", "cadzu", "cafne", "cakla", "calku",
"canci", "cando", "cange", "canja", "canko", "canlu", "canpa",
"canre", "canti", "carce", "carmi", "carna", "cartu", "carvi",
"casnu", "catke", "catlu", "catni", "catra", "caxno", "ce", "ce'i",
"ce'o", "cecla", "cecmu", "cedra", "cenba", "censa", "centi", "cerda",
"cerni", "certu", "cevni", "cfari", "cfika", "cfila", "cfine",
"cfipu", "ci", "ciblu", "cicna", "cidja", "cidni", "cidro", "cifnu",
"cigla", "cikna", "cikre", "ciksi", "cilce", "cilmo", "cilre",
"cilta", "cimde", "cimni", "cinba", "cindu", "cinfo", "cinje",
"cinki", "cinla", "cinmo", "cinri", "cinse", "cinta", "cinza",
"cipni", "cipra", "cirko", "cirla", "ciska", "cisma", "ciste",
"citka", "citno", "citri", "citsi", "civla", "cizra", "ckabu",
"ckafi", "ckaji", "ckana", "ckape", "ckasu", "ckeji", "ckiku",
"ckilu", "ckini", "ckire", "ckule", "ckunu", "cladu", "clani",
"claxu", "clika", "clira", "clite", "cliva", "clupa", "cmaci",
"cmalu", "cmana", "cmavo", "cmene", "cmila", "cmima", "cmoni",
"cnano", "cnebo", "cnemu", "cnici", "cnino", "cnisa", "cnita", "co",
"co'a", "co'e", "co'u", "cokcu", "condi", "cortu", "cpacu", "cpana",
"cpare", "cpedu", "cpina", "cradi", "crane", "creka", "crepu",
"cribe", "crida", "crino", "cripu", "crisa", "critu", "ctaru",
"ctebi", "cteki", "ctile", "ctino", "ctuca", "cukla", "cukta",
"culno", "cumki", "cumla", "cunmi", "cunso", "cuntu", "cupra",
"curmi", "curnu", "curve", "cusku", "cutci", "cutne", "cuxna", "da",
"da'a", "dacru", "dacti", "dadjo", "dakfu", "dakli", "damba", "damri",
"dandu", "danfu", "danlu", "danmo", "danre", "dansu", "danti",
"daplu", "dapma", "dargu", "darlu", "darno", "darsi", "darxi",
"daski", "dasni", "daspo", "dasri", "datka", "datni", "decti",
"degji", "dejni", "dekpu", "dekto", "delno", "dembi", "denci",
"denmi", "denpa", "dertu", "derxi", "desku", "detri", "dicra",
"dikca", "diklo", "dikni", "dilcu", "dilnu", "dimna", "dinju",
"dinko", "dirba", "dirce", "dirgo", "dizlo", "djacu", "djedi",
"djica", "djine", "djuno", "do", "donri", "dotco", "draci", "drani",
"drata", "drudi", "du", "du'u", "dugri", "dukse", "dukti", "dunda",
"dunja", "dunku", "dunli", "dunra", "dzena", "dzipo", "facki",
"fadni", "fagri", "falnu", "famti", "fancu", "fange", "fanmo",
"fanri", "fanta", "fanva", "fanza", "fapro", "farlu", "farna",
"farvi", "fasnu", "fatci", "fatne", "fatri", "febvi", "femti",
"fendi", "fengu", "fenki", "fenra", "fenso", "fepni", "fepri",
"ferti", "festi", "fetsi", "figre", "filso", "finpe", "finti",
"flalu", "flani", "flecu", "fliba", "flira", "fo'a", "fo'e", "fo'i",
"foldi", "fonmo", "fonxa", "forca", "fraso", "frati", "fraxu",
"frica", "friko", "frili", "frinu", "friti", "frumu", "fukpi",
"fulta", "funca", "fusra", "fuzme", "gacri", "gadri", "galfi",
"galtu", "galxe", "ganlo", "ganra", "ganse", "ganti", "ganxo",
"ganzu", "gapci", "gapru", "garna", "gasnu", "gasta", "genja",
"gento", "genxu", "gerku", "gerna", "gidva", "gigdo", "ginka",
"girzu", "gismu", "glare", "gleki", "gletu", "glico", "gluta",
"gocti", "gotro", "gradu", "grake", "grana", "grasu", "greku",
"grusi", "grute", "gubni", "gugde", "gundi", "gunka", "gunma",
"gunro", "gunse", "gunta", "gurni", "guska", "gusni", "gusta",
"gutci", "gutra", "guzme", "ja", "jabre", "jadni", "jakne", "jalge",
"jalna", "jalra", "jamfu", "jamna", "janbe", "janco", "janli",
"jansu", "janta", "jarbu", "jarco", "jarki", "jaspu", "jatna",
"javni", "jbama", "jbari", "jbena", "jbera", "jbini", "jdari",
"jdice", "jdika", "jdima", "jdini", "jduli", "je", "jecta", "jeftu",
"jegvo", "jei", "jelca", "jemna", "jenca", "jendu", "jenmi", "jerna",
"jersi", "jerxo", "jesni", "jetce", "jetnu", "jgalu", "jganu",
"jgari", "jgena", "jgina", "jgira", "jgita", "jibni", "jibri",
"jicla", "jicmu", "jijnu", "jikca", "jikru", "jilka", "jilra",
"jimca", "jimpe", "jimte", "jinci", "jinga", "jinku", "jinme",
"jinru", "jinsa", "jinto", "jinvi", "jinzi", "jipci", "jipno",
"jirna", "jisra", "jitfa", "jitro", "jivbu", "jivna", "jmaji",
"jmifa", "jmina", "jmive", "jo", "jo'e", "jo'u", "joi", "jordo",
"jorne", "ju", "jubme", "judri", "jufra", "jukni", "jukpa", "julne",
"jundi", "jungo", "junla", "junri", "junta", "jurme", "jursa",
"jutsi", "juxre", "jvinu", "ka", "kabri", "kacma", "kadno", "kafke",
"kagni", "kajde", "kajna", "kakne", "kakpa", "kalci", "kalri",
"kalsa", "kalte", "kamju", "kamni", "kampu", "kanba", "kancu",
"kandi", "kanji", "kanla", "kanro", "kansa", "kantu", "kanxe",
"karbi", "karce", "karda", "kargu", "karli", "karni", "katna",
"kavbu", "ke", "ke'e", "kecti", "kei", "kelci", "kelvo", "kenra",
"kensa", "kerfa", "kerlo", "ketco", "kevna", "kicne", "kijno",
"kilto", "kinli", "kisto", "klaji", "klaku", "klama", "klani",
"klesi", "klina", "kliru", "kliti", "klupe", "kluza", "kobli",
"kojna", "kolme", "komcu", "konju", "korbi", "korcu", "korka",
"kosta", "kramu", "krasi", "krati", "krefu", "krici", "krili",
"krinu", "krixa", "kruca", "kruji", "kruvi", "ku'a", "kubli", "kucli",
"kufra", "kukte", "kulnu", "kumfa", "kumte", "kunra", "kunti",
"kurfa", "kurji", "kurki", "kuspe", "kusru", "labno", "lacpu",
"lacri", "ladru", "lafti", "lakne", "lakse", "lalxu", "lamji",
"lanbi", "lanci", "lanka", "lanli", "lanme", "lante", "lanxe",
"lanzu", "larcu", "lasna", "lastu", "latmo", "latna", "lazni", "le'e",
"lebna", "lenjo", "lenku", "lerci", "lerfu", "li'i", "libjo", "lidne",
"lifri", "lijda", "limna", "lindi", "linji", "linsi", "linto",
"lisri", "liste", "litce", "litki", "litru", "livga", "livla", "lo'e",
"logji", "lojbo", "loldi", "lorxu", "lubno", "lujvo", "lumci",
"lunbe", "lunra", "lunsa", "mabla", "mabru", "macnu", "makcu",
"makfa", "maksi", "malsi", "mamta", "manci", "manfo", "manku",
"manri", "mansa", "manti", "mapku", "mapni", "mapti", "marbi",
"marce", "marde", "margu", "marji", "marna", "marxa", "masno",
"masti", "matci", "matli", "matne", "matra", "mavji", "maxri",
"mebri", "megdo", "mei", "mekso", "melbi", "meljo", "menli", "mensi",
"mentu", "merko", "merli", "mexno", "mi", "midju", "mifra", "mikce",
"mikri", "milti", "milxe", "minde", "minji", "minli", "minra",
"mintu", "mipri", "mirli", "misno", "misro", "mitre", "mixre",
"mlana", "mlatu", "mleca", "mledi", "mluni", "mo'a", "mo'i", "moi",
"mokca", "moklu", "molki", "molro", "morji", "morko", "morna",
"morsi", "mosra", "mraji", "mrilu", "mruli", "mu", "mu'e", "mucti",
"mudri", "mukti", "mulno", "munje", "mupli", "murse", "murta",
"muslo", "mutce", "muvdu", "muzga", "na", "na'e", "nabmi", "nakni",
"nalci", "namcu", "nanba", "nanca", "nandu", "nanla", "nanmu",
"nanvi", "narge", "narju", "natfe", "natmi", "navni", "naxle",
"nazbi", "nejni", "nelci", "nenri", "ni", "nibli", "nicte", "nikle",
"nilce", "nimre", "ninmu", "nirna", "nitcu", "nivji", "nixli", "no",
"no'e", "nobli", "notci", "nu", "nu'o", "nukni", "nupre", "nurma",
"nutli", "nuzba", "pa", "pacna", "pagbu", "pagre", "pajni", "palci",
"palku", "palne", "palta", "pambe", "panci", "pandi", "panje",
"panka", "panlo", "panpi", "panra", "pante", "panzi", "papri",
"parbi", "pastu", "patfu", "patlu", "patxu", "pe'a", "pelji", "pelxu",
"pemci", "penbi", "pencu", "pendo", "penmi", "pensi", "perli",
"pesxu", "petso", "pezli", "pi", "pi'u", "picti", "pijne", "pikci",
"pikta", "pilji", "pilka", "pilno", "pimlu", "pinca", "pindi",
"pinfu", "pinji", "pinka", "pinsi", "pinta", "pinxe", "pipno",
"pixra", "plana", "platu", "pleji", "plibu", "plini", "plipe",
"plise", "plita", "plixa", "pluja", "pluka", "pluta", "polje",
"polno", "ponjo", "ponse", "porpi", "porsi", "porto", "prali",
"prami", "prane", "preja", "prenu", "preti", "prije", "prina",
"pritu", "prosa", "pruce", "pruni", "pruxi", "pu'i", "pu'u", "pulce",
"pulji", "pulni", "punji", "punli", "purci", "purdi", "purmo",
"racli", "ractu", "radno", "rafsi", "ragve", "rakso", "raktu",
"ralci", "ralju", "ralte", "randa", "rango", "ranji", "ranmi",
"ransu", "ranti", "ranxi", "rapli", "rarna", "ratcu", "ratni", "re",
"rebla", "rectu", "remna", "renro", "renvi", "respa", "ricfu",
"rigni", "rijno", "rilti", "rimni", "rinci", "rinju", "rinka",
"rinsa", "rirci", "rirni", "rirxe", "rismi", "risna", "ritli",
"rivbi", "ro", "roi", "rokci", "romge", "ropno", "rorci", "rotsu",
"rozgu", "ruble", "rufsu", "runme", "runta", "rupnu", "rusko",
"rutni", "sabji", "sabnu", "sacki", "saclu", "sadjo", "sakci",
"sakli", "sakta", "salci", "salpo", "salta", "samcu", "sampu",
"sance", "sanga", "sanji", "sanli", "sanmi", "sanso", "santa",
"sarcu", "sarji", "sarlu", "sarxe", "saske", "satci", "satre",
"savru", "sazri", "se", "sefta", "selci", "selfu", "semto", "senci",
"senpi", "senta", "senva", "sepli", "serti", "setca", "sevzi",
"sfani", "sfasa", "sfofa", "sfubu", "si'o", "siclu", "sicni", "sidbo",
"sidju", "sigja", "silka", "silna", "simlu", "simsa", "simxu",
"since", "sinma", "sinso", "sinxa", "sipna", "sirji", "sirxo",
"sisku", "sisti", "sitna", "sivni", "skaci", "skami", "skapi",
"skari", "skicu", "skiji", "skina", "skori", "skoto", "skuro",
"slabu", "slaka", "slami", "slanu", "slari", "slasi", "sligu",
"slilu", "sliri", "slovo", "sluji", "sluni", "smacu", "smadi",
"smaji", "smani", "smoka", "smuci", "smuni", "snada", "snanu",
"snidu", "snime", "snipa", "snuji", "snura", "snuti", "so", "so'a",
"so'e", "so'i", "so'o", "so'u", "sobde", "sodna", "sodva", "softo",
"solji", "solri", "sombo", "sonci", "sorcu", "sorgu", "sovda",
"spaji", "spali", "spano", "spati", "speni", "spisa", "spita",
"spofu", "spoja", "spuda", "sputu", "sraji", "sraku", "sralo",
"srana", "srasu", "srera", "srito", "sruma", "sruri", "stace",
"stagi", "staku", "stali", "stani", "stapa", "stasu", "stati",
"steba", "steci", "stedu", "stela", "stero", "stici", "stidi",
"stika", "stizu", "stodi", "stuna", "stura", "stuzi", "su'e", "su'o",
"su'u", "sucta", "sudga", "sufti", "suksa", "sumji", "sumne", "sumti",
"sunga", "sunla", "surla", "sutra", "ta", "tabno", "tabra", "tadji",
"tadni", "tagji", "talsa", "tamca", "tamji", "tamne", "tanbo",
"tance", "tanjo", "tanko", "tanru", "tansi", "tanxe", "tapla",
"tarbi", "tarci", "tarla", "tarmi", "tarti", "taske", "tatpi",
"tatru", "tavla", "taxfu", "tcaci", "tcadu", "tcana", "tcati",
"tcena", "tcica", "tcidu", "tcika", "tcila", "tcima", "tcini",
"tcita", "te", "temci", "tenfa", "tengu", "terdi", "terpa", "terto",
"ti", "tigni", "tikpa", "tilju", "tinbe", "tinci", "tinsa", "tirna",
"tirse", "tirxu", "tisna", "titla", "tivni", "tixnu", "to'e", "toknu",
"toldi", "tonga", "tordu", "torni", "traji", "trano", "trati",
"trene", "tricu", "trina", "trixe", "troci", "tsali", "tsani",
"tsapi", "tsiju", "tsina", "tu", "tubnu", "tugni", "tujli", "tumla",
"tunba", "tunka", "tunlo", "tunta", "tuple", "turni", "tutci",
"tutra", "va", "vacri", "vajni", "valsi", "vamji", "vamtu", "vanbi",
"vanci", "vanju", "vasru", "vasxu", "ve", "ve'e", "vecnu", "venfu",
"vensa", "verba", "vi", "vibna", "vidni", "vidru", "vifne", "vikmi",
"viknu", "vimcu", "vindu", "vinji", "vipsi", "virnu", "viska",
"vitci", "vitke", "vitno", "vlagi", "vlile", "vlina", "vlipa", "vo",
"vofli", "voksa", "vorme", "vraga", "vreji", "vreta", "vrici",
"vrude", "vrusi", "vu", "vukro", "xa", "xabju", "xadba", "xadni",
"xagji", "xagri", "xajmi", "xaksu", "xalbo", "xalka", "xalni",
"xamgu", "xampo", "xamsi", "xance", "xanka", "xanri", "xanto",
"xarci", "xarju", "xarnu", "xasli", "xasne", "xatra", "xatsi",
"xazdo", "xe", "xebni", "xebro", "xecto", "xedja", "xekri", "xelso",
"xendo", "xenru", "xexso", "xindo", "xinmo", "xirma", "xislu",
"xispo", "xlali", "xlura", "xotli", "xrabo", "xrani", "xriso",
"xruba", "xruki", "xrula", "xruti", "xukmi", "xunre", "xurdo",
"xusra", "xutla", "za'i", "za'o", "zabna", "zajba", "zalvi", "zanru",
"zarci", "zargu", "zasni", "zasti", "zbabu", "zbani", "zbasu",
"zbepi", "zdani", "zdile", "ze", "ze'e", "ze'o", "zekri", "zenba",
"zepti", "zetro", "zgana", "zgike", "zi'o", "zifre", "zinki", "zirpu",
"zivle", "zmadu", "zmiku", "zo'a", "zo'i", "zu'o", "zukte", "zumri",
"zungi", "zunle", "zunti", "zutse", "zvati"};

static char *rafsi[] =
{"ba'u", "dri", "baj", "baf", "bak", "ba'e", "bav", "bac", "bad",
"ban", "bau", "bal", "ba'i", "ba'o", "bax", "baz", "bap", "bai",
"bra", "bag", "ba'a", "bar", "bas", "bat", "beb", "bem", "be'o",
"bed", "be'e", "beg", "bej", "be'i", "bes", "be'a", "ber", "ben",
"bef", "be'u", "bet", "bev", "bei", "biv", "biz", "bic", "bik", "big",
"bi'a", "bil", "bid", "bix", "bi'o", "bir", "bit", "bis", "bim",
"bi'u", "lab", "bla", "bli", "lot", "blo", "lo'i", "bor", "bol",
"boi", "bog", "bo'u", "bot", "bo'i", "bof", "bo'o", "bon", "bo'a",
"raz", "red", "bre", "bri", "bif", "bi'e", "bij", "rit", "rod",
"bo'e", "bun", "bu'a", "bus", "bu'i", "bul", "buj", "bu'o", "buk",
"bu'u", "bum", "bud", "bur", "bu'e", "bru", "caz", "cab", "ca'a",
"dzu", "caf", "cak", "cad", "cag", "caj", "ca'o", "cal", "ca'u",
"cna", "can", "cam", "cai", "car", "cat", "cav", "snu", "ca'e", "cta",
"ca'i", "cax", "cec", "cez", "ce'o", "cel", "ce'a", "cem", "ce'u",
"cne", "ces", "cen", "ced", "cer", "cre", "cev", "cei", "cfa", "fik",
"fi'a", "cfi", "fi'u", "cib", "blu", "dja", "cid", "dro", "cif",
"cig", "cik", "cki", "cic", "cim", "cli", "cil", "cij", "cni", "ci'i",
"cin", "cpi", "cip", "cri", "ci'a", "ci'e", "cti", "cit", "ci'o",
"cir", "civ", "ciz", "kaf", "kai", "cka", "cap", "cas", "kej", "cke",
"kik", "ci'u", "ki'i", "kir", "kul", "cu'e", "ku'u", "lau", "cla",
"cau", "lir", "lit", "liv", "li'a", "cup", "cma", "ma'a", "ma'o",
"cme", "me'e", "mi'a", "mim", "cmi", "cmo", "co'i", "na'o", "neb",
"ne'o", "nem", "ne'u", "nic", "nin", "ni'o", "nis", "nit", "ni'a",
"col", "co'a", "com", "co'e", "co'u", "cko", "con", "cno", "coi",
"cor", "cro", "cpa", "par", "cpe", "cra", "cek", "rep", "rib", "rid",
"ri'o", "rip", "cis", "teb", "tek", "ce'i", "ti'o", "ctu", "cuk",
"cku", "clu", "cum", "cu'i", "cul", "cun", "cu'o", "cu'u", "pra",
"cru", "cur", "cuv", "cus", "sku", "cuc", "cut", "cux", "cu'a", "dav",
"dza", "daz", "dac", "dai", "daj", "dak", "dab", "da'a", "dad", "daf",
"dal", "da'u", "dam", "da'e", "dan", "plu", "dap", "dag", "dau",
"dar", "da'o", "dax", "da'i", "das", "spo", "sri", "dec", "deg",
"dej", "dek", "del", "de'o", "deb", "den", "de'i", "dem", "dep",
"de'a", "der", "de'u", "dre", "des", "det", "dir", "dic", "klo",
"dik", "dil", "dim", "dij", "di'u", "di'o", "dib", "di'e", "dig",
"diz", "dzi", "jac", "jau", "dje", "dei", "dji", "jin", "jun", "ju'o",
"don", "doi", "dor", "do'i", "dot", "do'o", "dra", "dat", "rud",
"dru", "dub", "du'o", "dum", "dug", "dus", "du'e", "dut", "dud",
"du'a", "duj", "duk", "du'u", "dun", "du'i", "dur", "dze", "zip",
"zi'o", "fak", "fa'i", "fad", "fag", "fan", "fam", "fa'o", "faz",
"fap", "pro", "fal", "fa'u", "far", "fa'a", "fav", "fau", "fac",
"fat", "fa'e", "fai", "feb", "fem", "fed", "feg", "fe'u", "fek",
"fer", "fe'a", "fen", "fe'o", "fep", "fei", "fre", "fes", "fet",
"fe'i", "fig", "fis", "fip", "fi'e", "fin", "fi'i", "fla", "fle",
"fli", "fir", "fo'a", "fo'e", "fo'i", "flo", "foi", "fom", "fo'o",
"fon", "fro", "fas", "fra", "fax", "fic", "fi'o", "fil", "fit", "fru",
"fuk", "fu'i", "ful", "flu", "fun", "fu'a", "fur", "fuz", "fu'e",
"gai", "gad", "gaf", "ga'i", "gal", "ga'u", "ga'o", "gan", "gas",
"ga'e", "gax", "gaz", "gac", "gap", "gar", "gau", "gat", "gej", "get",
"ge'o", "gex", "ger", "ge'u", "gen", "ge'a", "gid", "gi'a", "gig",
"gi'o", "gik", "gir", "gri", "gim", "gi'u", "gla", "gek", "gei",
"let", "gle", "gic", "gli", "glu", "goc", "got", "rau", "gra", "ga'a",
"ras", "rek", "rus", "rut", "gub", "gug", "gu'e", "gud", "gun",
"gu'a", "gum", "gur", "gu'o", "gut", "gru", "guk", "gus", "gu'i",
"guc", "guz", "zme", "jav", "jad", "ja'i", "jag", "ja'e", "jaf",
"jma", "jam", "jab", "jan", "jal", "jas", "jat", "ja'o", "jak", "jap",
"ja'a", "jva", "bam", "jba", "jbe", "jer", "bin", "bi'i", "jar",
"jdi", "di'a", "din", "di'i", "dul", "jdu", "jev", "jve", "jec",
"je'a", "jef", "jeg", "je'o", "jez", "jel", "jme", "jen", "jed",
"jem", "jei", "je'i", "jex", "jes", "je'e", "jet", "je'u", "ja'u",
"jga", "jai", "jge", "gin", "jgi", "git", "jbi", "jib", "cmu", "jij",
"jik", "jil", "jic", "jmi", "jit", "jig", "ji'a", "jim", "jir", "jis",
"jiv", "ji'i", "jiz", "jip", "ji'o", "jif", "tro", "jvi", "jaj",
"min", "miv", "ji'e", "jov", "jom", "jo'u", "jol", "joi", "jor",
"jo'o", "jon", "jo'e", "juv", "jub", "jbu", "juf", "ju'a", "juk",
"jup", "ju'e", "jud", "ju'i", "jug", "jul", "jur", "jum", "jus",
"jut", "jux", "vin", "ji'u", "kam", "kag", "jde", "ka'e", "kar",
"kas", "kat", "kau", "kac", "kad", "kaj", "kal", "ka'o", "kan",
"ka'u", "kax", "kab", "ka'a", "kav", "kem", "kep", "ke'e", "kec",
"ke'i", "kez", "kel", "kei", "ke'o", "ken", "kes", "kre", "ker",
"ket", "tco", "kev", "ke'a", "kic", "ki'e", "kij", "ki'o", "kil",
"kis", "laj", "kak", "kla", "lai", "kle", "lei", "kli", "kit", "lup",
"lu'e", "luz", "kob", "ko'i", "koj", "ko'a", "kol", "ko'e", "kom",
"kon", "ko'u", "kor", "koi", "kro", "kok", "kos", "kra", "ka'i",
"ref", "ke'u", "kri", "rin", "ki'u", "kix", "ki'a", "kuc", "ruj",
"ruv", "kru", "kuz", "kub", "kuf", "kuk", "klu", "kum", "ku'a", "kun",
"kut", "kur", "kuj", "ku'i", "kup", "ku'e", "kus", "lap", "cpu",
"lac", "lad", "laf", "la'e", "lak", "la'u", "lam", "la'i", "lal",
"lan", "lax", "laz", "lar", "la'a", "la'o", "lem", "leb", "le'a",
"len", "le'o", "lek", "lec", "ler", "le'u", "liz", "lib", "li'e",
"lif", "fri", "jda", "lim", "lid", "lij", "li'i", "lin", "li'o",
"lis", "ste", "lic", "lik", "li'u", "lil", "lom", "loj", "lob", "jbo",
"lol", "loi", "lor", "lo'u", "lu'o", "luv", "jvo", "lum", "lu'i",
"lub", "lur", "lus", "mal", "mab", "cnu", "ma'u", "maf", "mak", "mas",
"mam", "mac", "man", "mar", "map", "mat", "mra", "ma'e", "mad", "mag",
"maj", "mai", "max", "sno", "ma'i", "mav", "xri", "meb", "meg", "mem",
"mei", "mek", "me'o", "mel", "mle", "mej", "men", "mes", "me'i",
"met", "me'u", "mer", "mre", "mex", "mib", "mij", "mif", "mic", "mik",
"mil", "mli", "mid", "mi'e", "mi'i", "mir", "mit", "mi'u", "mip",
"mis", "mi'o", "tre", "mix", "xre", "mla", "lat", "mec", "me'a",
"led", "lun", "mob", "mov", "mom", "moi", "moc", "mol", "mo'u", "mlo",
"mo'o", "moj", "mo'i", "mor", "mon", "mo'a", "mro", "mos", "mri",
"mru", "mum", "muf", "mut", "mud", "muk", "mu'i", "mul", "mu'o",
"muj", "mu'e", "mup", "mur", "mu'a", "mus", "tce", "muv", "mu'u",
"muz", "nar", "nal", "nam", "nak", "na'i", "nac", "na'u", "nab",
"na'a", "nad", "nau", "nav", "nag", "naj", "naf", "na'e", "nat",
"nai", "nax", "xle", "naz", "zbi", "nen", "nel", "nei", "ner", "ne'i",
"nil", "nib", "ni'i", "cte", "nik", "ni'e", "nim", "ni'u", "nir",
"tcu", "niv", "nix", "xli", "non", "nor", "no'e", "nol", "no'i",
"not", "noi", "nun", "nu'o", "nuk", "nup", "nu'e", "num", "nul",
"nu'i", "nuz", "pav", "pa'a", "pag", "pau", "gre", "pai", "pac",
"pak", "pan", "pad", "pa'o", "pap", "pat", "paz", "pab", "pas", "paf",
"pa'u", "pax", "pev", "ple", "pel", "pem", "peb", "pec", "pe'u",
"ped", "pe'o", "pen", "pe'i", "pes", "pei", "per", "pex", "pet",
"pez", "piz", "piv", "pic", "pi'i", "pil", "pi'a", "pli", "pim",
"pi'u", "pid", "pif", "pik", "pis", "pin", "pix", "pi'o", "pir",
"xra", "pla", "lej", "le'i", "pib", "pip", "pi'e", "pit", "lix",
"luj", "puk", "pu'a", "lut", "lu'a", "plo", "pol", "pon", "po'o",
"pos", "po'e", "pop", "po'i", "por", "poi", "pot", "pal", "pam",
"pa'i", "pa'e", "pej", "pe'a", "pre", "ret", "rei", "pij", "pri",
"ros", "ro'a", "ruc", "ru'e", "pun", "rux", "ru'i", "pus", "puv",
"puc", "pu'e", "puj", "pu'i", "pul", "pur", "pru", "pud", "pum",
"pu'o", "ra'o", "raf", "rav", "ra'u", "rac", "ral", "ra'e", "rad",
"rag", "ra'i", "ram", "ran", "rax", "rap", "rar", "rat", "rel", "reb",
"rec", "re'u", "rem", "re'a", "rer", "re'o", "rev", "re'i", "res",
"rif", "cfu", "rig", "rij", "ril", "rim", "ri'u", "rik", "ri'a",
"rir", "ri'e", "ris", "ri'i", "riv", "rol", "rom", "roi", "rok",
"ro'i", "rog", "ron", "ro'o", "ror", "rot", "tsu", "ro'u", "roz",
"zgu", "rub", "ble", "ruf", "rum", "rup", "ru'u", "ruk", "ru'o",
"run", "sab", "djo", "sak", "sal", "sat", "sla", "sa'o", "sap", "sna",
"sag", "sa'a", "saj", "sa'i", "sai", "sa'u", "sra", "sax", "ske",
"sa'e", "sav", "vru", "saz", "sel", "sfe", "sle", "sef", "se'u",
"sme", "sec", "sen", "set", "sev", "sne", "sep", "sei", "ser", "se'a",
"sez", "se'i", "sfa", "sfo", "sub", "su'u", "siz", "sil", "si'i",
"sib", "si'o", "dju", "sig", "sik", "mlu", "smi", "sim", "si'u",
"si'a", "sni", "sip", "sir", "six", "sis", "sti", "sit", "siv", "sam",
"kap", "ska", "ski", "sij", "kin", "sko", "kot", "ko'o", "ku'o",
"sau", "sar", "las", "lig", "sli", "lov", "lo'o", "slu", "sma", "smo",
"muc", "mun", "smu", "sad", "nan", "nid", "si'e", "nip", "nuj", "nur",
"nu'a", "nut", "soz", "soj", "sop", "sor", "so'i", "sos", "sot",
"sob", "so'e", "sod", "sof", "slo", "sol", "som", "so'o", "son",
"soi", "soc", "sro", "sog", "sov", "so'a", "paj", "san", "spa", "spe",
"spi", "pof", "po'u", "poj", "po'a", "spu", "put", "pu'u", "raj",
"rak", "ra'a", "sas", "sre", "ru'a", "rur", "sru", "sac", "tak",
"sta", "tap", "seb", "tec", "te'i", "sed", "tel", "te'o", "sic",
"sid", "ti'i", "tik", "tiz", "sto", "sun", "tur", "su'a", "tuz",
"stu", "sup", "su'e", "suz", "su'o", "suv", "suc", "sud", "sfu",
"suk", "suj", "sum", "su'i", "sug", "sul", "sur", "sut", "taz", "tab",
"tad", "tag", "tal", "taj", "ta'o", "tac", "tau", "tas", "tax",
"ta'e", "tar", "tam", "tai", "tra", "ta'i", "tat", "tav", "ta'a",
"taf", "ta'u", "cac", "tca", "ten", "tic", "tid", "til", "tim",
"ti'a", "ter", "tem", "tei", "tef", "teg", "te'u", "ted", "tep",
"te'a", "tet", "tif", "tig", "tip", "tij", "tib", "tin", "tir", "tis",
"tit", "tiv", "tix", "ti'u", "tol", "to'e", "tok", "tod", "tog",
"to'a", "tor", "to'u", "ton", "to'i", "rai", "ren", "re'e", "ric",
"tri", "rix", "ti'e", "toc", "toi", "tsa", "tan", "tsi", "sin", "tuf",
"tu'u", "tug", "tu'i", "tuj", "tum", "tu'a", "tub", "tuk", "tul",
"tu'o", "tun", "tup", "tu'e", "tru", "tci", "tut", "vaz", "var",
"vaj", "vai", "val", "vla", "vam", "va'i", "vat", "vab", "vac", "van",
"vas", "vau", "vax", "va'u", "vel", "ve'e", "ven", "ve'u", "vef",
"ves", "ver", "ve'a", "viz", "vib", "vir", "vif", "vim", "vi'i",
"vik", "vic", "vi'u", "vid", "vij", "vip", "vri", "vis", "vi'a",
"vit", "vi'e", "vi'o", "lag", "vil", "vli", "von", "vol", "voi",
"vok", "vo'a", "vor", "vro", "vra", "rej", "vei", "vre", "vud",
"vu'e", "vus", "vu'i", "vuz", "vur", "vu'o", "xav", "xa'u", "xab",
"xad", "xam", "xak", "xal", "xag", "xau", "xap", "xa'o", "xas", "xan",
"xa'e", "xar", "xac", "xa'i", "xaj", "xa'a", "xat", "xaz", "zdo",
"xel", "xen", "xei", "xeb", "bro", "xet", "cto", "xej", "xe'a", "xek",
"xe'i", "xes", "xed", "xe'o", "xer", "xe'u", "xex", "xin", "xim",
"xir", "xi'a", "xil", "xi'u", "xip", "xla", "xlu", "xol", "xoi",
"rab", "xai", "xis", "xi'o", "xub", "xuk", "rul", "xru", "xum",
"xu'i", "xun", "xu'e", "xur", "xu'o", "xus", "xu'a", "xul", "zaz",
"za'o", "zan", "za'a", "zaj", "zal", "zar", "zau", "zac", "zai",
"zag", "za'u", "zas", "zat", "za'i", "bab", "zba", "zbe", "zda",
"zdi", "zel", "ze'e", "zev", "ze'o", "zer", "zei", "zen", "ze'a",
"zep", "zet", "zga", "zgi", "gi'e", "zil", "zif", "zi'e", "zin",
"zi'i", "zir", "zi'u", "ziv", "vle", "zma", "mau", "zmi", "zon",
"zo'a", "zor", "zo'i", "zum", "zuk", "zu'e", "zmu", "zug", "zul",
"zun", "zu'i", "zut", "tse", "zva"};

/* Indices, one per gismu/cmavo, pointing to start of associated rafsi
   in rafsi table */

static int start_indices[] = {0, 1, 1, 2, 3, 4, 5, 5, 5, 5, 5, 6, 7,
8, 9, 9, 11, 13, 14, 15, 16, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24,
24, 24, 25, 27, 29, 30, 32, 34, 35, 36, 38, 39, 41, 42, 43, 43, 44,
45, 46, 47, 48, 49, 49, 51, 51, 52, 53, 54, 56, 57, 57, 58, 59, 62,
63, 65, 67, 69, 71, 73, 73, 73, 74, 76, 77, 79, 80, 81, 82, 83, 83,
83, 83, 85, 87, 88, 90, 92, 93, 94, 96, 97, 97, 98, 99, 100, 100, 101,
102, 102, 103, 103, 104, 105, 106, 107, 109, 110, 111, 111, 111, 113,
114, 115, 116, 117, 118, 119, 120, 120, 121, 122, 123, 124, 126, 128,
128, 129, 130, 131, 132, 133, 134, 136, 137, 139, 140, 140, 141, 142,
143, 143, 144, 145, 146, 147, 148, 149, 149, 150, 151, 152, 153, 154,
154, 154, 154, 154, 154, 155, 155, 155, 156, 157, 158, 158, 158, 159,
160, 161, 161, 162, 162, 163, 164, 166, 167, 167, 168, 169, 169, 170,
171, 172, 173, 174, 176, 177, 178, 179, 180, 182, 183, 184, 185, 186,
186, 187, 188, 190, 191, 191, 192, 193, 194, 196, 197, 199, 201, 202,
204, 206, 207, 209, 210, 212, 213, 214, 216, 217, 218, 221, 223, 224,
224, 225, 226, 226, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234,
234, 235, 237, 237, 238, 239, 240, 241, 242, 244, 245, 245, 247, 248,
249, 250, 251, 252, 254, 255, 256, 258, 260, 261, 262, 263, 264, 265,
265, 267, 267, 268, 269, 271, 272, 273, 273, 274, 275, 276, 277, 278,
280, 280, 282, 282, 283, 284, 285, 285, 285, 286, 287, 288, 288, 289,
291, 292, 294, 295, 297, 299, 300, 301, 302, 303, 304, 305, 306, 306,
307, 308, 310, 311, 312, 313, 314, 316, 318, 320, 321, 322, 324, 326,
328, 330, 330, 331, 332, 334, 336, 337, 338, 340, 341, 343, 344, 346,
348, 349, 350, 352, 354, 355, 356, 357, 357, 357, 357, 359, 359, 359,
359, 360, 362, 364, 366, 367, 368, 369, 371, 372, 373, 374, 375, 377,
378, 380, 382, 384, 384, 385, 386, 388, 389, 390, 392, 394, 395, 395,
396, 397, 398, 399, 400, 401, 403, 405, 406, 407, 408, 409, 410, 411,
412, 413, 413, 414, 415, 417, 419, 421, 422, 424, 425, 426, 428, 430,
430, 431, 432, 434, 434, 435, 436, 437, 438, 439, 440, 441, 442, 444,
445, 447, 449, 451, 453, 454, 456, 458, 459, 461, 463, 465, 466, 467,
468, 469, 470, 471, 472, 473, 474, 475, 476, 478, 479, 481, 482, 484,
484, 485, 486, 487, 489, 489, 490, 490, 492, 493, 493, 495, 495, 497,
497, 497, 499, 500, 501, 502, 503, 504, 505, 505, 506, 507, 508, 509,
510, 511, 512, 513, 514, 516, 517, 518, 518, 519, 521, 523, 525, 527,
528, 530, 531, 532, 533, 534, 535, 537, 537, 538, 539, 540, 541, 543,
544, 545, 546, 547, 548, 549, 550, 551, 552, 552, 553, 554, 555, 555,
556, 556, 557, 558, 559, 559, 561, 561, 562, 563, 564, 564, 566, 567,
567, 569, 569, 569, 570, 571, 571, 572, 573, 573, 574, 576, 577, 578,
579, 581, 583, 585, 586, 588, 588, 590, 591, 592, 593, 595, 596, 597,
598, 598, 599, 600, 601, 602, 604, 605, 605, 605, 605, 605, 606, 607,
607, 608, 608, 608, 609, 610, 611, 611, 611, 612, 612, 613, 614, 615,
616, 617, 618, 619, 620, 621, 621, 621, 621, 621, 621, 622, 623, 624,
626, 628, 629, 631, 632, 633, 634, 635, 636, 638, 640, 642, 643, 644,
645, 646, 647, 648, 649, 650, 652, 653, 653, 654, 656, 657, 659, 661,
663, 664, 666, 668, 669, 670, 671, 671, 672, 673, 675, 676, 676, 678,
680, 681, 682, 684, 685, 686, 686, 687, 688, 689, 691, 691, 692, 693,
694, 696, 696, 698, 699, 699, 701, 702, 703, 704, 705, 706, 707, 709,
709, 709, 709, 710, 711, 711, 712, 713, 714, 715, 715, 716, 716, 716,
717, 719, 721, 722, 723, 725, 726, 727, 728, 730, 731, 732, 733, 735,
736, 737, 738, 739, 740, 741, 742, 742, 743, 744, 745, 747, 749, 751,
752, 754, 756, 757, 758, 759, 760, 761, 762, 763, 764, 765, 766, 767,
768, 768, 769, 770, 770, 770, 771, 771, 772, 773, 774, 775, 776, 778,
778, 779, 780, 781, 781, 781, 781, 781, 782, 783, 784, 785, 787, 789,
791, 792, 793, 795, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806,
807, 809, 810, 810, 811, 813, 814, 814, 816, 816, 817, 819, 820, 821,
823, 824, 825, 826, 827, 829, 830, 832, 833, 834, 836, 837, 839, 840,
841, 841, 842, 843, 844, 845, 846, 847, 849, 851, 853, 854, 854, 856,
857, 858, 860, 861, 862, 863, 864, 865, 866, 868, 869, 870, 871, 871,
872, 873, 874, 875, 877, 879, 879, 881, 883, 884, 886, 888, 889, 891,
892, 893, 894, 894, 896, 897, 898, 899, 901, 902, 904, 906, 908, 909,
910, 911, 913, 914, 916, 917, 918, 919, 921, 922, 923, 924, 925, 925,
925, 925, 926, 927, 927, 927, 928, 929, 929, 930, 931, 931, 932, 933,
935, 935, 936, 937, 938, 939, 940, 941, 943, 945, 947, 949, 950, 951,
952, 953, 954, 955, 956, 956, 956, 956, 957, 959, 960, 962, 962, 963,
964, 964, 965, 966, 967, 968, 969, 971, 971, 972, 974, 975, 975, 977,
977, 978, 979, 980, 982, 984, 985, 986, 988, 990, 992, 994, 995, 996,
998, 999, 1001, 1002, 1004, 1005, 1006, 1006, 1008, 1010, 1011, 1013,
1014, 1015, 1017, 1017, 1017, 1019, 1020, 1022, 1023, 1025, 1025,
1025, 1026, 1027, 1028, 1028, 1029, 1030, 1031, 1032, 1033, 1034,
1035, 1036, 1036, 1037, 1038, 1039, 1040, 1040, 1041, 1042, 1043,
1045, 1047, 1049, 1051, 1052, 1054, 1055, 1056, 1057, 1058, 1058,
1059, 1061, 1061, 1061, 1062, 1063, 1064, 1064, 1065, 1066, 1067,
1069, 1071, 1072, 1074, 1075, 1078, 1080, 1082, 1083, 1084, 1084,
1086, 1088, 1089, 1090, 1090, 1090, 1090, 1091, 1092, 1093, 1094,
1095, 1096, 1096, 1096, 1097, 1098, 1100, 1101, 1102, 1103, 1103,
1103, 1104, 1105, 1105, 1106, 1107, 1107, 1108, 1110, 1111, 1112,
1113, 1114, 1116, 1117, 1118, 1119, 1120, 1122, 1124, 1125, 1126,
1128, 1128, 1129, 1130, 1132, 1133, 1134, 1135, 1137, 1138, 1139,
1140, 1140, 1141, 1142, 1144, 1144, 1145, 1145, 1146, 1147, 1148,
1149, 1150, 1151, 1152, 1153, 1153, 1154, 1155, 1156, 1157, 1158,
1159, 1160, 1162, 1163, 1164, 1164, 1164, 1164, 1165, 1166, 1167,
1168, 1168, 1170, 1171, 1171, 1171, 1171, 1172, 1172, 1173, 1174,
1176, 1177, 1178, 1179, 1180, 1181, 1182, 1184, 1185, 1186, 1187,
1188, 1190, 1191, 1192, 1194, 1194, 1195, 1196, 1197, 1198, 1200,
1202, 1204, 1205, 1207, 1208, 1208, 1209, 1210, 1211, 1212, 1212,
1214, 1216, 1217, 1219, 1220, 1221, 1221, 1222, 1223, 1224, 1224,
1225, 1227, 1228, 1228, 1229, 1230, 1230, 1231, 1231, 1231, 1232,
1234, 1235, 1236, 1237, 1238, 1240, 1241, 1242, 1243, 1244, 1246,
1248, 1250, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 1258, 1260,
1261, 1262, 1263, 1264, 1265, 1266, 1266, 1266, 1267, 1268, 1269,
1269, 1270, 1270, 1271, 1272, 1272, 1272, 1273, 1274, 1276, 1276,
1276, 1277, 1277, 1279, 1280, 1280, 1281, 1282, 1284, 1286, 1287,
1288, 1288, 1288, 1289, 1290, 1291, 1291, 1292, 1294, 1294, 1294,
1295, 1297, 1298, 1300, 1301, 1303, 1304, 1305, 1306, 1307, 1308,
1309, 1309, 1309, 1310, 1311, 1311, 1312, 1313, 1314, 1316, 1318,
1319, 1320, 1322, 1324, 1326, 1327, 1327, 1327, 1329, 1330, 1331,
1333, 1335, 1336, 1337, 1337, 1338, 1339, 1340, 1341, 1343, 1344,
1346, 1347, 1348, 1350, 1351, 1353, 1354, 1355, 1356, 1357, 1358,
1360, 1362, 1364, 1365, 1366, 1367, 1368, 1370, 1372, 1373, 1374,
1376, 1377, 1378, 1380, 1381, 1382, 1382, 1383, 1384, 1386, 1387,
1389, 1390, 1391, 1392, 1393, 1395, 1396, 1397, 1398, 1399, 1400,
1400, 1401, 1402, 1404, 1406, 1408, 1409, 1411, 1412, 1412, 1414,
1416, 1417, 1419, 1420, 1421, 1422, 1423, 1423, 1423, 1424, 1425,
1425, 1426, 1426, 1428, 1430, 1431, 1433, 1433, 1434, 1434, 1436,
1437, 1437, 1437, 1437, 1438, 1439, 1441, 1442, 1444, 1446, 1448,
1450, 1452, 1453, 1455, 1457, 1458, 1459, 1460, 1462, 1464, 1465,
1466, 1467, 1469, 1470, 1471, 1473, 1474, 1475, 1476, 1477, 1479,
1481, 1483, 1485, 1486, 1487, 1488, 1490, 1491, 1492, 1494, 1496,
1498, 1499, 1501, 1502, 1502, 1503, 1504, 1505, 1506, 1507, 1508,
1510, 1512, 1514, 1515, 1516, 1517, 1519, 1520, 1522, 1524, 1526,
1528, 1530, 1531, 1533, 1535, 1536, 1538, 1539, 1540, 1541, 1543,
1545};

/* For each gismu/cmavo, the number of associated rafsi */

static int n_rafsi[] = {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
2, 2, 1, 1, 1, 2, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 2, 2, 1, 2, 2,
1, 1, 2, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 1, 1, 2, 1, 0,
1, 1, 3, 1, 2, 2, 2, 2, 2, 0, 0, 1, 2, 1, 2, 1, 1, 1, 1, 0, 0, 0, 2,
2, 1, 2, 2, 1, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 2,
1, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 2, 2, 0, 1, 1, 1,
1, 1, 1, 2, 1, 2, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 2,
1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 0, 1, 1,
2, 1, 0, 1, 1, 1, 2, 1, 2, 2, 1, 2, 2, 1, 2, 1, 2, 1, 1, 2, 1, 1, 3,
2, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 2, 0, 1, 1, 1,
1, 1, 2, 1, 0, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 0, 2,
0, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 0, 2, 0, 1, 1, 1, 0, 0, 1, 1,
1, 0, 1, 2, 1, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1,
1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 0, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 2,
1, 1, 2, 2, 1, 1, 1, 0, 0, 0, 2, 0, 0, 0, 1, 2, 2, 2, 1, 1, 1, 2, 1,
1, 1, 1, 2, 1, 2, 2, 2, 0, 1, 1, 2, 1, 1, 2, 2, 1, 0, 1, 1, 1, 1, 1,
1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 2, 2, 1, 2, 1, 1, 2, 2,
0, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 1, 2, 2, 1,
2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 2, 0, 1, 1, 1,
2, 0, 1, 0, 2, 1, 0, 2, 0, 2, 0, 0, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2,
0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1,
1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 1, 0, 2, 0, 0, 1, 1, 0, 1, 1, 0, 1, 2,
1, 1, 1, 2, 2, 2, 1, 2, 0, 2, 1, 1, 1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 2,
1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1,
1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 2,
2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 1, 2, 1, 2, 2, 2, 1, 2, 2, 1, 1,
1, 0, 1, 1, 2, 1, 0, 2, 2, 1, 1, 2, 1, 1, 0, 1, 1, 1, 2, 0, 1, 1, 1,
2, 0, 2, 1, 0, 2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1,
0, 1, 0, 0, 1, 2, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1,
1, 1, 0, 1, 1, 1, 2, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 0, 0, 0, 0, 1,
1, 1, 1, 2, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0,
1, 2, 1, 0, 2, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 1, 2,
1, 1, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 0, 2, 1, 1, 2, 1, 1, 1, 1, 1,
1, 2, 1, 1, 1, 0, 1, 1, 1, 1, 2, 2, 0, 2, 2, 1, 2, 2, 1, 2, 1, 1, 1,
0, 2, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1,
1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 2, 0, 1, 1, 1, 1, 1,
1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 2, 1, 2, 0, 1, 1, 0,
1, 1, 1, 1, 1, 2, 0, 1, 2, 1, 0, 2, 0, 1, 1, 1, 2, 2, 1, 1, 2, 2, 2,
2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 0, 2, 2, 1, 2, 1, 1, 2, 0, 0, 2, 1, 2,
1, 2, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
1, 1, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 0, 1, 2, 0, 0, 1, 1, 1, 0, 1, 1,
1, 2, 2, 1, 2, 1, 3, 2, 2, 1, 1, 0, 2, 2, 1, 1, 0, 0, 0, 1, 1, 1, 1,
1, 1, 0, 0, 1, 1, 2, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 2, 1, 1, 1,
1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 0, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 0,
1, 1, 2, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 2,
1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 2, 1, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1,
1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 0, 1, 1, 1, 1, 2, 2, 2, 1, 2, 1,
0, 1, 1, 1, 1, 0, 2, 2, 1, 2, 1, 1, 0, 1, 1, 1, 0, 1, 2, 1, 0, 1, 1,
0, 1, 0, 0, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1,
1, 1, 0, 2, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1,
1, 2, 0, 0, 1, 0, 2, 1, 0, 1, 1, 2, 2, 1, 1, 0, 0, 1, 1, 1, 0, 1, 2,
0, 0, 1, 2, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 2,
2, 1, 1, 2, 2, 2, 1, 0, 0, 2, 1, 1, 2, 2, 1, 1, 0, 1, 1, 1, 1, 2, 1,
2, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2,
1, 1, 2, 1, 1, 0, 1, 1, 2, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1,
1, 2, 2, 2, 1, 2, 1, 0, 2, 2, 1, 2, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0,
2, 2, 1, 2, 0, 1, 0, 2, 1, 0, 0, 0, 1, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2,
2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1,
1, 2, 1, 1, 2, 2, 2, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1,
2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 1, 2, 1, 1, 1, 2, 2, 1};


#define N(x) (sizeof(x)/sizeof(x[0]))
  
/* Slightly wide definition of 'gismu' since some of the entities with
   rafsi are cmavo as well */
static int lookup_gismu(char *s) {
  /* Simple linear search for now */
  int n, i, found;
  n = N(gismu);
  found = 0;
  for (i=0; i<n; i++) {
    if (!strcmp(gismu[i], s)) {
      found = 1;
      break;
    } 
  }
  if (found) {
    return i;
  } else {
    return -1;
  }
}

/* Return 1 if the string t has the correct form to be a lujvo (note,
   there is no dictionary lookup of the apparent constituent rafsi) */
static int
is_valid_lujvo(char *t) {
  char *p;
  int debug = 0;

  if (debug) printf("Testing [%s] as valid lujvo\n", t);
  if (strlen(t) < 5) {
    if (debug) printf("Initial length too short, invalid\n");
    return 0;
  }
  while (*t) {
    if (debug) printf("Residual [%s]\n", t);
    p = strchr(t, 'y');
    if ((p-t) == 4) {
      /* Starts with abcdy, have to check whether abcd could be a valid rafsi */
      if (is_consonant(t[0]) && is_consonant(t[1]) && is_vowel(t[2]) && is_consonant(t[3]) && is_initialpairok(t)) {
        if (debug) printf("4 letter rafsi CCVC at start valid then y\n");
        t+=5;
      } else if (is_consonant(t[0]) && is_vowel(t[1]) && is_consonant(t[2]) && is_consonant(t[3]) && is_pairok(t)) {
        if (debug) printf("4 letter rafsi CVCC at start valid then y\n");
        t+=5;
      } else {
        if (debug) printf("4 letter rafsi at start invalid\n");
        return 0;
      }
    } else if ((p-t) == 3) {
      /* Starts with 3 letter rafsi then a join */
      if (is_ccv(t)) {
        if (is_initialpairok(t)) {
          /* Perhaps ought to check whether rafsi form is in dictionary? */
          if (debug) printf("3 letter rafsi CCV at start valid then y\n");
          return 1;
        } else {
          if (debug) printf("3 letter rafsi CCV at start invalid then y\n");
          return 0;
        }
      } else if (is_cvc(t)) {
        /* Dictionary test? */
        if (debug) printf("3 letter rafsi CVC at start valid then y\n");
        return 1;
      } else if (is_cvv(t)) {
        /* Not possible */
        fprintf(stderr, "Can't have y after CVV form rafsi\n");
        exit(1);
      }
    } else if ((p-t) < 3) {
      if (debug) printf("<3 letters left, invalid\n");
      return 0;
    } else if ((strlen(t) > 5) || (strlen(t) == 3)) {
      /* Strip leading rafsi if valid */
      if (is_ccv(t)) {
        if (is_initialpairok(t)) {
          if (debug) printf("Initial CCV, examine tail\n");
          t+=3;
          /* Go round loop again */
        } else {
          if (debug) printf("Initial invalid CCV\n");
          return 0;
        }
      } else if (is_cvc(t)) {
        if (debug) printf("Initial CVC, examine tail\n");
        t+=3;
        /* Go round again */
      } else if (is_cvv(t)) {
        if (debug) printf("Initial CVV, examine tail\n");
        if (t[2] = '\'') {
          t+=4;
        } else {
          t+=3;
        }
      } else {
        /* Not valid lujvo */
        if (debug) printf("Invalid, prefix not any rafsi form\n");
        return 0;
      }
    } else if (strlen(t) == 5) {
      /* Just a gismu left, assume OK if correct form */
      if (is_cvccv(t) || is_ccvcv(t)) { 
        if (debug) printf("Matches gismu form\n");
        return 1;
      } else {
        if (debug) printf("Unmatched 5 character form\n");
        return 0;
      }
    } else {
      if (debug) printf("Unrecognized length\n");
      return 0;
    }
  }
  /* If we fall out of the loop, all rafsi have been checked off. */
  return 1;
}

#define MAXT 50

typedef struct {
  char *word;
  int score;
} Lujvo;

static Lujvo lujvo[65536];
static int nl = 0;

/* Comparison function for qsort() */
static int compare_lujvo (const void *a, const void *b) {
  const Lujvo *aa = (const Lujvo *) a;
  const Lujvo *bb = (const Lujvo *) b;
  if (aa->score > bb->score) {
    return 1;
  } else if (aa->score < bb->score) {
    return -1;
  } else {
    return 0;
  }
}

#define MAXLUJVO 8

/* The main lujvo making routine */
static void makelujvo(char **tanru) {
  char t[MAXT][6]; /* The gismu/cmavo forms passed in */
  char r[MAXT][5][6]; /* The possible rafsi, up to 5 per arg */
  int nr[MAXT]; /* Number of rafsi per arg */
  char g[MAXT]; /* 'Glue' (hyphen) char to insert after respective
                   rafsi (0 for none) */
  int nt; /* Number of forms passed in */
  int i, j, k, n;
  int last;
  int index, si;
  int uselong;
  char rr[5];
  int c[MAXT]; /* Counters over the rafsi forms for each argument
                  (implements an arbitrarily-nested for loop) */

  uselong = 0;
  i=0;
  while (*tanru) {
    strcpy(t[i], *tanru);
    for (j=0; ; j++) {
      if (!t[i][j]) break;
      t[i][j] = tolower(t[i][j]);
      if (t[i][j] == 'h') t[i][j] = '\'';
    }
    index = lookup_gismu(t[i]);
    if (index < 0) {
      fprintf(stderr, "Cannot use component [%s] in forming lujvo\n", t[i]);
      exit(1);
    }

    ++tanru;
    last = !*tanru; /* This is the last one */

    n = n_rafsi[index];
    si = start_indices[index];
    if (last) {
      /* Use 5 letter rafsi and any short rafsi must end in a vowel */
      j = 0;
      for (k=0; k<n; k++) {
        if (ends_in_vowel(rafsi[si+k])) {
          strcpy(r[i][j], rafsi[si+k]);
          j++;
        }
      }
      if (uselong || j==0) {
        strcpy(r[i][0], t[i]);
        j++;
      }
      nr[i] = j;
    } else {
      /* Use 4 letter rafsi and any short rafsi at all */
      j = 0;
      for (k=0; k<n; k++) {
        strcpy(r[i][j], rafsi[si+k]);
        j++;
      }
      if ((uselong || j==0) && (strlen(t[i]) == 5)) {
        strcpy(r[i][0], t[i]);
        chop_last_char(r[i][0]);
        j ++;
      }
      nr[i] = j;
    }
    i++;
  }
  nt = i;

  /* Now have to work through all combinations of rafsi. */
  /* Print out rafsi for checking */
  printf("Possible rafsi for input words :\n");
  for (i=0; i<nt; i++) {
    for (j=0; j<nr[i]; j++) {
      printf("%s ", r[i][j]);
    }
    printf("\n");
  }
  printf("--------------------\n");
  printf(" Score  Lujvo\n");
  printf("--------------------\n");

  /* Initialise multi dimensional loop */
  for (i=0; i<nt; i++) {
    c[i] = 0;
  }

  /* The main loop over all permutations of rafsi */
  do {
    /* Set glue to zero */
    for (i=0; i<nt; i++) {
      g[i]= 0;
    }

    /* Work out glue */
    if ((nt > 2) && is_cvv(r[0][c[0]])) {
      /* Require r or n hyphen to stop initial cmavo falling off */
      if (r[1][c[1]][0] == 'r') {
        g[0] = 'n';
      } else {
        g[0] = 'r';
      }
    } else {
#if 0
      if (!strcmp(r[0][c[0]], "lo'i") && !strcmp(r[1][c[1]], "lei")) {
        printf("nt=%d test1=%d test2=%d\n", nt, is_cvv(r[0][c[0]]), is_ccv(r[1][c[1]]));
      }
#endif
      if ((nt == 2) && is_cvv(r[0][c[0]]) && !is_ccv(r[1][c[1]])) {
        if (r[1][c[1]][0] == 'r') {
          g[0] = 'n';
        } else {
          g[0] = 'r';
        }
      }
    }

    /* Insert y hyphens where necessary due to non-permissible
       consonant pairs. */
    for (i=0; i<nt-1; i++) {
      if ((g[i]==0) && !can_join(r[i][c[i]], r[i+1][c[i+1]])) {
        g[i] = 'y';
      }
    }

    /* Insert y hyphens after any 4 letter rafsi (except CV'V form) */
    for (i=0; i<nt-1; i++) {
      if (strlen(r[i][c[i]]) == 4 && r[i][c[i]][2] != '\'') {
        g[i] = 'y';
      }
    }
    
    if (is_cvc(r[0][c[0]])) {
      /* Have to apply step 5 ('tosmabru failure' test).  The test is,
         if the leading CV is stripped off, does the remainder
         constitute a valid lujvo on its own? */
      char temp[6*MAXT];
      char *p, *q;
      p = temp;
      *p++ = r[0][c[0]][2]; /* Pick up trailing consonant of 1st rafsi */
      if (g[0]) {
        /* Can't fail tosmabru test */
      } else {
        for (i=1; i<nt; i++) {
          for (q=&r[i][c[i]][0]; *q; q++) {
            *p++ = *q;
          }
          if ((i<nt-1) && g[i]) {
            *p++ = g[i];
          }
        }
        *p = 0;
        if (is_valid_lujvo(temp)) {
          /* Add glue after 1st rafsi */
#if 0
          printf("Glue needed\n");
#endif
          g[0] = 'y';
        }
      }      
    }      

    /* Now have all the glue characters determined.  Concatenate rafsi
       and glue and save the result for scoring. */
    {
      char temp[6*MAXT];
      char *p, *q;
      int L, A, H, R, V, rr;
      
      p = temp;
      for (i=0; i<nt; i++) {
        for (q=&r[i][c[i]][0]; *q; q++) {
          *p++ = *q;
        }
        if ((i<nt-1) && g[i]) {
          *p++ = g[i];
        }
      }
      *p = 0;
#if 0
      printf("%s\n", temp);
#endif
      lujvo[nl].word = (char *) malloc(strlen(temp) + 1);
      strcpy(lujvo[nl].word, temp);

      /* Work out score */
      L = A = H = R = V = 0;
      for (i=0; i<nt; i++) {
        L += strlen(r[i][c[i]]);
        for (j=0; r[i][c[i]][j]; j++) {
          if (r[i][c[i]][j] == '\'') {
            A++;
          }
          if (strchr("aeiou", r[i][c[i]][j])) V++;
        }
        if (i<nt-1 && g[i]) H++, L++;
        if (is_cvccv(r[i][c[i]])) {
          rr = 1;
        } else if (is_ccvcv(r[i][c[i]])) {
          rr = 3;
        } else if (strlen(r[i][c[i]]) == 4 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_vowel(r[i][c[i]][1]) &&
                   is_consonant(r[i][c[i]][2]) &&
                   is_consonant(r[i][c[i]][3])) {
          rr = 2;
        } else if (strlen(r[i][c[i]]) == 4 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_consonant(r[i][c[i]][1]) &&
                   is_vowel(r[i][c[i]][2]) &&
                   is_consonant(r[i][c[i]][3])) {
          rr = 4;
        } else if (strlen(r[i][c[i]]) == 4 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_vowel(r[i][c[i]][1]) &&
                   r[i][c[i]][2] == '\'' &&
                   is_vowel(r[i][c[i]][3])) {
          rr = 6;
        } else if (strlen(r[i][c[i]]) == 3 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_vowel(r[i][c[i]][1]) &&
                   is_consonant(r[i][c[i]][2])) {
          rr = 5;
        } else if (strlen(r[i][c[i]]) == 3 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_consonant(r[i][c[i]][1]) &&
                   is_vowel(r[i][c[i]][2])) {
          rr = 7;
        } else if (strlen(r[i][c[i]]) == 3 &&
                   is_consonant(r[i][c[i]][0]) &&
                   is_vowel(r[i][c[i]][1]) &&
                   is_vowel(r[i][c[i]][2])) {
          rr = 8;
        } else {
          fprintf(stderr, "Unmatched rafsi [%s]\n", r[i][c[i]]);
          exit(1);
        }
#if 0
        printf("Rafsi [%s] rr=%d\n", r[i][c[i]], rr);
#endif
        R += rr;
      }
#if 0
      printf("L=%d A=%d H=%d R=%d V=%d\n", L, A, H, R, V);
#endif
      lujvo[nl].score = 1000*L - 500*A + 100*H - 10*R - V;
      nl++;
    }

    /* Bump counter array */
    {
      int done = 1;
      for (i=0; i<nt; i++) {
        ++c[i];
        if (c[i] == nr[i]) {
          c[i] = 0;
        } else {
          done = 0;
          break;
        }
      }
      if (done) break;
    }
  } while (1);

  qsort(lujvo, nl, sizeof(Lujvo), compare_lujvo);

  if (nl>MAXLUJVO) nl = MAXLUJVO;
  for (i=0; i<nl; i++) {
    printf("%6d %s\n", lujvo[i].score, lujvo[i].word);
  }

}

int main (int argc, char **argv) {
  if (argc > 1 && !strcmp(argv[1], "-v")) {
    fprintf(stderr, "jvocuhadju version %s\n", version_string);
    exit(0);
  }
  makelujvo(argv+1);
}
