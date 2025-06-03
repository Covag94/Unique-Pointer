#include <iostream>
#include <utility>

template <typename T>
class UniquePtr
{
private:
    T *m_ptr;

public:
    // Constructor
    // explicit prevents implicit conversions
    explicit UniquePtr(T *p = nullptr) : m_ptr(p)
    {
    }

    // Destructor
    ~UniquePtr()
    {
        delete m_ptr;
    }

    // Dereference operator
    T &operator*() const
    {
        return *m_ptr;
    }

    T *operator->() const
    {
        return m_ptr;
    }

    // Not copyable
    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    // Move semantics
    UniquePtr(UniquePtr &&other) noexcept : m_ptr(std::exchange(other.m_ptr, nullptr))
    {
        // other.m_ptr = nullptr;
        std::cout << "Move constructing unique m_ptr..." << std::endl;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        // Must leave the moved object in valid state
        if (this != &other)
        {
            delete m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

public:
    T *get() const noexcept { return m_ptr; }

    // Release ownership of raw pointer
    [[nodiscard]] T *release() noexcept
    {
        return std::exchange(m_ptr, nullptr);
    }
    // Replace managed object with p
    void reset(T *p = nullptr) noexcept
    {
        T *old = std::exchange(m_ptr, p);
        delete old;
    }
    // Swap ownership
    void swap(UniquePtr &other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }
    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    // Allow reseting to nullptr
    UniquePtr &operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    friend void swap(UniquePtr &a, UniquePtr &b) noexcept
    {
        a.swap(b);
    }

    // Declared friend to deal with symmetry problem
    // ptr == nullptr ok, but nullptr == ptr not ok
    friend bool operator==(const UniquePtr &a, const UniquePtr &b) noexcept
    {
        return a.get() == b.get();
    }

    friend bool operator==(const UniquePtr &a, std::nullptr_t) noexcept
    {
        return a == nullptr;
    }

    friend bool operator==(std::nullptr_t, const UniquePtr &b) noexcept
    {
        return b == nullptr;
    }
};
