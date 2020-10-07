/*
	PROJECT 1
	ECS150
	LAN JIANG, MANAN SHAH
	Note that we may convert all those built-in commands into function away from the loop.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

int main(void)
{
        char cmd[CMDLINE_MAX];

        while (1) {
                char *nl;
                int retval, status;

                /* Print prompt */
                printf("sshell@ucd$ ");
                fflush(stdout);
		memset(cmd, 0, sizeof(cmd));
                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';


                /* Parsing command */
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
		if (parse_count == 17) {
			fprintf(stderr, "Error: too many process arguments\n");
			continue;
		}
		parsed[parse_count] = NULL;
		
		
		/* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
                
                if (!strcmp(parsed[0], "pwd")) {
                	char buf[512];
                	getcwd(buf, sizeof(buf));
                	printf("%s\n", buf);
                	continue;
                }
                
                if (!strcmp(parsed[0], "cd")) {
                	int chd = chdir(parsed[1]);
                	if (chd)
                		fprintf(stderr, "Error: cannot cd into directory\n");
                	continue;
                }
		/* Execute */
                int pid = fork();
                if (pid) {
                	wait(&status);
                	retval = WEXITSTATUS(status);
		} else {
			execvp(parsed[0], parsed);
		}
		if (pid)
                	fprintf(stderr, "Completed '%s' [%d]\n", cmd, retval);
		else {
			exit(233);
		}
        }

        return EXIT_SUCCESS;
}
