/***************************************
  $Header$

  Prototypes for functions
  ***************************************/

/* COPYRIGHT */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H    /*+ To stop multiple inclusions. +*/

#include <stdio.h>
#include <stdlib.h>
#include "nonterm.h"
#include "nodes.h"
#include "output.h"

#define new_string(s) strcpy((char *) Malloc(1+strlen(s)), (s))
#define new(T) (T *) Malloc(sizeof(T))
#define new_array(T, n) (T *) Malloc(sizeof(T) * (n))

/* In functions.c */
TreeNode *new_node(void);

TreeNode *
new_node_0(NonTerm nt);

TreeNode *
new_node_1(NonTerm nt, TreeNode *c1);

TreeNode *
new_node_2(NonTerm nt, TreeNode *c1, TreeNode *c2);

TreeNode *
new_node_3(NonTerm nt, TreeNode *c1, TreeNode *c2, TreeNode *c3);

TreeNode *
new_node_4(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4);

TreeNode *
new_node_5(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4, 
           TreeNode *c5);

TreeNode *
new_node_6(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4, 
           TreeNode *c5, TreeNode *c6);

TreeNode *
new_node_7(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7);

TreeNode *
new_node_8(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8);

TreeNode *
new_node_9(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
           TreeNode *c9);

TreeNode *
new_node_10(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10);

TreeNode *
new_node_11(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10, TreeNode *c11);

TreeNode *
new_node_12(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10, TreeNode *c11, TreeNode *c12);

extern void type_check(TreeNode *x, NonTerm type);
extern TreeNode * child_ref(TreeNode *x, int i);
extern int nch(TreeNode *x);
extern TreeNode *find_nth_child(TreeNode *x, int n, NonTerm type);
extern TreeNode *find_nth_cmavo_child(TreeNode *x, int n, int selmao);
extern int is_simple_nonterm(TreeNode *x);

/* In categ.c */
void categorize_tokens(TreeNode *head);

/* In lex2.c */
void lex2_initialise(void);
void add_token(TreeNode *tok);
void mark_eol(void);
void delete_node(TreeNode *x);
void show_tokens(void);
void preprocess_tokens(void);
void error_advance(int code);
int yylex(void);
void print_last_toks(void);


/* In lex1.c */
void parse_file(FILE *f);

/* In tree.c */
void compress_singletons(TreeNode *x);
void print_parse_tree(TreeNode *x);
void print_bracketed_text(TreeNode *top, int gloss);
void expand_bahe_ui(TreeNode *top);

/* In translate.c */
char * translate(char *word);
char *translate_unknown(char *w);
typedef enum {
  TCX_NOUN = 0,
  TCX_VERB = 1,
  TCX_QUAL = 2,
  TCX_TAG = 3
} TransContext;
char *adv_translate(char *w, int place, TransContext ctx);

/* In output.c */
extern void add_bracketing_tags(TreeNode *top);
extern void do_output(TreeNode *top, DriverVector *driver);

/* In properties.c */
#define YES 1
#define NO 0

extern XConversion *prop_conversion(TreeNode *x, int create);
extern XBaiConversion *prop_bai_conversion(TreeNode *x, int create);
extern XDontGloss *prop_dont_gloss(TreeNode *x, int create);
extern XTermVectors *prop_term_vectors(TreeNode *x, int create);
extern XTermVector *prop_term_vector(TreeNode *x, int create);
extern XTermTags *prop_term_tags(TreeNode *x, int create);
extern XGlosstype *prop_glosstype(TreeNode *x, int create);
extern XDoneTU1 *prop_done_tu1(TreeNode *x, int create);
extern XDoneS3 *prop_done_s3(TreeNode *x, int create);
extern XTenseCtx *prop_tense_ctx(TreeNode *x, int create);
extern XNegIndicator *prop_neg_indicator(TreeNode *x, int create);
extern XCaiIndicator *prop_cai_indicator(TreeNode *x, int create);
extern XConnective *prop_connective(TreeNode *x, int create);

/* In conversion.c */
extern void do_conversions(TreeNode *top);

/* In terms.c */
extern void terms_processing(TreeNode *top);

/* In memory.c */
extern void *Malloc(size_t n);
extern void Free(void *x);
extern void print_memory_statistics(void);

/* In tenses.c */
extern void tense_processing(TreeNode *top);

/* In connect.c */
extern void connectives_processing(TreeNode *top);

#endif /* FUNCTIONS_H */
