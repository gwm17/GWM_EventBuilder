OS_NAME := $(shell uname -s)
CC=g++
ROOTCFLAGS= `root-config --cflags`
ROOTGLIBS=`root-config --glibs`

ROOTDICT_INCL=./
CFLAGS= -std=c++11 -fPIC -g -Wall $(ROOTCFLAGS)
INCLDIR=./include
SRCDIR=./src
BINDIR=./bin
LIBDIR=./lib
CPPFLAGS= -I$(INCLDIR)
LDFLAGS=$(ROOTGLIBS)

BUSRCDIR=$(SRCDIR)/builder
PSRCDIR=$(SRCDIR)/plotter
BISRCDIR=$(SRCDIR)/binary2root
GUISRCDIR=$(SRCDIR)/gui

OBJDIR=./objs

BUSRC=$(wildcard $(BUSRCDIR)/*.cpp)
PSRC=$(wildcard $(PSRCDIR)/*.cpp)
BISRC=$(wildcard $(BISRCDIR)/*.cpp)
GUISRC=$(wildcard $(GUISRCDIR)/*.cpp)
BUOBJS=$(BUSRC:$(BUSRCDIR)/%.cpp=$(OBJDIR)/%.o)
POBJS=$(PSRC:$(PSRCDIR)/%.cpp=$(OBJDIR)/%.o)
BIOBJS=$(BISRC:$(BISRCDIR)/%.cpp=$(OBJDIR)/%.o)
GUIOBJS=$(GUISRC:$(GUISRCDIR)/%.cpp=$(OBJDIR)/%.o)

DICT_PAGES= $(INCLDIR)/DataStructs.h $(INCLDIR)/LinkDef_sps.h
DICT=$(SRCDIR)/sps_dict.cxx
DICTOBJ=$(OBJDIR)/sps_dict.o
DICTLIB=$(LIBDIR)/libSPSDict

GDICT_PAGES=$(INCLDIR)/EVBMainFrame.h $(INCLDIR)/FileViewFrame.h $(INCLDIR)/LinkDef_Gui.h
GDICT=$(SRCDIR)/gui_dict.cxx
GDICTOBJ=$(OBJDIR)/gui_dict.o

RCSRC=$(SRCDIR)/RunCollector.cpp
RCOBJ=$(OBJDIR)/RunCollector.o

SWSRC=$(SRCDIR)/Stopwatch.cpp
SWOBJ=$(OBJDIR)/Stopwatch.o

EVBSRC=$(SRCDIR)/GWMEventBuilder.cpp
EVBOBJ=$(OBJDIR)/GWMEventBuilder.o

#entry points
EVBGUIMSRC=$(SRCDIR)/gui_main.cpp
EVBGUIMAIN=$(OBJDIR)/gui_main.o

EVBMSRC=$(SRCDIR)/main.cpp
EVBMAIN=$(OBJDIR)/main.o

PCH_FILE=$(INCLDIR)/EventBuilder.h
PCH=$(INCLDIR)/EventBuilder.h.gch

EVBEXE=$(BINDIR)/GWMEVB
EVBCLEXE=$(BINDIR)/GWMEVB_CL

EXES = $(BUEXE) $(PEXE) $(BIEXE) $(EVBEXE) $(EVBCLEXE)
OBJS = $(BUOBJS) $(POBJS) $(BIOBJS) $(GUIOBJS) $(DICTOBJ) $(GDICTOBJ) $(RCOBJ) $(SWOBJ) $(EVBOBJ) $(EVBGUIMAIN) $(EVBMAIN)


.PHONY: all clean clean_header

all: $(PCH) $(EVBEXE) $(EVBCLEXE)

$(PCH): $(PCH_FILE)
	$(CC) $(CFLAGS) -x c++-header $^

$(EVBEXE): $(DICTOBJ) $(GDICTOBJ) $(RCOBJ) $(SWOBJ) $(BUOBJS) $(MOBJS) $(BIOBJS) $(GUIOBJS) $(POBJS) $(EVBOBJ) $(EVBGUIMAIN)
	$(CC) $^ -o $@ $(LDFLAGS)

$(EVBCLEXE): $(DICTOBJ) $(RCOBJ) $(SWOBJ) $(BUOBJS) $(MOBJS) $(BIOBJS) $(POBJS) $(EVBOBJ) $(EVBMAIN)
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

$(GDICTOBJ): $(GDICT)
	$(CC) $(CFLAGS) -I $(ROOTDICT_INCL) -o $@ -c $^
	mv $(SRCDIR)/*.pcm $(BINDIR)/

$(DICT): $(DICT_PAGES)
	rootcint -f $@ $^

$(GDICT): $(GDICT_PAGES)
	rootcint -f $@ $^

clean:
	$(RM) $(OBJS) $(EXES) $(DICT) $(GDICT) $(DICTLIB) ./$(LIBDIR)/*.pcm ./$(BINDIR)/*.pcm

clean_header:
	$(RM) $(PCH)

VPATH= $(SRCDIR):$(BUSRCDIR):$(BISRCDIR):$(PSRCDIR):$(GUISRCDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^
