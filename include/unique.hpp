#include <iostream>
#include <utility>

template <typename T>
struct DefaultDeleter
{
    void operator()(T *m_ptr) const noexcept
    {
        delete m_ptr;
    }
};

template <typename T>
struct DefaultDeleter<T[]>
{
    void operator()(T *m_ptr) const noexcept
    {
        delete[] m_ptr;
    }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr
{
private:
    T *m_ptr;
    [[no_unique_address]] Deleter m_deleter;

private:
    void swap(UniquePtr &other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_deleter, other.m_deleter);
    }

public:
    // Constructor
    // explicit prevents implicit conversions
    explicit UniquePtr(T *p = nullptr) : m_ptr(p), m_deleter()
    {
    }

    UniquePtr(T *p, const Deleter &d) : m_ptr(p), m_deleter(d) {}
    UniquePtr(T *p, Deleter &&d) : m_ptr(p), m_deleter(std::move(d)) {}

    // Destructor
    ~UniquePtr()
    {
        if (m_ptr)
        {
            m_deleter(m_ptr);
        }
    }

    // Not copyable
    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    // Move semantics
    UniquePtr(UniquePtr &&other) noexcept : m_ptr(std::exchange(other.m_ptr, nullptr)), m_deleter(std::move(other.m_deleter))
    {
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
            m_deleter = std::move(other.m_deleter);
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
    [[nodiscard]] Deleter& getDeleter() noexcept { return m_deleter; }
    [[nodiscard]] const Deleter& getDeleter() const noexcept { return m_deleter; }

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
        if (old)
        {
            m_deleter(old);
        }
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

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter>
{
private:
    T *m_ptr;
    [[no_unique_address]] Deleter m_deleter;

private:
    void swap(UniquePtr &other) noexcept
    {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_deleter, other.m_deleter);
    }

public:
    // Constructor
    // explicit prevents implicit conversions
    explicit UniquePtr(T *p = nullptr) : m_ptr(p), m_deleter()
    {
    }

    UniquePtr(T *p, const Deleter &d) : m_ptr(p), m_deleter(d) {}

    UniquePtr(T *p, Deleter &&d) : m_ptr(p), m_deleter(std::move(d)) {}

    // Destructor
    ~UniquePtr()
    {
        if (m_ptr)
        {
            m_deleter(m_ptr);
        }
    }

    // Not copyable
    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    // Move semantics
    UniquePtr(UniquePtr &&other) noexcept : m_ptr(std::exchange(other.m_ptr, nullptr)), m_deleter(std::move(other.m_deleter))
    {
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
            m_deleter = std::move(other.m_deleter);
        }
        return *this;
    }

    // Dereference operator - deleter for arrays
    [[nodiscard]] T &operator*() const noexcept = delete;

    [[nodiscard]] T *operator->() const noexcept = delete;

public:
    [[nodiscard]] T *get() const noexcept { return m_ptr; }
    [[nodiscard]] Deleter& getDeleter() noexcept { return m_deleter; }
    [[nodiscard]] const Deleter& getDeleter() const noexcept { return m_deleter; }

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
        if(old) {
            m_deleter(old);
        }
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