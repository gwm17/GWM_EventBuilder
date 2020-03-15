CC=g++
ROOTFLAGS= `root-config --cflags --glibs`
LIBARCHIVE=/usr/lib64/libarchive.so
CFLAGS= -std=c++11 -g -Wall $(ROOTFLAGS)
INCLDIR=./include
SRCDIR=./src
BINDIR=./bin
ASRCDIR=$(SRCDIR)/analyzer
MSRCDIR=$(SRCDIR)/merger
CSRCDIR=$(SRCDIR)/cleaner
BSRCDIR=$(SRCDIR)/binary2root
OBJDIR=./objs
AOBJDIR=$(OBJDIR)/analyzer
MOBJDIR=$(OBJDIR)/merger
COBJDIR=$(OBJDIR)/cleaner
BOBJDIR=$(OBJDIR)/binary2root
CPPFLAGS= -I$(INCLDIR) -I./
LDFLAGS= -L$(INCLDIR) $(ROOTFLAGS)
ASRC=$(wildcard $(ASRCDIR)/*.cpp)
MSRC=$(wildcard $(MSRCDIR)/*.cpp)
CSRC=$(wildcard $(CSRCDIR)/*.cpp)
BSRC=$(wildcard $(BSRCDIR)/*.cpp)
AOBJS=$(ASRC:$(ASRCDIR)/%.cpp=$(AOBJDIR)/%.o)
MOBJS=$(MSRC:$(MSRCDIR)/%.cpp=$(MOBJDIR)/%.o)
COBJS=$(CSRC:$(CSRCDIR)/%.cpp=$(COBJDIR)/%.o)
BOBJS=$(BSRC:$(BSRCDIR)/%.cpp=$(BOBJDIR)/%.o)
DICT_PAGES= $(INCLDIR)/DataStructs.h $(INCLDIR)/LinkDef_sps.h
AEXE=$(BINDIR)/analyzer
MEXE=$(BINDIR)/merger
CEXE=$(BINDIR)/cleaner
BEXE=$(BINDIR)/binary2root
DICT=$(SRCDIR)/sps_dict.cxx
LIB=$(OBJDIR)/sps_dict.o

.PHONY: all clean

all: $(AEXE) $(MEXE) $(CEXE) $(BEXE)

$(AEXE): $(LIB) $(AOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(MEXE): $(LIB) $(MOBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(CEXE): $(LIB) $(COBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BEXE): $(LIBARCHIVE) $(BOBJS)
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

$(BOBJDIR)/%.o: $(BSRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^

clean:
	$(RM) $(AOBJS) $(MOBJS) $(COBJS) $(BOBJS) $(AEXE) $(MEXE) $(CEXE) $(BEXE) $(DICT) $(LIB) ./*.pcm
