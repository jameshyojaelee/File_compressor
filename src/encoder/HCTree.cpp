#include "HCTree.hpp"

/*helper function to remove all nodes in tree*/
void remove(HCNode* node) {
    if (node->c0) remove(node->c0);
    if (node->c1) remove(node->c1);
    delete node;
}

/* TODO: Delete all objects on the heap to avoid memory leaks. */
HCTree::~HCTree() {
    remove(root);
    HCNode* root;
    leaves.clear();
}

/**
 * TODO: Build the HCTree from the given frequency vector. You can assume the
 * vector must have size 256 and each value at index i represents the frequency
 * of char with ASCII value i. Only non-zero frequency symbols should be used to
 * build the tree. The leaves vector must be updated so that it can be used in
 * encode() to improve performance.
 *
 * When building the HCTree, you should use the following tie-breaking rules to
 * match the output from reference solution in checkpoint:
 *
 *    1. HCNode with lower count should have higher priority. If count is the
 * same, then HCNode with a larger ascii value symbol should have higher
 * priority. (This should be already defined properly in the comparator in
 * HCNode.hpp)
 *    2. When popping two highest priority nodes from PQ, the higher priority
 * node will be the ‘c0’ child of the new parent HCNode.
 *    3. The symbol of any parent node should be taken from its 'c0' child.
 */
void HCTree::build(const vector<unsigned int>& freqs) {
    // create priority queue for HCNode. use the HCNode comparator from
    // HCNode.cpp
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    // loop through freqs and find any nonzero values.
    for (int i = 0; i < freqs.size(); i++) {
        if (freqs[i] != 0) {
            // create HCNode with that value and its index
            HCNode* newNode = new HCNode(freqs[i], i);
            nodeCount++;
            pq.push(newNode);  // push the new node to pq
        }
    }
    // pop nodes from pq and build the damn tree
    if (pq.size() != 1) {
        while (pq.size() != 1) {
            HCNode* a = pq.top();
            leaves.push_back(a);
            pq.pop();
            HCNode* b = pq.top();
            leaves.push_back(b);
            pq.pop();
            HCNode* tempP = new HCNode(a->count + b->count, a->symbol, a, b, 0);
            nodeCount++;
            a->p = b->p = tempP;
            pq.push(tempP);
        }
        // set the final node in pq and the ROOT
        root = pq.top();
        pq.pop();
    } else {
        HCNode* a = pq.top();
        root = a;
        leaves.push_back(a);
        pq.pop();
    }
}

void HCTree::recover(vector<pair<byte, int>> series) {
    // use seires vector to save all new nodes in pq
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> tq;
    for (int i = 1; i < series.size(); i++) {
        HCNode* newNode = new HCNode(series[i].second, series[i].first);
        tq.push(newNode);
    }
    // pop nodes from pq and build the damn tree
    if (tq.size() != 1) {
        while (tq.size() != 1) {
            HCNode* a = tq.top();
            leaves.push_back(a);
            tq.pop();
            HCNode* b = tq.top();
            leaves.push_back(b);
            tq.pop();
            HCNode* tempP = new HCNode(a->count + b->count, a->symbol, a, b, 0);
            a->p = b->p = tempP;
            tq.push(tempP);
        }
        // set the final node in pq and the ROOT
        root = tq.top();
        tq.pop();
    } else {
        HCNode* a = tq.top();
        root = a;
        leaves.push_back(a);
        tq.pop();
    }
}

/**
 * TODO: Write the encoding bits of the given symbol to the ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must be called before
 * to create the HCTree.
 */
void HCTree::encode(byte symbol, BitOutputStream& out) const {
    int ec = -1;
    for (int i = 0; i < leaves.size(); i++) {
        if (leaves[i]->symbol == symbol) {
            ec = i;
            break;  // must break when found. symbol may exist more than once.
        }
    }

    vector<char> bits;             // vector to save bits as we go up the tree
    string result;                 // string to return
    HCNode* tracker = leaves[ec];  // the node we want to find
    // use the index to encode the symbol
    if (ec != -1) {
        if (leaves.size() == 1) {
            bits.push_back('0');
        } else {
            while (tracker->p) {
                // if tracker is the left child, push 0 into bits. if not,
                // push 1.
                if (tracker == tracker->p->c0)
                    bits.push_back('0');
                else
                    bits.push_back('1');
                tracker = tracker->p;  // traverse up
            }
        }

        // reverse the order of bits and write it to bitoutputstream
        while (!bits.empty()) {
            if (bits.back() == '0')
                out.writeBit(0);
            else
                out.writeBit(1);
            bits.pop_back();
        }
    }
}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
 */
void HCTree::encode(byte symbol, ostream& out) const {
    int ec = -1;
    for (int i = 0; i < leaves.size(); i++) {
        if (leaves[i]->symbol == symbol) {
            ec = i;
            break;  // must break when found. symbol may exist more than once.
        }
    }

    vector<char> bits;             // vector to save bits as we go up the tree
    string result;                 // string to return
    HCNode* tracker = leaves[ec];  // the node we want to find
    // use the index to encode the symbol
    if (ec != -1) {
        // if tracker has parent (if tracker ain't the root), traverse up tree
        //************************* must have a case where there is only 1 node
        // in the tree *******
        if (leaves.size() == 1) {
            bits.push_back('0');
        } else {
            while (tracker->p) {
                // if tracker is the left child, push 0 into bits. if not,
                // push 1.
                if (tracker == tracker->p->c0)
                    bits.push_back('0');
                else
                    bits.push_back('1');
                tracker = tracker->p;  // traverse up
            }
        }
        // reverse the order of bits and save it as string
        while (!bits.empty()) {
            result.push_back(bits.back());
            bits.pop_back();
        }
        out << result;
    }
}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to
 * work, build() must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(BitInputStream& in) const {
    HCNode* tracker = root;  // tracker node
    // loop through the input string and traverse down the tree with each value

    while (tracker->c0 || tracker->c1) {
        unsigned int nextBit = in.readBit();
        if (nextBit == 0) {
            tracker = tracker->c0;
        } else if (nextBit == 1) {
            tracker = tracker->c1;
        }
    }
    return tracker->symbol;  // return symbol of the found node
}

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or
 * '1') from istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */

byte HCTree::decode(istream& in) const {
    HCNode* tracker = root;  // tracker node
    // loop through the input string and traverse down the tree with each value
    char num;

    while (tracker->c0 || tracker->c1) {
        in.get(num);
        if (num == '0') {
            tracker = tracker->c0;
        } else if (num == '1') {
            tracker = tracker->c1;
        }
    }
    return tracker->symbol;  // return symbol of the found node
}
