#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
int main (int argc, char* argv[]){
    assert (argc == 2);
    int N = atoi(argv[1]);
    for (int i = 0; i < N; i++){
        printf("x");
    }
}