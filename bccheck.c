/***************************************
  $Header$

  State machine to check for bad cmene.
  ***************************************/

/* Copyright (C) Richard P. Curnow  2001 */
/* LICENCE */

#if defined(TEST)
#include <stdio.h>
#endif

#include "bccheck.h"

enum tokens {/*{{{*/
  TA, TE, TI, TO, TU, TY, TD, TL,
  TOC, TAP, TCM, TXX
};
/*}}}*/
enum result {/*{{{*/
  BC_NOT_COMPLETE,
  BC_CMENE_OK,
  BC_CMENE_BAD_SPLIT,
  BC_CMENE_BAD_NOSPLIT
};
/*}}}*/
enum attribute {/*{{{*/
  ATTR_NONE,
  ATTR_SEEN_LD
};
/*}}}*/

/* Include tables defining DFA */
#include "bctables.c"

static unsigned char mapchar[256] =/*{{{*/
/* Map ASCII set to the tokens. */
{
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,

  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TAP ,
  TXX , TXX , TXX , TXX , TCM , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,

  TXX , TA  , TOC , TOC , TD  , TE  , TOC , TOC ,
  TOC , TI  , TOC , TOC , TL  , TOC , TOC , TO  ,
  TOC , TOC , TOC , TOC , TOC , TU  , TOC , TOC ,
  TOC , TY  , TOC , TXX , TXX , TXX , TXX , TXX ,

  TXX , TA  , TOC , TOC , TD  , TE  , TOC , TOC ,
  TOC , TI  , TOC , TOC , TL  , TOC , TOC , TO  ,
  TOC , TOC , TOC , TOC , TOC , TU  , TOC , TOC ,
  TOC , TY  , TOC , TXX , TXX , TXX , TXX , TXX ,

  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,

  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,

  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,

  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX ,
  TXX , TXX , TXX , TXX , TXX , TXX , TXX , TXX 
};

/*}}}*/
int is_bad_cmene(char *word, int *split, char **ladoi, char **tail)/*{{{*/
{
  char *p;
  char *ladoi_1;
  int cs = 0, ns;
  enum result res;

  for (p=word; *p; p++) {
    int mc = mapchar[(unsigned int) *p & 0xff];
    if (mc == TCM) continue;
    ns = bad_cmene_next_state(cs, mc);
    res = bad_cmene_exitval[ns];
    /* Deal with early exit conditions */
    if (res == BC_CMENE_BAD_NOSPLIT || res == BC_CMENE_BAD_SPLIT) {
      if (ladoi) *ladoi = ladoi_1;
      if (tail) *tail = p;
      break;
    }
    
    /* By doing this here, we protect against problem words like "salad",
       where the position of "d" would need to be remembered in case it starts
       "doi", but that would overwrite th stored position of "l" */
    if (bad_cmene_attribute[ns] == ATTR_SEEN_LD) ladoi_1 = p;
    
    cs = ns;
  }

  res = bad_cmene_exitval[ns];

  if (split) *split = 0;
  
  switch (res) {
    case BC_CMENE_OK:
      return 0;
    case BC_CMENE_BAD_SPLIT:
      if (split) *split = 1;
      return 1;
    case BC_CMENE_BAD_NOSPLIT:
    case BC_NOT_COMPLETE:
      return 1;
  }

  return 0; /* make compiler happy */

}/*}}}*/

#if defined(TEST)
static void do_check(char *s)/*{{{*/
{
  int res;
  char *ladoi, *tail;
  int split;
  char *p;

  res = is_bad_cmene(s, &split, &ladoi, &tail);
  printf("%s : %s", s, res ? "BAD" : "good");
  if (res) {
    if (split) {
      printf(" : ");
      for (p=s; *p; p++) {
        if (p == ladoi) putchar('+');
        if (p == tail)  putchar('+');
        putchar(*p);
      }
    }
  }
  printf("\n");
  return;
}
/*}}}*/
int main (int argc, char **argv)/*{{{*/
{

  do_check("laplas");
  do_check("stalaplas");
  do_check("stala'iplas");
  do_check("stala,iplas");
  do_check("stala'ip,las");
  do_check("laplus");
  do_check("alaun");
  do_check("ritcrd");
  do_check("ritc,r,d");
  do_check("mecylakorunas");
  do_check("mecylaukorunas");
  do_check("mecyla'ikorunas");
  do_check("mecyla,ikorunas");
  do_check("mecyda,ikorunas");

  return 0;
}/*}}}*/
#endif /* defined(TEST) */

         




