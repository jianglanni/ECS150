12345678901234567890123456789012345678901234567890123456789012345678901234567890
# Project 1: SIMPLE SHELL
## Parsing
First, we separate the output redirection (file name) with the rest of the cmd 
line by searching for ">" or ">>" in the original cmd line and replacing them
with '\0' if they exist, then we set another pointer that points to the file 
name. 

Next, we separate the commands connected by pipelining sign '|' by using 
**strtok**.

At last, we parse each command and save the arguments in its array by using
**strtok**. 
