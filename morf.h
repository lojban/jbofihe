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
  MT_GISMU,
  MT_LUJVO,
  MT_FUIVLA3, /* stage 3 */
  MT_FUIVLA4, /* stage 4 */
  MT_CMAVOS,
  MT_CMENE,
  MT_BAD_UPPERCASE
} MorfType;

/* Prototypes for fns */
extern MorfType morf_scan(char *s, char ***buf_end);

#endif /* MORF_H */

