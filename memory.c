/***************************************
  $Header$

  malloc/free wrappers to allow auditing.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

static unsigned long bytes_in_use = 0;
static unsigned long max_bytes_in_use = 0;

/*++++++++++++++++++++++++++++++
  Replacement for malloc()
  ++++++++++++++++++++++++++++++*/

void *Malloc(size_t n)
{
  char *s, *r;
  unsigned long *x;
  size_t nn;

  bytes_in_use += n;
  if (bytes_in_use > max_bytes_in_use) {
    max_bytes_in_use = bytes_in_use;
  }

  nn = n + 8;
  s = (char *) malloc(nn);
  x = (unsigned long *) s;
  *x = n;
  r = (void *) (s + 8);

  return r;
}

/*++++++++++++++++++++++++++++++
  Replacement for free()
  ++++++++++++++++++++++++++++++*/

void Free(void *x)
{
  char *r, *s;
  unsigned long *v;
  size_t nn;

  r = (char *) x;
  s = (void *) (r - 8);
  v = (unsigned long *) s;

  nn = *v;
  bytes_in_use -= nn;

  free(s);
}

/*++++++++++++++++++++++++++++++
  Print amount of memory used
  ++++++++++++++++++++++++++++++*/

void
print_memory_statistics(void)
{
  fprintf(stderr,
          "Bytes in use at end  : %8lu\n"
          "Maximum bytes in use : %8lu\n",
          bytes_in_use, max_bytes_in_use);

}     
