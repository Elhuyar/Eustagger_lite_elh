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
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

vector<string> filtratuEzagunak(vector<string> &emaitza,map<string,string> &analisi_mapa){
    vector<string> forma_ezagunak;
    stringstream sstr;
    int fIndex = 0;
    size_t found;
    for(int i = 0; i < emaitza.size(); i++){
        found = emaitza[i].find("/");
        if (found!=std::string::npos){
            if (analisi_mapa[emaitza[i]] != ""){
                forma_ezagunak.push_back(emaitza[i]);
                sstr.str("");
                sstr << fIndex;
                forma_ezagunak.push_back(sstr.str());
                emaitza.erase(emaitza.begin() + i,emaitza.begin() + i + 1);
                found = emaitza[i].find("/");
                while(!(found!=std::string::npos) && (emaitza.size() > i)){
                    emaitza.erase(emaitza.begin() + i,emaitza.begin() + i + 1);
                    found = emaitza[i].find("/");
                }
                i--;
            }
            fIndex++;
        }
    }
    reverse(forma_ezagunak.begin(), forma_ezagunak.end());
    return forma_ezagunak;
}

void konbinatuFiltratuak(string &fitxategiIzena,vector<string> &emaitzaFiltratua,map<string,string> &analisi_mapa){
    string PHAT = ".morf";
    string filSarrera = fitxategiIzena + PHAT;
    string filIrteera = fitxategiIzena + "_new" + PHAT;
    string line;
    ifstream infile (filSarrera.c_str());
    ofstream outfile (filIrteera.c_str());
    int ffIndex = 0;
    string ffTestua = "";
    int fIndex = 0;
    if (emaitzaFiltratua.size() > 0){
        if (outfile.is_open()){
            ffTestua = emaitzaFiltratua.back();
            emaitzaFiltratua.pop_back();
            ffIndex = atoi(emaitzaFiltratua.back().c_str());
            emaitzaFiltratua.pop_back();
            getline(infile,line);
            while((infile.is_open() && !infile.eof()) || (emaitzaFiltratua.size() > 0) || (fIndex <= ffIndex)){
                while (fIndex == ffIndex){
                    outfile << analisi_mapa[ffTestua] << "\n";
                    if (emaitzaFiltratua.size() > 0){
                        ffTestua = emaitzaFiltratua.back();
                        emaitzaFiltratua.pop_back();
                        ffIndex = atoi(emaitzaFiltratua.back().c_str());
                        emaitzaFiltratua.pop_back();
                    }
                     else{
                        ffIndex = 0;
                    }
                    fIndex++;
                }
                if (infile.is_open() && !infile.eof()){
                    outfile << line + "\n";
                    getline(infile,line);
                    while ((infile.is_open() && !infile.eof()) && !((line.substr(0,2) == "\"<")&&(line.substr(line.size()-2,2) == ">\""))){
                        outfile << line + "\n";
                        getline(infile,line);
                    } 
                }
                fIndex++;
            }
        }
        if (infile.is_open()){
            infile.close();
	    remove(filSarrera.c_str());
        }
        outfile.close();
        rename(filIrteera.c_str(),filSarrera.c_str());
    }
}
