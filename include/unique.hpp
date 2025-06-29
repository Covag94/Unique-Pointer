#include <iostream>
#include <utility>

template <typename T>
class UniquePtr
{
private:
    T *m_ptr;

private:
    void swap(UniquePtr &other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

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

    // Not copyable
    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    // Move semantics
    UniquePtr(UniquePtr &&other) noexcept : m_ptr(std::exchange(other.m_ptr, nullptr))
    {
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
        }
        return *this;
    }

    // Dereference operator
    [[nodiscard]] T &operator*() const noexcept
    {
        return *m_ptr;
    }

    [[nodiscard]] T *operator->() const noexcept
    {
        return m_ptr;
    }

public:
    [[nodiscard]] T *get() const noexcept { return m_ptr; }

    // Release ownership of raw pointer
    [[nodiscard]] T *release() noexcept
    {
        return std::exchange(m_ptr, nullptr);
    }

    // Replace managed object with p
    void reset(T *p = nullptr) noexcept
    {
        if (p == m_ptr)
        {
            return;
        }

        T *old = std::exchange(m_ptr, p);
        delete old;
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
        return a.get() == nullptr;
    }

    friend bool operator==(std::nullptr_t, const UniquePtr &b) noexcept
    {
        return b.get() == nullptr;
    }

    friend bool operator!=(const UniquePtr &a, const UniquePtr &b) noexcept
    {
        return a.get() != b.get();
    }

    friend bool operator!=(const UniquePtr &a, std::nullptr_t) noexcept
    {
        return a.get() != nullptr;
    }

    friend bool operator!=(std::nullptr_t, const UniquePtr &b) noexcept
    {
        return b.get() != nullptr;
    }
};

template <typename T>
class UniquePtr<T[]>
{
private:
    T *m_ptr;

private:
    void swap(UniquePtr &other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
    }

public:
    // Constructor
    // explicit prevents implicit conversions
    explicit UniquePtr(T *p = nullptr) : m_ptr(p)
    {
    }

    // Destructor
    ~UniquePtr()
    {
        delete[] m_ptr;
    }

    // Not copyable
    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    // Move semantics
    UniquePtr(UniquePtr &&other) noexcept : m_ptr(std::exchange(other.m_ptr, nullptr))
    {
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
        }
        return *this;
    }

    // Dereference operator
    [[nodiscard]] T &operator*() const noexcept = delete;

    [[nodiscard]] T *operator->() const noexcept = delete;

public:
    [[nodiscard]] T *get() const noexcept { return m_ptr; }

    // Release ownership of raw pointer
    [[nodiscard]] T *release() noexcept
    {
        return std::exchange(m_ptr, nullptr);
    }

    // Replace managed object with p
    void reset(T *p = nullptr) noexcept
    {
        if (p == m_ptr)
        {
            return;
        }

        T *old = std::exchange(m_ptr, p);
        delete[] old;
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

    [[nodiscard]] T &operator[](std::size_t i) const noexcept
    {
        return m_ptr[i];
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
        return a.get() == nullptr;
    }

    friend bool operator==(std::nullptr_t, const UniquePtr &b) noexcept
    {
        return b.get() == nullptr;
    }

    friend bool operator!=(const UniquePtr &a, const UniquePtr &b) noexcept
    {
        return a.get() != b.get();
    }

    friend bool operator!=(const UniquePtr &a, std::nullptr_t) noexcept
    {
        return a.get() != nullptr;
    }

    friend bool operator!=(std::nullptr_t, const UniquePtr &b) noexcept
    {
        return b.get() != nullptr;
    }
};

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args &&...args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
UniquePtr<T[]> make_unique(size_t size)
{
    return UniquePtr<T[]>(new T[size]);
}