// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include <set>
#include <sstream>
#include <string>
#include "textgen.h"

TEST(MarkovTest, PrefixInitSize) {
    Prefix p(NPREF, NONWORD);
    ASSERT_EQ(static_cast<int>(p.size()), NPREF);
}

TEST(MarkovTest, PrefixShiftsAfterAdd) {
    TextGen tg;
    Prefix p = { "one", "two" };
    tg.add(p, "three");
    EXPECT_EQ(p[0], "two");
    EXPECT_EQ(p[1], "three");
}

TEST(MarkovTest, SuffixStoredInTable) {
    TextGen tg;
    Prefix p = { "lived", "was" };
    tg.add(p, "king");
    Prefix key = { "lived", "was" };
    ASSERT_FALSE(tg.statetab[key].empty());
    EXPECT_EQ(tg.statetab[key][0], "king");
}

TEST(MarkovTest, MultiSuffixAccumulates) {
    TextGen tg;
    Prefix p = { "sea", "blue" };
    tg.add(p, "roars");
    Prefix p2 = { "sea", "blue" };
    tg.add(p2, "splashes");
    Prefix p3 = { "sea", "blue" };
    tg.add(p3, "flows");
    Prefix key = { "sea", "blue" };
    EXPECT_EQ(static_cast<int>(tg.statetab[key].size()), 3);
}

TEST(MarkovTest, SingleSuffixAlwaysChosen) {
    TextGen tg;
    std::istringstream in("a b c");
    tg.build(in);
    std::ostringstream out1, out2, out3;
    tg.generate(out1, 10);
    tg.generate(out2, 10);
    tg.generate(out3, 10);
    EXPECT_EQ(out1.str(), out2.str());
    EXPECT_EQ(out2.str(), out3.str());
}

TEST(MarkovTest, BuildFillsTableFromStream) {
    TextGen tg;
    std::istringstream in("the cat sat on the mat");
    tg.build(in);
    EXPECT_GT(static_cast<int>(tg.statetab.size()), 0);
}

TEST(MarkovTest, BuildEmptyStream) {
    TextGen tg;
    std::istringstream in("");
    tg.build(in);
    for (auto& entry : tg.statetab)
        for (auto& s : entry.second)
            EXPECT_EQ(s, std::string(NONWORD));
}

TEST(MarkovTest, GenerateRespectsMaxWords) {
    TextGen tg;
    std::istringstream in("one two three four five six seven eight nine ten");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 4);
    std::istringstream res(out.str());
    std::string w;
    int cnt = 0;
    while (res >> w) cnt++;
    EXPECT_LE(cnt, 4);
}

TEST(MarkovTest, GenerateNotEmpty) {
    TextGen tg;
    std::istringstream in("the cat sat on the mat by the door");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 5);
    EXPECT_FALSE(out.str().empty());
}

TEST(MarkovTest, GenerateOnlyKnownWords) {
    TextGen tg;
    std::istringstream in("cat learned his tales told");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 20);
    std::set<std::string> known = { "cat", "learned", "his", "tales", "told" };
    std::istringstream res(out.str());
    std::string w;
    while (res >> w)
        EXPECT_TRUE(known.count(w) > 0) << "Unknown word: " << w;
}

TEST(MarkovTest, TwoAddsGiveTwoSuffixes) {
    TextGen tg;
    Prefix p1 = { "white", "snow" };
    tg.add(p1, "falls");
    Prefix p2 = { "white", "snow" };
    tg.add(p2, "spins");
    Prefix key = { "white", "snow" };
    EXPECT_EQ(static_cast<int>(tg.statetab[key].size()), 2);
}

TEST(MarkovTest, DifferentPrefixesDifferentEntries) {
    TextGen tg;
    std::istringstream in("wind north cold winter frost ice");
    tg.build(in);
    EXPECT_GT(static_cast<int>(tg.statetab.size()), 1);
}

TEST(MarkovTest, GenerateZeroWords) {
    TextGen tg;
    std::istringstream in("one two three four five");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 0);
    std::istringstream res(out.str());
    std::string w;
    int cnt = 0;
    while (res >> w) cnt++;
    EXPECT_EQ(cnt, 0);
}

TEST(MarkovTest, BuildLargeInput) {
    TextGen tg;
    std::istringstream in("a b c d e f g h i j k l m n o p");
    tg.build(in);
    EXPECT_GT(static_cast<int>(tg.statetab.size()), 5);
}

TEST(MarkovTest, BuildAddsNonwordAtEnd) {
    TextGen tg;
    std::istringstream in("one two three");
    tg.build(in);
    bool found = false;
    for (auto& entry : tg.statetab)
        for (auto& s : entry.second)
            if (s == std::string(NONWORD)) found = true;
    EXPECT_TRUE(found);
}
