#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "analizatzailea.h"
#include <unistd.h>
#include "taipak.h"

int idatz_formatoa_anali()
{
  stringstream analiVersion;
  analiVersion << "anali honako aukerekin konpilatu da:" << endl;
#ifdef _USE_FOMA_
  analiVersion << "\tFOMAko transduktoreak erabilita" << endl;
#else
  analiVersion << "\tXEROXeko transduktoreak erabilita" << endl;
#endif // _USE_FOMA_
#ifdef __BRIDGE_VARIANTS__
  analiVersion << "\taldaeren analisirik ez da egingo" << endl;
#else
  analiVersion << "\taldaeren analisia egingo da" << endl;
#endif //__BRIDGE_VARIANTS__

  cerr << "Erabilera: anali [-lu2map] [-e lexiko_fitxategia]" << endl;
  cerr << "\t-l  : tokenizatzailearen irteera du sarrera gisa" << endl;
  cerr << "\t-u  : UZEIko lexikoa sarreratzat hartu" << endl;
  cerr << "\t-m  : modua = ESTANDAR_PIPE" << endl;
  cerr << "\t-2  : analisian bigarren aukera erabili, analizatzaile inkrementalaren ordez (defektuz ez)" << endl;
  cerr << "\t-e  : erabiltzailearen lexikoa erabiltzeko. Lexikoaren kokapen osoa eman behar da ondoren" << endl;
  cerr << "\t\tProbak egiteko edo lexikoaren formatua ikusteko, ikusi ~maguirrezaba008/corpusak/morfologiaLexikoa/lex_erab_4.4.lex fitxategia" << endl;
  cerr << "\t-a  : analisi guztiak utzi. Defektuz ez (desanbiguazio lokala)" << endl;
  cerr << "\t-p  : irteera guztiz parentizatua emateko (defektuz ez)" << endl;
  cerr << analiVersion.str() ;
  exit(1);
}
automata_lag autofil2;

main(int argc, char *argv[])
{
  int Sarrera_berezia = 0 ; /*** NEREA : 99/9/2 -L aukerarako */
  int lexiko_uzei = 0;
  int bigarren_aukera = 0; /*** IÑAKI : 2000/11/20 -2 aukerarako */
  int lex_ald = 0; /*** IÑAKI : 2000/12/20 erabiltzailearen lexikorako -L eta -2 ez dira onartzen*/
  int ez_estandarrak = 0;
  int parentizatua = 0;
  int deslokala = 1;
  int modua = ESTANDAR_AN;
  analizatzailea anali;
  char c;
  string lex_izena;
  vector<string> emaitza,sarrera;

 
  while ((c = getopt(argc, argv, "aApPlL2uUe:E:3mMhH")) != EOF) {
    switch (c) {
    case 'E':
    case 'e': lex_ald = 1; lex_izena=optarg; break;
    case 'a':
    case 'A': deslokala = 0; break;/*** Analisi guztiak mantendu*/
    case 'l':
    case 'L': Sarrera_berezia = 1; parentizatua = 1; break;/*** tipografikoak bakarrik */
    case 'u':
    case 'U': lexiko_uzei = 1; break;
    case 'p':
    case 'P': parentizatua = 1; Sarrera_berezia = 1; break;
    case 'm':
    case 'M': modua = ESTANDAR_PIPE; break;
    case '2': bigarren_aukera  = 1; ez_estandarrak = 0; break;
    case '3': bigarren_aukera  = 1; ez_estandarrak = 1; break;
    case 'h':
    default: idatz_formatoa_anali();
    }
  }
 
  anali.hasieraketak(Sarrera_berezia,lexiko_uzei,bigarren_aukera,ez_estandarrak,lex_ald,lex_izena,parentizatua,deslokala);
  emaitza = anali.analizatu(modua,&sarrera);
  anali.amaierakoak(); // Ez dabil ondo fomakoa
  return(1);
}

