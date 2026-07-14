#include "include/pricebuffer.h"

void PriceBuffer::write_to_buffer(string in){
    lock_guard<mutex> lock(m_);
    buffer_.push_back(std::move(in));
    if (buffer_.size() > CAPACITY) {
        buffer_.pop_front();
    }
}

vector<string> PriceBuffer::last_n(size_t n){
    lock_guard<mutex> lock(m_);
    size_t k = min(n, buffer_.size());
    return vector<string>(buffer_.end() - k, buffer_.end());
}