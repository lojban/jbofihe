# $Header$
#
# Makefile for parser/glosser
#
# COPYRIGHT
#

CC=gcc
CFLAGS= -g -Wall
OBJS2 = main.o lex1.o lex2.o cmavotab.o rpc.tab.o functions.o \
        categ.o nonterm.o tree.o translate.o latex.o latexblk.o \
        properties.o conversion.o terms.o memory.o tenses.o \
	output.o textout.o htmlout.o connect.o stag.o relative.o \
	textblk.o

SRCS2 = $(OBJS2:%.o=%.c)

all : jbofihe dictupdate

jbofihe : $(OBJS2)
	$(CC) $(CFLAGS) -o jbofihe $(OBJS2)

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
	$(CC) $(CFLAGS) -c translate.c

dictmake : dictmake.o
	$(CC) $(CFLAGS) -o dictmake dictmake.o

dictmake.o : dictmake.c
	$(CC) $(CFLAGS) -c dictmake.c

dictclean : dictclean.o
	$(CC) $(CFLAGS) -o dictclean dictclean.o

dictclean.o : dictclean.c
	$(CC) $(CFLAGS) -c dictclean.c

makelujvo : makelujvo.o
	$(CC) $(CFLAGS) -o makelujvo makelujvo.o

makelujvo.o : makelujvo.c
	$(CC) $(CFLAGS) -c makelujvo.c

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
	perl lujvop.pl < lujvo-list > lujvop.dict
	./dictmake dictionary.bin gismu.dict cmavo.dict lujvo.dict \
                                  oblik.dict raf4l.dict rafobl.dict \
                                  rafsid.dict rafsig.dict raf4lg.dict \
	                          lujvop.dict \
	                          extradict places.dat richard.lujvo extralujvo

dictupdate : dictionary.bin
	./dictmake dictionary.bin extradict places.dat richard.lujvo extralujvo

depend:
	gcc -MM $(INCDIR) $(SRCS2) > .depend

include .depend

FILES = lex1.c lex2.c categ.c \
	rpc2x.y \
	functions.c functions.h Makefile main.c cmavotab.c cmavotab.h \
	nodes.h uncom.l tree.c translate.c latex.c properties.c \
	conversion.c terms.c memory.c tenses.c output.c textout.c \
	output.h htmlout.c connect.c latex.h latexblk.c relative.c \
	subscript.c numeric.c prenex.c textblk.c \
	.depend TODO stag.y COPYRIGHT Makefile.in config.pl build_kit \
	action.perl terminator.pl cmavocode.pl \
	giscolon.pl cmacolon.pl lujvod.pl oblik.pl raf4l.pl rafobl.pl rafsid.pl \
        rafsig.pl raf4lg.pl lujvop.pl \
	places.pl extradict places.dat richard.lujvo extralujvo\
	dictmake.c dictclean.c makelujvo.c \
	work_* home_* cardplayer summer_new assisi2 story sinderelwyd sinder eclipse gerald

kit:
	tar czvf kit.tar.gz $(FILES)


zipfile:
	zip -9 kit.zip $(FILES)
