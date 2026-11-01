#include <pthread.h>
#include <math.h>


int eh_primo(int entrada) {
    for (int divisor = 2; divisor < sqrt(entrada); divisor++) {
        if (entrada % divisor == 0) {
            return 1;
        }
    }
    return 0;
}


int main() {
    return 0;
}