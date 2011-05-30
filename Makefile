#
# Makefile for textprinter
#
INSTALLDIR = ~/bin
MAIN = textprinter
GCC = gcc

# Normal
#CFLAGS = 
# For debug
CFLAGS = 
OBJECTS = 
HEADERS = 
LIBS = -lcairo

all : $(MAIN)

install : $(MAIN)
	cp $(MAIN) $(INSTALLDIR)

$(MAIN) : $(MAIN).c $(HEADERS) $(OBJECTS)
	$(GCC) $(CFLAGS) -o $(MAIN) $(MAIN).c $(OBJECTS)	$(LIBS)

