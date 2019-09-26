#define OUTPUT printf("%d\n", i)
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int i = 0;
  OUTPUT;

  if (fork()) {
    // this is the parent process
    i += 3;
    OUTPUT;
  } else {
    // this is the child process
    i += 1;
    OUTPUT;
    return (0);
  }
}

// 0 is false, 1 is true
// 0 returns to the newly created child process
// 1 returns to parent, the value contains the process ID of the newly created
// child process.
