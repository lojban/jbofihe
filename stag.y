/***************************************
  $Header$

  Small bison grammar used to recognize a <stag> followed by KE or BO.
  ***************************************/

/* COPYRIGHT */

%token STAG_BAI
%token STAG_BIhI
%token STAG_BU
%token STAG_BY
%token STAG_CAhA
%token STAG_CUhE
%token STAG_FAhA
%token STAG_FEhE
%token STAG_FOI
%token STAG_GAhO
%token STAG_JA
%token STAG_JOI
%token STAG_LAU
%token STAG_KI
%token STAG_MOhI
%token STAG_NA
%token STAG_NAhE
%token STAG_NAI
%token STAG_PA
%token STAG_PU
%token STAG_ROI
%token STAG_SE
%token STAG_TAhE
%token STAG_TEI
%token STAG_VA
%token STAG_VEhA
%token STAG_VIhA
%token STAG_ZAhO
%token STAG_ZEhA
%token STAG_ZI

%token STAG_KE
%token STAG_BO

/* This is the bogus token if we fall off the end of <stag> without
   finding BO or KE, i.e. find something else */
%token STAG_OTHER

%{
extern void stag_lookahead_bo(void);
extern void stag_lookahead_ke(void);
extern void stag_error(const char *);
extern int stag_lex(void);
%}

%%

all : stag_ke
    | stag_bo
    ;

stag_ke : stag STAG_KE
{ stag_lookahead_ke(); }
;

stag_bo : stag STAG_BO
{ stag_lookahead_bo(); }
;

stag :           simple_tense_modal
     | stag jek  simple_tense_modal
     | stag joik simple_tense_modal
     ;

joik :      STAG_SE STAG_JOI  STAG_NAI
     |      STAG_SE STAG_JOI
     |         STAG_JOI  STAG_NAI
     |         STAG_JOI
     |      STAG_SE STAG_BIhI STAG_NAI
     |      STAG_SE STAG_BIhI
     |         STAG_BIhI STAG_NAI
     |         STAG_BIhI
     | STAG_GAhO STAG_SE STAG_BIhI STAG_NAI STAG_GAhO
     | STAG_GAhO STAG_SE STAG_BIhI     STAG_GAhO
     | STAG_GAhO    STAG_BIhI STAG_NAI STAG_GAhO
     | STAG_GAhO    STAG_BIhI     STAG_GAhO
     ;

jek : STAG_NA STAG_SE STAG_JA STAG_NAI
    | STAG_NA STAG_SE STAG_JA
    | STAG_NA    STAG_JA STAG_NAI
    | STAG_NA    STAG_JA
    |    STAG_SE STAG_JA STAG_NAI
    |    STAG_SE STAG_JA
    |       STAG_JA STAG_NAI
    |       STAG_JA
    ;

simple_tense_modal : STAG_NAhE se_bai STAG_NAI STAG_KI
                   | STAG_NAhE se_bai STAG_NAI
                   | STAG_NAhE se_bai     STAG_KI
                   | STAG_NAhE se_bai
                   | STAG_NAhE    STAG_BAI STAG_NAI STAG_KI
                   | STAG_NAhE    STAG_BAI STAG_NAI
                   | STAG_NAhE    STAG_BAI     STAG_KI
                   | STAG_NAhE    STAG_BAI
                   |      se_bai STAG_NAI STAG_KI
                   |      se_bai STAG_NAI
                   |      se_bai     STAG_KI
                   |      se_bai
                   |         STAG_BAI STAG_NAI STAG_KI
                   |         STAG_BAI STAG_NAI
                   |         STAG_BAI     STAG_KI
                   |         STAG_BAI

                   | STAG_NAhE time  space STAG_CAhA STAG_KI
                   | STAG_NAhE time  space STAG_CAhA
                   | STAG_NAhE time  space      STAG_KI
                   | STAG_NAhE time  space
                   | STAG_NAhE time        STAG_CAhA STAG_KI
                   | STAG_NAhE time        STAG_CAhA
                   | STAG_NAhE time             STAG_KI
                   | STAG_NAhE time
                   |      time  space STAG_CAhA STAG_KI
                   |      time  space STAG_CAhA
                   |      time  space      STAG_KI
                   |      time  space
                   |      time        STAG_CAhA STAG_KI
                   |      time        STAG_CAhA
                   |      time             STAG_KI
                   |      time

                   | STAG_NAhE space time  STAG_CAhA STAG_KI
                   | STAG_NAhE space time  STAG_CAhA
                   | STAG_NAhE space time       STAG_KI
                   | STAG_NAhE space time 
                   | STAG_NAhE space       STAG_CAhA STAG_KI
                   | STAG_NAhE space       STAG_CAhA
                   | STAG_NAhE space            STAG_KI
                   | STAG_NAhE space
                   |      space time  STAG_CAhA STAG_KI
                   |      space time  STAG_CAhA
                   |      space time       STAG_KI
                   |      space time 
                   |      space       STAG_CAhA STAG_KI
                   |      space       STAG_CAhA
                   |      space            STAG_KI
                   |      space

                   |                             STAG_CAhA STAG_KI
                   |                             STAG_CAhA

                   | STAG_KI
                   | STAG_CUhE
                   ;

se_bai : STAG_SE STAG_BAI
       ;
time : STAG_ZI time_offset_seq zeha_pu_nai interval_property_seq
     | STAG_ZI time_offset_seq             interval_property_seq

     | STAG_ZI time_offset_seq zeha_pu_nai
     | STAG_ZI time_offset_seq

     | STAG_ZI                 zeha_pu_nai interval_property_seq
     | STAG_ZI                             interval_property_seq

     | STAG_ZI                 zeha_pu_nai
     | STAG_ZI                

     |    time_offset_seq zeha_pu_nai interval_property_seq
     |    time_offset_seq             interval_property_seq

     |    time_offset_seq zeha_pu_nai
     |    time_offset_seq

     |                    zeha_pu_nai interval_property_seq
     |                                interval_property_seq

     |                    zeha_pu_nai

     ;

zeha_pu_nai : STAG_ZEhA STAG_PU STAG_NAI
            | STAG_ZEhA STAG_PU
            | STAG_ZEhA
            ;


time_offset : STAG_PU STAG_NAI STAG_ZI
            | STAG_PU STAG_NAI
            | STAG_PU     STAG_ZI
            | STAG_PU
            ;

time_offset_seq : time_offset_seq time_offset
                |                 time_offset
                ;


space : STAG_VA space_offset_seq space_interval STAG_MOhI space_offset
      | STAG_VA space_offset_seq space_interval
      | STAG_VA space_offset_seq                STAG_MOhI space_offset
      | STAG_VA space_offset_seq

      | STAG_VA                  space_interval STAG_MOhI space_offset
      | STAG_VA                  space_interval
      | STAG_VA                                 STAG_MOhI space_offset
      | STAG_VA                 

      |    space_offset_seq space_interval STAG_MOhI space_offset
      |    space_offset_seq space_interval
      |    space_offset_seq                STAG_MOhI space_offset
      |    space_offset_seq

      |                     space_interval STAG_MOhI space_offset
      |                     space_interval
      |                                    STAG_MOhI space_offset
      ;

space_offset : STAG_FAhA STAG_NAI STAG_VA
             | STAG_FAhA STAG_NAI
             | STAG_FAhA     STAG_VA
             | STAG_FAhA
             ;

space_offset_seq : space_offset_seq space_offset
                 |                  space_offset
                 ;

space_interval : STAG_VEhA STAG_VIhA STAG_FAhA STAG_NAI space_int_props
               | STAG_VEhA STAG_VIhA STAG_FAhA     space_int_props
               | STAG_VEhA STAG_VIhA          space_int_props
               | STAG_VEhA      STAG_FAhA STAG_NAI space_int_props
               | STAG_VEhA      STAG_FAhA     space_int_props
               | STAG_VEhA               space_int_props
               |      STAG_VIhA STAG_FAhA STAG_NAI space_int_props
               |      STAG_VIhA STAG_FAhA     space_int_props
               |      STAG_VIhA          space_int_props

               | STAG_VEhA STAG_VIhA STAG_FAhA STAG_NAI
               | STAG_VEhA STAG_VIhA STAG_FAhA
               | STAG_VEhA STAG_VIhA
               | STAG_VEhA      STAG_FAhA STAG_NAI
               | STAG_VEhA      STAG_FAhA
               | STAG_VEhA
               |      STAG_VIhA STAG_FAhA STAG_NAI
               |      STAG_VIhA STAG_FAhA
               |      STAG_VIhA

               |                    space_int_props
               ;


space_int_props : space_int_props STAG_FEhE interval_property
                |                 STAG_FEhE interval_property
                ;

interval_property : number STAG_ROI STAG_NAI
                  | number STAG_ROI
                  | STAG_TAhE STAG_NAI
                  | STAG_TAhE
                  | STAG_ZAhO STAG_NAI
                  | STAG_ZAhO
                  ;

interval_property_seq : interval_property_seq interval_property
                      |                       interval_property
                      ;

number : STAG_PA
       | number STAG_PA
       | number lerfu_word
       ;


lerfu_string : lerfu_word
             | lerfu_string STAG_PA
             | lerfu_string lerfu_word
             ;

lerfu_word : STAG_BY
           | STAG_BU /* needs lexer tie-in */
           | STAG_LAU lerfu_word
           | STAG_TEI lerfu_string STAG_FOI
           ;

