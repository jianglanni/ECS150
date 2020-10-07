#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {

	char cmd[512] = "echo   We  did  it, Manan!";
	//printf("%s\n", cmd);
	char *parsed[16];
	char *token;
	int parse_count = 0;
	
	token = strtok(cmd, " ");
	while (parse_count <= 17 && token != NULL) {
		//printf("%s\n", token);
		parsed[parse_count] = token;
		token = strtok(NULL, " ");
		++parse_count;
	}
	
	parsed[parse_count] = NULL;
	
	int pid = fork(), status;
	if (pid) {
		wait(&status);
		printf("exit with value %d.\n", WEXITSTATUS(status));
	} else {
		//freopen("out.txt", "w", stdout);
		execvp(parsed[0], parsed);
	}
	
	return 0;
}
