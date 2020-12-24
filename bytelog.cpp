#include "byteLog.h"
#include <malloc.h>

template <typename T>
byteLog<T>::byteLog()
{
    this->data_ = static_cast<T*>(malloc(0));
    this->size_ = 0;
}

template <typename T>
int byteLog<T>::size()
{
    return size_;
}

template <typename T>
int byteLog<T>::max()
{
    return maximum_;
}

template <typename T>
void byteLog<T>::changeMax(int newMax)
{
    if (size_ > newMax)
    {
        T* tmp = static_cast<T*>( malloc(newMax*sizeof(T)) );
        memmove(tmp,data_+(maximum_ - newMax - 1)*sizeof(T), newMax);
        size_ = newMax;
        free(data_);
        data_ = tmp;
    }
    maximum_ = newMax;
}

template <typename T>
void byteLog<T>::push(T data)
{
    if (size_ != maximum_)
    {
        data_ = static_cast<T*>( realloc(data_, ++size_) );
    }
    else
    {
        T* tmp = static_cast<T*>( malloc(sizeof(data)) );
        memmove( tmp, data_+sizeof(T), (size_-1)*sizeof(T) ); // '0123' in data_, '123#' in tmp
        free(data_);
        data_ = tmp;
    }
    data_[size_-1] = data;
}

template <typename T>
int byteLog<T>::push(T* data, int amt)
{
    if (amt > maximum_)
        return -1;
    if (amt <= maximum_-size_)//works
    {
        data_ = static_cast<T*>(realloc(data_,static_cast<size_t>(size_+amt)));
        memmove(data_+static_cast<size_t>(size_), data, static_cast<size_t>(amt));
        size_+=amt;
        return 0;
    }
    else
    {
        T* tmp = static_cast<T*>( malloc(static_cast<size_t>(size_)) );
        memmove(tmp, data_+amt, static_cast<size_t>(size_ - amt));
        memmove(tmp+(size_ - amt), data, static_cast<size_t>(amt));
        int shift = (size_+amt) % maximum_;
        if (size_ + amt > maximum_)
            size_ = maximum_;
        else
            size_ += amt;
        free(data_);
        data_ = tmp;
        return shift;
    }
}

template <typename T>
T byteLog<T>::get(int elementNum)
{
    if (elementNum >= size_)
        return 0;
    return(data_[elementNum]);
}

template <typename T>
T* byteLog<T>::get(int lineNum, int lineSize)
{
    T* tmp = static_cast<T>( malloc(lineSize*sizeof(T)) );
    memmove(tmp, data_+lineNum*lineSize*sizeof(T), lineSize*sizeof(T));
    return tmp;
}

template <typename T>
void byteLog<T>::clear()
{
    free(data_);
    data_ = static_cast<T*>(malloc(0));
    size_ = 0;
}

template <typename T>
byteLog<T>::~byteLog()
{
    free(data_);
}
