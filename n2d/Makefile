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
OBJ = parse.o scan.o n2d.o expr.o tabcompr.o compdfa.o

all : n2d

n2d : $(OBJ)
	$(CC) $(CFLAGS) -o n2d $(OBJ)

parse.c parse.h : parse.y
	bison -v -d parse.y
	mv parse.tab.c parse.c
	mv parse.tab.h parse.h

parse.o : parse.c n2d.h

scan.c : scan.l
	flex -t -s scan.l > scan.c

scan.o : scan.c parse.h n2d.h

clean:
	rm n2d *.o scan.c parse.c parse.h

