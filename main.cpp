#include <iostream>
#include "unique.hpp"

// Test class
int main() {
    int* n = new int(42);
    UniquePtr<int> p2(n);
    std::cout << "p2 pointer value : " << *p2 << std::endl;

    //UniquePtr<Test> p(new Test());
    //p->hello();
}