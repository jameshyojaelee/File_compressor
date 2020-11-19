/**
 * TODO: file header
 *
 * Author: James Lee
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */

void pseudoCompression(const string& inFileName, const string& outFileName) {
    // open the file
    ifstream inFile(inFileName);
    ofstream outFile(outFileName);
    // create vector to save all char as ASCII
    vector<unsigned int> freqs(256);
    // get the length and save the entire file to a buffer
    inFile.seekg(0, inFile.end);
    int length = inFile.tellg();
    inFile.seekg(0, inFile.beg);
    // create buffer to read the file in
    char* buffer = new char[length];
    inFile.read(buffer, length);  // read the file into buffer
    inFile.close();               // close file
    // increment freqs for each char in buffer
    for (int i = 0; i < length; i++) {
        // must convert to unsigned char for extended ASCII larger than 127
        freqs[(unsigned char)buffer[i]]++;
    }
    // write down freqs into the output stream
    for (int i = 0; i < freqs.size(); i++) {
        outFile << freqs[i];
        // outFile.flush();
        outFile.put('\n');
    }
    // now build tree with freqs
    HCTree tree;
    tree.build(freqs);
    // encode each char in buffer and write it out in line 257
    for (int i = 0; i < length; i++) {
        tree.encode((unsigned char)buffer[i], outFile);
    }
    delete[] buffer;  // delete buffer
    outFile.close();  // close output stream
}
// using 2 ifstreams... slower
/*
void pseudoCompression(const string& inFileName, const string& outFileName) {
    // open the file
    ifstream inFile(inFileName);

    // create vector to save all char as ASCII
    vector<unsigned int> freqs(256);
    char ch;
    while (inFile.get(ch)) {
        freqs[ch]++;
    }
    inFile.close();
    ofstream outFile(outFileName);

    for (int i = 0; i < freqs.size(); i++) {
        outFile << freqs[i];
        // outFile.flush();
        outFile.put('\n');
    }

    // now build tree with freqs
    HCTree tree;
    tree.build(freqs);
    ifstream inFile2(inFileName);

    while (inFile2.get(ch)) {
        tree.encode(ch, outFile);
    }
    inFile2.close();
    outFile.close();
}*/

// helper function for file header
void preorder(HCNode* node, stringstream& ss) {
    if (node == NULL) {
        return;
    }

    ss.write((char*)(&(node->symbol)), sizeof(int));
    ss.write((char*)(&(node->count)), sizeof(int));

    preorder(node->c0, ss);
    preorder(node->c1, ss);
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    // open the file
    ifstream inFile(inFileName);
    // open in binary mode
    ofstream outFile(outFileName, ios::out | ios::binary);

    // create vector to save all char as ASCII
    vector<unsigned int> freqs(256);
    // get the length and save the entire file to a buffer
    inFile.seekg(0, inFile.end);
    int length = inFile.tellg();
    inFile.seekg(0, inFile.beg);

    // create buffer to read the file in
    char* buffer = new char[length];
    inFile.read(buffer, length);  // read the file into buffer
    inFile.close();               // close file
    // increment freqs for each char in buffer
    for (int i = 0; i < length; i++) {
        // must convert to unsigned char for extended ASCII larger than 127
        freqs[(unsigned char)buffer[i]]++;
    }

    // now build tree with freqs
    HCTree tree;
    tree.build(freqs);

    // contruct bit output stream
    BitOutputStream bos(outFile, 4000);

    // write file header ***
    // write down freqs into the output stream
    for (int i = 0; i < freqs.size(); i++) {
        outFile << freqs[i];
        // outFile.flush();
        outFile.put('\n');
    }

    for (int i = 0; i < length; i++) {
        tree.encode((unsigned char)buffer[i], bos);
    }

    bos.flush();  // flush the remaining bits from the bit buffer

    delete[] buffer;  // delete buffer
    outFile.close();  // close output stream
}

/* Main program that runs the compression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Compresses files using Huffman Encoding");
    options.positional_help("./path_to_input_file ./path_to_output_file");

    bool isAsciiOutput = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Write output in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAsciiOutput))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if original file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAsciiOutput) {
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}