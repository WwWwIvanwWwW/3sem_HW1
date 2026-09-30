#pragma once
#include <cstddef>
template <typename T> class UnqPtr
{
  private:
	T *ptr;

  public:
	UnqPtr(T *p = nullptr) : ptr(p) {};

	UnqPtr(const UnqPtr &) = delete;
	UnqPtr &operator=(const UnqPtr &) = delete;

	UnqPtr(UnqPtr &&other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	UnqPtr &operator=(UnqPtr &&other) noexcept
	{
		if (this != &other) {
			delete ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}

	T &operator*() const { return *ptr; }
	T *operator->() const { return ptr; }
	T *Get() const { return ptr; }
	~UnqPtr() { delete ptr; }

	T *release()
	{
		T *tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	void reset(T *p = nullptr)
	{
		if (ptr != p) {
			delete ptr;
			ptr = p;
		}
	}
};

template <typename T> class UnqPtr<T[]>
{
  private:
	T *ptr;

  public:
	UnqPtr(T *p = nullptr) : ptr(p) {};

	UnqPtr(const UnqPtr &) = delete;
	UnqPtr &operator=(const UnqPtr &) = delete;

	UnqPtr(UnqPtr &&other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	UnqPtr &operator=(UnqPtr &&other) noexcept
	{
		if (this != &other) {
			delete[] ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}

	T &operator[](std::size_t i) const { return ptr[i]; }
	T *Get() const { return ptr; }
	~UnqPtr() { delete[] ptr; }

	T *release()
	{
		T *tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	void reset(T *p = nullptr)
	{
		if (ptr != p) {
			delete[] ptr;
			ptr = p;
		}
	}
};
