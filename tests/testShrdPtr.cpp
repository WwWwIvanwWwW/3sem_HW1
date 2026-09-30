#include "ShrdPtr.hpp"
#include <gtest/gtest.h>
#include <limits>
#include <utility>

TEST(ShrdPtrTest, DefaultConstructorIsNull)
{
	ShrdPtr<int> p;
	EXPECT_EQ(p.get(), nullptr);
}

TEST(ShrdPtrTest, ConstructorFromPointer)
{
	int *raw = new int(42);
	ShrdPtr<int> p(raw);
	EXPECT_EQ(p.get(), raw);
}

TEST(ShrdPtrTest, ConstructorFromNullptr)
{
	ShrdPtr<int> p(nullptr);
	EXPECT_EQ(p.get(), nullptr);
}

TEST(ShrdPtrTest, DereferenceReturnsValue)
{
	ShrdPtr<int> p(new int(7));
	EXPECT_EQ(*p, 7);
}

TEST(ShrdPtrTest, DereferenceAllowsModification)
{
	ShrdPtr<int> p(new int(7));
	*p = 10;
	EXPECT_EQ(*p, 10);
}

TEST(ShrdPtrTest, ArrowAccessesMember)
{
	struct Point {
		int x;
		int y;
	};
	ShrdPtr<Point> p(new Point{1, 2});
	EXPECT_EQ(p->x, 1);
	EXPECT_EQ(p->y, 2);
}

TEST(ShrdPtrTest, CopyConstructorSharesPointer)
{
	ShrdPtr<int> a(new int(5));
	ShrdPtr<int> b(a);
	EXPECT_EQ(a.get(), b.get());
}

TEST(ShrdPtrTest, CopyConstructorIncrementsCount)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> a(new Tracked(&destroyed));
		ShrdPtr<Tracked> b(a);
		ShrdPtr<Tracked> c(b);
	}

	EXPECT_EQ(destroyed, 1);
}

TEST(ShrdPtrTest, CopyAssignmentSharesPointer)
{
	ShrdPtr<int> a(new int(1));
	ShrdPtr<int> b(new int(2));

	b = a;

	EXPECT_EQ(a.get(), b.get());
	EXPECT_EQ(*b, 1);
}

TEST(ShrdPtrTest, CopyAssignmentReleasesOldValue)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> a(new Tracked(&destroyed));
		ShrdPtr<Tracked> b(new Tracked(&destroyed));
		b = a;
	}

	EXPECT_EQ(destroyed, 2);
}

TEST(ShrdPtrTest, SelfCopyAssignmentKeepsPointer)
{
	ShrdPtr<int> a(new int(5));
	int *raw = a.get();

	a = a;

	EXPECT_EQ(a.get(), raw);
	EXPECT_EQ(*a, 5);
}

TEST(ShrdPtrTest, DestructorDeletesObjectOnce)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> p(new Tracked(&destroyed));
	}

	EXPECT_EQ(destroyed, 1);
}

TEST(ShrdPtrTest, DestructorDeletesAfterLastOwner)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> a(new Tracked(&destroyed));
		{
			ShrdPtr<Tracked> b(a);
			EXPECT_EQ(destroyed, 0);
		}
		EXPECT_EQ(destroyed, 0);
	}

	EXPECT_EQ(destroyed, 1);
}

TEST(ShrdPtrTest, ResetWithNewPointer)
{
	ShrdPtr<int> p(new int(1));
	p.reset(new int(2));
	EXPECT_EQ(*p, 2);
}

TEST(ShrdPtrTest, ResetWithNoArgumentMakesNull)
{
	ShrdPtr<int> p(new int(1));
	p.reset();
	EXPECT_EQ(p.get(), nullptr);
}

TEST(ShrdPtrTest, ResetWithSamePointerKeepsIt)
{
	int *raw = new int(5);
	ShrdPtr<int> p(raw);

	p.reset(raw);

	EXPECT_EQ(p.get(), raw);
	EXPECT_EQ(*p, 5);
}

TEST(ShrdPtrTest, ResetReleasesOldValueWhenLastOwner)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> p(new Tracked(&destroyed));
		p.reset(new Tracked(&destroyed));
	}

	EXPECT_EQ(destroyed, 2);
}

TEST(ShrdPtrTest, ResetDoesNotDeleteIfShared)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		~Tracked() { ++(*counter); }
	};

	{
		ShrdPtr<Tracked> a(new Tracked(&destroyed));
		ShrdPtr<Tracked> b(a);
		a.reset();
		EXPECT_EQ(destroyed, 0);
	}

	EXPECT_EQ(destroyed, 1);
}

TEST(ShrdPtrTest, WorksWithCustomType)
{
	struct Point {
		int x;
		int y;
	};
	ShrdPtr<Point> p(new Point{3, 4});

	EXPECT_EQ(p->x, 3);
	EXPECT_EQ(p->y, 4);
}

TEST(ShrdPtrTest, HoldsMaxInt)
{
	ShrdPtr<int> p(new int(std::numeric_limits<int>::max()));
	EXPECT_EQ(*p, std::numeric_limits<int>::max());
}

TEST(ShrdPtrTest, HoldsMinInt)
{
	ShrdPtr<int> p(new int(std::numeric_limits<int>::min()));
	EXPECT_EQ(*p, std::numeric_limits<int>::min());
}

TEST(ShrdPtrTest, HoldsZero)
{
	ShrdPtr<int> p(new int(0));
	EXPECT_EQ(*p, 0);
}

TEST(ShrdPtrTest, EmptyDestructorDoesNotCrash)
{
	ShrdPtr<int> p;
	SUCCEED();
}

TEST(ShrdPtrArrayTest, DefaultConstructorIsNull)
{
	ShrdPtr<int[]> p;
	EXPECT_EQ(p.get(), nullptr);
}

TEST(ShrdPtrArrayTest, ConstructorFromPointer)
{
	int *raw = new int[3]{1, 2, 3};
	ShrdPtr<int[]> p(raw);
	EXPECT_EQ(p.get(), raw);
}

TEST(ShrdPtrArrayTest, IndexAccessRead)
{
	ShrdPtr<int[]> p(new int[3]{1, 2, 3});
	EXPECT_EQ(p[0], 1);
	EXPECT_EQ(p[1], 2);
	EXPECT_EQ(p[2], 3);
}

TEST(ShrdPtrArrayTest, IndexAccessWrite)
{
	ShrdPtr<int[]> p(new int[3]{1, 2, 3});
	p[1] = 20;
	EXPECT_EQ(p[0], 1);
	EXPECT_EQ(p[1], 20);
	EXPECT_EQ(p[2], 3);
}

TEST(ShrdPtrArrayTest, CopySharesArray)
{
	ShrdPtr<int[]> a(new int[2]{5, 6});
	ShrdPtr<int[]> b(a);

	EXPECT_EQ(a.get(), b.get());
	EXPECT_EQ(b[0], 5);
	EXPECT_EQ(b[1], 6);
}

TEST(ShrdPtrArrayTest, DestructorCallsDeleteArrayOnce)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		Tracked(const Tracked &o) : counter(o.counter) {}
		~Tracked() { ++(*counter); }
	};

	Tracked proto(&destroyed);

	{
		Tracked *arr = new Tracked[3]{proto, proto, proto};
		ShrdPtr<Tracked[]> p(arr);
	}

	EXPECT_EQ(destroyed, 3);
}

TEST(ShrdPtrArrayTest, ResetDeletesOldArrayWhenLastOwner)
{
	static int destroyed = 0;
	struct Tracked {
		int *counter;
		explicit Tracked(int *c) : counter(c) {}
		Tracked(const Tracked &o) : counter(o.counter) {}
		~Tracked() { ++(*counter); }
	};

	Tracked proto(&destroyed);

	{
		Tracked *arr1 = new Tracked[2]{proto, proto};
		Tracked *arr2 = new Tracked[2]{proto, proto};
		ShrdPtr<Tracked[]> p(arr1);
		p.reset(arr2);
	}

	EXPECT_EQ(destroyed, 4);
}

TEST(ShrdPtrArrayTest, HoldsLargeArray)
{
	constexpr std::size_t N = 1000000;
	ShrdPtr<char[]> p(new char[N]);

	p[0] = 'a';
	p[N - 1] = 'z';

	EXPECT_EQ(p[0], 'a');
	EXPECT_EQ(p[N - 1], 'z');
}

TEST(ShrdPtrArrayTest, HoldsLimitValues)
{
	ShrdPtr<int[]> p(new int[2]{std::numeric_limits<int>::max(),
								std::numeric_limits<int>::min()});

	EXPECT_EQ(p[0], std::numeric_limits<int>::max());
	EXPECT_EQ(p[1], std::numeric_limits<int>::min());
}