# $Header$
#
# Makefile for parser/glosser
#
# COPYRIGHT
#

CC=gcc
CFLAGS= -g -Wall
LIBDIR=-L/opt/free/lib
INCDIR=-I/opt/free/include
OBJS2 = main.o lex1.o lex2.o cmavotab.o rpc.tab.o functions.o \
        categ.o nonterm.o tree.o translate.o latex.o \
        properties.o conversion.o terms.o memory.o tenses.o \
	output.o textout.o htmlout.o connect.o stag.o

SRCS2 = $(OBJS2:%.o=%.c)

all : jbofihe dictupdate

jbofihe : $(OBJS2)
	$(CC) $(CFLAGS) -o jbofihe $(OBJS2) $(LIBDIR) -lgdbm

clean:
	@rm *.dvi *.aux *.tex *.out *.log *~ *.ps *.pdf *.html

conflict :
	perl terminator.pl < rpc2x.y > rpc2x_noet.y
	bison -v rpc2x_noet.y

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.s : %.c
	$(CC) $(CFLAGS) -S $<

rpc2x_nc.y : rpc2x.y uncom
	./uncom < rpc2x.y > rpc2x_nc.y

rpc2x_act.y nonterm.h nonterm.c : rpc2x_nc.y
	perl ./action.perl < rpc2x_nc.y > rpc2x_act.y

# Don't compile bison generated part with debug!
rpc.tab.o : rpc.tab.c
	$(CC) -O2 -c -Wall rpc.tab.c

rpc.tab.c rpc.tab.h : rpc2x_act.y
	bison -v -d rpc2x_act.y
	mv rpc2x_act.tab.c rpc.tab.c
	mv rpc2x_act.tab.h rpc.tab.h

stag.c : stag.tab.c
	sed -e 's/YYSTYPE/STAG_YYSTYPE/g;' < stag.tab.c > stag.c

stag.h : stag.tab.h
	sed -e 's/YYSTYPE/STAG_YYSTYPE/g;' < stag.tab.h > stag.h

stag.tab.c stag.tab.h : stag.y
	bison -v -d -p stag_ stag.y

lexic.o : rpc.tab.h

lexic.c : lexic.l
	flex -t lexic.l > lexic.c

rpc.tab.o : nonterm.h rpc.tab.h rpc.tab.c

categ.o cmavotab.o lex2.o tree.o conversion.o terms.o : rpc.tab.h

uncom : uncom.o

uncom.o : uncom.c
	$(CC) -O2 -c uncom.c

uncom.c : uncom.l
	flex -t uncom.l > uncom.c

translate.o : translate.c
	$(CC) $(CFLAGS) -c translate.c $(INCDIR)

dictmake : dictmake.o
	$(CC) $(CFLAGS) -o dictmake dictmake.o $(LIBDIR) -lgdbm

dictmake.o : dictmake.c
	$(CC) $(CFLAGS) -c dictmake.c $(INCDIR)

dictclean : dictclean.o
	$(CC) $(CFLAGS) -o dictclean dictclean.o $(LIBDIR) -lgdbm

dictclean.o : dictclean.c
	$(CC) $(CFLAGS) -c dictclean.c $(INCDIR)

dictionary : dictmake
	perl giscolon.pl < gismu > gismu.dict
	perl cmacolon.pl < cmavo > cmavo.dict
	perl lujvod.pl < lujvo-list > lujvo.dict
	perl oblik.pl < oblique.key > oblik.dict
	perl raf4l.pl < gismu > raf4l.dict
	perl rafobl.pl > rafobl.dict
	perl rafsid.pl < rafsi > rafsid.dict
	perl rafsig.pl < rafsi > rafsig.dict
	perl raf4lg.pl < gismu > raf4lg.dict
	./dictmake dictionary.dbm gismu.dict cmavo.dict lujvo.dict oblik.dict raf4l.dict rafobl.dict rafsid.dict extradict

dictupdate : dictionary.dbm

dictionary.dbm :: extradict dictmake
	./dictmake dictionary.dbm extradict

depend:
	gcc -MM $(INCDIR) $(SRCS2) > .depend

include .depend

FILES = lex1.c lex2.c categ.c \
	rpc2x.y \
	functions.c functions.h Makefile main.c cmavotab.c cmavotab.h \
	nodes.h uncom.l tree.c translate.c latex.c properties.c \
	conversion.c terms.c memory.c tenses.c output.c textout.c \
	output.h htmlout.c connect.c \
	.depend TODO stag.y COPYRIGHT Makefile.in config.pl build_kit \
	action.perl terminator.pl \
	giscolon.pl cmacolon.pl lujvod.pl oblik.pl raf4l.pl rafobl.pl rafsid.pl extradict \
	dictmake.c dictclean.c \
	work_* home_* cardplayer summer_new

kit:
	tar czvf kit.tar.gz $(FILES)


zipfile:
	zip -9 kit.zip $(FILES)
