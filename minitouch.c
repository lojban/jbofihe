/*
  Program used to force the modification times of the files when compiling on
  DOS. (Workaround for what happens during summer time when non-volatile clock
  is kept on UTC - DJGPP make complains everything is in the future.)

  $Id$

  COPYRIGHT

*/

#include <sys/types.h>
#include <utime.h>

int main (int argc, char **argv)
{
  while (*argv) {
    if (utime(*argv, NULL) < 0) {
      perror("Couldn't set times for %s", *argv);
      exit(1);
    }
    ++argv;
  }

  return 0;
}
