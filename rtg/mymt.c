/*
  $Header$

  This is yet another implementation of the Mersenne Twister random number
  generator (ref. http://www.math.keio.ac.jp/~matumoto/emt.html).
 
  It is based somewhat on the version written by Shawn Cokus
  <Cokus@math.washington.edu>.  I've used the revised seeding approach that
  allows any seed value to be used safely.

  The main feature of this implementation is that I've tried to unroll the
  loops.  Thus the performance should be better on register-rich architectures
  for 2 reasons : 1. load->use distance is more likely to be hidden behind
  other useful work, and there are more places where the code can now dual
  issue into >1 ALU.  Hopefully constant loading is quick, as all state +
  constants are accessed as offsets from a single common base address.

  The output is written as raw 4-byte integers to stdout.  The intention is
  that this data is piped into another program (in my case a perl script that
  opens the generator with a piped-open).

  This implementation is Copyright 2001 Richard P. Curnow.

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation (either version 2 of the License or, at your
  option, any later version).  This library is distributed in the hope that
  it will be useful, but WITHOUT ANY WARRANTY, without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
  the GNU Library General Public License for more details.  You should have
  received a copy of the GNU Library General Public License along with this
  library; if not, write to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307, USA.

  The original codes contain the following copyright notices:

    Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura.
    Any feedback is very welcome. For any question, comments,
    see http://www.math.keio.ac.jp/matumoto/emt.html or email
    matumoto@math.keio.ac.jp.  When you use this, send an e-mail to
    <matumoto@math.keio.ac.jp> with an appropriate reference to your work.

*/

#include <stdio.h>
#include <stdlib.h>

/* Macros lifted in concept from the Cokus implementation */
#define msb(u)    ((u) & M31)
#define lsb(u)    ((u) & M0)
#define lo(u)     ((u) & M300)
#define mix(u, v) (msb(u)|lo(v))

struct mt_state {
  int left;
  unsigned long *next;
  unsigned long keep;

  /* Constants needed by reload */
  unsigned long M31;
  unsigned long M0;
  unsigned long M300;
  unsigned long K;

  /* 'Tempering' constants needed to mix the random values */
  unsigned long C1;
  unsigned long C2;

  /* The state of the generator */
  unsigned long state[625];
};

static struct mt_state state = {
  -1,0,0,
  0x80000000UL, 0x00000001UL, 0x7fffffffUL,
  0x9908b0dfUL, 0x9d2c5680UL, 0xefc60000UL
};
  
/* For debugging purposes */
static void printstate(void)
{
  int i;
  for (i=0; i<624; i++) {
    printf("%3d:%08lx\n", i, state.state[i]);
  }
}

/* Uses the revised seeding algorithm that allows any integer to work as a seed
 * value. */
unsigned long seedmt(unsigned long seed) {
  unsigned long k = 69069UL;
  unsigned long *s = state.state, x, y, m=0xffff0000UL;
  int j;
  x = seed;
  state.left=0;
  *s++ = x;
  for (j=624; --j;) {
    y = x & m;
    x = (k*x) + 1;
    y |= (x & m) >> 16;
    *s++ = y;
    x = (k*x) + 1;
  }
}

/* Unroll loops by a factor of 2.  Hopefully, this should help bury load/use
 * latency for any machine with enough registers, and allow dual-issue into >1
 * ALU */
static int reloadmt(void)
{
  int i;
  register unsigned long *p0, *p2, *pM;
  register unsigned long p, q, r, s, x, y; 
  register unsigned long a, b, c;
  register unsigned long K = state.K;
  register unsigned long M31 = state.M31;
  register unsigned long M0 = state.M0;
  register unsigned long M300 = state.M300;

  p0 = state.state;
  p2 = p0 + 2;
  pM = p0 + 397;
  a = *p0, b = p0[1], c = *pM++;
  for (i=0; i<113; i++) { /* First 226 entries, 2 per iter */
    p = mix(a,b)>>1;
    q = 0;
    a = *p2++;
    if (lsb(b)) q = K; /* Conditional move? */
    x = c ^ p ^ q;
    c = *pM++;
    *p0++ = x;
    r = mix(b,a)>>1;
    s = 0;
    b = *p2++;
    if (lsb(a)) s = K; /* Conditional move? */
    y = c ^ r ^ s;
    c = *pM++;
    *p0++ = y;
  }
  /* a=state[226], b=state[227], c=state[623] */
  p = mix(a,b)>>1;
  q = 0;
  a = *p2++;
  if (lsb(b)) q = K; /* Conditional move? */
  x = c ^ p ^ q;
  pM = state.state;
  *p0++ = x;
  c = *pM++;
  state.state[624] = c;

  /* a=state[228], b=state[227], c=state[0] */
  for (i=0; i<198; i++) { /* Next 396 entries, 2 per iter */
    r = mix(b,a)>>1;
    s = 0;
    b = *p2++;
    if (lsb(a)) s = K; /* Conditional move? */
    y = c ^ r ^ s;
    c = *pM++;
    *p0++ = y;
    p = mix(a,b)>>1;
    q = 0;
    a = *p2++;
    if (lsb(b)) q = K; /* Conditional move? */
    x = c ^ p ^ q;
    c = *pM++;
    *p0++ = x;
  }

  /* Now a=state[624](=state[0]), b=state[623], c=state[396] */
  state.state[623] = c ^ (mix(b,a)>>1) ^ (lsb(a) ? K : 0UL);
  
  state.left = 623;
  state.next = state.state;

#ifdef TEST
  printf("After reload\n");
  printstate();
#endif

  return 623; /* state.left */
}  

/* Return a random 32-bit integer.  Generate a pair of results on alternate
 * calls. (Allows dual-issue into >1 ALU.  The 'tempering' algorithm has little
 * or no parallelism in it for 1 point at a time.) */

unsigned long getmt(void)
{
  register unsigned long y0, y1;
  register int left = state.left;
  register unsigned long C1 = state.C1;
  register unsigned long C2 = state.C2;
  if (--left < 0) 
    left = reloadmt();

  if (!(left & 1)) { 
    state.left = left;
    return state.keep;
  } else {
    y0 = *(state.next++);
    y1 = *(state.next++);
    y0 ^= (y0 >> 11);
    y1 ^= (y1 >> 11);
    y0 ^= (y0 <<  7) & C1;
    y1 ^= (y1 <<  7) & C1;
    y0 ^= (y0 << 15) & C2;
    y1 ^= (y1 << 15) & C2;
    y0 ^= (y0 >> 18);
    y1 ^= (y1 >> 18);
    state.left = left;
    state.keep = y1;
    return y0;
  }
}
  
int main(int argc, char **argv) {
  int i, n;
  unsigned long k;

  if (argc > 1) {
    seedmt((unsigned long) atol(argv[1]));
  } else {
    seedmt(12345UL);
  }

#ifdef TEST
  printf("After seed\n");
  printstate();
  for (i=0; i<1250; i++) {
    k = getmt();
    printf("%6d:%08lx\n", i, k);
  }
#else
  while (1) {
    k = getmt();
    /* Write raw data to stdout */
    fwrite(&k, 1, 4, stdout);
  }
#endif
  return 0;
}
