//
// Created by alxdrzd on 4/1/26.
//
#include "bitset.h"

size_t bitset::blocks_needed(size_t bits) {
	if (bits == 0) {
		return 0;
	}
	return (bits + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK;
}

bitset::bitset() : data_(nullptr), capacity_(0) {
}

bitset::bitset(size_t initial_capacity) : capacity_(initial_capacity) {
	size_t blocks = blocks_needed(initial_capacity);
	if (blocks > 0) {
		data_ = new uint64_t[blocks];
		std::fill_n(data_, blocks, 0);
	} else {
		data_ = nullptr;
	}
}

bitset::bitset(bitset &&other) noexcept : data_(other.data_), capacity_(other.capacity_) {
	other.data_ = nullptr;
	other.capacity_ = 0;
}

bitset &bitset::operator=(bitset &&other) noexcept {
	if (this != &other) {
		delete[] this->data_;
		this->data_ = other.data_;
		this->capacity_ = other.capacity_;
		other.data_ = nullptr;
		other.capacity_ = 0;
	}
	return *this;
}

bitset &bitset::operator=(const bitset &other) {
	if (this != &other) {
		size_t blocks = blocks_needed(other.capacity_);
		uint64_t *new_data = nullptr;

		if (blocks > 0) {
			new_data = new uint64_t[blocks];
			std::copy_n(other.data_, blocks, new_data);
		}

		delete[] this->data_;
		this->data_ = new_data;
		this->capacity_ = other.capacity_;
	}
	return *this;
}

bitset::bitset(const bitset &other) : capacity_(other.capacity_) {
	size_t blocks = blocks_needed(capacity_);
	if (blocks > 0) {
		data_ = new uint64_t[blocks];
		std::copy_n(other.data_, blocks, data_);
	} else {
		data_ = nullptr;
	}
}

bitset::~bitset() {
	delete[] this->data_;
}

bool bitset::test(size_t k) const {
	if (k >= this->capacity_) {
		return false;
	}

	size_t block_idx = k / BITS_PER_BLOCK;
	size_t bit_idx = k % BITS_PER_BLOCK;

	uint64_t mask = (1ULL << bit_idx);
	return mask & this->data_[block_idx];
}

bool bitset::operator[](size_t k) const {
	if (k >= this->capacity_) {
		return false;
	}

	size_t block_idx = k / BITS_PER_BLOCK;
	size_t bit_idx = k % BITS_PER_BLOCK;

	uint64_t mask = (1ULL << bit_idx);
	return mask & this->data_[block_idx];
}

void bitset::clear() {
	if (this->data_) {
		std::fill_n(this->data_, blocks_needed(this->capacity_), 0);
	}
}

size_t bitset::size() const {
	return this->capacity_;
}

bool bitset::empty() const {
}

void bitset::set(size_t k, bool b) {
	size_t block_idx = k / BITS_PER_BLOCK;
	size_t bit_idx = k % BITS_PER_BLOCK;
	uint64_t mask = (1ULL << bit_idx);
	if (k < this->capacity_) {
		if (b != this->test(k)) {
			if (b) {
				this->data_[block_idx] |= mask;
			} else {
				this->data_[block_idx] &= ~mask;
			}
		}
	} else if (b) {
		size_t new_capacity = std::max(k + 1, std::max(this->capacity_ * 2, static_cast<size_t>(64)));
		size_t new_blocks = blocks_needed(new_capacity);
		size_t old_blocks = blocks_needed(this->capacity_);

		uint64_t *new_data = new uint64_t[new_blocks];
		std::fill_n(new_data, new_blocks, 0);
		if (this->data_) {
			std::copy_n(this->data_, old_blocks, new_data);
		}
		new_data[block_idx] |= mask;
		delete[] this->data_;
		this->data_ = new_data;
		this->capacity_ = new_capacity;
	}
}

bitset bitset::union_with(const bitset &other) const {


}
