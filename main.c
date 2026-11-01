#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int eh_primo(int entrada) {
    if (entrada < 2) {
        return 0;
    }
    if (entrada == 2) {
        return 1;
    }

    for (int divisor = 2; divisor <= ceil(sqrt(entrada)); divisor++) {
        if (entrada % divisor == 0) {
            return 0;
        }
    }
    return 1;
}


int gerar_numero() {
    return rand() % 32000;
}


int main() {
    srand(1);

    for (int i = 0; i < 30; i++)
    {
        printf("%d:", i);
        if (eh_primo(i) == 0) {
            printf(" não");
        }
        printf(" é primo\n");
    }
    
    return 0;
}