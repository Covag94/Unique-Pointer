#include <gtest/gtest.h>
#include "unique.hpp"

TEST(UniquePtrTest, CreateAndAccessTest)
{
    int *raw_ptr = new int(42);
    UniquePtr<int> p1(raw_ptr);

    EXPECT_EQ(*p1 == 42, true);
    EXPECT_EQ(p1.get(), raw_ptr);

    UniquePtr<int> p2(new int(17));
    EXPECT_EQ(*p2 == 17, true);
    EXPECT_EQ(p2.get() != nullptr, true);
}

TEST(UniquePtrTest, MoveConstructorTest)
{
    UniquePtr<int> p{UniquePtr<int>(new int(17))};

    EXPECT_EQ(*p, 17);
    EXPECT_EQ(p != nullptr, true);
}

TEST(UniquePtrTest, MoveAssignmentTest)
{
    UniquePtr<int> p1(new int(42));
    p1 = UniquePtr<int>(new int(17));

    EXPECT_EQ(p1 != nullptr, true);
    EXPECT_EQ(*p1 == 17, true);
}

// Modifiers
/* release() : Returns the pointer to resource and releases ownership*/
TEST(UniquePtrTest, ReleaseTest)
{
    UniquePtr<double> ptr(new double(3.14));
    double *rawPtr = ptr.release();

    EXPECT_EQ(ptr == nullptr, true);
    EXPECT_EQ(rawPtr != nullptr, true);
    EXPECT_EQ(*rawPtr == 3.14, true);
}

/* reset() :  replaces the managed object */
TEST(UniquePtrTest, ResetUniquePtr)
{
    UniquePtr<int> ptr(new int(10));
    ptr.reset(new int(20));
    EXPECT_EQ(ptr != nullptr, true);
    EXPECT_EQ(*ptr == 20, true);

    // Self-reset test
    ptr.reset(ptr.get());
}

/* swap() : swap the managed objects */
TEST(UniquePtrTest, SwapTest)
{
    int *first = new int(42);
    int *second = new int(17);

    UniquePtr<int> p1(first);
    UniquePtr<int> p2(second);

    swap(p1, p2);

    EXPECT_EQ(p2.get() == first && p1.get() == second, true);
    EXPECT_EQ(((*p1) == 17) && ((*p2) == 42), true);
}

// Observers
/* get() : Returns a pointer to the managed object or nullptr*/
TEST(UniquePtrTest, GetTest)
{
    double *resource = new double(0.50);
    UniquePtr p(resource);

    EXPECT_EQ(p.get() == resource, true);
    EXPECT_EQ(*(p.get()) == 0.50, true);
}

// Pointer-like functions
TEST(UniquePtrTest, IndirectionOperatorTest)
{
    struct X
    {
        int _n;

        X() = default;
        X(int n) : _n{n} {}
        ~X() = default;
        int foo() { return _n; }
    };

    UniquePtr<X> ptr(new X(10));
    EXPECT_EQ((*ptr)._n == 10, true);
    EXPECT_EQ(ptr->foo() == 10, true);
}

TEST(UniquePtrTest, PointerToArrayOfTConstructionAndAccess)
{
    UniquePtr<int[]> p(new int[5]{1, 2, 3, 4, 5});

    EXPECT_EQ(p != nullptr, true);
    EXPECT_EQ(p[0] == 1, true);
    EXPECT_EQ(p[2] == 3, true);
    p.release();
    EXPECT_EQ(p == nullptr, true);
}

struct CustomDeleter
{
    bool *flag;
    CustomDeleter(bool *f) : flag(f) {}
    void operator()(int *ptr) const
    {
        *flag = true;
        delete ptr;
    }
};

TEST(UniquePtrTest, CustomDeleterSingleObject)
{
    bool deleter_called = false;

    {
        UniquePtr<int, CustomDeleter> ptr(new int(42), CustomDeleter(&deleter_called));
        EXPECT_EQ(*ptr, 42);
        EXPECT_EQ(deleter_called, false);
    }

    EXPECT_EQ(deleter_called, true); // should be true after destruction
}

TEST(UniquePtrTest, CustomDeleterMoveConstructor)
{
    bool deleter_called = false;

    UniquePtr<int, CustomDeleter> ptr1(new int(42), CustomDeleter(&deleter_called));
    UniquePtr<int, CustomDeleter> ptr2(std::move(ptr1));

    EXPECT_EQ(*ptr2, 42);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(deleter_called, false);

    ptr2.reset();
    EXPECT_EQ(deleter_called, true);
}

TEST(UniquePtrTest, CustomDeleterMoveAssignment)
{
    bool deleter1_called = false;
    bool deleter2_called = false;

    UniquePtr<int, CustomDeleter> ptr1(new int(42), CustomDeleter(&deleter1_called));
    UniquePtr<int, CustomDeleter> ptr2(new int(17), CustomDeleter(&deleter2_called));

    ptr1 = std::move(ptr2);

    EXPECT_EQ(deleter1_called, true);
    EXPECT_EQ(deleter2_called, false);
    EXPECT_EQ(*ptr1, 17);
    EXPECT_EQ(ptr2.get(), nullptr);
}

TEST(UniquePtrTest, CustomDeleterArray)
{
    bool deleter_called = false;

    struct ArrayDeleter
    {
        bool *flag;
        ArrayDeleter(bool *f) : flag(f) {}
        void operator()(int *ptr) const
        {
            *flag = true;
            delete[] ptr;
        }
    };

    {
        UniquePtr<int[], ArrayDeleter> ptr(new int[5]{1, 2, 3, 4, 5}, ArrayDeleter(&deleter_called));
        EXPECT_EQ(ptr[0], 1);
        EXPECT_EQ(ptr[4], 5);
        EXPECT_EQ(deleter_called, false);
    }

    EXPECT_EQ(deleter_called, true);
}