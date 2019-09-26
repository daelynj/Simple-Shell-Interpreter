#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0
#define PATH_MAX 4096
#define HOSTNAME_MAX 1024
#define INPUT_SIZE_MAX 128

void print_input_field() {
  char hostname[HOSTNAME_MAX];
  gethostname(hostname, HOSTNAME_MAX);

  char cwd[PATH_MAX];
  char *path = getcwd(cwd, sizeof(cwd));

  char *login = getlogin();

  printf("SSI: %s@", login);
  printf("%s  ", hostname);
  printf("%s >", path);
}

void get_user_input(char **user_input) {
  print_input_field();

  char *raw_user_input = readline(" ");

  char *token = strtok(raw_user_input, " ");

  int i;
  for (i = 0; i < INPUT_SIZE_MAX; i++) {
    user_input[i] = token;
    token = strtok(NULL, " ");
  }

  if (!strcmp(user_input[0], "quit")) {
    exit(false);
  }
}

void execute_user_input(char **user_input) {
  if (!strcmp(user_input[0], "cd")) {
    printf("%s", user_input[0]);
  } else {
    pid_t pid = fork();

    if (pid == 0) {
      // this is in the child process
      int execution_condition = execvp(user_input[0], user_input);
      if (execution_condition == -1) {
        printf("invalid command\n");
      }
    } else if (pid < 0) {
      printf("forking error");
    } else {
      // this is in the parent process
      pid_t wait_pid;
      int condition;

      do {
        wait_pid = waitpid(pid, &condition, WUNTRACED);
      } while (!WIFSIGNALED(condition) && !WIFEXITED(condition));
    }
  }
}

int main() {
  while (true) {
    char *user_input[INPUT_SIZE_MAX];

    get_user_input(user_input);
    execute_user_input(user_input);
  }

  return true;
}

// PART 2

// if strcmp(argv[0], "cd") == 0 PART 2
// else PART 1

// if argv[1] == NULL || strcmp(argv[1], "~") == 0 ---> go to HOME (env)
// else go to argv[1]

// "env | grep HOME" in BASH to verify


// PART 3

// strcmp(argv[0], "bg") == 0
// new argv: remove "bg"
// fork:
//   child: execvp(newArgv[0], newArgv)
//     parent:
//       if # of bg_process == 0
//        root = bg_pro1
//       else { append bg_proNext }

// should use a linked list  for storing bg process info, with add/delete etc.

// struct bg_pro {
//   pid t pid;
//   char command[1024];
//   struct bg_pro* next; // pointer to the next structure, or a \0  
// }
// root should point to a structure.. should point to a structure..  where each
// struct is a process

// check if child terminates
// if # of bg_process > 0
//   pid_t ter = waitpid(0, NULL, WNOHANG);
//   while ter > 0 {
//     if root = pid -> ter
//       print root -> pid & root -> command & "has terminated"
//       root = root -> next
//     else
//       loop until we find cur -> next -> pid == ter
//       print cur -> next -> pid & cur -> next -> command & "has terminated"
//       cur -> next = cur -> next -> next
//     ter = waitpid(0, NULL, WNOHANG);
//   }

// bglist
// strcmp(argv[0], "bglist") == 0;
// print info in the linked list and number of bg processes

// bg ls -l == ls -l &
//   