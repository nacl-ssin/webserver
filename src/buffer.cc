//
// Created by 26372 on 2022/5/27.
//

#include <iostream>
#include "buffer.h"
#include "logger.h"

Buffer::Buffer() : _data(nullptr), _size(0), _rd_idx(0), _capacity(0) {
}


Buffer::Buffer(size_type capacity) : _data(new char[capacity]), _size(0), _rd_idx(0), _capacity(capacity) {
}

Buffer::Buffer(const Buffer &bf) {
	_size = bf._size;
	_rd_idx = bf._rd_idx;
	_capacity = bf._capacity;
	_data = new char[_capacity];
	memcpy(_data, bf._data, _size);
}

Buffer::Buffer(Buffer &&rv) noexcept {
	swap(rv);
}


Buffer::~Buffer() {
	delete[] _data;
	_data = nullptr;
}


void Buffer::append(const char *data, Buffer::size_type insize) {
	// 空间不足
	if (_size + insize > _capacity) {
		size_type nc = _capacity + static_cast<int>(insize * 1.5);
		reserve(nc);
	}

	memcpy(_data + _size, data, insize);
	_size += insize;
}


void Buffer::seek(Buffer::size_type offset, Buffer::HOW_SEEK how) {
	if (how == SET_OFFSET) {
		_rd_idx = offset;
	}

	if (how == SET_CURT) {
		_rd_idx += offset;
	}
}


void Buffer::reserve(Buffer::size_type n) {
	if (n > _capacity) {
		char *tmp = new char[n];
		memcpy(tmp, _data, _size);
		delete[] _data;
		_data = tmp;
		_capacity = n;
	}
}




