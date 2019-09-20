#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include "myprint.h"
#include <stdio.h>

#define STD_IN 0
char pwd[1024];
char input[1024];

char* prompt = "Kelsh: ";
char* args[1025];
char* cd_cmd = "cd";
char* exit_cmd = "exit";
char* pwd_cmd = "pwd";
char* debug_cmd = "debugargs";
int arg_count = 0;
int overwrite = 0;
char* in_to_program;
char* out_to_file;

void pwd_print() {
    getcwd(pwd, 1024);
    myprint(prompt);
    myprint(pwd);
    myprint("$ ");
}

int find_nonspace(char* string) {
    int i = 0;
    while (string[i] !='\0') {
        if (string[i] != ' ' && string[i] != '>' && string[i] != '<') {
            return i;
        }
        i++;
    }
    return -1;
}


void check_arrows() {
    int left_arrow = 0;
    int right_arrow = 0;
    //finds indexes of arrows
    for (int i=0; i < strlen(input); i++) {
        if (input[i] == '<') {
            left_arrow = i;
        }

        else if (input[i] == '>') {
            right_arrow = i;
            if (input[++i] == '>') {
                overwrite = 0;
            }
            else {
                overwrite = 1;
            }
        }
    }

    if (left_arrow == 0 && right_arrow ==0)
        return;
    else if (left_arrow == 0) {
        out_to_file = (input+right_arrow+find_nonspace(input+right_arrow));
        input[right_arrow] = '\0';
    }
    else if (right_arrow == 0) {
        in_to_program = (input+left_arrow+find_nonspace(input+left_arrow));
        input[left_arrow] = '\0';
    }

    else {
        if (left_arrow > right_arrow) {
            out_to_file = (input+right_arrow+find_nonspace(input+right_arrow));
            in_to_program = (input+left_arrow+find_nonspace(input+left_arrow));
        }
        else {
            in_to_program = (input+left_arrow+find_nonspace(input+left_arrow));
            out_to_file = (input+right_arrow+find_nonspace(input+right_arrow));
        }
        input[left_arrow] = '\0';
        input[right_arrow] = '\0';
    }

    if (in_to_program != NULL) {
        for (int i=0; i < strlen(in_to_program); i++) {
            if (in_to_program[i] == ' ') {
                in_to_program[i] = '\0';
            }
        }
    }

    if (out_to_file != NULL) {
        for (int i=0; i < strlen(out_to_file); i++) {
            if (out_to_file[i] == ' ') {
                out_to_file[i] = '\0';
            }
        }
    }
}




void find_args() {
    arg_count = 0;
    char* pointer = input;
    while (*pointer != '\0') {
        if (*pointer == ' ') {
            *(pointer++) = '\0';
            continue;
        }
        args[arg_count++] = pointer;
        while (*pointer != ' ' && *pointer != '\0') {
            pointer++;
        }
    }
    args[arg_count++] = NULL;
}

void debugargs_func() {
    myprint("cmd: [");
    myprint(args[0]);
    myprint("]");
    myline(" ");

    for (int i = 1; i < arg_count; i++) {
        if (args[i] == NULL) {
            myline("args: [(null)]");
        }

        else {
            myprint("arg: [");
            myprint(args[i]);
            myprint("]");
            myline(" ");
        }
    }
}

void main() {
    myline("******THIS IS KELSEY'S SHELL....NOT BASH!!!!******");

    while (1) {
        pwd_print();
        int len = read(0, input, 1024);  
        input[len-1] = '\0';
        check_arrows();
        find_args();

        if (strcmp(input, cd_cmd) == 0) {
            if (chdir(args[1]) == 0) {}
            else {
                myline(strerror(errno));
            }
        }

        else if (strcmp(input, pwd_cmd) == 0) { 
            pwd_print();
            myline(" ");
        }

        else if (strcmp(input, exit_cmd) == 0) {
            break;
        }

        else if (strcmp(input, debug_cmd) == 0) {
            debugargs_func();
        }

        else {
            int pid = fork();
            if (pid == 0) {
                if (in_to_program != NULL) {
                    close(STD_IN);
                    open(in_to_program, O_RDONLY);
                }
                if (out_to_file != NULL) {
                    close(STD_OUT);
                    if (overwrite == 1) {
                        open(out_to_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
                    }
                    else {
                        open(out_to_file, O_WRONLY|O_CREAT|O_APPEND, S_IRWXU);
                    }
                }
                execvp(args[0], args);
                myline("Error: Something went wrong. Try Again.");
                exit(1);
            }
            else {
                wait(NULL);
                out_to_file = NULL;
                in_to_program = NULL;
            }
        }
    }
}
