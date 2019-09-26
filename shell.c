#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0
#define PATH_MAX 4096
#define HOSTNAME_MAX 1024
#define LOGIN_MAX 1024
#define PROMPT_MAX 6144

char *get_hostname() {
  char *hostname = malloc(HOSTNAME_MAX * sizeof(char));
  hostname[HOSTNAME_MAX - 1] = '\0';
  gethostname(hostname, HOSTNAME_MAX);

  return hostname;
}

char *get_path() {
  char cwd[PATH_MAX];
  char *path = malloc(PATH_MAX * sizeof(char));
  path[PATH_MAX - 1] = '\0';
  path = getcwd(cwd, sizeof(cwd));

  return path;
}

char *get_login() {
  char *login = malloc(LOGIN_MAX * sizeof(char));
  login[LOGIN_MAX - 1] = '\0';
  login = getlogin();

  return login;
}

char *get_prompt(char *hostname, char *path, char *login) {
  char *prompt = malloc(PROMPT_MAX * sizeof(char));
  prompt[PROMPT_MAX - 1] = '\0';

  snprintf(prompt, PROMPT_MAX, "SSI: %s@%s:  %s > ", login, hostname, path);

  return prompt;
}

char *build_input_field() {
  char *hostname = get_hostname();
  char *path = get_path();
  char *login = get_login();
  char *prompt = get_prompt(hostname, path, login);

  return prompt;
}

char *get_user_input() {
  char *input_field = build_input_field();

  char *user_input = readline(input_field);

  return user_input;
}

int main() {
  while (true) {
    char *user_input = get_user_input();
    printf("%s\n", user_input);
  }

  return 0;
}
