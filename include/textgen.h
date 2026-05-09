// Copyright 2026 b83
#pragma once
#include <string>
#include <deque>
#include <map>
#include <vector>

const int NPREF = 2;
const int MAXGEN = 1000;
const char NONWORD[] = "\n";

typedef std::deque<std::string> Prefix;

class TextGen {
 public:
  std::map<Prefix, std::vector<std::string>> statetab;
  void add(Prefix& pref, const std::string& word);
  void build(std::istream& in);
  void generate(std::ostream& out, int maxwords = MAXGEN);
};


