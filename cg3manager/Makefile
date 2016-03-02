# #################
# CG3 kudeatzeko modulua

#deps: pcre++, vislcg 3, boost

INCLUDE_PCREPLUSPLUS = 
INCLUDE_BOOST = 
INCLUDE_CG3 = 
INCLUDE_FREELING = 
INCLUDE =  $(INCLUDE_PCREPLUSPLUS) $(INCLUDE_CG3) $(INCLUDE_FREELING) $(INCLUDE_BOOST)

SOURCES = cgmanager.cc cgconv.cc euparole.cc

MEMBERS=$(SOURCES:.cc=.o)

CCC=g++
CPPFLAGS= -DHAVE_BOOST
CCFLAGS= -O -fpic 

INCLUDEDIR = $(INCLUDE)

COMPILE.cc=$(CCC) $(INCLUDEDIR) $(CCFLAGS) $(CPPFLAGS) -c

all: $(MEMBERS)


$(MEMBERS): $(SOURCES)


clean: FORCE
	'rm' -f $(MEMBERS)

install: FORCE
	install -d -m 755 $(INSTALL_PATH)/include/
	install -m 644 *.h $(INSTALL_PATH)/include/

FORCE:
