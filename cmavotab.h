/***************************************
  $Header$

  Definition of the record type used for the cmavo table.
  ***************************************/

/* COPYRIGHT */

#ifndef CMAVOTAB_H
#define CMAVOTAB_H    /*+ To stop multiple inclusions. +*/

typedef struct {
  char *cmavo;
  int selmao;
  char *meaning;
} CmavoCell;

extern CmavoCell cmavo_table[];

#endif /* CMAVOTAB_H */
