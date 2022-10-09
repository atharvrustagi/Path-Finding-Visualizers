#include <iostream>
#include <time.h>


int main() {
    RAND_MAX;
    srand(time(0));
    for (int i = 0; i < 10; ++i)
        std::cout << rand() << ' ';
    std::cout << '\n';
    return 0;
}