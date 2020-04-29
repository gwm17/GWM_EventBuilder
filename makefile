
OS_NAME := $(shell uname -s)
CC=g++
ROOTCFLAGS= `root-config --cflags`
ROOTGLIBS=`root-config --glibs`

LIBARCHIVE=/usr/local/opt/libarchive/lib/libarchive.dylib
LIBARCHIVE_INCL=/usr/local/opt/libarchive/include
ROOTDICT_INCL=./
CFLAGS= -std=c++11 -fPIC -g -Wall $(ROOTCFLAGS)
INCLDIR=./include
SRCDIR=./src
BINDIR=./bin
LIBDIR=./lib
CPPFLAGS= -I$(INCLDIR)
LDFLAGS=$(ROOTGLIBS)

ASRCDIR=$(SRCDIR)/analyzer
MSRCDIR=$(SRCDIR)/merger
CSRCDIR=$(SRCDIR)/cleaner
BSRCDIR=$(SRCDIR)/binary2root

OBJDIR=./objs
AOBJDIR=$(OBJDIR)/analyzer
MOBJDIR=$(OBJDIR)/merger
COBJDIR=$(OBJDIR)/cleaner
BOBJDIR=$(OBJDIR)/binary2root

ASRC=$(wildcard $(ASRCDIR)/*.cpp)
MSRC=$(wildcard $(MSRCDIR)/*.cpp)
CSRC=$(wildcard $(CSRCDIR)/*.cpp)
BSRC=$(wildcard $(BSRCDIR)/*.cpp)
AOBJS=$(ASRC:$(ASRCDIR)/%.cpp=$(AOBJDIR)/%.o)
MOBJS=$(MSRC:$(MSRCDIR)/%.cpp=$(MOBJDIR)/%.o)
COBJS=$(CSRC:$(CSRCDIR)/%.cpp=$(COBJDIR)/%.o)
BOBJS=$(BSRC:$(BSRCDIR)/%.cpp=$(BOBJDIR)/%.o)

DICT_PAGES= $(INCLDIR)/DataStructs.h $(INCLDIR)/LinkDef_sps.h
DICT=$(SRCDIR)/sps_dict.cxx
DICTOBJ=$(OBJDIR)/sps_dict.o
DICTLIB=$(LIBDIR)/libSPSDict

RCSRC=$(SRCDIR)/RunCollector.cpp
RCOBJ=$(OBJDIR)/RunCollector.o

PCH_FILE=$(INCLDIR)/EventBuilder.h
PCH=$(INCLDIR)/EventBuilder.h.gch

AEXE=$(BINDIR)/analyzer
MEXE=$(BINDIR)/merger
CEXE=$(BINDIR)/cleaner
BEXE=$(BINDIR)/binary2root

.PHONY: all clean clean_header

all: $(PCH) $(ALIBS) $(AEXE) $(MEXE) $(CEXE) $(BEXE)

$(PCH): $(PCH_FILE)
	$(CC) $(CFLAGS) -x c++-header $^

$(AEXE): $(DICTOBJ) $(RCOBJ) $(AOBJS)
	$(CC) $^ -o $@ $(LDFLAGS) 

$(MEXE): $(DICTOBJ) $(RCOBJ) $(MOBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CEXE): $(DICTOBJ) $(RCOBJ) $(COBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BEXE): $(LIBARCHIVE) $(RCOBJ) $(BOBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(DICTOBJ): $(DICT)
	$(CC) $(CFLAGS) -I $(ROOTDICT_INCL) -o $@ -c $^
ifeq ($(OS_NAME), Darwin) 
	$(CC) $(CFLAGS) $(LDFLAGS) $@ -dynamiclib -o $(DICTLIB).dylib
	cp $(SRCDIR)/*.pcm $(LIBDIR)
else
ifeq ($(OS_NAME), Linux)
	$(CC) $(CFLAGS) $(LDFLAGS) $@ -shared -o $(DICTLIB).so
	cp $(SRCDIR)/*.pcm $(LIBDIR)
endif
endif
	mv $(SRCDIR)/*.pcm ./$(BINDIR)/

$(DICT): $(DICT_PAGES)
	rootcint -f $@ $^

$(RCOBJ): $(RCSRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(AOBJDIR)/%.o: $(ASRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(MOBJDIR)/%.o: $(MSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(COBJDIR)/%.o: $(CSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(BOBJDIR)/%.o: $(BSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -I $(LIBARCHIVE_INCL) -o $@ -c $^

$(LIBDIR)/lib%.dylib: $(AOBJDIR)/%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -dynamiclib -o $@

$(LIBDIR)/lib%.so: $(AOBJDIR)/%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -shared -o $@

clean:
	$(RM) $(AOBJS) $(MOBJS) $(COBJS) $(BOBJS) $(AEXE) $(ALIBS) $(MEXE) $(CEXE) $(BEXE) $(DICT) $(DICTOBJ) $(DICTLIB) $(RCOBJ) ./$(LIBDIR)/*.pcm ./$(BINDIR)/*.pcm

clean_header:
	$(RM) $(PCH)
