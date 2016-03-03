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


#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

#include "dat_orok.h"
#include "constants_decl.h"

#include "formatua.h"

#ifdef _USE_SWI_
#include "SWI-cpp.h"
#endif

// socket gehigarria
// Default server parameters
#define DEFAULT_MAX_WORKERS 16   // maximum number of workers simultaneously active.
#define DEFAULT_QUEUE_SIZE 500   // maximum number of waiting clients

#include "fsocket.h"
#include <sys/wait.h>
#include <semaphore.h>

using namespace std;

int edbl_bertsioa = 4;
static map<string,string> analisi_mapa;

// socket gehigarria
sem_t semaf;
socket_CS *sock; 

std::string getEnvVar(std::string const& key);

extern void segHasieraketak(int sar_lem, int lex_uzei, int bigarren, int ez_est, int erab_lex, string &lexiko_izena, int parentizatua, int deslokala);
extern void segAmaierakoak();

extern vector<string> segmentazioaSortuRawBat(string &fitxategiIzena);
extern vector<string> filtratuEzagunak(vector<string> &emaitza,map<string,string> &analisi_mapa);
extern void segmentazioaSortuRawBi(string &fitxategiIzena, string &segIrteera,vector<string> &emaitza);
extern void morfosintaxiaSortuRaw(string &fitxategiIzena, string &segIrteera, bool haul_seguruak, bool cg3form);
extern void konbinatuFiltratuak(string &fitxategiIzena,vector<string> &emaitzaFiltratua,map<string,string> &analisi_mapa);
extern string prozesatuCG3Raw(int maila, string &oinIzen);

void help() {
    stringstream eustaggerVersion;
    eustaggerVersion << "eustagger honako aukerekin konpilatu da:" << endl;
    #ifdef _USE_FOMA_
    eustaggerVersion << "\tFOMAko transduktoreak erabilita" << endl;
    #else
    eustaggerVersion << "\tXEROXeko transduktoreak erabilita" << endl;
    #endif // _USE_FOMA_
    #ifdef _USE_SWI_
    eustaggerVersion << "\tSWI-prolog erabilita morfosintaxian" << endl;
    #else
    eustaggerVersion << "\tsicstus-prolog erabilita morfosintaxian" << endl;
    #endif // _USE_SWI_
    #ifdef __BRIDGE_VARIANTS__
    eustaggerVersion << "\taldaeren analisirik ez da egingo" << endl;
    #else
    eustaggerVersion << "\taldaeren analisia egingo da" << endl;
    #endif //__BRIDGE_VARIANTS__

    cerr << "Erabilera:" <<endl;
    cerr << "eustagger_lite [-hs] [-p portua] [-a tamaina] [-d fitxategia] [-m maila]" << endl;
    cerr << "-h laguntza hau" << endl;
    cerr << "-s HAUL seguruak prozesatu (defektuz ez)" << endl;
    cerr << "-p [portua] socket-ek erabili behar duten portua" << endl;
    cerr << "-a [1|2|3] zenbat aurre analisi kargatu (defektuz 1) 1->187.535 analisi, 2->426.435 analisi, 3->928.728 analisi"<< endl;
    cerr << "-d [lex fitxategia] erabiltzailearen hiztegia (defektuz ez)" << endl;
    cerr << "-m [0|1|2|3|4] (defektuz 2)" << endl;
    cerr << "-m 0 denean ez du desanbiguatuko" << endl;  
    cerr << "-m 4 denean bakarrik aplikatuko du CG3 desanbiguatzeko" << endl;
    cerr << eustaggerVersion.str() ;
    exit(EXIT_FAILURE);
}

map<string,string> initMap(int a_size) {
  ///  printf("\n\n## Analisiak irakurtzen... ##\n\n");
  map<string,string> a_map;
  
  char* tmp = 0;
  char fitxIzena[200];
  
  if ( (tmp = getenv("IXA_PREFIX")) != 0 ){
    strcpy( fitxIzena , tmp);
  } 
    
  strcat( fitxIzena, "/var/eustagger_lite/morfologia/" );
  
  char filename[200];
  sprintf(filename,"analisia_%d.dat",a_size);  
  strcat( fitxIzena, filename );    
  ifstream infile(fitxIzena);
  string line;
  string map_index = "";
  size_t ocurrences;
  size_t pos;
  while (std::getline(infile, line)){

    if (line != ""){
      if (line.substr(0,2) == "\"<"){
	map_index = line;
	replace(map_index.begin(), map_index.end(), '\"', '/');
	ocurrences = count(map_index.begin(),map_index.end(),'/');
	if (ocurrences < 3){
	  map_index = map_index + "/";
	}
	a_map[map_index] = line;
      }
      else{
	a_map[map_index] = a_map[map_index] + "\n" + line;
      }
    }
  }
  infile.close();
  ///  printf("\n\n## Ikurritako analisi kopurua: %d ##\n\n",a_map.size());
  return a_map;
}

// socket gehigarria
//---- Capture signal informing that a child ended ---
void child_ended(int n) {
  int status;
  wait(&status);  
  sem_post(&semaf);
}

//----  Capture signal to shut server down cleanly
void terminate (int param) {
  wcerr<<L"SERVER.DISPATCHER: Signal received. Stopping"<<endl;
  exit(0);
}

//----  Initialize server socket, signals, etc.
void InitServer(int port_number) {
  pid_t myPID=getpid();
  char host[256];
  strcpy(host, "localhost"); 
  
  // open sockets to listen for clients
  sock = new socket_CS(port_number,DEFAULT_QUEUE_SIZE);
  
  // Capture terminating signals, to exit cleanly.
  signal(SIGTERM,terminate); 
  signal(SIGQUIT,terminate);   
  // Be signaled when children finish, to keep count of active workers.
  signal(SIGCHLD,child_ended); 
  // Init worker count sempahore
  sem_init(&semaf,0,DEFAULT_MAX_WORKERS);
}

//----  Wait for a client and fork a worker to attend its requests
int WaitClient() {
  int pid=0;
  wcerr<<L"SERVER.DISPATCHER: Waiting for a free worker slot"<<endl;
  sem_wait(&semaf);
  wcerr<<L"SERVER.DISPATCHER: Waiting connections"<<endl;
  sock->wait_client();
  
  pid = fork();
  if (pid < 0) wcerr<<L"ERROR on fork"<<endl;
  
  if (pid!=0) {
    // we are the parent. Close client socket and wait for next client
    sock->set_parent();
    wcerr<<L"SERVER.DISPATCHER: Connection established. Forked worker "<<pid<<"."<<endl;
  }
  else { 
    // we are the child. Close request socket and prepare to get data from client.
    sock->set_child();
  }
  
  return pid;
}

//---- Clean up and end worker when client finishes.
void CloseWorker(){
  wcerr<<L"SERVER.WORKER: client ended. Closing connection."<<endl;
  sock->close_connection();
  exit(0);
}

/////// Functions to wrap I/O mode (server socket) ////////

//---- Read a line from input channel
int ReadLine(string &text) {
  int n=0;
  n = sock->read_message(text);
  return n;
}

int main(int argc, char *argv[]){
  int Sarrera_berezia = 1 ; /*** 99/9/2 -L aukerarako */
  int lexiko_uzei = 0;
  int bigarren_aukera = 0; /*** 2000/11/20 -2 aukerarako */
  int lex_ald = 0; /*** 2000/12/20 erabiltzailearen lexikorako -L eta -2 ez dira onartzen*/
  int ez_estandarrak = 0;
  int maila = 2;
  int deslokala = 1;
  int parentizatua = 1;
  int emaitzik = 0;
  int analisi_ezagunak = 1;
  int analisi_kopurua = 1;
  bool haul_seguruak = false;
  char c;
  vector<string> emaitza;
  vector<string> emaitzaFiltratua;
  string lex_izena; 
  string fitxategiIzena;
  string segIrteera;
  string erantzuna;
  int portua = 0;
  
  while ((c = getopt(argc, argv, "sShHP:p:A:a:M:m:D:d:")) != EOF) {
    switch (c) {
    case 'S':
    case 's': haul_seguruak = 1; break;
    case 'M':
    case 'm': maila =atoi(optarg); break;
    case 'P':
    case 'p': portua=atoi(optarg); break;
    case 'A':
    case 'a': analisi_ezagunak =1;analisi_kopurua=atoi(optarg); break;
    case 'D':
    case 'd': lex_ald =1;lex_izena=optarg; break;
    case 'H':
    case 'h':
    default: help();
    }
  }
  
#ifdef _USE_SWI_
  PlEngine e(argv[0]);
#endif
  
  if (Sarrera_berezia) parentizatua = 1;
  edbl_bertsioa = 4;
  segHasieraketak(Sarrera_berezia,lexiko_uzei,bigarren_aukera,ez_estandarrak,lex_ald,lex_izena,parentizatua,deslokala);
  if (analisi_ezagunak){
    if (analisi_kopurua < 0) analisi_kopurua = 0;
    if (analisi_kopurua > 100) analisi_kopurua = 100;
    analisi_mapa = initMap(analisi_kopurua);
  }
  
  if (portua > 0) {
    InitServer(portua);
    while (1) {
      int n=WaitClient();
      if (n!=0) continue;
      ReadLine(fitxategiIzena);
      
      erantzuna = "";
      emaitza = segmentazioaSortuRawBat(fitxategiIzena);
      if (analisi_ezagunak){
	emaitzaFiltratua = filtratuEzagunak(emaitza,analisi_mapa);
      }
      emaitzik = emaitza.size() + emaitzaFiltratua.size();
      if (emaitza.size() > 0){
	segmentazioaSortuRawBi(fitxategiIzena,segIrteera,emaitza);
	morfosintaxiaSortuRaw(fitxategiIzena,segIrteera,haul_seguruak,OUT_MG);
      }
      if (emaitzaFiltratua.size() > 0){
	konbinatuFiltratuak(fitxategiIzena,emaitzaFiltratua,analisi_mapa);
      }
      if (emaitzik > 0){
	erantzuna = prozesatuCG3Raw(maila,fitxategiIzena) ;
      }
      sock->write_message(erantzuna);
      CloseWorker();
    }
  }
  else if (optind < argc) {
    fitxategiIzena = argv[optind];
    erantzuna = "";
    emaitza = segmentazioaSortuRawBat(fitxategiIzena);
    if (analisi_ezagunak){
      emaitzaFiltratua = filtratuEzagunak(emaitza,analisi_mapa);
    }
    emaitzik = emaitza.size() + emaitzaFiltratua.size();
    if (emaitza.size() > 0){
      segmentazioaSortuRawBi(fitxategiIzena,segIrteera,emaitza);
      morfosintaxiaSortuRaw(fitxategiIzena,segIrteera,haul_seguruak,OUT_MG);
    }
    if (emaitzaFiltratua.size() > 0){
      konbinatuFiltratuak(fitxategiIzena,emaitzaFiltratua,analisi_mapa);
    }
    if (emaitzik > 0){
      erantzuna = prozesatuCG3Raw(maila,fitxategiIzena) ;
    }
    cout <<  erantzuna <<endl;
  }    
  segAmaierakoak();
}
