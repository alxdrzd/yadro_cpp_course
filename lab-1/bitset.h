//
// Created by alxdrzd on 4/1/26.
//

#ifndef BITSET_H
#define BITSET_H

#include <algorithm>
#include <cstdint>


class bitset {
public:
	bitset();
	explicit bitset(size_t);
	bitset(bitset&&) noexcept;
	bitset& operator=(bitset&&) noexcept;
	bitset& operator=(const bitset&) noexcept;
	bitset(const bitset&) noexcept;
	~bitset();

	bool test(size_t k) const;
	bool operator[](size_t k) const;
	void clear();
	size_t size() const;
	bool empty() const;

	void set(size_t k, bool b);

	bitset union_with(const bitset& other) const;



private:
	static const size_t BITS_PER_BLOCK = 64;
	uint64_t* data_;
	size_t capacity_;

	static size_t blocks_needed(size_t);


};

#endif //BITSET_H
