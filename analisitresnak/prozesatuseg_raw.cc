//////////////////////////////////////////////////////////////////
//
//  EUSTAGGER LITE
//
//  Copyright (C) 1996-2013  IXA Taldea
//                           EHU-UPV
//
//  This file is part of EUSTAGGER LITE.
//
//  EUSTAGGER LITE is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  EUSTAGGER LITE is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
//  Contact: Ixa Taldea (ixa@ehu.es)
//           649 Posta kutxa
//           20080 Donostia (Basque Country)
//
//////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "aurreproz_raw.h"
#include "taipak.h"
#include "dat_orok.h"
#include "formatua.h"

#include "analizatzailea.h"


extern int XEROX;
extern int xeroxerako;
extern int edbl_bertsioa;
analizatzailea anali;
automata_lag autofil2;
aurreprozRaw proz;

extern int irakur_automata(char *fp,automata_lag *aut,int erre,int zut);


std::string getEnvVar(std::string const& key);


void segHasieraketak(int sar_lem, int lex_uzei, int bigarren, int ez_est, int erab_lex, string &lexiko_izena, int parentizatua, int deslokala) {
    anali.hasieraketak(sar_lem, lex_uzei, bigarren, ez_est, erab_lex, lexiko_izena, parentizatua, deslokala);
}

void segAmaierakoak() {
 anali.amaierakoak();
}

vector<string> segmentazioaSortuRawBat(string &fitxategiIzena) {
  vector<string> emaitza;

  char *autoFil=strdup("autofiltro2.dat");
  char *autoAurre=strdup(AURREAN_LEX);

  irakur_automata(autoFil,&autofil2,ESTADU2_MAX,MULTZO2_MAX);
  free(autoFil);

  if (fitxategiIzena.length() == 0) {
   char *stdIn=strdup("stdin");
   proz.init(autoAurre,stdIn);
   free(stdIn);
  }
  else {
    char *sarfitx = strdup(fitxategiIzena.c_str());
    proz.init(autoAurre,sarfitx);
    free(sarfitx);
  }
  free(autoAurre);
  proz.aurreprozesua(ANALIRA,&emaitza);
  return emaitza;
}

void segmentazioaSortuRawBi(string &fitxategiIzena, string &segIrteera,vector<string> &emaitza) {
    vector<string>::iterator hasi,amai,non;
    emaitza = anali.analizatu(TAULAN,&emaitza);
    stringstream segString;

    hasi = emaitza.begin();
    amai = emaitza.end();
    for (non=hasi;non!=amai;++non) {
        segString << *non;
    }
    segIrteera = segString.str();
}
