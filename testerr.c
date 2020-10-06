#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
	char buf[80];
	getcwd(buf, sizeof(buf));
	printf("%s\n", buf);
	chdir("../");
	memset(buf, 0, sizeof(buf));
	getcwd(buf, sizeof(buf));
	printf("%s\n", buf);
	return 0;
}
