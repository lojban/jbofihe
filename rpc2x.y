/** -*-Fundamental-*- *************************************
  $Header$

  Bison parser generator input for lojban grammar.

  This is developed directly from the bnf.300 file.  A number of extra
  tokens are added (in categ.c) so that the parser developed from this
  is LALR(1).  bnf.300 has been extended in a few areas - for syntax
  error handling, and to make certain sub-categories of some rules
  directly recognizable in the back-end processing.  The same language
  should still be recognized as that defined in bnf.300.

  With acknowledgements to the Logical Language Group, as discussed at
  the top of bnf.300.

  The string \* ET *\ indicates where an elidable terminator is
  optionally missing from a rule.  A perl script (terminator.pl) can be
  used to strip these rules, leaving a grammer where all rules are
  closed by terminators.  By running that grammar through bison, you can
  look for residual shift/reduce conflicts, if any.  (The only ones left
  here result from the error processing.)  (There is a small crock for
  the <stag gik> case, q.v.).

  ***************************************/

/* COPYRIGHT */

%token GARBAGE

%token A
%token BAhE
%token BAI
%token BE
%token BEhO
%token BEI
%token BIhE
%token BIhI
%token BO
%token BOI
%token BRIVLA
%token BU
%token BY
%token CAhA
%token CAI
%token CEhE
%token CEI
%token CMENE
%token CO
%token COI
%token CU
%token CUhE
%token DAhO
%token DOhU
%token DOI
%token FA
%token FAhA
%token FAhO
%token FEhE
%token FEhU
%token FIhO
%token FOI
%token FUhA
%token FUhE
%token FUhO
%token GA
%token GAhO
%token GEhU
%token GI
%token GIhA
%token GOhA
%token GOI
%token GUhA
%token I
%token JA
%token JAI
%token JOhI
%token JOI
%token KE
%token KEhE
%token KEI
%token KI
%token KOhA
%token KUhE
%token KUhO
%token KU
%token LA
%token LAhE
%token LAU
%token LE
%token LEhU
%token LI
%token LIhU
%token LOhO
%token LOhU
%token LU
%token LUhU
%token MAhO
%token MAI
%token ME
%token MEhU
%token MOhE
%token MOhI
%token MOI
%token NA
%token NAhE
%token NAhU
%token NAI
%token NIhE
%token NIhO
%token NOI
%token NU
%token NUhA
%token NUhI
%token NUhU
%token PA
%token PEhA
%token PEhE
%token PEhO
%token POhA
%token PU
%token RAhO
%token ROI
%token SA
%token SE
%token SEhU
%token SEI
%token SI
%token SOI
%token SU
%token TAhE
%token TEhU
%token TEI
%token TO
%token TOI
%token TUhE
%token TUhU
%token UI
%token VA
%token VAU
%token VEhA
%token VEhO
%token VEI
%token VIhA
%token VUhO
%token VUhU
%token XI
%token Y
%token ZAhO
%token ZEhA
%token ZEI
%token ZI
%token ZIhE
%token ZO
%token ZOhU
%token ZOI

/* EXTRA TOKENS REQUIRED TO MAKE GRAMMAR LALR(1) */
%token START_EK
%token START_GIHEK
%token START_GUHEK
%token START_JEK
%token START_JOIK
%token START_GEK

%token START_BAI

%token EK_KE
%token EK_BO

%token JEK_KE
%token JEK_BO

%token JOIK_KE
%token JOIK_BO

%token I_JEKJOIK
%token I_BO

%token GIHEK_KE
%token GIHEK_BO

%token NAhE_BO
%token NAhE_time
%token NAhE_space

%token NUMBER_MAI
%token NUMBER_MOI
%token NUMBER_ROI

%token EOF_MARK

%{
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include <string.h>

#include "nodes.h"
#include "nonterm.h"
#include "functions.h"

#define YYSTYPE TreeNode *

extern TreeNode *top;

extern void yyerror(const char *);

extern int last_tok_line;
extern int last_tok_column;

%}

%%

/*
text<0> = [NAI ...] [CMENE ... # | (indicators & free ...)] [joik-jek] text-1
*/

all : chunks
    ;

/*
all : text
    ;
*/

/* This is an artifact of the error handling mechanism  - sometimes the parser will
   reduce right out to the top rule which is hopeless */
chunks : text EOF_MARK
       | text error
{ $$ = $1; yyclearin; }
       | chunks text EOF_MARK
       | chunks text error
{ fprintf(stderr, "Syntax error following text ending at line %d column %d\n",
          @2.last_line, @2.last_column);
  yyclearin;
  $$ = new_node_2(CHUNKS, $1, $2); }
       ;


text : NAI_seq CMENE_seq free_seq joik_opt_ke free_seq text_1
     | NAI_seq CMENE_seq free_seq jek_opt_ke  free_seq text_1
     | NAI_seq CMENE_seq free_seq joik_opt_ke          text_1
     | NAI_seq CMENE_seq free_seq jek_opt_ke           text_1
     | NAI_seq CMENE_seq free_seq                      text_1

     | NAI_seq CMENE_seq          joik_opt_ke free_seq text_1
     | NAI_seq CMENE_seq          jek_opt_ke  free_seq text_1
     | NAI_seq CMENE_seq          joik_opt_ke          text_1
     | NAI_seq CMENE_seq          jek_opt_ke           text_1
     | NAI_seq CMENE_seq                               text_1

     | NAI_seq indicators free_seq joik_opt_ke free_seq text_1
     | NAI_seq indicators free_seq jek_opt_ke  free_seq text_1
     | NAI_seq indicators free_seq joik_opt_ke          text_1
     | NAI_seq indicators free_seq jek_opt_ke           text_1
     | NAI_seq indicators free_seq                      text_1

     | NAI_seq indicators          joik_opt_ke free_seq text_1
     | NAI_seq indicators          jek_opt_ke  free_seq text_1
     | NAI_seq indicators          joik_opt_ke          text_1
     | NAI_seq indicators          jek_opt_ke           text_1
     | NAI_seq indicators                               text_1

     | NAI_seq            free_seq joik_opt_ke free_seq text_1
     | NAI_seq            free_seq jek_opt_ke  free_seq text_1
     | NAI_seq            free_seq joik_opt_ke          text_1
     | NAI_seq            free_seq jek_opt_ke           text_1
     | NAI_seq            free_seq                      text_1

     | NAI_seq                     joik_opt_ke free_seq text_1
     | NAI_seq                     jek_opt_ke  free_seq text_1
     | NAI_seq                     joik_opt_ke          text_1
     | NAI_seq                     jek_opt_ke           text_1
     | NAI_seq                                          text_1

     |         CMENE_seq free_seq joik_opt_ke free_seq text_1
     |         CMENE_seq free_seq jek_opt_ke  free_seq text_1
     |         CMENE_seq free_seq joik_opt_ke          text_1
     |         CMENE_seq free_seq jek_opt_ke           text_1
     |         CMENE_seq free_seq                      text_1

     |         CMENE_seq          joik_opt_ke free_seq text_1
     |         CMENE_seq          jek_opt_ke  free_seq text_1
     |         CMENE_seq          joik_opt_ke          text_1
     |         CMENE_seq          jek_opt_ke           text_1
     |         CMENE_seq                               text_1

     |         indicators free_seq joik_opt_ke free_seq text_1
     |         indicators free_seq jek_opt_ke  free_seq text_1
     |         indicators free_seq joik_opt_ke          text_1
     |         indicators free_seq jek_opt_ke           text_1
     |         indicators free_seq                      text_1

     |         indicators          joik_opt_ke free_seq text_1
     |         indicators          jek_opt_ke  free_seq text_1
     |         indicators          joik_opt_ke          text_1
     |         indicators          jek_opt_ke           text_1
     |         indicators                               text_1

     |                    free_seq joik_opt_ke free_seq text_1
     |                    free_seq jek_opt_ke  free_seq text_1
     |                    free_seq joik_opt_ke          text_1
     |                    free_seq jek_opt_ke           text_1
     |                    free_seq                      text_1

     |                             joik_opt_ke free_seq text_1
     |                             jek_opt_ke  free_seq text_1
     |                             joik_opt_ke          text_1
     |                             jek_opt_ke           text_1
     |                                                  text_1

     | text_no_text_1

     | /* empty */
     ;

text_no_text_1 : NAI_seq CMENE_seq free_seq joik_opt_ke free_seq
     | NAI_seq CMENE_seq free_seq jek_opt_ke  free_seq
     | NAI_seq CMENE_seq free_seq joik_opt_ke
     | NAI_seq CMENE_seq free_seq jek_opt_ke
     | NAI_seq CMENE_seq free_seq

     | NAI_seq CMENE_seq          joik_opt_ke free_seq
     | NAI_seq CMENE_seq          jek_opt_ke  free_seq
     | NAI_seq CMENE_seq          joik_opt_ke
     | NAI_seq CMENE_seq          jek_opt_ke
     | NAI_seq CMENE_seq

     | NAI_seq indicators free_seq joik_opt_ke free_seq
     | NAI_seq indicators free_seq jek_opt_ke free_seq
     | NAI_seq indicators free_seq joik_opt_ke
     | NAI_seq indicators free_seq jek_opt_ke
     | NAI_seq indicators free_seq

     | NAI_seq indicators          joik_opt_ke free_seq
     | NAI_seq indicators          jek_opt_ke  free_seq
     | NAI_seq indicators          joik_opt_ke
     | NAI_seq indicators          jek_opt_ke
     | NAI_seq indicators

     | NAI_seq            free_seq joik_opt_ke free_seq
     | NAI_seq            free_seq jek_opt_ke  free_seq
     | NAI_seq            free_seq joik_opt_ke
     | NAI_seq            free_seq jek_opt_ke
     | NAI_seq            free_seq

     | NAI_seq                     joik_opt_ke free_seq
     | NAI_seq                     jek_opt_ke  free_seq
     | NAI_seq                     joik_opt_ke
     | NAI_seq                     jek_opt_ke
     | NAI_seq

     |         CMENE_seq free_seq joik_opt_ke free_seq
     |         CMENE_seq free_seq jek_opt_ke free_seq
     |         CMENE_seq free_seq joik_opt_ke
     |         CMENE_seq free_seq jek_opt_ke
     |         CMENE_seq free_seq

     |         CMENE_seq          joik_opt_ke free_seq
     |         CMENE_seq          jek_opt_ke  free_seq
     |         CMENE_seq          joik_opt_ke
     |         CMENE_seq          jek_opt_ke
     |         CMENE_seq

     |         indicators free_seq joik_opt_ke free_seq
     |         indicators free_seq jek_opt_ke  free_seq
     |         indicators free_seq joik_opt_ke
     |         indicators free_seq jek_opt_ke
     |         indicators free_seq

     |         indicators          joik_opt_ke free_seq
     |         indicators          jek_opt_ke  free_seq
     |         indicators          joik_opt_ke
     |         indicators          jek_opt_ke
     |         indicators

     |                    free_seq joik_opt_ke free_seq
     |                    free_seq jek_opt_ke  free_seq
     |                    free_seq joik_opt_ke
     |                    free_seq jek_opt_ke
     |                    free_seq

     |                             joik_opt_ke free_seq
     |                             jek_opt_ke  free_seq
     |                             joik_opt_ke
     |                             jek_opt_ke

     ;


/*
text-1<2> = [(I [jek | joik] [[stag] BO] #) ... | NIhO ... #] [paragraphs]
*/

text_1 : text_1A paragraphs
       | text_1A
       |         paragraphs
       ;

text_1A : text_1B
        | NIhO_seq_free_seq
        ;

text_1B : text_1C
        | text_1B text_1C
        ;

/* Differs from i_jj_stag_bo because it allows the construct
   without BO at the end . */

text_1C :         I_BO I joik stag BO free_seq
        |         I_BO I jek  stag BO free_seq
        |         I_BO I      stag BO free_seq
        |         I_BO I joik stag BO
        |         I_BO I jek  stag BO
        |         I_BO I      stag BO
        |         I_BO I joik      BO free_seq
        |         I_BO I jek       BO free_seq
        |         I_BO I           BO free_seq
        |         I_BO I joik      BO
        |         I_BO I jek       BO
        |         I_BO I           BO
        |         I joik_opt_ke         free_seq
        |         I jek_opt_ke          free_seq
        |         I              free_seq
        |         I joik_opt_ke
        |         I jek_opt_ke
        |         I    
        ;


/*
paragraphs<4> = paragraph [NIhO ... # paragraphs]
*/

/* Rewrite <paragraphs> to make this left recursive : important as there may be
   lots of these in a text. */

/*
paragraph<10> = (statement | fragment) [I # [statement | fragment]] ...
*/

paragraphs :                              paragraph
           | paragraphs NIhO_seq_free_seq paragraph
           ;

paragraph : statement
          | fragment
          | error
{
  fprintf(stderr, "Syntax error in paragraph at line %d\n", last_tok_line);
  error_advance(0);
  yyclearin;
}

          | paragraph i_opt_free_seq statement
          | paragraph i_opt_free_seq fragment
          | paragraph i_opt_free_seq error

{
  fprintf(stderr, "Syntax error in paragraph at line %d\n", last_tok_line);
  error_advance(0);
  yyclearin;
  $$ = $1;
}

          | paragraph i_opt_free_seq

          ;

i_opt_free_seq : I
               | I free_seq
               ;

/*
statement<11> = statement-1 | prenex statement
*/

statement : statement_1
          | prenex statement
          ;

/*
statement-1<12> = statement-2 [I joik-jek [statement-2]] ...
*/

statement_1 : statement_2
            | statement_1 i_joik_jek statement_2
            | statement_1 i_joik_jek
            ;

i_joik_jek : I_JEKJOIK I joik_opt_ke free_seq
           | I_JEKJOIK I joik_opt_ke
           | I_JEKJOIK I jek_opt_ke free_seq
           | I_JEKJOIK I jek_opt_ke
           ;


/*
statement-2<13> = statement-3 [I [jek | joik] [stag] BO # [statement-2]]
*/

statement_2 : statement_3
            | statement_3 i_jj_stag_bo
            | statement_3 i_jj_stag_bo statement_2
            ;


i_jj_stag_bo : I_BO I joik stag BO free_seq
             | I_BO I joik stag BO
             | I_BO I joik      BO free_seq
             | I_BO I joik      BO
             | I_BO I jek  stag BO free_seq
             | I_BO I jek  stag BO
             | I_BO I jek       BO free_seq
             | I_BO I jek       BO
             | I_BO I      stag BO free_seq
             | I_BO I      stag BO
             | I_BO I           BO free_seq
             | I_BO I           BO
             ;

/*
statement-3<14> = sentence | [tag] TUhE # text-1 /TUhU#/
*/

statement_3 : sentence

            | tag TUhE free_seq text_1 TUhU free_seq
            | tag TUhE free_seq text_1 TUhU
            | tag TUhE free_seq text_1 /* ET */

            | tag TUhE          text_1 TUhU free_seq
            | tag TUhE          text_1 TUhU
            | tag TUhE          text_1 /* ET */

            |     TUhE free_seq text_1 TUhU free_seq
            |     TUhE free_seq text_1 TUhU
            |     TUhE free_seq text_1 /* ET */

            |     TUhE          text_1 TUhU free_seq
            |     TUhE          text_1 TUhU
            |     TUhE          text_1 /* ET */
            ;

/*
fragment<20> = ek # | gihek # | quantifier | NA # | terms /VAU#/ | prenex |
        relative-clauses | links | linkargs
        */

fragment : ek free_seq
         | ek
         | gihek free_seq
         | gihek
         | quantifier
         | NA free_seq
         | NA
         | terms VAU free_seq
         | terms VAU
         | terms /* ET */
         | prenex
         | relative_clauses
         | links
         | linkargs
         ;

/*
prenex<30> = terms ZOhU #
*/

prenex : terms ZOhU free_seq
       | terms ZOhU
       ;

/*
sentence<40> = [terms [CU #]] bridi-tail
*/

sentence : terms CU free_seq bridi_tail
         | terms CU          bridi_tail
         | terms CU error
{ fprintf(stderr, "Syntax error following CU at line %d column %d\n",
          @2.first_line, @2.first_column);
  $$ = new_node_2(SENTENCE, $1, $2); }
         | no_cu_sentence
         | observative_sentence


/* The following are all artificial */

         | terms START_GIHEK /* error */
{
 fprintf(stderr, "Missing selbri before GIhA at line %d column %d\n",
         @2.first_line, @2.first_column);
 error_advance(0);
 $$ = $1;
 YYERROR;
}

         | terms GIHEK_KE /* error */
{
 fprintf(stderr, "Missing selbri before GIhA at line %d column %d\n",
         @2.first_line, @2.first_column);
 error_advance(0);
 $$ = $1;
 YYERROR;
}

         | terms GIHEK_BO /* error */
{
 fprintf(stderr, "Missing selbri before GIhA at line %d column %d\n",
         @2.first_line, @2.first_column);
 error_advance(0);
 $$ = $1;
 YYERROR;
}

         ;

no_cu_sentence : terms bridi_tail
               ;

/* A misnomer if there is a 'fa' in the tail terms, however ... */
observative_sentence : bridi_tail
                     ;


/*
subsentence<41> = sentence | prenex subsentence
*/

subsentence : sentence
            | prenex subsentence
            ;

/*
bridi-tail<50> = bridi-tail-1
        [gihek [stag] KE # bridi-tail /KEhE#/ tail-terms]
          */


bridi_tail : bridi_tail_1
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail KEhE free_seq tail_terms
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail KEhE free_seq /* ET */
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail KEhE          tail_terms
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail KEhE          /* ET */
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail /* ET */      tail_terms
           | bridi_tail_1 gihek_stag_ke KE free_seq bridi_tail               /* ET */

           | bridi_tail_1 gihek_stag_ke KE          bridi_tail KEhE free_seq tail_terms
           | bridi_tail_1 gihek_stag_ke KE          bridi_tail KEhE free_seq /* ET */
           | bridi_tail_1 gihek_stag_ke KE          bridi_tail KEhE          tail_terms
           | bridi_tail_1 gihek_stag_ke KE          bridi_tail KEhE          /* ET */
           | bridi_tail_1 gihek_stag_ke KE          bridi_tail /* ET */      tail_terms
           | bridi_tail_1 gihek_stag_ke KE          bridi_tail               /* ET */
           ;

gihek_stag_ke : GIHEK_KE gihek stag
              | GIHEK_KE gihek
              ;

/*
bridi-tail-1<51> = bridi-tail-2 [gihek # bridi-tail-2 tail-terms] ...
*/

bridi_tail_1 : bridi_tail_2
             | bridi_tail_1 gihek free_seq bridi_tail_2 tail_terms
             | bridi_tail_1 gihek free_seq bridi_tail_2 /* ET */
             | bridi_tail_1 gihek          bridi_tail_2 tail_terms
             | bridi_tail_1 gihek          bridi_tail_2 /* ET */
             ;

/*
bridi-tail-2<52> = bridi-tail-3 [gihek [stag] BO # bridi-tail-2 tail-terms]
*/

bridi_tail_2 : bridi_tail_3
             | bridi_tail_2 gihek_stag_bo bridi_tail_2 tail_terms
             | bridi_tail_2 gihek_stag_bo bridi_tail_2 /* ET */
             ;

gihek_stag_bo : GIHEK_BO gihek stag BO free_seq
              | GIHEK_BO gihek stag BO
              | GIHEK_BO gihek      BO free_seq
              | GIHEK_BO gihek      BO
              ;


/*
bridi-tail-3<53> = selbri tail-terms | gek-sentence
*/

bridi_tail_3 : main_selbri tail_terms
             | main_selbri /* ET */
             | gek_sentence
             ;

main_selbri : selbri
            ;

/* The case of empty tail_terms must be handled in the parent rule and
   tagged with [* ET *] because there is a VAU elided in such a case */

tail_terms : terms VAU free_seq
           | terms VAU
           | terms /* ET */
           |       VAU free_seq
           |       VAU
           ;

/*
gek-sentence<54> = gek subsentence gik subsentence tail-terms |
        [tag] KE # gek-sentence /KEhE#/ | NA # gek-sentence
        */

gek_sentence : gek subsentence gik subsentence tail_terms
             | gek subsentence gik subsentence /* ET */

             | tag KE free_seq gek_sentence KEhE free_seq
             | tag KE free_seq gek_sentence KEhE
             | tag KE free_seq gek_sentence /* ET */

             | tag KE          gek_sentence KEhE free_seq
             | tag KE          gek_sentence KEhE
             | tag KE          gek_sentence /* ET */

             |     KE free_seq gek_sentence KEhE free_seq
             |     KE free_seq gek_sentence KEhE
             |     KE free_seq gek_sentence /* ET */

             |     KE          gek_sentence KEhE free_seq
             |     KE          gek_sentence KEhE
             |     KE          gek_sentence /* ET */

             | NA free_seq gek_sentence
             | NA          gek_sentence
             ;

/*
  This can reduce to null, so must be folded into everywhere it appears,
  unfortunately */
/*
tail-terms<71> = [terms] /VAU#/
*/

/*
terms<80> = terms-1 ...
*/

terms : terms_1
      | terms terms_1
      ;

/*
terms-1<81> = terms-2 [PEhE # joik-jek terms-2] ...
*/

terms_1 : terms_2
        | terms_1 PEhE free_seq joik free_seq terms_2
        | terms_1 PEhE free_seq joik          terms_2
        | terms_1 PEhE free_seq jek  free_seq terms_2
        | terms_1 PEhE free_seq jek           terms_2
        | terms_1 PEhE          joik free_seq terms_2
        | terms_1 PEhE          joik          terms_2
        | terms_1 PEhE          jek  free_seq terms_2
        | terms_1 PEhE          jek           terms_2
        ;


/*
terms-2<82> = term [CEhE # term] ...
*/

terms_2 : term
        | terms_2 CEhE free_seq term
        | terms_2 CEhE          term
        ;


/*
term<83> = sumti | (tag | FA #) (sumti | /KU#/) | termset | NA KU #
*/

term : term_plain_sumti
     | term_tagged_sumti
     | term_placed_sumti
     | term_floating_tense
     | termset
     | term_floating_negate
     | term_other
     ;

term_plain_sumti : sumti
                 ;

term_placed_sumti : FA free_seq sumti
                  | FA          sumti
                  ;

term_tagged_sumti : tag sumti
                  ;

term_floating_tense : tag KU free_seq
                    | tag KU
                    | tag /* ET */
                    ;

term_floating_negate : NA KU free_seq
                     | NA KU
                     ;

/* Where on earth do these arise?  'FA' on its own can be in a fragment as an answer to a fa'i question,
   but what about the others??? */
term_other : FA free_seq KU free_seq
           | FA free_seq KU
           | FA free_seq /* ET */
           | FA          KU free_seq
           | FA          KU 
           | FA /* ET */
           ;

/*
termset<85> = NUhI # gek terms /NUhU#/ gik terms /NUhU#/ |
        NUhI # terms /NUhU#/
        */

termset : NUhI free_seq gek terms NUhU free_seq gik terms NUhU free_seq
        | NUhI free_seq gek terms NUhU free_seq gik terms NUhU
        | NUhI free_seq gek terms NUhU free_seq gik terms /* ET */

        | NUhI free_seq gek terms NUhU          gik terms NUhU free_seq
        | NUhI free_seq gek terms NUhU          gik terms NUhU
        | NUhI free_seq gek terms NUhU          gik terms /* ET */

        | NUhI free_seq gek terms               gik terms NUhU free_seq /* ET */
        | NUhI free_seq gek terms               gik terms NUhU /* ET */
        | NUhI free_seq gek terms               gik terms /* ET */
 
        | NUhI          gek terms NUhU free_seq gik terms NUhU free_seq
        | NUhI          gek terms NUhU free_seq gik terms NUhU
        | NUhI          gek terms NUhU free_seq gik terms /* ET */

        | NUhI          gek terms NUhU          gik terms NUhU free_seq
        | NUhI          gek terms NUhU          gik terms NUhU
        | NUhI          gek terms NUhU          gik terms /* ET */

        | NUhI          gek terms               gik terms NUhU free_seq /* ET */
        | NUhI          gek terms               gik terms NUhU /* ET */
        | NUhI          gek terms               gik terms /* ET */

        | NUhI free_seq     terms NUhU free_seq
        | NUhI free_seq     terms NUhU
        | NUhI free_seq     terms /* ET */

        | NUhI              terms NUhU free_seq
        | NUhI              terms NUhU
        | NUhI              terms /* ET */
        ;

/*
sumti<90> = sumti-1 [VUhO # relative-clauses]
*/

sumti : sumti_1
      | sumti_1 VUhO free_seq relative_clauses
      | sumti_1 VUhO          relative_clauses
      ;

/*
sumti-1<91> = sumti-2 [(ek | joik) [stag] KE # sumti /KEhE#/]
*/


sumti_1 : sumti_2
        | sumti_2 joik_ek_ke ke_sumti
        ;

joik_ek_ke : JOIK_KE joik stag
           | JOIK_KE joik
           | EK_KE   ek   stag
           | EK_KE   ek
           ;

ke_sumti : KE free_seq sumti KEhE free_seq
         | KE free_seq sumti KEhE
         | KE free_seq sumti /* ET */
         | KE          sumti KEhE free_seq
         | KE          sumti KEhE
         | KE          sumti /* ET */
         ;

/*
sumti-2<92> = sumti-3 [joik-ek sumti-3] ...
*/

sumti_2 : sumti_3
        | sumti_2 joik free_seq sumti_3
        | sumti_2 joik          sumti_3
        | sumti_2 ek   free_seq sumti_3
        | sumti_2 ek            sumti_3
        ;


/*
sumti-3<93> = sumti-4 [(ek | joik) [stag] BO # sumti-3]
*/


sumti_3 : sumti_4
        | sumti_4 joik_ek_stag_bo sumti_3
        ;

joik_ek_stag_bo : JOIK_BO joik stag BO free_seq
                | JOIK_BO joik stag BO
                | JOIK_BO joik      BO free_seq
                | JOIK_BO joik      BO
                | EK_BO   ek   stag BO free_seq
                | EK_BO   ek   stag BO
                | EK_BO   ek        BO free_seq
                | EK_BO   ek        BO
                ;

/*
sumti-4<94> = sumti-5 | gek sumti gik sumti-4
*/

sumti_4 : sumti_5
        | gek sumti gik sumti_4
        ;

/*
sumti-5<95> = [quantifier] sumti-6 [relative-clauses] |
        quantifier selbri /KU#/ [relative-clauses]
        */

sumti_5 : sumti_5a relative_clauses
        | sumti_5a
        | sumti_5b relative_clauses
        | sumti_5b
        ;

sumti_5a : quantifier sumti_6
         |            sumti_6
         ;

sumti_5b : quantifier selbri KU free_seq
         | quantifier selbri KU
         | quantifier selbri /* ET */
         ;

/*
sumti-6<97> = (LAhE # | NAhE BO #) [relative-clauses] sumti /LUhU#/ |
        KOhA # | lerfu-string /BOI#/ | LA # [relative-clauses] CMENE ... # |
        (LA | LE) # sumti-tail /KU#/ | LI # mex /LOhO#/ |
        ZO any-word # | LU text /LIhU#/ | LOhU any-word ... LEhU # |
        ZOI any-word anything any-word #
        */

sumti_6 : LAhE    free_seq relative_clauses sumti LUhU free_seq
        | LAhE    free_seq relative_clauses sumti LUhU
        | LAhE    free_seq relative_clauses sumti /* ET */
        | LAhE    free_seq                  sumti LUhU free_seq
        | LAhE    free_seq                  sumti LUhU
        | LAhE    free_seq                  sumti /* ET */

        | LAhE             relative_clauses sumti LUhU free_seq
        | LAhE             relative_clauses sumti LUhU
        | LAhE             relative_clauses sumti /* ET */
        | LAhE                              sumti LUhU free_seq
        | LAhE                              sumti LUhU
        | LAhE                              sumti /* ET */

        | NAhE_BO NAhE BO free_seq relative_clauses sumti LUhU free_seq
        | NAhE_BO NAhE BO free_seq relative_clauses sumti LUhU
        | NAhE_BO NAhE BO free_seq relative_clauses sumti /* ET */
        | NAhE_BO NAhE BO free_seq                  sumti LUhU free_seq
        | NAhE_BO NAhE BO free_seq                  sumti LUhU
        | NAhE_BO NAhE BO free_seq                  sumti /* ET */

        | NAhE_BO NAhE BO          relative_clauses sumti LUhU free_seq
        | NAhE_BO NAhE BO          relative_clauses sumti LUhU
        | NAhE_BO NAhE BO          relative_clauses sumti /* ET */
        | NAhE_BO NAhE BO                           sumti LUhU free_seq
        | NAhE_BO NAhE BO                           sumti LUhU
        | NAhE_BO NAhE BO                           sumti /* ET */

        | KOhA    free_seq
        | KOhA

        | lerfu_string BOI free_seq
        | lerfu_string BOI
        | lerfu_string /* ET */

        | LA      free_seq relative_clauses CMENE_seq  free_seq
        | LA      free_seq relative_clauses CMENE_seq
        | LA      free_seq                  CMENE_seq  free_seq
        | LA      free_seq                  CMENE_seq

        | LA               relative_clauses CMENE_seq  free_seq
        | LA               relative_clauses CMENE_seq
        | LA                                CMENE_seq  free_seq
        | LA                                CMENE_seq

        | LE      free_seq sumti_tail KU free_seq
        | LE      free_seq sumti_tail KU
        | LE      free_seq sumti_tail /* ET */

        | LE               sumti_tail KU free_seq
        | LE               sumti_tail KU
        | LE               sumti_tail /* ET */

        | LA      free_seq sumti_tail KU free_seq
        | LA      free_seq sumti_tail KU
        | LA      free_seq sumti_tail /* ET */

        | LA               sumti_tail KU free_seq
        | LA               sumti_tail KU
        | LA               sumti_tail /* ET */

        | LI      free_seq mex LOhO free_seq
        | LI      free_seq mex LOhO
        | LI      free_seq mex /* ET */

        | LI               mex LOhO free_seq
        | LI               mex LOhO
        | LI               mex /* ET */

        | ZO free_seq /* Needs lexer tie-in */
        | ZO          /* Needs lexer tie-in */
        | LU text LIhU free_seq
        | LU text LIhU
        | LU text  /* ET */
        | LOhU free_seq /* Needs lexer tie-in */
        | LOhU          /* Needs lexer tie-in */
        | ZOI  free_seq /* Needs lexer tie-in */
        | ZOI           /* Needs lexer tie-in */
        ;

/*
sumti-tail<111> = [sumti-6 [relative-clauses]] sumti-tail-1 |
        relative-clauses sumti-tail-1
        */

sumti_tail : sumti_6 relative_clauses sumti_tail_1
           | sumti_6                  sumti_tail_1
           |                          sumti_tail_1
           |         relative_clauses sumti_tail_1
           ;

/*
sumti-tail-1<112> = [quantifier] selbri [relative-clauses] | quantifier sumti
*/


/* 1A split off so that there is a single antecedent node for the relative clauses case */

sumti_tail_1 : sumti_tail_1A relative_clauses
             | sumti_tail_1A
             | quantifier sumti
             ;

sumti_tail_1A : quantifier selbri
              |            selbri
              ;

/*
relative-clauses<121> = relative-clause [ZIhE # relative-clause] ...
*/

relative_clauses : relative_clause_seq
                 ;

relative_clause_seq : relative_clause
                    | relative_clause_seq ZIhE free_seq relative_clause
                    | relative_clause_seq ZIhE          relative_clause
                    ;

/*
relative-clause<122> = GOI # term /GEhU#/ | NOI # subsentence /KUhO#/
*/

relative_clause : term_relative_clause
                | full_relative_clause
                ;

term_relative_clause : GOI free_seq term GEhU free_seq
                     | GOI free_seq term GEhU
                     | GOI free_seq term /* ET */
                     | GOI          term GEhU free_seq
                     | GOI          term GEhU
                     | GOI          term /* ET */
                     ;

full_relative_clause : NOI free_seq subsentence KUhO free_seq
                     | NOI free_seq subsentence KUhO
                     | NOI free_seq subsentence /* ET */
                     | NOI          subsentence KUhO free_seq
                     | NOI          subsentence KUhO
                     | NOI          subsentence /* ET */
                     ;

/*
selbri<130> = [tag] selbri-1
*/

selbri : tag selbri_1
       |     selbri_1
       ;

 /*
selbri-1<131> = selbri-2 | NA # selbri
*/

selbri_1 : selbri_2
         | NA free_seq selbri
         | NA          selbri
         ;

/*
selbri-2<132> = selbri-3 [CO # selbri-2]
*/

selbri_2 : selbri_3 CO free_seq selbri_2
         | selbri_3 CO          selbri_2
         | selbri_3
         ;

/*
selbri-3<133> = selbri-4 ...
*/

selbri_3 : selbri_3 selbri_4
         |          selbri_4
         ;

/*
selbri-4<134> = selbri-5
        [joik-jek selbri-5 | joik [stag] KE # selbri-3 /KEhE#/] ...
        */


/* This is where things get REALLY hairy because of the KE # selbri_3 construct
   inside tanru_unit_2.  Separate cases need to be broken out */

selbri_4 : selbri_5
         | selbri_4 joik_opt_ke free_seq selbri_5
         | selbri_4 joik_opt_ke          selbri_5
         | selbri_4 jek_opt_ke  free_seq selbri_5
         | selbri_4 jek_opt_ke           selbri_5
         | selbri_4 joik_stag_ke ke_selbri_3
         ;

joik_stag_ke : JOIK_KE joik stag
             ;

ke_selbri_3 : KE free_seq selbri_3 KEhE free_seq
            | KE free_seq selbri_3 KEhE
            | KE free_seq selbri_3 /* ET */
            | KE          selbri_3 KEhE free_seq
            | KE          selbri_3 KEhE
            | KE          selbri_3 /* ET */
            ;



/*
selbri-5<135> = selbri-6 [(jek | joik) [stag] BO # selbri-5]
*/

selbri_5 : selbri_6
         | selbri_6 joik_jek_stag_bo selbri_5
         ;

joik_jek_stag_bo : JOIK_BO joik stag BO free_seq
                 | JOIK_BO joik stag BO
                 | JOIK_BO joik      BO free_seq
                 | JOIK_BO joik      BO
                 | JEK_BO  jek  stag BO free_seq
                 | JEK_BO  jek  stag BO
                 | JEK_BO  jek       BO free_seq
                 | JEK_BO  jek       BO
                 ;

/*
selbri-6<136> = tanru-unit [BO # selbri-6] | [NAhE #] guhek selbri gik selbri-6
*/

selbri_6 : tanru_unit
         | tanru_unit BO free_seq selbri_6
         | tanru_unit BO          selbri_6

         | NAhE free_seq guhek selbri gik selbri_6
         | NAhE          guhek selbri gik selbri_6
         |               guhek selbri gik selbri_6
         ;


/*
tanru-unit<150> = tanru-unit-1 [CEI # tanru-unit-1] ...
*/

tanru_unit : tanru_unit_1
           | tanru_unit CEI free_seq tanru_unit_1
           | tanru_unit CEI          tanru_unit_1
           ;


/*
tanru-unit-1<151> = tanru-unit-2 [linkargs]
*/

tanru_unit_1 : tanru_unit_2
             | tanru_unit_2 linkargs
             ;


/*
tanru-unit-2<152> = BRIVLA # | GOhA [RAhO] # | KE # selbri-3 /KEhE#/ |
        ME # sumti /MEhU#/ [MOI #] | (number | lerfu-string) MOI # |
        NUhA # mex-operator | SE # tanru-unit-2 | JAI # [tag] tanru-unit-2 |
        any-word (ZEI any-word) ... | NAhE # tanru-unit-2 |
        NU [NAI] # [joik-jek NU [NAI] #] ... subsentence /KEI#/
        */


/* All the cases that warrant special attention wrt handling
  conversion operators and how the propagate into following
  constructions are pulled out as subrules */

tanru_unit_2      : BRIVLA free_seq
                  | BRIVLA

                  | GOhA RAhO free_seq
                  | GOhA RAhO
                  | GOhA      free_seq
                  | GOhA

                  | ke_selbri3_tu2

                  | ME free_seq sumti    MEhU free_seq MOI free_seq
                  | ME free_seq sumti    MEhU free_seq MOI
                  | ME free_seq sumti    MEhU free_seq /* ET */
                  | ME free_seq sumti    MEhU          MOI free_seq
                  | ME free_seq sumti    MEhU          MOI
                  | ME free_seq sumti    MEhU /* ET */
                  | ME free_seq sumti                  MOI free_seq
                  | ME free_seq sumti                  MOI
                  | ME free_seq sumti         /* ET */

                  | ME          sumti    MEhU free_seq MOI free_seq
                  | ME          sumti    MEhU free_seq MOI
                  | ME          sumti    MEhU free_seq /* ET */
                  | ME          sumti    MEhU          MOI free_seq
                  | ME          sumti    MEhU          MOI
                  | ME          sumti    MEhU /* ET */
                  | ME          sumti                  MOI free_seq
                  | ME          sumti                  MOI
                  | ME          sumti         /* ET */

                  | number_moi_tu2

                  | NUhA free_seq mex_operator
                  | NUhA          mex_operator

                  | se_tu2

                  | jai_tag_tu2

                  | jai_tu2

                  | ZEI /* needs lexical tie-in */

                  | nahe_tu2

                  | abstraction

                  ;

ke_selbri3_tu2 : KE free_seq selbri_3 KEhE free_seq
               | KE free_seq selbri_3 KEhE
               | KE free_seq selbri_3 /* ET */
               | KE          selbri_3 KEhE free_seq
               | KE          selbri_3 KEhE
               | KE          selbri_3 /* ET */
               ;

number_moi_tu2 : NUMBER_MOI number       MOI free_seq
               | NUMBER_MOI number       MOI
               | NUMBER_MOI lerfu_string MOI free_seq
               | NUMBER_MOI lerfu_string MOI
               ;

se_tu2 : SE free_seq tanru_unit_2
       | SE          tanru_unit_2
       ;

jai_tag_tu2 : JAI free_seq tag tanru_unit_2
            | JAI          tag tanru_unit_2
            ;

jai_tu2 : JAI free_seq     tanru_unit_2
        | JAI              tanru_unit_2
        ;

nahe_tu2 : NAhE free_seq tanru_unit_2
         | NAhE          tanru_unit_2
         ;

/* This needs rework to make it easy to spot the case of NU <brivla>
  in the later analysis, as this would often be gloss-able as a single
  word */

abstraction : nu_nai_seq subsentence KEI free_seq
            | nu_nai_seq subsentence KEI
            | nu_nai_seq subsentence /* ET */
            ;

nu_nai_seq : NU NAI free_seq
           | NU     free_seq
           | NU NAI
           | NU
           | nu_nai_seq joik free_seq NU NAI free_seq
           | nu_nai_seq joik free_seq NU NAI
           | nu_nai_seq joik free_seq NU     free_seq
           | nu_nai_seq joik free_seq NU
           | nu_nai_seq joik          NU NAI free_seq
           | nu_nai_seq joik          NU NAI
           | nu_nai_seq joik          NU     free_seq
           | nu_nai_seq joik          NU
           | nu_nai_seq jek  free_seq NU NAI free_seq
           | nu_nai_seq jek  free_seq NU NAI
           | nu_nai_seq jek  free_seq NU     free_seq
           | nu_nai_seq jek  free_seq NU
           | nu_nai_seq jek           NU NAI free_seq
           | nu_nai_seq jek           NU NAI
           | nu_nai_seq jek           NU     free_seq
           | nu_nai_seq jek           NU
           ;


/*
linkargs<160> = BE # term [links] /BEhO#/
*/

linkargs : BE free_seq term links BEhO free_seq
         | BE free_seq term links BEhO
         | BE free_seq term links /* ET */
         | BE          term links BEhO free_seq
         | BE          term links BEhO
         | BE          term links /* ET */
         | BE free_seq term       BEhO free_seq
         | BE free_seq term       BEhO
         | BE free_seq term       /* ET */
         | BE          term       BEhO free_seq
         | BE          term       BEhO
         | BE          term       /* ET */
         ;

/*
links<161> = BEI # term [links]
*/

links : BEI free_seq term links
      | BEI          term links
      | BEI free_seq term
      | BEI          term
      ;

/*
quantifier<300> = number /BOI#/ | VEI # mex /VEhO#/
*/

quantifier : number BOI free_seq
           | number BOI
           | number /* ET */
           | VEI free_seq mex VEhO free_seq
           | VEI free_seq mex VEhO
           | VEI free_seq mex /* ET */
           | VEI          mex VEhO free_seq
           | VEI          mex VEhO
           | VEI          mex /* ET */
           ;

/*
mex<310> = mex-1 [operator mex-1] ... | FUhA # rp-expression
*/

mex : mex_infix
    | mex_rp
    ;

mex_rp : FUhA free_seq rp_expression
       | FUhA          rp_expression
       ;

mex_infix : mex_1
          | mex_infix operator mex_1
          ;

/*
mex-1<311> = mex-2 [BIhE # operator mex-1]
*/

mex_1 : mex_2
      | mex_2 BIhE free_seq operator mex_1
      | mex_2 BIhE          operator mex_1
      ;

/*
mex-2<312> = operand | [PEhO #] operator mex-2 ... /KUhE#/
*/

mex_2 : operand
      | PEhO free_seq operator mex_2_seq KUhE free_seq
      | PEhO free_seq operator mex_2_seq KUhE
      | PEhO free_seq operator mex_2_seq /* ET */

      | PEhO          operator mex_2_seq KUhE free_seq
      | PEhO          operator mex_2_seq KUhE
      | PEhO          operator mex_2_seq /* ET */

      |               operator mex_2_seq KUhE free_seq
      |               operator mex_2_seq KUhE
      |               operator mex_2_seq /* ET */
      ;

mex_2_seq :           mex_2
          | mex_2_seq mex_2
          ;

/*
rp-expression<330> = rp-operand rp-operand operator
*/

rp_expression : rp_expression rp_expression operator
              | operand       rp_expression operator
              | rp_expression operand       operator
              | operand       operand       operator
              ;

/*
rp-operand<332> = operand | rp-expression
*/

/*
rp_operand : operand
           | rp_expression
           ;
*/

/*
operator<370> = operator-1
        [joik-jek operator-1 | joik [stag] KE # operator /KEhE#/] ...
        */

operator : operator_1
         | operator joik_opt_ke free_seq operator_1
         | operator joik_opt_ke          operator_1
         | operator jek_opt_ke  free_seq operator_1
         | operator jek_opt_ke           operator_1
         | operator joik_stag_ke ke_operator
         ;

ke_operator : KE free_seq operator KEhE free_seq
            | KE free_seq operator KEhE
            | KE free_seq operator /* ET */
            | KE          operator KEhE free_seq
            | KE          operator KEhE
            | KE          operator /* ET */
            ;


/*
operator-1<371> = operator-2 | guhek operator-1 gik operator-2 |
        operator-2 (jek | joik) [stag] BO # operator-1
        */

operator_1 : operator_2
           | guhek operator_1 gik operator_2
           | operator_2 joik_jek_stag_bo operator_1
           ;


/*
operator-2<372> = mex-operator | KE # operator /KEhE#/
*/

operator_2 : mex_operator
           | KE free_seq operator KEhE free_seq
           | KE free_seq operator KEhE
           | KE free_seq operator /* ET */
           | KE          operator KEhE free_seq
           | KE          operator KEhE
           | KE          operator /* ET */
           ;


/*
mex-operator<374> = SE # mex-operator | NAhE # mex-operator |
        MAhO # mex /TEhU#/ | NAhU # selbri /TEhU#/ | VUhU #
        */

mex_operator : SE free_seq mex_operator
             | SE          mex_operator
             | NAhE free_seq mex_operator
             | NAhE          mex_operator
             | MAhO free_seq mex TEhU free_seq
             | MAhO free_seq mex TEhU
             | MAhO free_seq mex /* ET */
             | MAhO          mex TEhU free_seq
             | MAhO          mex TEhU
             | MAhO          mex /* ET */
             | NAhU free_seq selbri TEhU free_seq
             | NAhU free_seq selbri TEhU
             | NAhU free_seq selbri /* ET */
             | NAhU          selbri TEhU free_seq
             | NAhU          selbri TEhU
             | NAhU          selbri /* ET */
             | VUhU free_seq
             | VUhU
             ;

/*
operand<381> = operand-1 [(ek | joik) [stag] KE # operand /KEhE#/]
*/

operand : operand_1
        | operand_1 joik_ek_ke ke_operand
        ;

ke_operand : KE free_seq operand KEhE free_seq
           | KE free_seq operand KEhE
           | KE free_seq operand /* ET */
           | KE          operand KEhE free_seq
           | KE          operand KEhE
           | KE          operand /* ET */
           ;


/*
operand-1<382> = operand-2 [joik-ek operand-2] ...
*/

operand_1 : operand_2
          | operand_1 joik free_seq operand_2
          | operand_1 joik          operand_2
          | operand_1 jek  free_seq operand_2
          | operand_1 jek           operand_2
          ;


/*
operand-2<383> = operand-3 [(ek | joik) [stag] BO # operand-2]
*/

operand_2 : operand_3
          | operand_3 joik_ek_stag_bo operand_2
          ;



/*
operand-3<385> = quantifier | lerfu-string /BOI#/ |
        NIhE # selbri /TEhU#/ | MOhE # sumti /TEhU#/ |
        JOhI # mex-2 ... /TEhU#/ | gek operand gik operand-3 |
        (LAhE # | NAhE BO #) operand /LUhU#/
        */

operand_3 : quantifier

          | lerfu_string BOI free_seq
          | lerfu_string BOI
          | lerfu_string /* ET */

          | NIhE free_seq selbri TEhU free_seq
          | NIhE free_seq selbri TEhU
          | NIhE free_seq selbri /* ET */
          | NIhE          selbri TEhU free_seq
          | NIhE          selbri TEhU
          | NIhE          selbri /* ET */

          | MOhE free_seq sumti  TEhU free_seq
          | MOhE free_seq sumti  TEhU
          | MOhE free_seq sumti /* ET */
          | MOhE          sumti  TEhU free_seq
          | MOhE          sumti  TEhU
          | MOhE          sumti /* ET */

          | JOhI free_seq mex_2_seq TEhU free_seq
          | JOhI free_seq mex_2_seq TEhU
          | JOhI free_seq mex_2_seq /* ET */
          | JOhI          mex_2_seq TEhU free_seq
          | JOhI          mex_2_seq TEhU
          | JOhI          mex_2_seq /* ET */

          | gek operand gik operand_3

          | LAhE free_seq operand LUhU free_seq
          | LAhE free_seq operand LUhU
          | LAhE free_seq operand /* ET */
          | LAhE          operand LUhU free_seq
          | LAhE          operand LUhU
          | LAhE          operand /* ET */

          | NAhE_BO NAhE BO free_seq operand LUhU free_seq
          | NAhE_BO NAhE BO free_seq operand LUhU
          | NAhE_BO NAhE BO free_seq operand /* ET */
          | NAhE_BO NAhE BO          operand LUhU free_seq
          | NAhE_BO NAhE BO          operand LUhU
          | NAhE_BO NAhE BO          operand /* ET */
          ;

/*
number<812> = PA [PA | lerfu-word] ...
*/


number : PA
       | number PA
       | number lerfu_word
       ;


/*
lerfu-string<817> = lerfu-word [PA | lerfu-word] ...
*/


lerfu_string : lerfu_word
             | lerfu_string PA
             | lerfu_string lerfu_word
             ;


/*
lerfu-word<987> = BY | any-word BU | LAU lerfu-word | TEI lerfu-string FOI
*/

lerfu_word : BY
           | BU /* needs lexer tie-in */
           | LAU lerfu_word
           | TEI lerfu_string FOI
           ;

/*
ek<802> = [NA] [SE] A [NAI]
*/

ek : START_EK NA SE A NAI
   | START_EK NA SE A
   | START_EK NA    A NAI
   | START_EK NA    A
   | START_EK    SE A NAI
   | START_EK    SE A
   | START_EK       A NAI
   | START_EK       A
   ;

/*
gihek<818> = [NA] [SE] GIhA [NAI]
*/

gihek : START_GIHEK NA SE GIhA NAI
      | START_GIHEK NA SE GIhA
      | START_GIHEK NA    GIhA NAI
      | START_GIHEK NA    GIhA
      | START_GIHEK    SE GIhA NAI
      | START_GIHEK    SE GIhA
      | START_GIHEK       GIhA NAI
      | START_GIHEK       GIhA
      ;

/*
jek<805> = [NA] [SE] JA [NAI]
*/

jek : START_JEK NA SE JA NAI
    | START_JEK NA SE JA
    | START_JEK NA    JA NAI
    | START_JEK NA    JA
    | START_JEK    SE JA NAI
    | START_JEK    SE JA
    | START_JEK       JA NAI
    | START_JEK       JA
    ;

jek_opt_ke :        START_JEK NA SE JA NAI
           |        START_JEK NA SE JA
           |        START_JEK NA    JA NAI
           |        START_JEK NA    JA
           |        START_JEK    SE JA NAI
           |        START_JEK    SE JA
           |        START_JEK       JA NAI
           |        START_JEK       JA
           | JEK_KE START_JEK NA SE JA NAI
           | JEK_KE START_JEK NA SE JA
           | JEK_KE START_JEK NA    JA NAI
           | JEK_KE START_JEK NA    JA
           | JEK_KE START_JEK    SE JA NAI
           | JEK_KE START_JEK    SE JA
           | JEK_KE START_JEK       JA NAI
           | JEK_KE START_JEK       JA
           ;

jek_opt_kebo :        START_JEK NA SE JA NAI
             |        START_JEK NA SE JA
             |        START_JEK NA    JA NAI
             |        START_JEK NA    JA
             |        START_JEK    SE JA NAI
             |        START_JEK    SE JA
             |        START_JEK       JA NAI
             |        START_JEK       JA
             | JEK_KE START_JEK NA SE JA NAI
             | JEK_KE START_JEK NA SE JA
             | JEK_KE START_JEK NA    JA NAI
             | JEK_KE START_JEK NA    JA
             | JEK_KE START_JEK    SE JA NAI
             | JEK_KE START_JEK    SE JA
             | JEK_KE START_JEK       JA NAI
             | JEK_KE START_JEK       JA
             | JEK_BO START_JEK NA SE JA NAI
             | JEK_BO START_JEK NA SE JA
             | JEK_BO START_JEK NA    JA NAI
             | JEK_BO START_JEK NA    JA
             | JEK_BO START_JEK    SE JA NAI
             | JEK_BO START_JEK    SE JA
             | JEK_BO START_JEK       JA NAI
             | JEK_BO START_JEK       JA
             ;

/*
joik<806> = [SE] JOI [NAI] | interval | GAhO interval GAhO
*/

joik : START_JOIK      SE JOI  NAI
     | START_JOIK      SE JOI
     | START_JOIK         JOI  NAI
     | START_JOIK         JOI
     | START_JOIK      SE BIhI NAI
     | START_JOIK      SE BIhI
     | START_JOIK         BIhI NAI
     | START_JOIK         BIhI
     | START_JOIK GAhO SE BIhI NAI GAhO
     | START_JOIK GAhO SE BIhI     GAhO
     | START_JOIK GAhO    BIhI NAI GAhO
     | START_JOIK GAhO    BIhI     GAhO
     ;

/* Used where we get a joik that can never be guarded by KE later, but
where there might be a KE in the following construction, so we might
introduce JOIK_KE before */

joik_opt_ke :         START_JOIK      SE JOI  NAI
            |         START_JOIK      SE JOI
            |         START_JOIK         JOI  NAI
            |         START_JOIK         JOI
            |         START_JOIK      SE BIhI NAI
            |         START_JOIK      SE BIhI
            |         START_JOIK         BIhI NAI
            |         START_JOIK         BIhI
            |         START_JOIK GAhO SE BIhI NAI GAhO
            |         START_JOIK GAhO SE BIhI     GAhO
            |         START_JOIK GAhO    BIhI NAI GAhO
            |         START_JOIK GAhO    BIhI     GAhO
            | JOIK_KE START_JOIK      SE JOI  NAI
            | JOIK_KE START_JOIK      SE JOI
            | JOIK_KE START_JOIK         JOI  NAI
            | JOIK_KE START_JOIK         JOI
            | JOIK_KE START_JOIK      SE BIhI NAI
            | JOIK_KE START_JOIK      SE BIhI
            | JOIK_KE START_JOIK         BIhI NAI
            | JOIK_KE START_JOIK         BIhI
            | JOIK_KE START_JOIK GAhO SE BIhI NAI GAhO
            | JOIK_KE START_JOIK GAhO SE BIhI     GAhO
            | JOIK_KE START_JOIK GAhO    BIhI NAI GAhO
            | JOIK_KE START_JOIK GAhO    BIhI     GAhO
            ;

joik_opt_kebo :         START_JOIK      SE JOI  NAI
              |         START_JOIK      SE JOI
              |         START_JOIK         JOI  NAI
              |         START_JOIK         JOI
              |         START_JOIK      SE BIhI NAI
              |         START_JOIK      SE BIhI
              |         START_JOIK         BIhI NAI
              |         START_JOIK         BIhI
              |         START_JOIK GAhO SE BIhI NAI GAhO
              |         START_JOIK GAhO SE BIhI     GAhO
              |         START_JOIK GAhO    BIhI NAI GAhO
              |         START_JOIK GAhO    BIhI     GAhO
              | JOIK_KE START_JOIK      SE JOI  NAI
              | JOIK_KE START_JOIK      SE JOI
              | JOIK_KE START_JOIK         JOI  NAI
              | JOIK_KE START_JOIK         JOI
              | JOIK_KE START_JOIK      SE BIhI NAI
              | JOIK_KE START_JOIK      SE BIhI
              | JOIK_KE START_JOIK         BIhI NAI
              | JOIK_KE START_JOIK         BIhI
              | JOIK_KE START_JOIK GAhO SE BIhI NAI GAhO
              | JOIK_KE START_JOIK GAhO SE BIhI     GAhO
              | JOIK_KE START_JOIK GAhO    BIhI NAI GAhO
              | JOIK_KE START_JOIK GAhO    BIhI     GAhO
              | JOIK_BO START_JOIK      SE JOI  NAI
              | JOIK_BO START_JOIK      SE JOI
              | JOIK_BO START_JOIK         JOI  NAI
              | JOIK_BO START_JOIK         JOI
              | JOIK_BO START_JOIK      SE BIhI NAI
              | JOIK_BO START_JOIK      SE BIhI
              | JOIK_BO START_JOIK         BIhI NAI
              | JOIK_BO START_JOIK         BIhI
              | JOIK_BO START_JOIK GAhO SE BIhI NAI GAhO
              | JOIK_BO START_JOIK GAhO SE BIhI     GAhO
              | JOIK_BO START_JOIK GAhO    BIhI NAI GAhO
              | JOIK_BO START_JOIK GAhO    BIhI     GAhO
              ;

       
/*
interval<932> = [SE] BIhI [NAI]
 Inlined into joik and not used elsewhere 
*/

/*
joik-ek<421> = joik # | ek #

joik-jek<422> = joik # | jek #

The above are fully folded into other rules.
*/

/*
gek<807> = [SE] GA [NAI] # | joik GI # | stag gik
*/

gek : START_GEK SE GA NAI free_seq
    | START_GEK SE GA NAI
    | START_GEK SE GA     free_seq
    | START_GEK SE GA    
    | START_GEK    GA NAI free_seq
    | START_GEK    GA NAI
    | START_GEK    GA     free_seq
    | START_GEK    GA    
    | START_GEK joik GI free_seq
    | START_GEK joik GI
    | /* SG */ stag gik
    ;

/*
guhek<808> = [SE] GUhA [NAI] #
*/

guhek : START_GUHEK SE GUhA NAI free_seq
      | START_GUHEK SE GUhA NAI
      | START_GUHEK SE GUhA     free_seq
      | START_GUHEK SE GUhA    
      | START_GUHEK    GUhA NAI free_seq
      | START_GUHEK    GUhA NAI
      | START_GUHEK    GUhA     free_seq
      | START_GUHEK    GUhA    
      ;

/*
gik<816> = GI [NAI] #
*/

gik : GI NAI free_seq
    | GI NAI
    | GI     free_seq
    | GI
    ;

/*
tag<491> = tense-modal [joik-jek tense-modal] ...
*/


tag : ctag
    | stag
    ;


ctag :                             complex_tense_modal
     | ctag joik_opt_kebo free_seq complex_tense_modal
     | ctag joik_opt_kebo          complex_tense_modal
     | ctag jek_opt_kebo  free_seq complex_tense_modal
     | ctag jek_opt_kebo           complex_tense_modal
     | ctag joik_opt_kebo free_seq simple_tense_modal
     | ctag joik_opt_kebo          simple_tense_modal
     | ctag jek_opt_kebo  free_seq simple_tense_modal
     | ctag jek_opt_kebo           simple_tense_modal
     | stag joik_opt_kebo free_seq complex_tense_modal
     | stag joik_opt_kebo          complex_tense_modal
     | stag jek_opt_kebo  free_seq complex_tense_modal
     | stag jek_opt_kebo           complex_tense_modal
     ;


complex_tense_modal : FIhO free_seq selbri FEhU free_seq
                    | FIhO free_seq selbri FEhU
                    | FIhO free_seq selbri /* ET */
                    | FIhO          selbri FEhU free_seq
                    | FIhO          selbri FEhU
                    | FIhO          selbri /* ET */
                    | simple_tense_modal free_seq
                    ;

/*
stag<971> = simple-tense-modal [(jek | joik) simple-tense-modal] ...
*/


stag :                    simple_tense_modal
     | stag jek_opt_kebo  simple_tense_modal
     | stag joik_opt_kebo simple_tense_modal
     ;


/*
tense-modal<815> = simple-tense-modal # | FIhO # selbri /FEhU#/
*/

/*
simple-tense-modal<972> = [NAhE] [SE] BAI [NAI] [KI] |
        [NAhE] (time [space] | space [time]) & CAhA [KI] | KI | CUhE

*/

simple_tense_modal : START_BAI NAhE se_bai NAI KI
                   | START_BAI NAhE se_bai NAI
                   | START_BAI NAhE se_bai     KI
                   | START_BAI NAhE se_bai
                   | START_BAI NAhE    BAI NAI KI
                   | START_BAI NAhE    BAI NAI
                   | START_BAI NAhE    BAI     KI
                   | START_BAI NAhE    BAI
                   | START_BAI      se_bai NAI KI
                   | START_BAI      se_bai NAI
                   | START_BAI      se_bai     KI
                   | START_BAI      se_bai
                   | START_BAI         BAI NAI KI
                   | START_BAI         BAI NAI
                   | START_BAI         BAI     KI
                   | START_BAI         BAI

                   | NAhE_time NAhE time  space CAhA KI
                   | NAhE_time NAhE time  space CAhA
                   | NAhE_time NAhE time  space      KI
                   | NAhE_time NAhE time  space
                   | NAhE_time NAhE time        CAhA KI
                   | NAhE_time NAhE time        CAhA
                   | NAhE_time NAhE time             KI
                   | NAhE_time NAhE time
                   |                time  space CAhA KI
                   |                time  space CAhA
                   |                time  space      KI
                   |                time  space
                   |                time        CAhA KI
                   |                time        CAhA
                   |                time             KI
                   |                time

                   | NAhE_space NAhE space time  CAhA KI
                   | NAhE_space NAhE space time  CAhA
                   | NAhE_space NAhE space time       KI
                   | NAhE_space NAhE space time 
                   | NAhE_space NAhE space       CAhA KI
                   | NAhE_space NAhE space       CAhA
                   | NAhE_space NAhE space            KI
                   | NAhE_space NAhE space
                   |                 space time  CAhA KI
                   |                 space time  CAhA
                   |                 space time       KI
                   |                 space time 
                   |                 space       CAhA KI
                   |                 space       CAhA
                   |                 space            KI
                   |                 space

                   |                             CAhA KI
                   |                             CAhA

                   | KI
                   | CUhE
                   ;


/* A special non-terminal to make it easy to recognize this in
  the post-processing */

se_bai : SE BAI
       ;


/*
time<1030> = ZI & time-offset ... & ZEhA [PU [NAI]] & interval-property ...
*/

time : ZI time_offset_seq zeha_pu_nai interval_property_seq
     | ZI time_offset_seq             interval_property_seq

     | ZI time_offset_seq zeha_pu_nai
     | ZI time_offset_seq

     | ZI                 zeha_pu_nai interval_property_seq
     | ZI                             interval_property_seq

     | ZI                 zeha_pu_nai
     | ZI                

     |    time_offset_seq zeha_pu_nai interval_property_seq
     |    time_offset_seq             interval_property_seq

     |    time_offset_seq zeha_pu_nai
     |    time_offset_seq

     |                    zeha_pu_nai interval_property_seq
     |                                interval_property_seq

     |                    zeha_pu_nai

     ;

zeha_pu_nai : ZEhA PU NAI
            | ZEhA PU
            | ZEhA
            ;

/*
time-offset<1033> = PU [NAI] [ZI]
*/

time_offset : PU NAI ZI
            | PU NAI
            | PU     ZI
            | PU
            ;

time_offset_seq : time_offset_seq time_offset
                |                 time_offset
                ;

/*
space<1040> = VA & space-offset ... & space-interval & (MOhI space-offset)
*/

space : VA space_offset_seq space_interval MOhI space_offset
      | VA space_offset_seq space_interval
      | VA space_offset_seq                MOhI space_offset
      | VA space_offset_seq

      | VA                  space_interval MOhI space_offset
      | VA                  space_interval
      | VA                                 MOhI space_offset
      | VA                 

      |    space_offset_seq space_interval MOhI space_offset
      |    space_offset_seq space_interval
      |    space_offset_seq                MOhI space_offset
      |    space_offset_seq

      |                     space_interval MOhI space_offset
      |                     space_interval
      |                                    MOhI space_offset
      ;

/*
space-offset<1045> = FAhA [NAI] [VA]
*/

space_offset : FAhA NAI VA
             | FAhA NAI
             | FAhA     VA
             | FAhA
             ;

space_offset_seq : space_offset_seq space_offset
                 |                  space_offset
                 ;

/*
space-interval<1046> = ((VEhA & VIhA) [FAhA [NAI]]) & space-int-props
*/

space_interval : VEhA VIhA FAhA NAI space_int_props
               | VEhA VIhA FAhA     space_int_props
               | VEhA VIhA          space_int_props
               | VEhA      FAhA NAI space_int_props
               | VEhA      FAhA     space_int_props
               | VEhA               space_int_props
               |      VIhA FAhA NAI space_int_props
               |      VIhA FAhA     space_int_props
               |      VIhA          space_int_props

               | VEhA VIhA FAhA NAI
               | VEhA VIhA FAhA
               | VEhA VIhA
               | VEhA      FAhA NAI
               | VEhA      FAhA
               | VEhA
               |      VIhA FAhA NAI
               |      VIhA FAhA
               |      VIhA

               |                    space_int_props
               ;

/*
space-int-props<1049> = (FEhE interval-property) ...
*/

space_int_props : space_int_props space_int_prop
                |                 space_int_prop
                ;

space_int_prop : FEhE interval_property
               ;

/*
interval-property<1051> = number ROI [NAI] | TAhE [NAI] | ZAhO [NAI]
*/

interval_property : NUMBER_ROI number ROI NAI
                  | NUMBER_ROI number ROI
                  | TAhE NAI
                  | TAhE
                  | ZAhO NAI
                  | ZAhO
                  ;

interval_property_seq : interval_property_seq interval_property
                      |                       interval_property
                      ;


/*
free<32> = SEI # [terms [CU #]] selbri /SEhU/ | SOI # sumti [sumti] /SEhU/ |
        vocative [relative-clauses] selbri [relative-clauses] /DOhU/ |
        vocative [relative-clauses] CMENE ... # [relative-clauses] /DOhU/ |
        vocative [sumti] /DOhU/ | (number | lerfu-string) MAI |
        TO text /TOI/ | XI # (number | lerfu-string) /BOI/ |
        XI # VEI # mex /VEhO/
*/

free_seq : free_seq free
         |          free
         ;

free : metalinguistic
     | reciprocity
     | free_vocative
     | utterance_ordinal
     | parenthetical
     | subscript
     ;


metalinguistic : SEI free_seq terms CU free_seq metalinguistic_main_selbri SEhU
               | SEI free_seq terms CU free_seq metalinguistic_main_selbri /* ET */
               | SEI free_seq terms CU          metalinguistic_main_selbri SEhU
               | SEI free_seq terms CU          metalinguistic_main_selbri /* ET */
               | SEI free_seq terms             metalinguistic_main_selbri SEhU
               | SEI free_seq terms             metalinguistic_main_selbri /* ET */
               | SEI free_seq                   metalinguistic_main_selbri SEhU
               | SEI free_seq                   metalinguistic_main_selbri /* ET */

               | SEI          terms CU free_seq metalinguistic_main_selbri SEhU
               | SEI          terms CU free_seq metalinguistic_main_selbri /* ET */
               | SEI          terms CU          metalinguistic_main_selbri SEhU
               | SEI          terms CU          metalinguistic_main_selbri /* ET */
               | SEI          terms             metalinguistic_main_selbri SEhU
               | SEI          terms             metalinguistic_main_selbri /* ET */
               | SEI                            metalinguistic_main_selbri SEhU
               | SEI                            metalinguistic_main_selbri /* ET */
               ;

metalinguistic_main_selbri : selbri
                           ;

reciprocity : SOI free_seq sumti sumti SEhU
            | SOI free_seq sumti sumti /* ET */
            | SOI free_seq sumti       SEhU
            | SOI free_seq sumti /* ET */
            | SOI          sumti sumti SEhU
            | SOI          sumti sumti /* ET */
            | SOI          sumti       SEhU
            | SOI          sumti /* ET */
            ;


free_vocative : vocative relative_clauses selbri relative_clauses DOhU
              | vocative relative_clauses selbri relative_clauses /* ET */
              | vocative relative_clauses selbri                  DOhU
              | vocative relative_clauses selbri /* ET */
              | vocative                  selbri relative_clauses DOhU
              | vocative                  selbri relative_clauses /* ET */
              | vocative                  selbri                  DOhU
              | vocative                  selbri /* ET */

              | vocative relative_clauses CMENE_seq free_seq relative_clauses DOhU
              | vocative relative_clauses CMENE_seq free_seq relative_clauses /* ET */
              | vocative relative_clauses CMENE_seq free_seq                  DOhU
              | vocative relative_clauses CMENE_seq free_seq /* ET */

              | vocative                  CMENE_seq free_seq relative_clauses DOhU
              | vocative                  CMENE_seq free_seq relative_clauses /* ET */
              | vocative                  CMENE_seq free_seq                  DOhU
              | vocative                  CMENE_seq free_seq /* ET */

              | vocative relative_clauses CMENE_seq          relative_clauses DOhU
              | vocative relative_clauses CMENE_seq          relative_clauses /* ET */
              | vocative relative_clauses CMENE_seq                           DOhU
              | vocative relative_clauses CMENE_seq          /* ET */

              | vocative                  CMENE_seq          relative_clauses DOhU
              | vocative                  CMENE_seq          relative_clauses /* ET */
              | vocative                  CMENE_seq                           DOhU
              | vocative                  CMENE_seq          /* ET */

              | vocative sumti DOhU
              | vocative sumti /* ET */
              | vocative       DOhU
              | vocative /* ET */
              ;

utterance_ordinal : NUMBER_MAI number       MAI
                  | NUMBER_MAI lerfu_string MAI
                  ;

parenthetical : TO text TOI
              | TO text /* ET */
              ;

subscript : XI free_seq number       BOI
          | XI free_seq number /* ET */
          | XI          number       BOI
          | XI          number /* ET */
          | XI free_seq lerfu_string BOI
          | XI free_seq lerfu_string /* ET */
          | XI          lerfu_string BOI
          | XI          lerfu_string /* ET */

          | XI free_seq VEI free_seq mex VEhO
          | XI free_seq VEI free_seq mex /* ET */
          | XI free_seq VEI          mex VEhO
          | XI free_seq VEI          mex /* ET */
          | XI          VEI free_seq mex VEhO
          | XI          VEI free_seq mex /* ET */
          | XI          VEI          mex VEhO
          | XI          VEI          mex /* ET */
          ;




/*
vocative<415> = (COI [NAI]) ... & DOI
*/

vocative : coi_nai_seq DOI
         | coi_nai_seq
         |             DOI
         ;

coi_nai_seq : COI NAI
            | COI
            | coi_nai_seq COI NAI
            | coi_nai_seq COI
            ;


/*
indicators<411> = [FUhE] indicator ...
*/

indicators : FUhE indicator_seq
           |      indicator_seq
           ;

indicator_seq : indicator_seq indicator
              |               indicator
              ;

/*
indicator<413> =  (UI | CAI) [NAI] | Y | DAhO | FUhO
*/

indicator : UI  NAI
          | UI
          | CAI NAI
          | CAI
          | Y
          | DAhO
          | FUhO
          ;

NAI_seq : NAI_seq NAI
        |         NAI
        ;

CMENE_seq : CMENE_seq CMENE
          |           CMENE
          ;

NIhO_seq_free_seq : NIhO_seq free_seq
                  | NIhO_seq
                  ;

NIhO_seq : NIhO_seq NIhO
         |          NIhO
         ;
