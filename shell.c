// Daelyn Jones
// V00805241
// CSC 360 Assignment 1

#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This isn't the stone age, thus I opt to redefine 1 and 0 as true and false.
#define true 1
#define false 0
#define PATH_MAX 4096
#define HOSTNAME_MAX 1024
#define INPUT_SIZE_MAX 128

// This is from the lab, although I opt to use a pointer for the char array so
// that I can iterate through it in a way that I understand.
struct bg_pro {
  pid_t pid;
  char *command[INPUT_SIZE_MAX];
  struct bg_pro *next;
};

// We keep track of a head and tail so that we don't need to iterate through
// the list when we need to reach the tail.
struct bg_pro *bg_pro_list_head = NULL;
struct bg_pro *bg_pro_list_tail = NULL;

// We delete processes from the list when we find that it has completed. We
// don't want to have "zombie" processes in our list.
void delete_process_from_list(pid_t pid) {
  struct bg_pro *i = bg_pro_list_head;
  struct bg_pro *j = NULL;

  while (i != NULL) {
    if (i->pid == pid) {
      printf("%d: ", pid);
      int k;
      for (k = 0; k < INPUT_SIZE_MAX; k++) {
        if (i->command[k] != NULL) {
          printf("%s ", i->command[k]);
        }
      }
      printf("has terminated.\n");
      if (i == bg_pro_list_head) {
        bg_pro_list_head = bg_pro_list_head->next;
      } else {
        j->next = i->next;
      }
      free(i);
      return;
    }
    j = i;
    i = i->next;
  }
}

// When a new background process starts up we need to add it to the list.
// There are two circumstances here, where the process is the first one, or
// where we are appending it to the list.
void add_process_to_list(pid_t pid, char **user_input) {
  struct bg_pro *new_process = malloc(sizeof(struct bg_pro));

  int i;
  for (i = 0; i < INPUT_SIZE_MAX; i++) {
    if (user_input[i] != NULL) {
      new_process->command[i] = user_input[i + 1];
    }
  }

  new_process->pid = pid;
  new_process->next = NULL;

  if (bg_pro_list_head == NULL) {
    bg_pro_list_head = new_process;
    bg_pro_list_tail = new_process;
  } else {
    bg_pro_list_tail->next = new_process;
    bg_pro_list_tail = new_process;
  }
}

// We always want to check the background processes when we can to determine if
// we have any completed processes. This is necessary so that we don't leave
// dead processes in the list.
void check_background_processes() {
  pid_t pid;
  int status;

  while (true) {
    pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0 && WIFEXITED(status)) {
      delete_process_from_list(pid);
    } else {
      break;
    }
  }
}

// We use the suggested commands for the assignment to retrieve information to
// build the command line and print this at all times before any user input is
// to be received.
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

// We follow the standard way of tokenizing user input using readline().
// We need to tokenize so that we can react accordingly to the user input.
void get_user_input(char **user_input) {
  print_input_field();

  char *raw_user_input = readline(" ");

  char *current_token = strtok(raw_user_input, " ");

  int i;
  for (i = 0; i < INPUT_SIZE_MAX; i++) {
    user_input[i] = current_token;
    current_token = strtok(NULL, " ");
  }
}

// Simply use chdir() to change the user directory. Per instruction, getenv() is
// used to set the directory to home upon specific inputs.
void change_directories(char **user_input) {
  if (user_input[1] == NULL || !strcmp(user_input[1], "~")) {
    char *home = getenv("HOME");
    chdir(home);
  } else if (chdir(user_input[1]) != 0) {
    printf("error while changing directories\n");
  }
}

// When a user provides a built in shell command such as ls we use execvp() to
// execute that command. We fork so that this happens in a child process.
// Because this isn't meant to run in the background, we wait for the child
// process to complete before we continue.
void execute_builtin_commands(char **user_input) {
  pid_t pid = fork();
  pid_t wait_pid;
  int condition;

  if (pid == 0) {
    // this is in the child process
    int execution_condition = execvp(user_input[0], user_input);
    if (execution_condition == -1) {
      printf("invalid command\n");
      exit(1);
    }
  } else if (pid < 0) {
    printf("forking error\n");
  } else {
    // this is in the parent process
    do {
      wait_pid = waitpid(pid, &condition, WUNTRACED);
    } while (!WIFSIGNALED(condition) && !WIFEXITED(condition));
  }
}

// The difference between this and the built_in_commands method is that here we
// don't wait for the child process to finish executing. Instead, we allow
// background processes to continue running and check periodically if it has
// finished executing. We also make sure to add new processes to our global
// list. I found that I needed to add a small sleep so that we don't end up with
// a bugged input field.
void execute_background_task(char **user_input) {
  if (user_input[1] == NULL) {
    printf("invalid command\n");
    return;
  }

  pid_t pid = fork();

  if (pid == 0) {
    // this is in the child process
    int execution_condition = execvp(user_input[1], &user_input[1]);
    if (execution_condition == -1) {
      printf("invalid command\n");
      exit(1);
    }
  } else if (pid < 0) {
    printf("forking error\n");
  } else {
    // this is in the parent process
    printf("New background process: %d\n", pid);
    add_process_to_list(pid, user_input);
    usleep(5000);
  }
}

// When we receive the bglist command we simply want to iterate through our list
// and print accordingly.
void print_background_task_list() {
  struct bg_pro *i = bg_pro_list_head;

  int j = 0;
  while (i != NULL) {
    j++;
    printf("%d: ", i->pid);
    int k;
    for (k = 0; k < INPUT_SIZE_MAX; k++) {
      if (i->command[k] != NULL) {
        printf("%s ", i->command[k]);
      }
    }
    i = i->next;
    printf("\n");
  }
  printf("Total Background jobs: %d\n", j);
}

// Depending on the command given in the shell we want to react accordingly.
void execute_user_input(char **user_input) {
  // I added a quit command so that I could exit the shell easier.
  if (!strcmp(user_input[0], "quit")) {
    exit(0);
  } else if (!strcmp(user_input[0], "cd")) {
    change_directories(user_input);
  } else if (!strcmp(user_input[0], "bg")) {
    execute_background_task(user_input);
  } else if (!strcmp(user_input[0], "bglist")) {
    print_background_task_list();
  } else {
    execute_builtin_commands(user_input);
  }
}

// We need an infinite loop such that we always ask the user for new input.
// By using a char pointer we can maintain the user input in one location and
// simply pass it as a parameter, receive it as a double pointer, and edit it
// accordingly.
int main() {
  while (true) {
    char *user_input[INPUT_SIZE_MAX];

    get_user_input(user_input);
    execute_user_input(user_input);
    check_background_processes();
  }

  return true;
}
