#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCTreeFixture : public ::testing::Test {
  protected:
    HCTree tree;

  public:
    SimpleHCTreeFixture() {
        // initialization code here
        vector<unsigned int> freqs(256);
        freqs['a'] = 2;
        freqs['b'] = 3;
        tree.build(freqs);
    }
};
//***************************************************************************
//******************************BITSTREAM tests******************************
TEST_F(SimpleHCTreeFixture, TEST_BIT_ENCODE) {
    stringstream ss;
    BitOutputStream bos(ss, 100);
    tree.encode('a', bos);
    bos.flush();
    ASSERT_EQ(ss.get(), 0);
}

TEST_F(SimpleHCTreeFixture, TEST__BIT_DECODE) {
    stringstream ss;
    BitOutputStream bos(ss, 100);
    tree.encode('b', bos);
    bos.flush();

    BitInputStream bis(ss, 100);
    byte result;
    if (bis.eof()) {
        result = tree.decode(bis);
    }
    ASSERT_EQ(result, 'b');
}

//***************************************************************************
//********************************ASCII tests********************************
TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('a', os);
    ASSERT_EQ(os.str(), "0");
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("1");
    ASSERT_EQ(tree.decode(is), 'b');
}

// test for the HCNode comparator, with two different counts
TEST_F(SimpleHCTreeFixture, TEST_HCNODE_1) {
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    HCNode* a = new HCNode(1, 'a');
    HCNode* b = new HCNode(2, 'b');
    pq.push(a);
    pq.push(b);
    ASSERT_EQ(a, pq.top());
    delete a;
    delete b;
}
// test for the HCNode comparator, two symbols with the SAME COUNT.
TEST_F(SimpleHCTreeFixture, TEST_HCNODE_2) {
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    HCNode* a = new HCNode(1, 'a');
    HCNode* b = new HCNode(1, 'b');
    pq.push(a);
    pq.push(b);
    ASSERT_EQ(b, pq.top());
    delete a;
    delete b;
}

// test build function in HCTree.cpp
TEST_F(SimpleHCTreeFixture, TEST_BUILD_1) {
    HCTree tree2;
    vector<unsigned int> freqs2(256);
    freqs2['a'] = 100;
    freqs2['b'] = 50;
    freqs2['f'] = 60;
    freqs2['j'] = 900;
    freqs2['z'] = 30;
    tree2.build(freqs2);

    /*          a(1140)
                /     \
            a(240)   j(900)
            /     \
        a(100)    f(140)
                /    \
            f(60)    z(80)
                    /    \
                    z(30)  b(50)
    */
    ASSERT_EQ('a', (tree2.root)->symbol);
    ASSERT_EQ('a', (tree2.root->c0)->symbol);
    ASSERT_EQ('j', (tree2.root->c1)->symbol);
    ASSERT_EQ('a', (tree2.root->c0->c0)->symbol);
    ASSERT_EQ('f', (tree2.root->c0->c1)->symbol);
    ASSERT_EQ('f', (tree2.root->c0->c1->c0)->symbol);
    ASSERT_EQ('z', (tree2.root->c0->c1->c1)->symbol);
    ASSERT_EQ('z', (tree2.root->c0->c1->c1->c0)->symbol);
    ASSERT_EQ('b', (tree2.root->c0->c1->c1->c1)->symbol);
}

// test build.  inserting only one char.
TEST_F(SimpleHCTreeFixture, TEST_BUILD_2) {
    HCTree tree3;
    vector<unsigned int> freqs3(256);
    freqs3['a'] = 100;
    tree3.build(freqs3);

    cout << *tree3.root << endl;
    ASSERT_EQ('a', (tree3.root)->symbol);
}

// test recover
TEST_F(SimpleHCTreeFixture, TEST_RECOVER) {
    vector<pair<byte, int>> series;
    series.push_back(make_pair('a', 5));
    series.push_back(make_pair('a', 2));
    series.push_back(make_pair('b', 3));
    HCTree tree3;
    tree3.recover(series);

    cout << *tree3.root << endl;
    ASSERT_EQ('a', (tree3.root)->symbol);
}