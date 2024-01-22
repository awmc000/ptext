#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <commands.h>
#include <ctype.h>
#include <files.h>
#include <stdafx.h>
#include <userfuncs.h>
#include <utils.h>

commandBind commands[] = {{"set-status", &user_setStatus}, {NULL, NULL}};
char *getFunctionName(char *command);

void doLine(char *line) {
  int i;
  line = rtrim(line);
  char *fname = getFunctionName(line);
  for (i = 0; commands[i].name != NULL; i++) {
    if (strcmp(commands[i].name, fname) == 0) {
      break;
    }
  }

  if (commands[i].name == NULL) {
    setStatusMessage("Function %s doesn't exist", fname);
    return;
  }

  args = &fname[strlen(fname) + 1];
  (void)(*commands[i].func)();
}

void execCommand(void) {
  char *command = getPrompt(":%s", NULL);
  doLine(command);
  free(command);
}

void doFile(char *filename) {
  FILE *fp = fopen(filename, "r");
  char *line = NULL;
  int len = 0;
  size_t size = 0;
  while ((len = (int)getline(&line, &size, fp)) != -1) {
    doLine(line);
  }
  fclose(fp);
  free(line);
}

char *getFunctionName(char *command) {
  command = &command[0];
  char *fname = token(command);
  return fname;
}
