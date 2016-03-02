# Hau erabiltzeko honako ingurune aldagaiak definitu behar dira:

#deps: morfologia

INCLUDE=  -I../morfologia

SOURCES= automata.cc token_raw.cc file_mng_raw.cc status_raw.cc an_lex_raw.cc aurreproz_raw.cc

INCLUDEMORF=$(INCLUDE) 

MEMBERS=$(SOURCES:.cc=.o)


CCC= g++
CCFLAGS= -O -fpic -Wall 
CPPFLAGS= 

COMPILE.cc=$(CCC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDE) -c

all: $(MEMBERS)


$(MEMBERS): $(SOURCES)


clean: FORCE
	rm -f $(MEMBERS)


FORCE:
