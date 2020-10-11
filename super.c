#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
	char cmd[] = "I wonder if U know        >    out.txt";
	printf("Original string: %s\n", cmd);
	char *filename_finder;
	filename_finder = strchr(cmd, '>');
	if (filename_finder) {
		*filename_finder = '\0';
		++filename_finder;
		while (*filename_finder == ' ') {
			*filename_finder = '\0';
			++filename_finder;
		}
	}
	printf("Parsed File name: %s\n", filename_finder);
	
	char *parsed[16];
	char *token;
	int parse_count = 0;
	
	token = strtok(cmd, " ");
	while (parse_count <= 17 && token != NULL) {
		parsed[parse_count] = token;
		token = strtok(NULL, " ");
		++parse_count;
	}
	if (parse_count == 17) {
		fprintf(stderr, "Error: too many process arguments\n");
	}
	parsed[parse_count] = NULL;
	printf("The last argv of cmd: %s\n", parsed[parse_count - 1]);
	return 0;
}
