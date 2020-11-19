/**
 * TODO: file header
 *
 * Author:
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: Pseudo decompression with ascii encoding and naive header (checkpoint)
 */
void pseudoDecompression(const string& inFileName, const string& outFileName) {
    ifstream inFile(inFileName);
    ofstream outFile(outFileName);

    vector<unsigned int> freqs(256);

    string line;
    int index;
    for (int i = 0; i < freqs.size(); i++) {
        getline(inFile, line);
        istringstream(line) >> index;
        freqs[i] += index;
    }
    // build tree with freqs we got
    HCTree tree;
    tree.build(freqs);

    // get the encoded string at line 257
    string input;
    getline(inFile, input);
    // put the string into iss for DECODE to read
    istringstream is(input);

    while (is.peek() != EOF) {
        outFile << tree.decode(is);
    }

    inFile.close();  // close ifstream
    outFile.close();
}

/* TODO: True decompression with bitwise i/o and small header (final) */
void trueDecompression(const string& inFileName, const string& outFileName) {
    ifstream inFile(inFileName, ios::in | ios::binary);
    ofstream outFile(outFileName);
    vector<unsigned int> freqs(256);

    string line;
    int index;
    for (int i = 0; i < freqs.size(); i++) {
        getline(inFile, line);
        istringstream(line) >> index;
        freqs[i] += index;
    }

    // build tree with freqs we got
    HCTree tree;
    tree.build(freqs);

    // get the encoding bits
    BitInputStream bis(inFile, 4000);

    while (1) {
        outFile << tree.decode(bis);

        if (bis.eof()) break;
    }

    inFile.close();  // close ifstream

    outFile.close();
}

/* Main program that runs the decompression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Uncompresses files using Huffman Encoding");
    options.positional_help(
        "./path_to_compressed_input_file ./path_to_output_file");

    bool isAscii = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Read input in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAscii))("input", "",
                                       cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit.");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // if compressed file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        exit(0);
    }

    if (isAscii) {
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
