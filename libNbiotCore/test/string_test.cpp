/* ========================================================================
 * LibNbiot: string_test.cpp
 *
 * Copyright (c) 2018, Edgar Hindemith, Yassine Amraue, Thorsten
 * Krautscheid, Kolja Vornholt, T-Systems International GmbH
 * contact: libnbiot@t-systems.com, opensource@telekom.de
 *
 * This file is distributed under the conditions of the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * For details see the file LICENSE at the toplevel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressed or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ========================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiotstring.h>
#include <nbiotstringlist.h>

using namespace nbiot;

TEST(StringConstructionTest, ConstructEmptyString) {
    string s;

    EXPECT_NE(nullptr, s.getData());
    EXPECT_TRUE(s.empty());
    EXPECT_LT(0, s.npos);
}

TEST(StringConstructionTest, ConstructStringFromCharArray) {
    const char *str = nullptr;

    string s1("Blah");
    string s2("Blah", 4);
    string s3("Blah", 10);
    string s4("Blah", 2);
    string s5("Blah", 0);
    string s6("Blah", -10);
    string s7("");
    string s8(str);

    EXPECT_STREQ(s1.getData(), "Blah");
    EXPECT_EQ(4, s1.size());

    EXPECT_STREQ(s2.getData(), "Blah");
    EXPECT_EQ(4, s1.size());

    EXPECT_STREQ(s3.getData(), "Blah");
    EXPECT_EQ(10, s3.size());

    EXPECT_STREQ(s4.getData(), "Bl");
    EXPECT_EQ(2, s4.size());

    EXPECT_STREQ(s5.getData(), "");
    EXPECT_EQ(0, s5.size());

    /* EXPECT_STREQ(s6.getData(), "");
    EXPECT_EQ(0, s6.size()); */

    EXPECT_EQ(s6.getData(), nullptr);
    EXPECT_EQ(0, s6.size());

    EXPECT_STREQ(s7.getData(), "");
    EXPECT_EQ(0, s7.size());

    EXPECT_STREQ(s8.getData(), "");
    EXPECT_EQ(0, s8.size());
}

TEST(StringConstructionTest, ConstructCopyOfString) {
    string s1("Blah");
    string s1_copy(s1);

    EXPECT_STREQ(s1.getData(), s1_copy.getData());
    EXPECT_EQ(s1.size(), s1_copy.size());
    EXPECT_NE(s1.getData(), s1_copy.getData());

    string s2("");
    string s2_copy(s2);

    EXPECT_STREQ(s2.getData(), s2_copy.getData());
    EXPECT_EQ(s2.size(), s2_copy.size());
    EXPECT_NE(s2.getData(), s2_copy.getData());
}

TEST(StringOperatorTest, AssignAnotherString) {
    string s1("Blah");
    string s1_copy;
    s1_copy = s1;

    EXPECT_STREQ(s1.getData(), s1_copy.getData());
    EXPECT_EQ(s1.size(), s1_copy.size());
    EXPECT_NE(&s1, &s1_copy);

    string s2("");
    string s2_copy;

    s2_copy = s2;

    EXPECT_STREQ(s2.getData(), s2_copy.getData());
    EXPECT_EQ(s2.size(), s2_copy.size());
    EXPECT_NE(s2.getData(), s2_copy.getData());
}

TEST(StringOperatorTest, AssignCharArray) {
    const char c1[] = "Blah";
    string s1_copy;
    s1_copy = c1;

    EXPECT_STREQ(c1, s1_copy.getData());
    EXPECT_EQ(4, s1_copy.size());

    const char c2[] = "";
    string s2_copy;

    s2_copy = c2;

    EXPECT_STREQ(c2, s2_copy.getData());
    EXPECT_EQ(0, s2_copy.size());
}

TEST(StringOperatorTest, AddAssignOtherString) {
    string s1("Blah");
    string s2("Blah");
    string s3("BlahBlah");

    s1 += s2;

    EXPECT_STREQ(s1.getData(), s3.getData());
    EXPECT_EQ(8, s1.size());
    EXPECT_NE(s1.getData(), s3.getData());
}

TEST(StringOperatorTest, AddAssignCharArray) {
    string s1("Blah");
    const char c1[] = "Blah";
    string s3("BlahBlah");

    s1 += c1;

    EXPECT_STREQ(s1.getData(), s3.getData());
    EXPECT_EQ(8, s1.size());
    EXPECT_NE(s1.getData(), s3.getData());
}

TEST(StringOperatorTest, AddAssignChar) {
    string s1("Blah");
    const char c1 = 'B';
    string s3("BlahB");

    s1 += c1;

    EXPECT_STREQ(s1.getData(), s3.getData());
    EXPECT_EQ(5, s1.size());
    EXPECT_NE(s1.getData(), s3.getData());
}

TEST(StringOperatorTest, EqualToOtherString) {
    string s1("Blah");
    string s2("Blah");
    string s3("NotBlah");

    EXPECT_EQ(s1, s2);
    EXPECT_FALSE(s1 == s3);
}

TEST(StringOperatorTest, EqualToCharArray) {
    string s1("Blah");
    const char c1[] = "Blah";
    const char c2[] = "NotBlah";

    EXPECT_EQ(s1, c1);
    EXPECT_FALSE(s1 == c2);
}

TEST(StringOperatorTest, BracketOperator) {
    string s1("Blah");

    EXPECT_EQ(s1[0], 'B');
    EXPECT_EQ(s1[1], 'l');
    EXPECT_EQ(s1[2], 'a');
    EXPECT_EQ(s1[3], 'h');

    EXPECT_EQ(s1[4], 0);
    EXPECT_EQ(s1[10], 0);
}

TEST(StringOperatorTest, BracketOperatorOnEmptyString) {
    string s1("");
    EXPECT_EQ(s1[0], 0);
    EXPECT_EQ(s1[10], 0);
}

TEST(StringTest, ObjectAtIndex) {
    string s1("Blah");

    EXPECT_EQ(s1.at(0), 'B');
    EXPECT_EQ(s1.at(1), 'l');
    EXPECT_EQ(s1.at(2), 'a');
    EXPECT_EQ(s1.at(3), 'h');

    EXPECT_EQ(s1.at(4), 0);
    EXPECT_EQ(s1.at(10), 0);
}

TEST(StringTest, ObjectAtIndexOnEmptyString) {
    string s1("");

    EXPECT_EQ(s1.at(0), 0);
    EXPECT_EQ(s1.at(10), 0);
}

TEST(StringTest, GetSize) {
    string s1("Blah");

    EXPECT_EQ(4, s1.size());
}

TEST(StringTest, GetSizeOfEmptyString) {
    string s1("");

    EXPECT_EQ(0, s1.size());
}

TEST(StringTest, IsEmpty) {
    string s1("");
    string s2("Blah");
    EXPECT_TRUE(s1.empty());

    EXPECT_FALSE(s2.empty());
}

TEST(StringTest, PushBackChar) {
    string s1("Blah");
    s1.push_back('c');
    EXPECT_EQ(s1[4], 'c');
}

TEST(StringTest, TakeFrontChar) {
    string s1("Blah");
    EXPECT_EQ(s1.take_front(), 'B');
}

TEST(StringTest, TakeFrontCharOfEmptyString) {
    string s1("");
    EXPECT_EQ(s1.take_front(), '\0');
}

TEST(StringTest, FindIndexOfCharArray) {
    string s1("Blah");

    EXPECT_EQ(0, s1.find("Bl"));
    EXPECT_EQ(2, s1.find("ah"));

    EXPECT_EQ(-1, s1.find("aX"));
}

TEST(StringTest, FindIndexOfCharArrayOnEmptyString) {
    string s1("");

    EXPECT_EQ(-1, s1.find("Xx"));
}

TEST(StringTest, FindIndexOfBiggerCharArrayThanString) {
    string s1("Blah");
    EXPECT_EQ(-1, s1.find("BlahBlah"));
}

TEST(StringTest, FindIndexOfChar) {
    string s1("Blah");

    EXPECT_EQ(0, s1.find('B'));
    EXPECT_EQ(1, s1.find('l'));
    EXPECT_EQ(2, s1.find('a'));
    EXPECT_EQ(3, s1.find('h'));

    EXPECT_EQ(-1, s1.find('X'));
}

TEST(StringTest, FindIndexOfCharOnEmptyString) {
    string s1("");

    EXPECT_EQ(0, s1.find(""));
}

TEST(StringTest, GetNormalSubstr) {
    string s1("BlahBlub");

    EXPECT_STREQ("Blah", (s1.substr(0, 4)).c_str());
    EXPECT_STREQ("BlahBlub", (s1.substr(0, 8)).c_str());
    EXPECT_STREQ("Blub", (s1.substr(4, 8)).c_str());
    EXPECT_STREQ("ahBlub", (s1.substr(2, 6)).c_str());
}

TEST(StringTest, GetEmptySubstr) {
    string s1("Blah");

    EXPECT_STREQ("", (s1.substr(0, 0)).c_str());

    string s2("");
    EXPECT_STREQ("", (s2.substr(0, 0)).c_str());
}

TEST(StringTest, GetSubstrNegativeStartPos) {
    string s1("Blah");

    EXPECT_STREQ("", (s1.substr(-2, 0)).c_str());
}

TEST(StringTest, GetBiggerSubstrThanActualString) {
    string s1("Blah");
    string s2 = (s1.substr(0, 10));
    EXPECT_STREQ("Blah", s2.c_str());
    EXPECT_EQ(4, s2.size());
}

TEST(StringTest, AppendCharArray) {
    string s1("Blah");
    const char c1[] = "Blub";
    s1.append(c1);
    s1.append("Bla");

    EXPECT_STREQ("BlahBlubBla", s1.c_str());
}

TEST(StringTest, AppendCharArrayToEmptyString) {
    string s1("");
    const char c1[] = "Blub";
    s1.append(c1);
    s1.append("Bla");

    EXPECT_STREQ("BlubBla", s1.c_str());
}

TEST(StringTest, AppendEmptyCharArray) {
    string s1("");
    const char c1[] = "";
    s1.append(c1);

    EXPECT_STREQ("", s1.c_str());

    string s2("Blah");
    s2.append("");

    EXPECT_STREQ("Blah", s2.c_str());

}

TEST(StringTest, AppendString) {
    string s1("Blah");
    string s2("Blub");
    s1.append(s2);

    EXPECT_STREQ("BlahBlub", s1.c_str());
}

TEST(StringTest, AppendStringToEmptyString) {
    string s1("");
    string s2("Blub");
    s1.append(s2);

    EXPECT_STREQ("Blub", s1.c_str());
}

TEST(StringTest, AppendEmptyString) {
    string s1("");
    string s2("");

    s1.append(s2);

    EXPECT_STREQ("", s1.c_str());

    string s3("Blah");
    s3.append(s2);

    EXPECT_STREQ("Blah", s3.c_str());
}

TEST(StringTest, SplitOnCharSeparator) {
    string s1("Blah,Blub");
    StringList sl;

    sl = s1.split(',', false);
    EXPECT_STREQ("Blah", sl[0].c_str());
    EXPECT_STREQ("Blub", sl[1].c_str());

    sl = s1.split(',', true);
    EXPECT_STREQ("Blah", sl[0].c_str());
    EXPECT_STREQ(",", sl[1].c_str());
    EXPECT_STREQ("Blub", sl[2].c_str());

    sl = s1.split('.', false);

    EXPECT_STREQ(sl[0].c_str(), "Blah,Blub");
}

TEST(StringTest, SplitOnCharArraySeparator) {
    string s1("Blah,Blub");
    StringList sl;

    sl = s1.split("a,B", false);
    EXPECT_STREQ("l", sl[0].c_str());
    EXPECT_STREQ("h", sl[1].c_str());
    EXPECT_STREQ("lub", sl[2].c_str());
    EXPECT_EQ(3, sl.count());

    sl = s1.split("a,B", true);
    EXPECT_STREQ("B", sl[0].c_str());
    EXPECT_STREQ("l", sl[1].c_str());
    EXPECT_STREQ("a", sl[2].c_str());
    EXPECT_STREQ("h", sl[3].c_str());
    EXPECT_STREQ(",", sl[4].c_str());
    EXPECT_STREQ("B", sl[5].c_str());
    EXPECT_STREQ("lub", sl[6].c_str());
    EXPECT_EQ(7, sl.count());

    sl = s1.split("xxx", false);

    EXPECT_STREQ(sl[0].c_str(), "Blah,Blub");
}

TEST(StringTest, SplitEmpty) {
    string s1("");
    StringList sl;

    sl = s1.split("");
    EXPECT_STREQ("", sl[0].c_str());
    EXPECT_EQ(1, sl.count());

    sl = s1.split('.');
    EXPECT_STREQ("", sl[0].c_str());
    EXPECT_EQ(1, sl.count());

    sl = s1.split(".");
    EXPECT_STREQ("", sl[0].c_str());
    EXPECT_EQ(1, sl.count());
}

//TEST(StringTest, SplitNullptrSeparator) {
//    string s1("Blah");
//    StringList sl;
//    char *sep = nullptr;
//
//    sl = s1.split(sep);
//}

TEST(StringTest, CompareNoCase) {
    string s1("HaLLo");
    EXPECT_TRUE(s1.compareNoCase("hallO"));
    EXPECT_FALSE(s1.compareNoCase("hallOX"));

    string s2("");
    EXPECT_TRUE(s2.compareNoCase(""));
    EXPECT_FALSE(s2.compareNoCase("notempty"));
}

//TEST(StringTest, CompareNoCaseToNullptr) {
//    string s1("HaLLo");
//    EXPECT_FALSE(s1.compareNoCase((const char*) nullptr));
//}

TEST(StringTest, CopyToCharArray) {
    string s1("Blah");
    char cp1[5];

    size_t rc = s1.copy(cp1, 4);

    EXPECT_STREQ(s1.c_str(), cp1);
    EXPECT_EQ(4, rc);

    // Empty string

    string s2("");
    char cp2[1];

    rc = s2.copy(cp2, 0);

    EXPECT_STREQ(s2.c_str(), cp2);
    EXPECT_EQ(0, rc);
}

TEST(StringTest, CopyToNullptr) {
    string s1("Blah");
    char *cp1 = nullptr;

    size_t rc = s1.copy(cp1, 0);

    EXPECT_EQ(0, rc);
}

TEST(StringTest, ConvertToHex) {
    string s1("Blah");
    string s2 = s1.toHex();

    EXPECT_STREQ("426C6168", s2.c_str());
}

TEST(StringTest, ConvertEmptyStringToHex) {
    string s3("");
    s3.toHex();

    EXPECT_STREQ("", s3.c_str());
}

TEST(StringTest, JoinStrings) {
    StringList sl;
    string s1("Blah");
    string s2("Blub");
    string s3("Foo");

    sl.append(s1);
    sl.append(s2);
    sl.append(s3);

    string s12 = string::join(sl, 0, 2);

    EXPECT_STREQ("BlahBlub", s12.c_str());

    s12 = string::join(sl);

    EXPECT_STREQ("BlahBlubFoo", s12.c_str());
}

TEST(StringTest, JoinStringsOutsideBounds) {
    StringList sl;
    string s1("Blah");
    string s2("Blub");

    sl.append(s1);
    sl.append(s2);

    string s12 = string::join(sl, 0, 5);

    EXPECT_STREQ("BlahBlub", s12.c_str());

    s12 = string::join(sl, 5, 5);

    EXPECT_STREQ("", s12.c_str());
}

//TEST(StringTest, JoinStringsNegativePos) {
//    StringList sl;
//    string s1("Blah");
//    string s2("Blub");
//
//    sl.append(s1);
//    sl.append(s2);
//
//    string s12 = string::join(sl, 0, -10);
//
//    EXPECT_STREQ("", s12.c_str());
//}

TEST(StringTest, JoinEmptyStrings) {
    StringList sl;
    string s1("");
    string s2("");

    sl.append(s1);
    sl.append(s2);

    string s12 = string::join(sl);

    EXPECT_STREQ("", s12.c_str());
}

TEST(StringTest, ConvertFromHex) {
    string s2 = string::fromHex("426C6168");

    EXPECT_STREQ("Blah", s2.c_str());
}

TEST(StringTest, ConvertFromHexEmptyString) {
    string s2 = string::fromHex("");

    EXPECT_STREQ("", s2.c_str());
}

//TEST(StringTest, ConvertFromHexNullptr) {
//    string s2 = string::fromHex(nullptr);
//
//    EXPECT_STREQ("", s2.c_str());
//}

TEST(StringTest, Printf) {
    string s1 = string::Printf("PrintfTest,%s,%d,%c,%f,%%", "OK", 10, 'c', 10.10);
    EXPECT_STREQ("PrintfTest,OK,10,c,10.100000,%", s1.c_str());
}

TEST(StringTest, PrintfEmptyString) {
    string s1 = string::Printf("");
    EXPECT_STREQ("", s1.c_str());
}

TEST(StringTest, Check7Bit) {

}

TEST(StringTest, Check7BitEmptyString) {

}
