#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
using namespace std;

string sha256(string input) {
    // Constants for SHA-256
    constexpr unsigned int K[] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Initialize hash values
    unsigned int H[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    //  padding the message
    string paddedMessage = input;
    paddedMessage += '\x80';
    while ((paddedMessage.size() % 64) != 56) {
        paddedMessage += '\x00';
    }

    // Append original message length in bits
    uint64_t messageLength = input.size() * 8;
    paddedMessage += string(8, '\x00');
    for (int i = 0; i < 8; i++) {
        paddedMessage[paddedMessage.size() - 8 + i] = (messageLength >> (56 - 8 * i)) & 0xFF;
    }

    // Process blocks
    for (int i = 0; i < paddedMessage.size(); i += 64) {
        // Break block into 16 32-bit words
        vector<int> words(64, 0);
        for (int j = 0; j < 16; ++j) {
            words[j] = (static_cast<unsigned int>(paddedMessage[i + j * 4]) << 24) |
                       (static_cast<unsigned int>(paddedMessage[i + j * 4 + 1]) << 16) |
                       (static_cast<unsigned int>(paddedMessage[i + j * 4 + 2]) << 8) |
                       (static_cast<unsigned int>(paddedMessage[i + j * 4 + 3]));
        }

        // Extend the first 16 words into the remaining 48 words
        for (int j = 16; j < 64; ++j) {
            int s0 = (words[j - 15] >> 7 | words[j - 15] << 25) ^
                                    (words[j - 15] >> 18 | words[j - 15] << 14) ^ (words[j - 15] >> 3);
            int s1 = (words[j - 2] >> 17 | words[j - 2] << 15) ^
                                    (words[j - 2] >> 19 | words[j - 2] << 13) ^ (words[j - 2] >> 10);

            words[j] = words[j - 16] + s0 + words[j - 7] + s1;
        }

        // Initialize working variables
        int a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];

        // Compression function main loop
        for (int j = 0; j < 64; ++j) {
            int S1 = (e >> 6 | e << 26) ^ (e >> 11 | e << 21) ^ (e >> 25 | e << 7);
            int ch = (e & f) ^ (~e & g);
            int temp1 = h + S1 + ch + K[j] + words[j];
            int S0 = (a >> 2 | a << 30) ^ (a >> 13 | a << 19) ^ (a >> 22 | a << 10);
            int maj = (a & b) ^ (a & c) ^ (b & c);
            int temp2 = S0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        // Update hash values
        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    // Combine hash values
    ostringstream resultStream;
    for (unsigned int value : H) {
        resultStream << hex << setfill('0') << setw(8) << value;
    }

    return resultStream.str();
} 