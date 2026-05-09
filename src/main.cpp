#include "textgen.h"
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Cannot open input.txt" << std::endl;
        return 1;
    }

    TextGen gen;
    gen.build(in);

    std::ofstream out("result/gen.txt");
    gen.generate(out);

    std::cout << "Done! See result/gen.txt" << std::endl;
    return 0;
}