/***************************************
  $Header$

  Header file shared between generic output code and the backend
  drivers.
  ***************************************/

/* COPYRIGHT */

#ifndef OUTPUT_H
#define OUTPUT_H    /*+ To stop multiple inclusions. +*/

#include "nodes.h"

typedef struct {
  void (*initialise)(void);
  void (*prologue)(void);
  void (*epilogue)(void);
  void (*open_bracket)(BracketType type, int subscript);
  void (*close_bracket)(BracketType type, int subscript);
  void (*set_eols)(int);
  void (*lojban_text)(char *);
  void (*translation)(char *);
  void (*start_tags)(void);
  void (*end_tags)(void);
  void (*start_tag)(void);
  void (*write_tag_text)(char *, char *, char *, int);
  void (*write_partial_tag_text)(char *);
} DriverVector;

#endif /* OUTPUT_H */
