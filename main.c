#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 1024

int byte_cd(char **args);
int byte_help(char **args);
int byte_exit(char **args);
int byte_launch(char **args); // Function prototype


char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &byte_cd,
  &byte_help,
  &byte_exit
};

int byte_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int byte_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "byte: expected argument to \"cd\"\n");
  } else {
    if (SetCurrentDirectoryA(args[1]) == 0) {
      perror("byte");
    }
  }
  return 1;
}

int byte_help(char **args)
{
  int i;
  printf("Simple Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built-in commands:\n");

  for (i = 0; i < byte_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the help command for information on other programs.\n");
  return 1;
}

int byte_exit(char **args)
{
  return 0;
}

int byte_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < byte_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return byte_launch(args);
}

int byte_launch(char **args)
{
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(STARTUPINFOA));
  si.cb = sizeof(STARTUPINFOA);

  char command[MAX_COMMAND_LENGTH] = "";
  int i = 0;
  while (args[i] != NULL) {
    strcat(command, args[i]);
    strcat(command, " ");
    i++;
  }

  if (!CreateProcessA(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    fprintf(stderr, "Failed to create process: %s\n", command);
    return 1;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 1;
}

char *byte_read_line(void)
{
  char *line = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
  fgets(line, MAX_COMMAND_LENGTH, stdin);
  line[strcspn(line, "\n")] = '\0';
  return line;
}

#define BYTE_TOK_BUFSIZE 64
#define BYTE_TOK_DELIM " \t\r\n\a"

char **byte_split_line(char *line)
{
  int bufsize = BYTE_TOK_BUFSIZE;
  int position = 0;
  char **tokens = (char **)malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "byte: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, BYTE_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += BYTE_TOK_BUFSIZE;
      tokens = (char **)realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "byte: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, BYTE_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void byte_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = byte_read_line();
    args = byte_split_line(line);
    status = byte_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  byte_loop();
  return EXIT_SUCCESS;
}
