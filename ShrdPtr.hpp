#pragma once
#include <cstddef>

template <typename T> class ShrdPtr
{
  private:
	T *ptr;
	int *ref_count;

  public:
	ShrdPtr(T *p = nullptr) : ptr(p), ref_count(new int(1)) {}

	ShrdPtr(const ShrdPtr &other) : ptr(other.ptr), ref_count(other.ref_count)
	{
		++(*ref_count);
	}

	ShrdPtr &operator=(const ShrdPtr &other)
	{
		if (this != &other) {
			if (--(*ref_count) == 0) {
				delete ptr;
				delete ref_count;
			}
			ptr = other.ptr;
			ref_count = other.ref_count;
			++(*ref_count);
		}
		return *this;
	}

	~ShrdPtr()
	{
		if (--(*ref_count) == 0) {
			delete ptr;
			delete ref_count;
		}
	}

	T &operator*() const { return *ptr; }
	T *operator->() const { return ptr; }
	T *get() const { return ptr; }

	void reset(T *p = nullptr)
	{
		if (ptr != p) {
			if (--(*ref_count) == 0) {
				delete ptr;
				delete ref_count;
			}
			ptr = p;
			ref_count = new int(1);
		}
	}
};

template <typename T> class ShrdPtr<T[]>
{
  private:
	T *ptr;
	int *ref_count;

  public:
	ShrdPtr(T *p = nullptr) : ptr(p), ref_count(new int(1)) {}

	ShrdPtr(const ShrdPtr &other) : ptr(other.ptr), ref_count(other.ref_count)
	{
		++(*ref_count);
	}

	ShrdPtr &operator=(const ShrdPtr &other)
	{
		if (this != &other) {
			if (--(*ref_count) == 0) {
				delete[] ptr;
				delete ref_count;
			}
			ptr = other.ptr;
			ref_count = other.ref_count;
			++(*ref_count);
		}
		return *this;
	}

	~ShrdPtr()
	{
		if (--(*ref_count) == 0) {
			delete[] ptr;
			delete ref_count;
		}
	}

	T &operator[](std::size_t i) const { return ptr[i]; }
	T *get() const { return ptr; }

	void reset(T *p = nullptr)
	{
		if (ptr != p) {
			if (--(*ref_count) == 0) {
				delete[] ptr;
				delete ref_count;
			}
			ptr = p;
			ref_count = new int(1);
		}
	}
};