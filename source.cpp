#include <math.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#define P32 0xB7E15163
#define Q32 0x9E3779B9
#define R 12
#define W 32
const int KeyBytes = 16;

unsigned int ROL(unsigned int x, unsigned int n) {
    return (x << (n & 31)) | (x >> (32 - (n & 31)));
}

unsigned int ROR(unsigned int x, unsigned int n) {
    return (x >> (n & 31)) | (x << (32 - (n & 31)));
}

void RC5_Key_Shedule(unsigned int* K, unsigned int* S) {
    int c = KeyBytes / 4;
    int t = 2 * (R + 1);
    unsigned int L[4] = {0};

    for (int i = 0; i < c; ++i) {
        L[i] = K[i];
    }

    S[0] = P32;
    for (int i = 1; i < t; ++i)
        S[i] = S[i - 1] + Q32;

    unsigned int A = 0, B = 0;
    int i = 0, j = 0;
    for (int s = 0; s < 3 * max(t, c); ++s) {
        A = S[i] = ROL(S[i] + A + B, 3);
        B = L[j] = ROL(L[j] + A + B, A + B);
        i = (i + 1) % t;
        j = (j + 1) % c;
    }
}

void RC5_Encrypt(unsigned int* M, unsigned int* C, unsigned int* S) {
    unsigned int A = M[0] + S[0];
    unsigned int B = M[1] + S[1];
    for (int i = 1; i <= R; ++i) {
        A = ROL(A ^ B, B) + S[2 * i];
        B = ROL(B ^ A, A) + S[2 * i + 1];
    }
    C[0] = A;
    C[1] = B;
}

void RC5_Decrypt(unsigned int* C, unsigned int* M, unsigned int* S) {
    unsigned int B = C[1];
    unsigned int A = C[0];
    for (int i = R; i >= 1; --i) {
        B = ROR(B - S[2 * i + 1], A) ^ A;
        A = ROR(A - S[2 * i], B) ^ B;
    }
    M[1] = B - S[1];
    M[0] = A - S[0];
}

string Task1(unsigned int* source, unsigned int sourceSize, unsigned int* key, bool encryptionMode) {
    stringstream functionOutput;
    unsigned int S[2 * (R + 1)];
    RC5_Key_Shedule(key, S);

    for (unsigned int i = 0; i < sourceSize; i += 2) {
        unsigned int block[2];
        if (encryptionMode) {
            RC5_Encrypt(&source[i], block, S);
        } else {
            RC5_Decrypt(&source[i], block, S);
        }

        for (int j = 0; j < 2; ++j) {
            functionOutput << uppercase << hex << setfill('0') << setw(8) << block[j] << " ";
        }
    }

    string result = functionOutput.str();
    if (!result.empty() && result.back() == ' ')
        result.pop_back();

    return result;
}
