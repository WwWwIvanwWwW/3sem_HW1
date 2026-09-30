#include "UnqPtr.hpp"
#include <gtest/gtest.h>
#include <utility>

struct Tracked {
	static inline int alive = 0;
	int value;
	Tracked(int v = 0) : value(v) { ++alive; }
	Tracked(const Tracked &) = delete;
	Tracked &operator=(const Tracked &) = delete;
	~Tracked() { --alive; }
};

struct Foo {
	int x;
	Foo(int x) : x(x) {}
};

TEST(UnqPtrTest, DefaultConstructor)
{
	UnqPtr<int> p;
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrTest, NullConstructor)
{
	UnqPtr<int> p(nullptr);
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrTest, ConstructorFromPointer)
{
	int *raw = new int(42);
	UnqPtr<int> p(raw);
	EXPECT_EQ(p.Get(), raw);
	EXPECT_EQ(*p, 42);
}

TEST(UnqPtrTest, MoveConstructor)
{
	Tracked::alive = 0;
	UnqPtr<Tracked> a(new Tracked(1));
	UnqPtr<Tracked> b(std::move(a));
	EXPECT_EQ(a.Get(), nullptr);
	EXPECT_EQ(b->value, 1);
	EXPECT_EQ(Tracked::alive, 1);
}

TEST(UnqPtrTest, MoveAssignment)
{
	Tracked::alive = 0;
	UnqPtr<Tracked> a(new Tracked(1));
	UnqPtr<Tracked> b(new Tracked(2));
	b = std::move(a);
	EXPECT_EQ(a.Get(), nullptr);
	EXPECT_EQ(b->value, 1);
	EXPECT_EQ(Tracked::alive, 1);
}

TEST(UnqPtrTest, DestructorDeletes)
{
	Tracked::alive = 0;
	{
		UnqPtr<Tracked> p(new Tracked(5));
		EXPECT_EQ(Tracked::alive, 1);
	}
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrTest, Dereference)
{
	UnqPtr<int> p(new int(10));
	EXPECT_EQ(*p, 10);
	*p = 20;
	EXPECT_EQ(*p, 20);
}

TEST(UnqPtrTest, Arrow)
{
	UnqPtr<Foo> p(new Foo(7));
	EXPECT_EQ(p->x, 7);
	p->x = 8;
	EXPECT_EQ(p->x, 8);
}

TEST(UnqPtrTest, Get)
{
	int *raw = new int(42);
	UnqPtr<int> p(raw);
	EXPECT_EQ(p.Get(), raw);
}

TEST(UnqPtrTest, Release)
{
	Tracked::alive = 0;
	UnqPtr<Tracked> p(new Tracked(3));
	Tracked *raw = p.release();
	EXPECT_EQ(Tracked::alive, 1);
	EXPECT_EQ(p.Get(), nullptr);
	delete raw;
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrTest, ReleaseNull)
{
	UnqPtr<int> p;
	EXPECT_EQ(p.release(), nullptr);
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrTest, Reset)
{
	Tracked::alive = 0;
	UnqPtr<Tracked> p(new Tracked(1));
	EXPECT_EQ(Tracked::alive, 1);
	p.reset(new Tracked(2));
	EXPECT_EQ(Tracked::alive, 1);
	EXPECT_EQ(p->value, 2);
}

TEST(UnqPtrTest, ResetToNull)
{
	Tracked::alive = 0;
	UnqPtr<Tracked> p(new Tracked(1));
	p.reset();
	EXPECT_EQ(p.Get(), nullptr);
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrTest, ResetNullThenSet)
{
	UnqPtr<int> p;
	p.reset(new int(5));
	EXPECT_EQ(*p, 5);
	p.reset();
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrTest, TypeTraits)
{
	static_assert(!std::is_copy_constructible_v<UnqPtr<int>>);
	static_assert(!std::is_copy_assignable_v<UnqPtr<int>>);
	static_assert(std::is_move_constructible_v<UnqPtr<int>>);
	static_assert(std::is_move_assignable_v<UnqPtr<int>>);
}

TEST(UnqPtrArrayTest, DefaultConstructor)
{
	UnqPtr<int[]> p;
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrArrayTest, NullConstructor)
{
	UnqPtr<int[]> p(nullptr);
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrArrayTest, ConstructorFromPointer)
{
	int *raw = new int[3]{1, 2, 3};
	UnqPtr<int[]> p(raw);
	EXPECT_EQ(p.Get(), raw);
	EXPECT_EQ(p[0], 1);
	EXPECT_EQ(p[1], 2);
	EXPECT_EQ(p[2], 3);
}

TEST(UnqPtrArrayTest, Indexing)
{
	UnqPtr<int[]> p(new int[4]{0, 0, 0, 0});
	p[0] = 10;
	p[1] = 20;
	p[2] = 30;
	p[3] = 40;
	EXPECT_EQ(p[0], 10);
	EXPECT_EQ(p[1], 20);
	EXPECT_EQ(p[2], 30);
	EXPECT_EQ(p[3], 40);
}

TEST(UnqPtrArrayTest, MoveConstructor)
{
	Tracked::alive = 0;
	UnqPtr<Tracked[]> a(new Tracked[3]);
	UnqPtr<Tracked[]> b(std::move(a));
	EXPECT_EQ(a.Get(), nullptr);
	EXPECT_NE(b.Get(), nullptr);
	EXPECT_EQ(Tracked::alive, 3);
}

TEST(UnqPtrArrayTest, MoveAssignment)
{
	Tracked::alive = 0;
	UnqPtr<Tracked[]> a(new Tracked[2]);
	UnqPtr<Tracked[]> b(new Tracked[4]);
	EXPECT_EQ(Tracked::alive, 6);
	b = std::move(a);
	EXPECT_EQ(a.Get(), nullptr);
	EXPECT_EQ(Tracked::alive, 2);
}

TEST(UnqPtrArrayTest, DestructorDeletesArray)
{
	Tracked::alive = 0;
	{
		UnqPtr<Tracked[]> p(new Tracked[5]);
		EXPECT_EQ(Tracked::alive, 5);
	}
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrArrayTest, Get)
{
	int *raw = new int[2]{7, 8};
	UnqPtr<int[]> p(raw);
	EXPECT_EQ(p.Get(), raw);
}

TEST(UnqPtrArrayTest, Release)
{
	Tracked::alive = 0;
	UnqPtr<Tracked[]> p(new Tracked[3]);
	Tracked *raw = p.release();
	EXPECT_EQ(Tracked::alive, 3);
	EXPECT_EQ(p.Get(), nullptr);
	delete[] raw;
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrArrayTest, ReleaseNull)
{
	UnqPtr<int[]> p;
	EXPECT_EQ(p.release(), nullptr);
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrArrayTest, Reset)
{
	Tracked::alive = 0;
	UnqPtr<Tracked[]> p(new Tracked[2]);
	EXPECT_EQ(Tracked::alive, 2);
	p.reset(new Tracked[4]);
	EXPECT_EQ(Tracked::alive, 4);
}

TEST(UnqPtrArrayTest, ResetToNull)
{
	Tracked::alive = 0;
	UnqPtr<Tracked[]> p(new Tracked[3]);
	p.reset();
	EXPECT_EQ(p.Get(), nullptr);
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrArrayTest, ResetNullThenSet)
{
	UnqPtr<int[]> p;
	p.reset(new int[2]{1, 2});
	EXPECT_EQ(p[0], 1);
	EXPECT_EQ(p[1], 2);
	p.reset();
	EXPECT_EQ(p.Get(), nullptr);
}

TEST(UnqPtrArrayTest, UsesDeleteArrayNotDelete)
{
	Tracked::alive = 0;
	{
		UnqPtr<Tracked[]> p(new Tracked[3]);
		EXPECT_EQ(Tracked::alive, 3);
	}
	EXPECT_EQ(Tracked::alive, 0);
}

TEST(UnqPtrArrayTest, MoveDoesNotLeak)
{
	Tracked::alive = 0;
	{
		UnqPtr<Tracked[]> a(new Tracked[2]);
		UnqPtr<Tracked[]> b(std::move(a));
		UnqPtr<Tracked[]> c(new Tracked[3]);
		c = std::move(b);
		EXPECT_EQ(Tracked::alive, 2);
	}
	EXPECT_EQ(Tracked::alive, 0);
}
