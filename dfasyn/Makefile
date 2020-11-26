# $Header$
#
# Makefile for NFA->DFA conversion utility
#
# Copyright (C) Richard P. Curnow  2000-2001
# LICENCE
#

CC=gcc
#CFLAGS=-g
#CFLAGS=-O2 -pg
CFLAGS=-O2
OBJ = parse.tab.o scan.o n2d.o expr.o tabcompr.o compdfa.o

all : dfasyn

dfasyn : $(OBJ)
	$(CC) $(CFLAGS) -o dfasyn $(OBJ)

parse.tab.c parse.tab.h : parse.y
	bison -v -d parse.y

parse.tab.o : parse.tab.c n2d.h

scan.c : scan.l
	flex -t -s scan.l > scan.c

scan.o : scan.c parse.tab.h n2d.h

clean:
	rm -f dfasyn *.o scan.c parse.tab.c parse.tab.h parse.output

