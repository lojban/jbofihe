/***************************************
  $Header$

  Header file for interface between morf.c and built file morf_dfa.c
  
  ***************************************/

/* Copyright (C) Richard P. Curnow  2000-2001 */
/* LICENCE */

#ifndef MORF_DFA_H
#define MORF_DFA_H

enum raw_category {/*{{{*/
  R_UNKNOWN,
  R_CMAVOS, R_CMAVOS_END_CY,
  R_GISMU_0, R_GISMU_1,
  R_LUJVO_0, R_LUJVO_1,
  R_CULTURAL_LUJVO_0, R_CULTURAL_LUJVO_1,
  R_STAGE3_0, R_STAGE3_1, R_STAGE3_1_CVC,
  R_X_STAGE3_0, R_X_STAGE3_0_CVC,
  R_X_STAGE3_1, R_X_STAGE3_1_CVC,
  R_STAGE4_0, R_STAGE4_1,
  R_CMENE,
  R_BAD_TOSMABRU, R_CULTURAL_BAD_TOSMABRU,
  R_BAD_SLINKUI
};
/*}}}*/

enum state_attribute {/*{{{*/
  AT_UNKNOWN, /* nothing-to-do option */
  AT_S3_3, /* after hyphen triplet for short-rafsi stage 3 */
  AT_S3_4, /* after hyphen triplet for long-rafsi stage 3 */
  AT_XS3_3, /* after hyphen triplet for short-rafsi extended stage 3 */
  AT_XS3_4, /* after hyphen triplet for long-rafsi extended stage 3 */
};
/*}}}*/

extern enum state_attribute morf_attribute[];
extern enum raw_category morf_exitval[];
extern int morf_next_state(int, int);

#endif /* MORF_DFA_H */


