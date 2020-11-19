#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "BitInputStream.hpp"

using namespace std;
using namespace testing;

TEST(BitInputStreamTests, SIMPLE_TEST) {
    string byteStr = "10000000";
    char byte = (char)stoi(byteStr, nullptr, 2);

    stringstream ss;
    ss.write(&byte, 1);
    BitInputStream bis(ss, 1);

    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
}

// use a different bufSize
TEST(BitInputStreamTests, SIMPLE_TEST_2) {
    string byteStr = "11111111";
    char byte = (char)stoi(byteStr, nullptr, 2);

    string byteStr2 = "10101010";
    char byte2 = (char)stoi(byteStr2, nullptr, 2);

    stringstream ss;
    ss.write(&byte, 1);
    ss.write(&byte2, 1);
    BitInputStream bis(ss, 8);

    for (int i = 0; i < 8; i++) ASSERT_EQ(1, bis.readBit());
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(1, bis.readBit());
        ASSERT_EQ(0, bis.readBit());
    }
}

TEST(BitInputStreamTests, SIMPLE_TEST_3) {
    stringstream ss;
    ss.write("a", 1);  // a is 01100001 in bit
    BitInputStream bis(ss, 1);

    ASSERT_EQ(0, bis.readBit());
    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
}