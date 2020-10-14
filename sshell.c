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

struct parsed_str {
	char *parsed[16];
};

int main(void)
{
        char cmd[CMDLINE_MAX];

        while (1) {
                char *nl;
                int retval[4], status;

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

		char saved_cmd[CMDLINE_MAX];
		strcpy(saved_cmd, cmd);
		//strcat(cmd, " ");
                /* Parsing command */
                
		int file_use = 0;  //0 for no files, 1 for overwriting, 2 for appending
		char *filename_finder;
		filename_finder = strchr(cmd, '>');
		if (filename_finder) {
			++file_use;
			*filename_finder = '\0';
			++filename_finder;
			if (*filename_finder == '>') {
				++file_use;
				*filename_finder = '\0';
				++filename_finder;
			}
			while (*filename_finder == ' ') {
				*filename_finder = '\0';
				++filename_finder;
			}
		}
		if (file_use) {
			if (!strlen(filename_finder)) {
				fprintf(stderr, "Error: no output file\n");
				continue;
			}	
		}
		
		char *sep_cmd[4];
		char *token;
		int cmd_count = 0;
		token = strtok(cmd, "|");
		while (cmd_count <= 4 && token != NULL) {
			sep_cmd[cmd_count] = token;
			token = strtok(NULL, "|");
			++cmd_count;
		}
		if (token != NULL && cmd_count > 4) {
			fprintf(stderr, "Error: too many pipe signs\n");
			continue;
		}
		struct parsed_str container[4];
		int failed_parse = 0;
		
		for (int ii = 0; ii < cmd_count; ++ii) {
			int parse_count = 0;
			token = strtok(sep_cmd[ii], " ");
			while (parse_count <= 16 && token != NULL) {
				container[ii].parsed[parse_count] = token;
				token = strtok(NULL, " ");
				++parse_count;
			}
			if (parse_count == 16) {
				fprintf(stderr, "Error: too many process arguments\n");
				failed_parse = 1;
				break;
			}
			container[ii].parsed[parse_count] = NULL;
			if (!parse_count) {
				fprintf(stderr, "Error: missing command\n");
				failed_parse = 1;
				break;
			}
			
		}
		
		if (failed_parse)
			continue;
		/* Builtin command */
		
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }
		
                if (!strcmp(container[0].parsed[0], "cd")) {
                	int chd = chdir(container[0].parsed[1]);
                	if (chd)
                		fprintf(stderr, "Error: cannot cd into directory\n");
                	continue;
                }
                
		/* Execute */
		int current_command = 0;
		int current_input_fd = STDIN_FILENO;
		while (current_command < cmd_count) {
			int fd[2];
			pipe(fd);
                	int pid = fork();
                	if (pid) {
                		wait(&status);
                		retval[current_command] = WEXITSTATUS(status);
			} else {
				/*
				if (file_use == 1) {
					freopen(filename_finder, "w", stdout);
				}
				if (file_use == 2) {
					freopen(filename_finder, "a+", stdout);
				}
				*/
				dup2(current_input_fd, STDIN_FILENO);
				if (current_command != cmd_count - 1)
					dup2(fd[1], STDOUT_FILENO);
				else {
					
					if (file_use == 1) {
						freopen(filename_finder, "w", stdout);
					}
					if (file_use == 2) {
						freopen(filename_finder, "a+", stdout);
					}
				}
				if (!strcmp(container[current_command].parsed[0], "pwd")) {
                			char buf[512];
                			getcwd(buf, sizeof(buf));
                			fprintf(stdout, "%s\n", buf);
                			exit(0);
                		}
                		execvp(container[current_command].parsed[0], container[current_command].parsed);
			}
			if (!pid) {
                		exit(233);
                	}
			current_input_fd = fd[0];
			++current_command;
		}
		
		
		
		/* Finish */
	        fprintf(stderr, "Completed '%s' ", saved_cmd);
	        for (int ii = 0; ii < cmd_count; ++ii)
	                fprintf(stderr, "[%d]", retval[ii]);
	        fprintf(stderr, "\n");
        }

        return EXIT_SUCCESS;
}
