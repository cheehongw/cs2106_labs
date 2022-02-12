#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {

    printf("Be patient, the program will take around 7 seconds to run.\n");
    printf("At the end you can do \"cat results.out\" to see the result.\n");

    //
    // Add code here to pipe from ./slow 5 to ./talk and redirect
    // output of ./talk to results.out
    // I.e. your program should do the equivalent of ./slow 5 | talk > results.out
    // WITHOUT using | and > from the shell.
    //

    int p[2];

    if (pipe(p) < 2)
        perror("lab2p2f: ");

    if (fork() != 0) { //parent
        char buffer[4096];
        close(p[1]); //not writing
        wait(NULL); //wait for child to finish

        int fp_out = open("./result.out", O_CREAT | O_WRONLY);
        dup2(p[0], STDIN_FILENO);
        dup2(fp_out, STDOUT_FILENO);
        execlp("./talk", "talk", (char *) 0);
        close(p[0]);
        close(fp_out);

    } else {
        close(p[0]); //not reading
        dup2(p[1], STDOUT_FILENO); //open p[1] on stdout
        close(p[1]);  //close p[1]
        execlp("./slow", "slow", "5", NULL);
        close(STDOUT_FILENO); //close pipe

    }



}

