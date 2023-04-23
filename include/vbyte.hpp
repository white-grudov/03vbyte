#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "packed_int.hpp"

namespace pfp
{
    class PackedArrayUnlim {
        public:
            PackedArrayUnlim(size_t n, size_t k) : arrays_size_(0), 
                                                   n_(std::pow(2, std::ceil(std::log2(n)))), 
                                                   k_(k),
                                                   mask_(std::log2(n_)) {
                arrays_ = std::vector<PackedIntegerArray*>(4, new PackedIntegerArray(n_, k_));
            }
            ~PackedArrayUnlim() {
                for (size_t i = 0; i < arrays_.size(); ++i) {
                    delete arrays_[i];
                }
            }

            inline uint64_t get(size_t index) {
                size_t quot = index >> mask_;
                size_t rem = index & (n_ - 1);
                return arrays_[quot]->get(rem);
            }

            inline void set(size_t index, uint64_t value) {
                if (index >= (arrays_size_ << mask_)) [[unlikely]] {
                    allocate_new();
                }
                size_t quot = index >> mask_;
                size_t rem = index & (n_ - 1);
                arrays_[quot]->set(rem, value);
            }
        private:
            void allocate_new() {
                arrays_[arrays_size_++] = new PackedIntegerArray(n_, k_);
            }
            std::vector<PackedIntegerArray*> arrays_;
            size_t arrays_size_;
            size_t n_;
            size_t k_;
            size_t mask_;
    };
    class VByte {
    public:
        VByte(uint64_t size, size_t k) : size_(size),
                                         k_(k),
                                         blocks_(0),
                                         current_(0),
                                         data_(size, k + 1) {}
        ~VByte() {}

        uint64_t size() const {
            return size_;
        }

        uint64_t blocks() const {
            return blocks_;
        }

        void encode(uint64_t value) {
            while (true) {
                data_.set(blocks_++, value & ((1ull << k_) - 1));
                if (value < (1ull << k_)) break;
                value >>= k_;
            }
            uint64_t stop = data_.get(blocks_ - 1);
            data_.set(blocks_ - 1, stop | (1ull << k_));
        }

        uint64_t decode_next() {
            uint64_t result = 0;
            uint64_t shift = 0;
            bool stop_bit = false;
            while (!stop_bit) {
                if (data_.get(current_) & (1ull << k_)) stop_bit = true;
                uint64_t value = (data_.get(current_++) & ((1ull << k_) - 1)) << (k_ * shift++);
                result += value;
            }
            return result;
        }

    private:
        uint64_t size_;
        uint64_t k_;
        uint64_t blocks_;
        uint64_t current_;
        PackedArrayUnlim data_;
    };
} // namespace pfp