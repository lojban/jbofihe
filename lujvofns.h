/***************************************
  $Header$

  
  ***************************************/

/* COPYRIGHT */

#ifndef LUJVOFNS_H
#define LUJVOFNS_H    /*+ To stop multiple inclusions. +*/

int is_consonant(char c);
int is_uppercase_consonant(char c);
int is_vowel(char c);
int is_voiced(char c);
int is_unvoiced(char c);
int is_sibilant(char c);
int is_ccv(char *s);
int is_cvv(char *s);
int is_cvc(char *s);
int is_cvccv(char *s);
int is_ccvcv(char *s);
int is_pairok(char *s);
int is_initialpairok(char *s);
int is_valid_lujvo(char *t);



#endif /* LUJVOFNS_H */
