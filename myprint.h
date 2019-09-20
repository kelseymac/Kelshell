#include <unistd.h>
#include <string.h>

#define STD_OUT 1

char new_line = '\n';

void myprint(char* to_print) {
    write(STD_OUT, to_print, strlen(to_print));
}

void myline(char* to_print) {
    myprint(to_print);
    write(STD_OUT, &new_line, 1);
}
