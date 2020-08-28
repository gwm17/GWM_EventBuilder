
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

BUSRCDIR=$(SRCDIR)/builder
MSRCDIR=$(SRCDIR)/merger
PSRCDIR=$(SRCDIR)/plotter
BISRCDIR=$(SRCDIR)/binary2root

OBJDIR=./objs

BUSRC=$(wildcard $(BUSRCDIR)/*.cpp)
MSRC=$(wildcard $(MSRCDIR)/*.cpp)
PSRC=$(wildcard $(PSRCDIR)/*.cpp)
BISRC=$(wildcard $(BISRCDIR)/*.cpp)
BUOBJS=$(BUSRC:$(BUSRCDIR)/%.cpp=$(OBJDIR)/%.o)
MOBJS=$(MSRC:$(MSRCDIR)/%.cpp=$(OBJDIR)/%.o)
POBJS=$(PSRC:$(PSRCDIR)/%.cpp=$(OBJDIR)/%.o)
BIOBJS=$(BISRC:$(BISRCDIR)/%.cpp=$(OBJDIR)/%.o)

DICT_PAGES= $(INCLDIR)/DataStructs.h $(INCLDIR)/LinkDef_sps.h
DICT=$(SRCDIR)/sps_dict.cxx
DICTOBJ=$(OBJDIR)/sps_dict.o
DICTLIB=$(LIBDIR)/libSPSDict

RCSRC=$(SRCDIR)/RunCollector.cpp
RCOBJ=$(OBJDIR)/RunCollector.o

PCH_FILE=$(INCLDIR)/EventBuilder.h
PCH=$(INCLDIR)/EventBuilder.h.gch

BUEXE=$(BINDIR)/builder
MEXE=$(BINDIR)/merger
PEXE=$(BINDIR)/plotter
BIEXE=$(BINDIR)/binary2root

EXES = $(BUEXE) $(MEXE) $(PEXE) $(BIEXE)
OBJS = $(BUOBJS) $(MOBJS) $(POBJS) $(BIOBJS) $(DICTOBJ) $(RCOBJ)

.PHONY: all clean clean_header

all: $(PCH) $(BUEXE) $(MEXE) $(PEXE) $(BIEXE)

$(PCH): $(PCH_FILE)
	$(CC) $(CFLAGS) -x c++-header $^

$(BUEXE): $(DICTOBJ) $(RCOBJ) $(BUOBJS)
	$(CC) $^ -o $@ $(LDFLAGS) 

$(MEXE): $(DICTOBJ) $(RCOBJ) $(MOBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(PEXE): $(DICTOBJ) $(RCOBJ) $(POBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BIEXE): $(LIBARCHIVE) $(RCOBJ) $(BIOBJS)
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

$(LIBDIR)/lib%.dylib: $(AOBJDIR)/%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -dynamiclib -o $@

$(LIBDIR)/lib%.so: $(AOBJDIR)/%.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -shared -o $@

clean:
	$(RM) $(OBJS) $(EXES) $(DICT) $(DICTLIB) ./$(LIBDIR)/*.pcm ./$(BINDIR)/*.pcm

clean_header:
	$(RM) $(PCH)

VPATH= $(SRCDIR):$(BUSRCDIR):$(BISRCDIR):$(MSRCDIR):$(PSRCDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -I $(LIBARCHIVE_INCL) -o $@ -c $^
