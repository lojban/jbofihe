/***************************************
  $Header$

  Program for reading plaintext dictionaries in the form

  lojban:english:comment      or just
  lojban:english
  
  and compiling them into a GDBM database.  If the lojban is of the
  form X1 (i.e. ends in a digit), the entries X1n, X1v, X1a and X1t
  are deleted if present.
  ***************************************/

/* COPYRIGHT */

#include <gdbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void
preen(GDBM_FILE db, char *s) {
  char buffer[128];
  char *suffixes[] = {"n", "v", "a", "t"};
  int i;
  datum x;
  for (i=0; i<4; i++) {
    strcpy(buffer, s);
    strcat(buffer, suffixes[i]);
    x.dptr = buffer;
    x.dsize = strlen(buffer);
    gdbm_delete(db, x);
  }
}

static void
do_file(FILE *f, GDBM_FILE db)
{
  char line[2048];
  char src[1024];
  char dest[1024];
  char *p, *q, *r;
  datum ds, dt;

  while (fgets(line, sizeof(line), f)) {
    line[strlen(line)-1] = 0;
    p = line;
    while (*p && isspace(*p)) p++;
    if (!*p) {
      /* Line is blank */
      continue;
    }
    if (strchr("#!;", *p)) {
      /* line is a comment */
      continue;
    }
    q = src;
    while (*p && *p!=':') {
      *q++ = *p++;
    }
    if (!*p) {
      fprintf(stderr, "Line [%s] does not contain a colon\n", line);
    } else {
      *q = 0;
      p++;
      r = dest;
      while (*p && *p!=':') {
        *r++ = *p++;
      }
      *r = 0;
      /* If we're about to add a case of something, check we get rid
         of any more specific entries first. */
      if (isdigit(src[strlen(src)-1])) {
        preen(db, src);
      }
      ds.dptr = src;
      ds.dsize = strlen(src);
      dt.dptr = dest;
      dt.dsize = strlen(dest);
      gdbm_store(db, ds, dt, GDBM_REPLACE);
    }
  }
}

/*
  Input is a sequence of lines with colons separating the source from the dest. */

int main (int argc, char **argv) {
  char *dbname;
  GDBM_FILE db;
  FILE *in;
  
  if (argc < 3) {
    fprintf(stderr, "Usage : %s <dbname> <source1> ... <sourceN>\n", argv[0]);
    exit(1);
  }

  dbname = argv[1];
  argv += 2;

  db = gdbm_open(dbname, 0, GDBM_WRCREAT | GDBM_FAST, 0644, NULL);
  if (*argv) {
    while (*argv) {
      in = fopen(*argv, "r");
      if (!in) {
        fprintf(stderr, "Could not open %s\n", *argv);
      } else {
        do_file(in, db);
        fclose(in);
      }
      ++argv;
    }
  } else {
    do_file(stdin, db);
  }

  gdbm_close(db);

  return 0;
}
