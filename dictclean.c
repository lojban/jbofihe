/***************************************
  $Header$

  This program opens a GDBM format file and compacts it.
  ***************************************/

/* COPYRIGHT */

#include <gdbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main (int argc, char **argv) {
  char *dbname;
  GDBM_FILE db;
  FILE *in;
  
  if (argc < 2) {
    fprintf(stderr, "Usage : %s <dbname>\n", argv[0]);
    exit(1);
  }

  dbname = argv[1];

  db = gdbm_open(dbname, 0, GDBM_WRITER | GDBM_FAST, 0644, NULL);

  gdbm_reorganize(db);

  gdbm_close(db);

  return 0;
}
