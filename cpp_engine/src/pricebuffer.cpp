#include "include/pricebuffer.h"

void PriceBuffer::write_to_buffer(string in){
    lock_guard<mutex> lock(m_);
    id_++;
    if (buffer_.size() > CAPACITY) {
        buffer_.pop_front();
    }
    buffer_.push_back(std::move(in));
}

deque<string> PriceBuffer::last_n(size_t n) const{
    lock_guard<mutex> lock(m_);
    size_t k = min(n, buffer_.size());
    return deque<string>(buffer_.end() - k, buffer_.end());
}


bool PriceBuffer::operator==(const PriceBuffer &o) const{
    return this->last_n(1) == o.last_n(1);
}

PriceBuffer& PriceBuffer::operator=(const PriceBuffer &o){
    if(this != &o){
        scoped_lock lock(m_, o.m_);
        buffer_ = o.buffer_;
    }
    return *this;
}

uint64_t PriceBuffer::getId() const{
    return id_.load();
}

PriceBuffer::Snapshot PriceBuffer::getSnapshot() const{
    lock_guard<mutex> lock(m_);
    return{
        getId(),
        buffer_
    };
}

PriceBuffer::Snapshot PriceBuffer::getSnapshot(int n) const{
    lock_guard<mutex> lock(m_);

    size_t k = min(static_cast<size_t>(n), buffer_.size());

    return {
        id_.load(),
        deque<string>(buffer_.end() - k, buffer_.end())
    };
}