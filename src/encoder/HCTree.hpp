#ifndef HCTREE_HPP
#define HCTREE_HPP

#include <fstream>
#include <queue>
#include <vector>

#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "HCNode.hpp"

using namespace std;

static int decodeIndex = 0;

class HCTree {
  public:
    HCNode* root;            // the root of HCTree
    vector<HCNode*> leaves;  // a vector storing pointers to all leaf HCNodes
    unsigned int nodeCount;

  public:
    // HCTree constructor to initialize root and leaves

    HCTree() {
        root = nullptr;
        // initialize leaves with size of 256
        vector<HCNode*> leaves(256);
    }

    ~HCTree();

    // function to print out root of the tree
    HCNode* testRoot(HCTree) { return root; };

    void build(const vector<unsigned int>& freqs);

    // helper function to recover the original tree
    void recover(vector<pair<byte, int>> series);

    void encode(byte symbol, BitOutputStream& out) const;

    void encode(byte symbol, ostream& out) const;

    byte decode(BitInputStream& in) const;

    byte decode(istream& in) const;
};

#endif  // HCTREE_HPP
