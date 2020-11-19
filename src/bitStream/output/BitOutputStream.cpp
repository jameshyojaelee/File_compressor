#include "BitOutputStream.hpp"

// destructor
BitOutputStream::~BitOutputStream() {
    // deallocate memory for buffer
    delete[] buf;
}

/**
 * TODO: Write the part of the buffer that was written by the user to the
 * output stream, and then clear the buffer to allow further use. You may
 * use fill() to zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation
 * that may cause a timeout.
 */
void BitOutputStream::flush() {
    unsigned int flushSize;
    if (nbits % 8 == 0)
        flushSize = nbits / 8;
    else
        flushSize = ((nbits / 8) + 1);

    out.write(buf, flushSize);       // write the buffer to the output stream
    fill(buf, buf + bufSize, '\0');  // zero-fill buffer
    nbits = 0;                       // reset nbits index
}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {
    unsigned int byteIndex = nbits / 8;
    unsigned int bitIndex = nbits % 8;

    if (nbits == bufSize * 8) {
        flush();  // if buffer is full, flush first
    }
    // write bit to buffer
    buf[byteIndex] = buf[byteIndex] | (i << (7 - bitIndex));
    nbits++;  // move forward index
}
