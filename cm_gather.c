/***************************************
  $Header$

  Functions for gathering text into a list and preprocessing the list.
  ***************************************/

/* COPYRIGHT */


#include "cm.h"

/* ================================================== */

typedef enum {
  TT_CMAVO,
  TT_BRIVLA,
  TT_CMENE,
  TT_NEWLINE,
  TT_PAREN
} toktype;

typedef struct node {
  struct node *next;
  struct node *prev;
  toktype type;
  char *lojban;
  char *selmao;
  char *trans;
} node;


/* ================================================== */

static node head = {&head, &head};

/* ================================================== */

static void
add_node(toktype type, const char *text)
{
  node *nn = new(node);
  nn->type = type;
  nn->lojban = new_string(text);
  nn->prev = head.prev;
  nn->next = &head;
  head.prev->next = nn;
  head.prev = nn;
}



/* ================================================== */

void
gather_cmavo (const char *x)
{
  add_node(TT_CMAVO, x);
}

/* ================================================== */

void
gather_brivla (const char *x)
{
  add_node(TT_BRIVLA, x);
}

/* ================================================== */

void
gather_cmene (const char *x)
{
  add_node(TT_CMENE, x);
}

/* ================================================== */

void
gather_newline(void)
{
  add_node(TT_NEWLINE, "");
}

/* ================================================== */

void
gather_fallthru(const char *x)
{
  fprintf(stderr, "This fell through [%s]\n", x);
}

/* ================================================== */

void
gather_paren(const char *x)
{
  add_node(TT_PAREN, x);
}


/* ================================================== */

#if 0
static void
try_cmavo_fragment(node *p)
{
  char buf_in[32];
  char buf[2048];
  char buf_out[256];

  char *q, *r, *t;
  buf[0] = 0;
  q = p->lojban;
  if (*q == '.') q++;
  while (*q) {
    r = buf_in;
    do {
      *r++ = *q++;
    } while (*q && strchr("aeiou'", *q));
    *r = 0;
    t = translate(buf_in);
    if (t) {
      strcat(buf, t);
      strcat(buf, " ");
    } else {
      strcat(buf, "? ");
    }
  }

  p->trans = new_string(buf);
}
#endif

/* ================================================== */

static char *
translate_selmao(char *x) {
  char buffer[64];
  strcpy(buffer, "+");
  strcat(buffer, x);
  return translate(buffer);
}

/* ================================================== */

static void
try_cmavo_fragment(char *lojban, char **transbuf, char* (*transfn)(char *))
{
  char buf[256], buf2[256];
  char *pbuf;
  char *t, *e;
  char bufout[1024];
  int len;

  bufout[0] = 0;

  strcpy(buf, lojban);

  pbuf = buf;
  if (*pbuf == '.') {
    pbuf++;
  }
#if DIAG
  fprintf(stderr, "Start with %s\n", pbuf);
#endif
  while (*pbuf) {
    e = pbuf;
    while (*e) e++;
    while (e > pbuf) {
      len = e - pbuf;
#if DIAG
      fprintf(stderr, "Length = %d\n", len);
#endif
      strncpy(buf2, pbuf, len);
      buf2[len] = 0;
      t = (*transfn)(buf2);
      if (t) {
#if DIAG
        fprintf(stderr, "Got trans %s for %s\n", t, buf2);
#endif
        if (bufout[0]) {
          strcat(bufout, " ");
        }
        strcat(bufout, t);
        pbuf += len;
#if DIAG
        fprintf(stderr, "Now looking at %s\n", pbuf);
#endif
        goto next_outer;
      } else {
#if DIAG
        fprintf(stderr,"Failed to get a translation for %s\n", buf2);
#endif
        do {
          e--;
        } while (strchr("aeiuo'", *e));
      }
    }
    if (bufout[0]) {
      strcat(bufout, " ");
    }
    strcat(bufout, "?");
    do {
      pbuf++;
    } while (*pbuf && strchr("aeiou'", *pbuf));

    if (*pbuf == '.') ++pbuf;

  next_outer:
    ;
    
  }

  (*transbuf) = new_string(bufout);

}

/* ================================================== */

void
do_trans(void)
{
  node *p;
  char *t, *x;
  char buf[32];

  for (p=head.next; p!=&head; p=p->next) {
    switch (p->type) {
      case TT_CMAVO:
        x = p->lojban;
        if (*x == '.') x++;
        t = translate(x);
        if (t) {
          p->trans = new_string(t);

          strcpy(buf, "+");
          strcat(buf, x);
          t = translate(buf);
          if (t) {
            p->selmao = new_string(t);
          } else {
            p->selmao = new_string("");
          }
        } else {
          try_cmavo_fragment(p->lojban, &p->trans, translate);
          try_cmavo_fragment(p->lojban, &p->selmao, translate_selmao);
        }
        break;

      case TT_BRIVLA:
        t = translate(p->lojban);
        if (t) {
          p->trans = new_string(t);
        } else {
          t = translate_unknown(p->lojban);
          if (t) {
            p->trans = new_string(t);
          } else {
            p->trans = new_string("?");
          }
        }
        break;
      default:
        p->trans = new_string("");
    }
  }
}

/* ================================================== */

void
do_output(void)
{
  node *p;
  for (p=head.next; p!=&head; p=p->next) {
    if (p->type == TT_NEWLINE) {
      output_newline();
    } else if (p->type == TT_PAREN) {
      output_paren(p->lojban);
    } else if (p->type == TT_CMAVO) {
      output(p->lojban, p->trans, p->selmao);
    } else if (p->type == TT_BRIVLA) {
      output(p->lojban, p->trans, "BRIVLA");
    } else if (p->type == TT_CMENE) {
      output(p->lojban, p->trans, "CMENE");
    }
  }
}

/* ================================================== */
