#ifndef INCLUDE_OBJECTPOOL
#define INCLUDE_OBJECTPOOL

#include <functional>
#include <optional>
#include <queue>

template <typename T>
class Object
{
private:
    std::function<T *()> d_create;
    std::function<void(Object<T> &&)> d_reset;

public:
    T *d_val;

    Object() : d_val(nullptr) {}

    Object(std::function<T *()> create, std::function<void(Object<T> &&)> reset)
        : d_create(create), d_reset(reset)
    {
        d_val = d_create();
    }

    Object(Object<T> &&obj)
    {
        *this = std::move(obj);
    }

    Object<T> &operator=(Object<T> &&obj)
    {
        if (this != &obj)
        {
            this->d_val = obj.d_val;
            this->d_create = std::move(obj.d_create);
            this->d_reset = std::move(obj.d_reset);
            obj.d_val = nullptr;
        }

        return *this;
    }

    Object(const Object<T> &obj) = delete;
    Object<T> &operator=(const Object<T> &obj) = delete;

    bool isValid() const
    {
        return d_val != nullptr;
    }

    ~Object()
    {
        if (isValid())
        {
            d_reset(std::move(*this));
        }
    }
};

template <typename T, typename C, typename D>
class ObjectPool
{
private:
    size_t d_capacity;
    C d_create;
    std::optional<size_t> d_maxCapacity;
    std::queue<Object<T> > d_objects;
    D d_release;
    std::function<void(Object<T> &&)> d_reset;

public:
    ObjectPool(size_t capacity, std::optional<size_t> maxCapacity, C create, D release)
        : d_capacity(capacity), d_create(create), d_release(release), d_maxCapacity(maxCapacity)
    {
        d_reset = [&](Object<T> &&obj)
        {
            this->d_objects.emplace(std::move(obj));
        };

        for (size_t i = 0; i < d_capacity; ++i)
        {
            d_objects.emplace(d_create, d_reset);
        }
    }

    Object<T> getObject();

    void releaseObject(Object<T> &obj);

    ~ObjectPool();
};

template <typename T, typename C, typename D>
Object<T> ObjectPool<T, C, D>::getObject()
{
    Object<T> obj;

    if (d_objects.empty() && (!d_maxCapacity.has_value() || d_capacity != d_maxCapacity))
    {
        size_t upperBound = d_maxCapacity.has_value() && d_capacity << 1 > d_maxCapacity.value() ? d_maxCapacity.value() : d_capacity << 1;
        for (size_t i = d_capacity; i < upperBound; ++i)
        {
            d_objects.emplace(d_create, d_reset);
        }

        d_capacity = upperBound;
    }

    if (!d_objects.empty())
    {
        obj = std::move(d_objects.front());
        d_objects.pop();
    }

    return obj;
}

template <typename T, typename C, typename D>
void ObjectPool<T, C, D>::releaseObject(Object<T> &obj)
{
    d_release(obj);
    delete (obj.d_val);
    obj.d_val = nullptr;
}

template <typename T, typename C, typename D>
ObjectPool<T, C, D>::~ObjectPool()
{
    while (!d_objects.empty())
    {
        releaseObject(d_objects.front());
        d_objects.pop();
    }
}

#endif // INCLUDE_OBJECT_POOL