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

struct bg_pro {
  pid_t pid;
  char command;
  struct bg_pro *next;
};

struct bg_pro *bg_pro_list_head = NULL;
struct bg_pro *bg_pro_list_tail = NULL;

void delete_process_from_list(pid_t pid) {
  struct bg_pro *i = bg_pro_list_head;
  struct bg_pro *j = NULL;

  while (i != NULL) {
    if (i->pid == pid) {
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

void add_process_to_list(pid_t pid, char **user_input) {
  struct bg_pro *new_process = malloc(sizeof(struct bg_pro));

  new_process->pid = pid;
  new_process->command = *user_input[1];
  new_process->next = NULL;

  if (bg_pro_list_head == NULL) {
    bg_pro_list_head = new_process;
    bg_pro_list_tail = new_process;
  } else {
    bg_pro_list_tail->next = new_process;
    bg_pro_list_tail = new_process;
  }
}

void check_background_processes() {
  pid_t pid;
  int status;

  while (true) {
    pid = waitpid(-1, &status, WNOHANG);

    if (pid > 0 && WIFEXITED(status)) {
      printf("\nBackground process %d has terminated.\n", pid);
      delete_process_from_list(pid);
    } else {
      break;
    }
  }
}

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

  char *current_token = strtok(raw_user_input, " ");

  int i;
  for (i = 0; i < INPUT_SIZE_MAX; i++) {
    user_input[i] = current_token;
    current_token = strtok(NULL, " ");
  }
}

void change_directories(char **user_input) {
  if (user_input[1] == NULL || !strcmp(user_input[1], "~")) {
    char *home = getenv("HOME");
    chdir(home);
  } else if (chdir(user_input[1]) != 0) {
    printf("error while changing directories\n");
  }
}

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

void print_background_task_list() {
  struct bg_pro *i = bg_pro_list_head;

  int j = 0;
  while (i != NULL) {
    j++;
    printf("%d: %c\n", i->pid, i->command);
    i = i->next;
  }
  printf("Total Background jobs: %d\n", j);
}

void execute_user_input(char **user_input) {
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

int main() {
  while (true) {
    char *user_input[INPUT_SIZE_MAX];

    get_user_input(user_input);
    execute_user_input(user_input);
    check_background_processes();
  }

  return true;
}
