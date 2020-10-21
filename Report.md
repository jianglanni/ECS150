# Project 1: SIMPLE SHELL
## Parsing
First, we separate the output redirection (file name) with the rest of the cmd 
line by searching for ">" or ">>" in the original cmd line and replacing them 
with '\0' if they exist, then we set another pointer that points to the file 
name. If this very pointer points to an empty string, we call a "no output file"
 error. If it points to a string that does not contain a legal file name, we 
call a "mislocated output redirection" error. 

Next, we separate the commands connected by pipelining sign '|' by using 
**strtok()**. 

At last, we parse each command and save the arguments in its array by using 
**strtok()**. If there are no arguments or too many arguments, we call an 
error. 

## Execute commands
We use an infinite loop to keep processing incoming command lines. Commands 
except '*cd*' and '*exit*' will be executed in child processes due to potential 
output redirection.

### Builtin commands
#### cd
We call **chdir()** to change working directory, if **chdir()** returns 1, then 
we call out an error. Otherwise, it should be successful. 

#### exit
We print "Bye..." on *stderr* and break the loop to terminate the shell itself.

#### pwd
We call **getcwd()** to get our current working directory and print it to the 
destination. 

#### sls
We use a *struct dirent* to open files in current directory and use *struct 
stat* to keep the files' names and sizes by calling **readdir()** and
**stat()**. Then we print data other than '.' and '..' to the destination. We 
learned the usages of the functions from Geeks for Geeks, at 
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/

### General commands
For commands other than builtin commands, we call **execvp()** in child process 
with an extra "NULL" at the end of the arguments vector. 

If a child process is still running the shell itself after calling 
**execvp()**, we call out a "command not found" error. 

### Pipelining
We have a loop to execute commands from the same line one by one. 

We will set a pipe for every execution. Each execution gets input from the 
previous cmd's pipe and output to its own pipe. Specially, The first cmd will 
get inputs from *STDIN*, and the last cmd will write to *STDOUT* or to the 
assigned output file. 

### Output redirection
We call **freopen()** in the child processes to do the output redirections, 
because this function will irreversibly redirect *STDOUT* to the files. 
For the cases we should overwrite the given file, we use argument "w" in 
**freopen()**. For the cases we append output to existing contents, we use
"a+" when calling the function. 
