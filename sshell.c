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
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

struct parsed_str {
	char *parsed[16];
};

void parse_filename(char *cmd, char **filename_finder, int *file_use) {
	*filename_finder = strchr(cmd, '>');
	if (*filename_finder) {
		++*file_use;
		**filename_finder = '\0';
		++*filename_finder;
		if (**filename_finder == '>') {
			++*file_use;
			**filename_finder = '\0';
			++*filename_finder;
		}
		while (**filename_finder == ' ') {
			**filename_finder = '\0';
			++*filename_finder;
		}
	}
}

void redirect_to_file(char *filename_finder, int file_use) {
	if (file_use == 1) {
		freopen(filename_finder, "w", stdout);
			
	}
	if (file_use == 2) {
		freopen(filename_finder, "a+", stdout);
	}
}

void parse_command(struct parsed_str *container, int *failed_parse, char *sep_cmd) {
	int parse_count = 0;
	char *token;
	token = strtok(sep_cmd, " ");
	while (parse_count <= 16 && token != NULL) {
		container -> parsed[parse_count] = token;
		token = strtok(NULL, " ");
		++parse_count;
	}
	if (parse_count > 16) {
		fprintf(stderr, "Error: too many process arguments\n");
		*failed_parse = 1;
		return;
	}
	container -> parsed[parse_count] = NULL;
	if (!parse_count) {
		fprintf(stderr, "Error: missing command\n");
		*failed_parse = 1;
		return;
	}
}

void list_files_with_size(){
        struct dirent *dirEnt;
        struct stat fStruct;
        DIR *dir = opendir(".");

        if(dir == NULL){
                fprintf(stderr,"Error: cannot open directory\n");
                return;
        }

        while((dirEnt = readdir(dir))!=NULL){
                stat(dirEnt->d_name,&fStruct);
                fprintf(stdout, "%s (%lld bytes)\n", dirEnt -> d_name, (long long)fStruct.st_size);
        }
}


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

                /* Parsing file name */
		int file_use = 0;  //0 for no files, 1 for overwriting, 2 for appending
		char *filename_finder = "";
		parse_filename(cmd, &filename_finder, &file_use);
		if (file_use) {
			if (!strlen(filename_finder)) {
				fprintf(stderr, "Error: no output file\n");
				continue;
			}

			char *pipe_sign_seeker = strchr(filename_finder, '|');
			if (pipe_sign_seeker) {
				fprintf(stderr, "Error: mislocated output redirection\n");
				continue;
			}

			int tester_fd = 0;
			tester_fd = open(filename_finder, O_CREAT | O_APPEND, 0644);
			if (tester_fd == -1) {
				fprintf(stderr, "Error: cannot open output file\n");
				continue;
			}
			close(tester_fd);
		}
		
		/* Separate pipelined commands */
		strcat(cmd, " "); 
		int detect_empty_cmd = 0;
		for (int i = 1; i < (int) strlen(cmd); ++i)
			if (cmd[i-1] == '|' && cmd[i] == '|') {
				detect_empty_cmd = 1;
				break;
			}
		if (detect_empty_cmd || cmd[0] == '|') {
			fprintf(stderr, "Error: missing command\n");
			continue;
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

		/* Parse each command */
		struct parsed_str container[4];
		int failed_parse = 0;
		for (int i = 0; i < cmd_count; ++i) {
			parse_command(&container[i], &failed_parse, sep_cmd[i]);
		}
		if (failed_parse)
			continue;

		/* exit and cd */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        fprintf(stderr, "+ completed '%s' [0]\n", saved_cmd);
                        break;
                }
		
                if (!strcmp(container[0].parsed[0], "cd")) {
                	if (chdir(container[0].parsed[1])) {
                		fprintf(stderr, "Error: cannot cd into directory\n");
                		fprintf(stderr, "+ completed '%s' [1]\n", saved_cmd);
                	} else {
                		fprintf(stderr, "+ completed '%s' [0]\n", saved_cmd);
                	}
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
				dup2(current_input_fd, STDIN_FILENO);
				if (current_command != cmd_count - 1)
					dup2(fd[1], STDOUT_FILENO);
				else {
					redirect_to_file(filename_finder, file_use);
				}
				/* pwd */
				if (!strcmp(container[current_command].parsed[0], "pwd")) {
                			char buf[512];
                			getcwd(buf, sizeof(buf));
                			fprintf(stdout, "%s\n", buf);
                			exit(0);
                		}
                		/* sls */
				if(!strcmp(container[current_command].parsed[0], "sls")) {
					list_files_with_size();
					exit(0);
				}
                		execvp(container[current_command].parsed[0], container[current_command].parsed);
			}
			if (!pid) {
				fprintf(stderr, "Error: command not found\n");
                		exit(1);
                	}
                	close(fd[1]);
			current_input_fd = fd[0];
			++current_command;
		}

		/* Finish */
	        fprintf(stderr, "+ completed '%s' ", saved_cmd);
	        for (int ii = 0; ii < cmd_count; ++ii)
	                fprintf(stderr, "[%d]", retval[ii]);
	        fprintf(stderr, "\n");
        }

        return EXIT_SUCCESS;
}
