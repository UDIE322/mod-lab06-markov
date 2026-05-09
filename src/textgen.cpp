// Copyright 2026 b83
#include "textgen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <random>

void TextGen::add(Prefix& pref, const std::string& word) {
    statetab[pref].push_back(word);
    if (static_cast<int>(pref.size()) >= NPREF) {
        pref.pop_front();
    }
    pref.push_back(word);
}

void TextGen::build(std::istream& in) {
    srand(static_cast<unsigned>(time(nullptr)));
    Prefix pref;
    for (int i = 0; i < NPREF; i++)
        pref.push_back(NONWORD);

    std::string word;
    while (in >> word)
        add(pref, word);

    add(pref, NONWORD);
}

void TextGen::generate(std::ostream& out, int maxwords) {
    Prefix pref;
    for (int i = 0; i < NPREF; i++)
        pref.push_back(NONWORD);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1000);

    for (int i = 0; i < maxwords; i++) {
        auto& suffixes = statetab[pref];
        if (suffixes.empty()) break;

        int r = dis(gen) % suffixes.size();

        std::string word = suffixes[r];

        if (word == NONWORD) break;

        out << word << " ";

        pref.pop_front();
        pref.push_back(word);
    }
    out << std::endl;
}
