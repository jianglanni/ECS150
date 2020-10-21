12345678901234567890123456789012345678901234567890123456789012345678901234567890
# Project 1: SIMPLE SHELL
## Parsing
First, we separate the output redirection (file name) with the rest of the cmd 
line by searching for ">" or ">>" in the original cmd line and replacing them
with '\0' if they exist, then we set another pointer that points to the file 
name. 

Next, we separate the commands connected by pipelining sign '|' by using 
**strtok()**.

At last, we parse each command and save the arguments in its array by using
**strtok()**. 

## Execute commands
We use an infinite loop to keep processing incoming command lines. Commands
except '*cd*' and '*exit*' will be executed in child processes due to potential
output redirection.

### Builtin commands
#### cd
We call **chdir()** to change working directory, if **chdir()** returns 1, then 
we call out an error. Otherwise, it should be successful. 

#### exit
We print "Bye..." on stderr and break the loop to terminate the shell itself.

#### pwd
We call **getcwd()** to get our current working directory and print it to the 
destination. 

#### sls
We use a *struct dirent* to open files in current directory and use *struct 
stat* to keep the files' names and sizes by calling **readdir()** and
**stat()**. Then we print them other than '.' and '..' to the destination. 

### General commands
