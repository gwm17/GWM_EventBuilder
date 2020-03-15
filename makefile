CC=g++
ROOTFLAGS= `root-config --cflags --glibs`
CFLAGS= -std=c++11 -g -Wall $(ROOTFLAGS)
INCLDIR=./include
SRCDIR=./src
BINDIR=./bin
ASRCDIR=$(SRCDIR)/analyzer
MSRCDIR=$(SRCDIR)/merger
CSRCDIR=$(SRCDIR)/cleaner
OBJDIR=./objs
AOBJDIR=$(OBJDIR)/analyzer
MOBJDIR=$(OBJDIR)/merger
COBJDIR=$(OBJDIR)/cleaner
CPPFLAGS= -I$(INCLDIR) -I./
LDFLAGS= -L$(INCLDIR) $(ROOTFLAGS)
ASRC=$(wildcard $(ASRCDIR)/*.cpp)
MSRC=$(wildcard $(MSRCDIR)/*.cpp)
CSRC=$(wildcard $(CSRCDIR)/*.cpp)
AOBJS=$(ASRC:$(ASRCDIR)/%.cpp=$(AOBJDIR)/%.o)
MOBJS=$(MSRC:$(MSRCDIR)/%.cpp=$(MOBJDIR)/%.o)
COBJS=$(CSRC:$(CSRCDIR)/%.cpp=$(COBJDIR)/%.o)
DICT_PAGES= $(INCLDIR)/DataStructs.h $(INCLDIR)/LinkDef_sps.h
AEXE=$(BINDIR)/analyzer
MEXE=$(BINDIR)/merger
CEXE=$(BINDIR)/cleaner
DICT=$(SRCDIR)/sps_dict.cxx
LIB=$(OBJDIR)/sps_dict.o

.PHONY: all clean

all: $(AEXE) $(MEXE) $(CEXE)

$(AEXE): $(LIB) $(AOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(MEXE): $(LIB) $(MOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(CEXE): $(LIB) $(COBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(LIB): $(DICT)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^
	mv $(SRCDIR)/*.pcm ./

$(DICT): $(DICT_PAGES)
	rootcint -f $@ -c $^

$(AOBJDIR)/%.o: $(ASRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(MOBJDIR)/%.o: $(MSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

$(COBJDIR)/%.o: $(CSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

clean:
	$(RM) $(AOBJS) $(MOBJS) $(COBJS) $(AEXE) $(MEXE) $(CEXE) $(DICT) $(LIB) ./*.pcm
