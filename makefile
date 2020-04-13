CC=g++
ROOTCFLAGS= `root-config --cflags`
ROOTGLIBS=`root-config --glibs`

LIBARCHIVE=/usr/local/opt/libarchive/lib/libarchive.dylib
LIBARCHIVE_INCL=/usr/local/opt/libarchive/include
CFLAGS= -std=c++11 -g -Wall $(ROOTCFLAGS)
INCLDIR=./include
SRCDIR=./src
BINDIR=./bin
CPPFLAGS= -I$(INCLDIR) -I./ -I$(LIBARCHIVE_INCL)
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
LIB=$(OBJDIR)/sps_dict.o

RCSRC=$(SRCDIR)/RunCollector.cpp
RCOBJ=$(OBJDIR)/RunCollector.o

AEXE=$(BINDIR)/analyzer
MEXE=$(BINDIR)/merger
CEXE=$(BINDIR)/cleaner
BEXE=$(BINDIR)/binary2root

.PHONY: all clean

all: $(AEXE) $(MEXE) $(CEXE) $(BEXE)

$(AEXE): $(LIB) $(RCOBJ) $(AOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(MEXE): $(LIB) $(RCOBJ) $(MOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(CEXE): $(LIB) $(RCOBJ) $(COBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BEXE): $(LIBARCHIVE) $(RCOBJ) $(BOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(LIB): $(DICT)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^
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
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

clean:
	$(RM) $(AOBJS) $(MOBJS) $(COBJS) $(BOBJS) $(AEXE) $(MEXE) $(CEXE) $(BEXE) $(DICT) $(LIB) $(RCOBJ) ./$(BINDIR)/*.pcm
