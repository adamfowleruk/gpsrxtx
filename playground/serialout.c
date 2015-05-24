#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringSerial.h>

int main() {
  int fd;
  if ((fd = serialOpen("/dev/ttyAMA0",9600))<0) {
    fprintf(stderr,"FAIL");
    return 1;
  }

  for (;;) {
    putchar(serialGetchar(fd));
    fflush (stdout) ;
  }
}
