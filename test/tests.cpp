// Copyright 2020 GHA Test Team
#include <gtest/gtest.h>
#include "textgen.h"
#include <sstream>
#include <set>
#include <string>

// 1: префикс правильного размера после инициализации
TEST(MarkovTest, PrefixInitSize) {
    Prefix p(NPREF, NONWORD);
    ASSERT_EQ((int)p.size(), NPREF);
}

// 2: после add префикс сдвигается
TEST(MarkovTest, PrefixShiftsAfterAdd) {
    TextGen tg;
    Prefix p = { "один", "два" };
    tg.add(p, "три");
    EXPECT_EQ(p[0], "два");
    EXPECT_EQ(p[1], "три");
}

// 3:суффикс записывается в таблицу
TEST(MarkovTest, SuffixStoredInTable) {
    TextGen tg;
    Prefix p = { "жил", "был" };
    tg.add(p, "царь");
    Prefix key = { "жил", "был" };
    ASSERT_FALSE(tg.statetab[key].empty());
    EXPECT_EQ(tg.statetab[key][0], "царь");
}

// 4: несколько суффиксов для одного префикса накапливаются
TEST(MarkovTest, MultiSuffixAccumulates) {
    TextGen tg;
    Prefix p = { "море", "синее" };
    tg.add(p, "шумит");
    Prefix p2 = { "море", "синее" };
    tg.add(p2, "плещет");
    Prefix p3 = { "море", "синее" };
    tg.add(p3, "бурлит");
    Prefix key = { "море", "синее" };
    EXPECT_EQ((int)tg.statetab[key].size(), 3);
}

// 5: единственный суффикс всегда выбирается одинаково
TEST(MarkovTest, SingleSuffixAlwaysChosen) {
    TextGen tg;
    std::istringstream in("а б в");
    tg.build(in);
    std::ostringstream out1, out2, out3;
    tg.generate(out1, 10);
    tg.generate(out2, 10);
    tg.generate(out3, 10);
    EXPECT_EQ(out1.str(), out2.str());
    EXPECT_EQ(out2.str(), out3.str());
}

// 6: build заполняет таблицу из потока
TEST(MarkovTest, BuildFillsTableFromStream) {
    TextGen tg;
    std::istringstream in("у лукоморья дуб зеленый златая цепь на дубе том");
    tg.build(in);
    EXPECT_GT((int)tg.statetab.size(), 0);
}

// 7:build на пустом потоке — только NONWORD записи
TEST(MarkovTest, BuildEmptyStream) {
    TextGen tg;
    std::istringstream in("");
    tg.build(in);
    for (auto& entry : tg.statetab)
        for (auto& s : entry.second)
            EXPECT_EQ(s, NONWORD);
}

// 8: генерация не превышает заданный лимит слов
TEST(MarkovTest, GenerateRespectsMaxWords) {
    TextGen tg;
    std::istringstream in("раз два три четыре пять шесть семь восемь девять десять");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 4);
    std::istringstream res(out.str());
    std::string w;
    int cnt = 0;
    while (res >> w) cnt++;
    EXPECT_LE(cnt, 4);
}

// 9: генерация выдаёт непустой результат
TEST(MarkovTest, GenerateNotEmpty) {
    TextGen tg;
    std::istringstream in("у лукоморья дуб зеленый златая цепь на дубе том");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 5);
    EXPECT_FALSE(out.str().empty());
}

// 10: сгенерированный текст содержит только слова из исходника
TEST(MarkovTest, GenerateOnlyKnownWords) {
    TextGen tg;
    std::istringstream in("кот ученый свои сказки говорил");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 20);
    std::set<std::string> known = { "кот", "ученый", "свои", "сказки", "говорил" };
    std::istringstream res(out.str());
    std::string w;
    while (res >> w)
        EXPECT_TRUE(known.count(w) > 0) << "Неизвестное слово: " << w;
}

// 11: два вызова add с одним префиксом дают два суффикса
TEST(MarkovTest, TwoAddsGiveTwoSuffixes) {
    TextGen tg;
    Prefix p1 = { "белый", "снег" };
    tg.add(p1, "падает");
    Prefix p2 = { "белый", "снег" };
    tg.add(p2, "кружится");
    Prefix key = { "белый", "снег" };
    EXPECT_EQ((int)tg.statetab[key].size(), 2);
}

// 12: после build разные префиксы дают разные записи
TEST(MarkovTest, DifferentPrefixesDifferentEntries) {
    TextGen tg;
    std::istringstream in("ветер север холод зима мороз лед");
    tg.build(in);
    EXPECT_GT((int)tg.statetab.size(), 1);
}

// 13: генерация с лимитом 0 даёт пустую строку
TEST(MarkovTest, GenerateZeroWords) {
    TextGen tg;
    std::istringstream in("раз два три четыре пять");
    tg.build(in);
    std::ostringstream out;
    tg.generate(out, 0);
    std::istringstream res(out.str());
    std::string w;
    int cnt = 0;
    while (res >> w) cnt++;
    EXPECT_EQ(cnt, 0);
}

// 14: build на длинном тексте создаёт большую таблицу
TEST(MarkovTest, BuildLargeInput) {
    TextGen tg;
    std::istringstream in("а б в г д е ж з и к л м н о п р с т у ф х ц ч ш щ");
    tg.build(in);
    EXPECT_GT((int)tg.statetab.size(), 5);
}

// 15:суфикс NONWORD добавляется в конец таблицы при build
TEST(MarkovTest, BuildAddsNonwordAtEnd) {
    TextGen tg;
    std::istringstream in("один два три");
    tg.build(in);
    bool found = false;
    for (auto& entry : tg.statetab)
        for (auto& s : entry.second)
            if (s == NONWORD) found = true;
    EXPECT_TRUE(found);
}