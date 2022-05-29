//
// Created by 26372 on 2022/5/27.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H

#include <cstring>


class Buffer {
	using size_type = unsigned long;

public:
	char *_data;
	size_type _size;
	size_type _rd_idx;
	size_type _capacity;

public:
	enum HOW_SEEK {
		SET_OFFSET = 0,
		SET_CURT
	};

public:

	Buffer();

	explicit Buffer(size_type capacity);

	Buffer(const Buffer &bf);

	Buffer(Buffer &&rvalue) noexcept;

	~Buffer();

	/**
	 * 追加数据
	 * @param data
	 * @param size
	 */
	void append(const char *data, size_type size);


	/**
	 *	移动读取位置
	 * @param offset
	 * @param how
	 */
	void seek(size_type offset, HOW_SEEK how);


	void reserve(size_type size);

	Buffer &operator=(Buffer bf) {
		swap(bf);
		return *this;
	}

	Buffer &operator=(Buffer &&rvalue) noexcept {
		swap(rvalue);
		return *this;
	}

	inline size_type size() const {
		return _size;
	}

	inline size_type unsent_size() const {
		return _size - _rd_idx;
	}

	inline size_type capacity() const {
		return _capacity;
	}


	inline char *rd_ptr() const {
		return _data + _rd_idx;
	}


	inline void clear() {
		_rd_idx = 0;
		_size = 0;
	}

	inline bool empty() const {
		return _rd_idx == _size;
	}

	void swap(Buffer &bf) {
		std::swap(_data, bf._data);
		std::swap(_size, bf._size);
		std::swap(_rd_idx, bf._rd_idx);
		std::swap(_capacity, bf._capacity);
	}
};

#endif //WEBSERVER_BUFFER_H
