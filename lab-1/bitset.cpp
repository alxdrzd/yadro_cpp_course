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
		data_ = std::make_unique<uint64_t[]>(blocks);
	}
}

bitset::bitset(bitset &&other) noexcept : data_(std::move(other.data_)), capacity_(other.capacity_) {
	other.capacity_ = 0;
}

bitset &bitset::operator=(bitset &&other) noexcept {
	if (this != &other) {
		this->data_ = std::move(other.data_);
		this->capacity_ = other.capacity_;
		other.capacity_ = 0;
	}
	return *this;
}

bitset &bitset::operator=(const bitset &other) {
	if (this != &other) {
		size_t blocks = blocks_needed(other.capacity_);
		if (blocks > 0) {
			auto new_data = std::make_unique<uint64_t[]>(blocks);
			std::copy_n(other.data_.get(), blocks, new_data.get());
			data_ = std::move(new_data);
		} else {
			data_.reset();
		}
		capacity_ = other.capacity_;
	}
	return *this;
}

bitset::bitset(const bitset &other) : capacity_(other.capacity_) {
	size_t blocks = blocks_needed(capacity_);
	if (blocks > 0) {
		data_ = std::make_unique<uint64_t[]>(blocks);
		std::copy_n(other.data_.get(), blocks, data_.get());
	}
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
		std::fill_n(this->data_.get(), blocks_needed(this->capacity_), 0);
	}
}

size_t bitset::size() const {
	return this->capacity_;
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

		auto new_data = std::make_unique<uint64_t[]>(new_blocks);
		if (this->data_) {
			std::copy_n(this->data_.get(), old_blocks, new_data.get());
		}
		new_data[block_idx] |= mask;

		this->data_ = std::move(new_data);
		this->capacity_ = new_capacity;
	}
}

bitset bitset::union_with(const bitset &other) const {
	size_t union_capacity = std::max(this->capacity_, other.capacity_);

	if (union_capacity == 0) {
		return bitset{};
	}
	bitset result(union_capacity);


	size_t first_blocks = blocks_needed(this->capacity_);
	size_t second_blocks = blocks_needed(other.capacity_);
	size_t min_blocks = std::min(first_blocks, second_blocks);
	size_t union_blocks = blocks_needed(union_capacity);


	for (size_t i = 0; i < min_blocks; ++i) {
		result.data_[i] = this->data_[i] | other.data_[i];
	}

	if (first_blocks < second_blocks) {
		std::copy(other.data_.get() + min_blocks, other.data_.get() + second_blocks, result.data_.get() + min_blocks);
	} else if (first_blocks > second_blocks) {
		std::copy(this->data_.get() + min_blocks, this->data_.get() + first_blocks, result.data_.get() + min_blocks);
	}

	return result;
}

bitset bitset::intersection(const bitset &other) const {
	size_t intersection_capacity = std::min(this->capacity_, other.capacity_);

	if (intersection_capacity == 0) {
		return bitset{};
	}
	bitset result(intersection_capacity);


	size_t first_blocks = blocks_needed(this->capacity_);
	size_t second_blocks = blocks_needed(other.capacity_);
	size_t min_blocks = std::min(first_blocks, second_blocks);

	for (size_t i = 0; i < min_blocks; ++i) {
		result.data_[i] = this->data_[i] & other.data_[i];
	}

	return result;
}

bool bitset::is_subset(const bitset &other) const {
	size_t self_blocks = blocks_needed(this->capacity_);
	size_t other_blocks = blocks_needed(other.capacity_);
	size_t min_blocks = std::min(self_blocks, other_blocks);

	for (size_t i = 0; i < min_blocks; ++i) {
		if ((this->data_[i] & other.data_[i]) != this->data_[i]) {
			return false;
		}
	}

	for (size_t i = min_blocks; i < self_blocks; ++i) {
		if (this->data_[i] != 0) {
			return false;
		}
	}

	return true;
}

bool bitset::empty() const {
	if (!this->data_) {
		return true;
	}
	size_t blocks = blocks_needed(this->capacity_);
	for (size_t i = 0; i < blocks; ++i) {
		if (this->data_[i] != 0) {
			return false;
		}
	}
	return true;
}
