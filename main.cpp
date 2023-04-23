#include <iostream>
#include <fstream>
#include <unistd.h>

#include "include/vbyte.hpp"

uint64_t next_number(std::istream& in) {
    uint64_t result;
    in.read(reinterpret_cast<char*>(&result), sizeof(result));
    return result;
}

void test_vbyte(std::istream& in, bool sorted, uint64_t k) {  
    std::cout << std::flush;
    std::cerr << std::flush;

    uint64_t n = next_number(in);
    pfp::VByte vbyte(n, k);

    uint64_t prev = 0;
    for (uint64_t i = 0; i < n; ++i) {
        if (sorted) {
            uint64_t current = next_number(in);
            vbyte.encode(current - prev);
            prev = current;
        } else {
            vbyte.encode(next_number(in));
        }
    }

    prev = 0;
    std::cerr << vbyte.blocks() << '\n';
    for (uint64_t i = 0; i < n; ++i) {
        if (sorted) {
            prev += vbyte.decode_next();
            std::cout << prev << '\n';
        } else {
            std::cout << vbyte.decode_next() << '\n';
        }
    }

    std::cout << std::flush;
    std::cerr << std::flush;
}

int main(int argc, char const* argv[]) {
    int input_file = 0;
    uint64_t k = 7;
    bool sorted = false;

    int i = 1;
    while (i < argc) {
        std::string s(argv[i++]);
        if (s.compare("-s") == 0) {
            sorted = true;
        } else if (s.compare("-k") == 0) {
            k = std::stoi(argv[i++]);
        } else {
            input_file = i - 1;
        }
    }

    // input_file = 1;
    // argv[input_file] = "b30_1.txt";

    std::ifstream in = std::ifstream(argv[input_file], std::ios::binary);
    if (input_file > 0) {
        test_vbyte(in, sorted, k);
    } else {
        test_vbyte(std::cin, sorted, k);
    }

    return 0;
}