/***************************************
  $Header$

  Header file for morphology functions module.
  
  ***************************************/

#ifndef MORF_H
#define MORF_H

/* Return type.  Note that gismu/lujvo/fu'ivla are compressed together.  This
 * could be exposed later if the rest of the main program has a use for the
 * information. */
typedef enum {
  MT_BOGUS,
  MT_BRIVLA, /* gismu/lujvo/fu'ivla */
  MT_CMAVOS,
  MT_CMENE,
  MT_BAD_UPPERCASE
} MorfType;

/* Prototypes for fns */
extern MorfType morf_scan(char *s, char ***buf_end);

#endif /* MORF_H */

