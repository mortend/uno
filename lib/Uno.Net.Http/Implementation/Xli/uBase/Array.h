#pragma once
#include <uBase/Config.h>
#include <uBase/Object.h>
#include <uBase/Exception.h>

namespace uBase
{
    /** \ingroup uContainer */
    template <typename T> class U_DLLEXPORT Array
    {
        static const size_t BufSize = 4;

        T* data;
        T buf[BufSize];
        size_t used;
        size_t capacity;

        Array(const Array& copy);
        Array& operator =(const Array& copy);

    public:
        Array()
        {
            used = 0;
            capacity = BufSize;
            data = buf;
        }

        explicit Array(size_t size)
        {
            used = 0;
            capacity = BufSize;
            data = buf;
            Resize(size);
        }

        explicit Array(size_t count, const T* initItems)
        {
            used = 0;
            capacity = BufSize;
            data = buf;
            Resize(count);

            for (size_t i = 0; i < count; i++)
                data[i] = initItems[i];
        }

        ~Array()
        {
            if (data != buf)
                delete [] data;
        }

        void Reserve(size_t newCapacity)
        {
            if (newCapacity > capacity)
            {
                T* newData = new T[newCapacity];

                for (size_t i = 0; i < used; i++)
                    newData[i] = data[i];

                if (data != buf)
                    delete [] data;

                data = newData;
                capacity = newCapacity;
            }
        }

        void Trim()
        {
            if (data != buf)
            {
                if (used <= BufSize)
                {
                    for (size_t i = 0; i < used; i++)
                        buf[i] = data[i];

                    delete [] data;
                    data = buf;
                    capacity = BufSize;
                }
                else if (used < capacity)
                {
                    T* newData = new T[used];

                    for (size_t i = 0; i < used; i++)
                        newData[i] = data[i];

                    delete [] data;
                    data = newData;
                    capacity = used;
                }
            }
        }

        void Resize(size_t newSize)
        {
            Reserve(newSize);
            used = newSize;
        }

        void Expand()
        {
            if (used > capacity / 2)
                Reserve(capacity * 2);
        }

        size_t Length() const
        {
            return used;
        }

        size_t FirstIndex() const
        {
            return 0;
        }

        size_t LastIndex() const
        {
            return used - 1;
        }

        size_t Add()
        {
            if (capacity <= used)
                Reserve(capacity * 2);

            return used++;
        }

        size_t Add(const T& item)
        {
            if (capacity <= used)
                Reserve(capacity * 2);

            data[used] = item;
            return used++;
        }

        size_t AddRange(const T* items, size_t count)
        {
            size_t res = used;

            for (size_t i = 0; i < count; i++)
                Add(items[i]);

            return res;
        }

        size_t AddRange(const Array<T>& values)
        {
            return AddRange(values.data, values.used);
        }

        size_t Insert(size_t index, const T& item)
        {
#ifdef DEBUG
            if (index > used)
                throw IndexOutOfRangeException();
#endif
            Add(item);

            for (size_t i = used - 1; i > index; i--)
                data[i] = data[i - 1];

            data[index] = item;
            return index;
        }

        int IndexOf(const T& value) const
        {
            for (size_t i = 0; i < used; i++)
                if (data[i] == value)
                    return (int)i;

            return -1;
        }

        int LastIndexOf(const T& elm) const
        {
            for (int i = (int)used - 1; i >= 0; i--)
                if (data[i] == elm)
                    return (int)i;

            return -1;
        }

        bool Contains(const T& value) const
        {
            int i = IndexOf(value);
            return i != -1;
        }

        void RemoveAt(size_t index)
        {
#ifdef DEBUG
            if (index >= used)
                throw IndexOutOfRangeException();
#endif
            for (size_t i = index; i < used - 1; i++)
                data[i] = data[i + 1];

            used--;
        }

        bool Remove(const T& item)
        {
            size_t i = IndexOf(item);

            if (i != -1)
            {
                RemoveAt(i);
                return true;
            }

            return false;
        }

        void RemoveAll(const T& item)
        {
            while (Remove(item));
        }

        void SwapRemoveAt(size_t i)
        {
            Swap(Get(i), Last());
            used--;
        }

        void RemoveRange(size_t start, size_t count)
        {
#ifdef DEBUG
            if (start >= used || start + count > used)
                throw IndexOutOfRangeException();
#endif
            for (size_t i = start; i < used - count; i++)
                data[i] = data[i + count];

            used -= count;
        }

        T RemoveFirst()
        {
            T temp = First();
            RemoveAt(0);
            return temp;
        }

        T RemoveLast()
        {
            T temp = Last();
            RemoveAt(used-1);
            return temp;
        }

        T& Get(size_t index)
        {
#ifdef DEBUG
            if (index >= used)
                throw IndexOutOfRangeException();
#endif
            return data[index];
        }

        const T& Get(size_t index) const
        {
#ifdef DEBUG
            if (index >= used)
                throw IndexOutOfRangeException();
#endif
            return data[index];
        }

        void Clear()
        {
            used = 0;
        }

        T& operator [](size_t index)
        {
            return Get(index);
        }

        const T& operator [](size_t index) const
        {
            return Get(index);
        }

        T* Ptr()
        {
            return data;
        }

        const T* Ptr() const
        {
            return data;
        }

        T& First()
        {
            return Get(0);
        }

        const T& First() const
        {
            return Get(0);
        }

        T& Last()
        {
            return Get(used - 1);
        }

        const T& Last() const
        {
            return Get(used - 1);
        }

        void Reverse()
        {
            for (size_t a = 0, b = used; a < --b; a++)
                Swap(data[a], data[b]);
        }
    };

    /** \ingroup uContainer */
    template <typename T> class ArrayRef: public Object
    {
    public:
        Array<T> Data;
    };
}
