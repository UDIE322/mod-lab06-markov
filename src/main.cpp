// Copyright 2026 b83
#include <fstream>
#include <iostream>
#include "textgen.h"


int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Cannot open input.txt" << std::endl;
        return 1;
    }

    TextGen gen;
    gen.build(in);

    std::ofstream out("result/gen.txt");
    if (!out) {
        std::cerr << "Cannot open result/gen.txt for writing" << std::endl;
        return 1;
    }

    gen.generate(out);
    std::cout << "Done! See result/gen.txt" << std::endl;

    return 0;
}
