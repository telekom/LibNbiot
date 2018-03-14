//
// Created by Yassine Amraue on 09.02.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nbiotlist.h>

using namespace nbiot;

typedef int T;

TEST(NbiotListTest, ContructEmptyList) {
    list<T> *l = new list<T>();

    ASSERT_NE(nullptr, l);
    EXPECT_EQ(0, l->count());
}

TEST(NbiotListTest, ContructEmptyListWithInvalidItem) {
    list<T> *l = new list<T>(-1);

    ASSERT_NE(nullptr, l);
    EXPECT_EQ(0, l->count());
}

TEST(NbiotListTest, AppendItem) {
    list<T> l;

    l.append(1);
    l.append(10);
    l.append(100);

    EXPECT_EQ(1, l[0]);
    EXPECT_EQ(10, l[1]);
    EXPECT_EQ(100, l[2]);
}

TEST(NbiotListTest, AppendNullptr) {
    list<T *> l;

    l.append(nullptr);
    EXPECT_EQ(1, l.count());
}

TEST(NbiotListTest, ConstructCopyOfAList) {
    list<T> l;

    l.append(1);
    l.append(10);
    l.append(100);

    list<T> l_copy(l);

    EXPECT_EQ(l[0], l_copy[0]);
    EXPECT_EQ(l[1], l_copy[1]);
    EXPECT_EQ(l[2], l_copy[2]);
}

TEST(NbiotListTest, AssignmentOperator) {
    list<T> l;

    l.append(1);
    l.append(10);
    l.append(100);

    list<T> l2;
    l2 = l;

    EXPECT_EQ(l[0], l2[0]);
    EXPECT_EQ(l[1], l2[1]);
    EXPECT_EQ(l[2], l2[2]);
}

TEST(NbiotListTest, GetItemAtValidIndex) {
    list<T> l;

    l.append(1);
    l.append(10);
    l.append(100);

    EXPECT_EQ(1, l.at(0));
    EXPECT_EQ(10, l.at(1));
    EXPECT_EQ(100, l.at(2));
}

TEST(NbiotListTest, GetItemAtIndexOutOfBounds) {
    list<T> l(-1);

    l.append(1);

    EXPECT_EQ(-1, l.at(1));
}

TEST(NbiotListTest, GetItemAtNegtiveIndex) {
    list<T> l(-1);

    l.append(1);

    EXPECT_EQ(-1, l.at(-1));
}

TEST(NbiotListTest, GetItemBracketOperatorValidIndex) {
    list<T> l;

    l.append(1);
    l.append(10);
    l.append(100);

    EXPECT_EQ(1, l[0]);
    EXPECT_EQ(10, l[1]);
    EXPECT_EQ(100, l[2]);
}

TEST(NbiotListTest, GetItemBracketOperatorIndexOutOfBounds) {
    list<T> l(-1);

    l.append(1);

    EXPECT_EQ(-1, l[1]);
}

TEST(NbiotListTest, GetItemBracketOperatorNegtiveIndex) {
    list<T> l(-1);

    l.append(1);

    EXPECT_EQ(-1, l[-1]);
}

TEST(NbiotListTest, GetItemRemoveValidIndex) {
//    list<T> l;
//
//    l.append(0);
//    l.append(1);
//    l.append(2);
//    l.append(3);
//    l.append(4);
//
//    l.remove(1);
//
//    EXPECT_EQ(0, l[0]);
//    EXPECT_EQ(2, l[1]);
//    EXPECT_EQ(3, l[2]);
//    EXPECT_EQ(4, l[3]);
}

TEST(NbiotListTest, GetItemRemoveIndexOutOfBounds) {
    list<T> l;

    l.append(0);
    l.append(1);
    l.append(2);
    l.append(3);

    l.remove(10);

    EXPECT_EQ(0, l[0]);
    EXPECT_EQ(1, l[1]);
    EXPECT_EQ(2, l[2]);
    EXPECT_EQ(3, l[3]);
}

TEST(NbiotListTest, GetItemRemoveNegtiveIndex) {
    list<T> l;

    l.append(0);
    l.append(1);
    l.append(2);
    l.append(3);

    l.remove(-10);

    EXPECT_EQ(0, l[0]);
    EXPECT_EQ(1, l[1]);
    EXPECT_EQ(2, l[2]);
    EXPECT_EQ(3, l[3]);
}

TEST(NbiotListTest, TakeFront) {
    list<T> l;

    l.append(0);
    l.append(1);
    l.append(2);
    l.append(3);

    T front = l.take_front();

    EXPECT_EQ(1, l[0]);
    EXPECT_EQ(2, l[1]);
    EXPECT_EQ(3, l[2]);

    EXPECT_EQ(0, front);
}

TEST(NbiotListTest, TakeFrontEmptyList) {
    list<T> l(-1);
    T front = l.take_front();
    EXPECT_EQ(-1, front);
    EXPECT_EQ(0, l.count());
}

TEST(NbiotListTest, PopItem) {
    list<T> l;

    l.append(0);
    l.append(1);
    l.append(2);
    l.append(3);

    l.pop();

    EXPECT_EQ(1, l[0]);
    EXPECT_EQ(2, l[1]);
    EXPECT_EQ(3, l[2]);
}

TEST(NbiotListTest, PopEmptyList) {
    list<T> l;
    l.pop();
    EXPECT_EQ(0, l.count());
}

TEST(NbiotListTest, FindItem) {
    list<T> l(-1);

    l.append(0);
    l.append(10);
    l.append(100);
    l.append(1000);

    EXPECT_EQ(0, l.find(0));
    EXPECT_EQ(10, l.find(10));
    EXPECT_EQ(100, l.find(100));
    EXPECT_EQ(1000, l.find(1000));
    EXPECT_EQ(-1, l.find(10000));
}

TEST(NbiotListTest, FindEmptyList) {
    list<T> l(-1);

    EXPECT_EQ(-1, l.find(10000));
}

TEST(NbiotListTest, IndexOfItem) {
    list<T> l;

    l.append(0);
    l.append(10);
    l.append(100);
    l.append(1000);

    EXPECT_EQ(0, l.indexOf(0));
    EXPECT_EQ(1, l.indexOf(10));
    EXPECT_EQ(2, l.indexOf(100));
    EXPECT_EQ(3, l.indexOf(1000));
    EXPECT_EQ(-1, l.indexOf(10000));
}

TEST(NbiotListTest, IndexOfEmptyList) {
    list<T> l(-1);

    EXPECT_EQ(-1, l.indexOf(10000));
}

TEST(NbiotListTest, ClearNonEmptyList) {
    list<T> l;

    l.append(0);
    l.append(10);
    l.append(100);
    l.append(1000);

    EXPECT_EQ(4, l.count());

    l.clear();

    EXPECT_EQ(0, l.count());
}

TEST(NbiotListTest, ClearEmptyList) {
    list<T> l;

    EXPECT_EQ(0, l.count());

    l.clear();

    EXPECT_EQ(0, l.count());
}

TEST(NbiotListTest, IsEmpty) {
    list<T> l;
    EXPECT_TRUE(l.isEmpty());
    l.append(1);
    EXPECT_FALSE(l.isEmpty());
}

TEST(NbiotListTest, BeginOfNonEmptyList) {
    list<T> l;

    l.append(0);
    l.append(10);
    l.append(100);
    l.append(1000);

    ListIterator<T> b = l.begin();
    EXPECT_EQ(0, *b);
    l.pop();
    b = l.begin();
    EXPECT_EQ(10, *b);
    l.pop();
    b = l.begin();
    EXPECT_EQ(100, *b);
    l.pop();
    b = l.begin();
    EXPECT_EQ(1000, *b);

}

TEST(NbiotListTest, BeginOfEmptyList) {
    list<T> l(-1);
    ListIterator<T> b = l.begin();
    EXPECT_EQ(-1, *b);
}

TEST(NbiotListTest, EndOfNonEmptyList) {
//    list<T> l;
//
//    l.append(0);
//    l.append(10);
//    l.append(100);
//    l.append(1000);
//
//    ListIterator<T> b = l.end();
//    EXPECT_EQ(1000, *b);
}

TEST(NbiotListTest, EndOfEmptyList) {
//    list<T> l(-1);
//
//    ListIterator<T> b = l.end();
//    EXPECT_EQ(-1, *b);
}