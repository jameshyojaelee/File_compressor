#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "BitOutputStream.hpp"

using namespace std;
using namespace testing;

// test for one bit
TEST(BitOutputStreamTests, SIMPLE_TEST) {
    stringstream ss;
    BitOutputStream bos(ss, 1);
    bos.writeBit(1);
    bos.flush();

    string bitsStr = "10000000";
    unsigned int asciiVal = stoi(bitsStr, nullptr, 2);
    ASSERT_EQ(ss.get(), asciiVal);
}

// test for 8 bits
TEST(BitOutputStreamTests, SIMPLE_TEST_2) {
    stringstream ss;
    BitOutputStream bos(ss, 1);
    for (int i = 0; i < 4; i++) {
        bos.writeBit(1);
        bos.writeBit(0);
    }
    bos.flush();

    string bitsStr = "10101010";
    unsigned int asciiVal = stoi(bitsStr, nullptr, 2);
    ASSERT_EQ(ss.get(), asciiVal);
}

// test for larger buffer size
TEST(BitOutputStreamTests, SIMPLE_TEST_3) {
    stringstream ss;
    BitOutputStream bos(ss, 8);
    for (int i = 0; i < 4; i++) {
        bos.writeBit(1);
        bos.writeBit(0);
    }
    bos.flush();

    string bitsStr = "10101010";
    unsigned int asciiVal = stoi(bitsStr, nullptr, 2);
    ASSERT_EQ(ss.get(), asciiVal);
}