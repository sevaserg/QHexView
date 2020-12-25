#include "bytelog.h"
#include <malloc.h>

template <typename T>
byteLog<T>::byteLog()
{
    lineSize_ = 20;
    int maximum_ = 200000;
    this->data_ = static_cast<T*>(malloc(0));
    this->size_ = 0;
}

template <typename T>
int byteLog<T>::size()
{
    return size_;
}

template <typename T>
int byteLog<T>::lineSize()
{

}

template <typename T>
void byteLog<T>::setLineSize(int newLineSize)
{
    lineSize_ = newLineSize;
}

template <typename T>
int byteLog<T>::max()
{
    return maximum_;
}

template <typename T>
void byteLog<T>::setMax(int newMax)
{
    if (size_ > newMax)                                                 // Если новый максимальный размер меньше числа элементов
    {
        T* tmp = static_cast<T*>( malloc(newMax*sizeof(T)) );           //создаём массив tmp величиной с новый максимум
        memmove(tmp,data_+(maximum_ - newMax - 1)*sizeof(T), newMax);   //переносим все последние влезающие значения из data_
        size_ = newMax;                                                 //меняем размер на новый максимум
        free(data_);                                                    //меняем старый "большой" массив
        data_ = tmp;                                                    //на новый
    }
    maximum_ = newMax;                                                  //максимум меняем
}

template <typename T>
void byteLog<T>::setMax(int linesAmt, int lineSize)
{
    lineSize_ = lineSize;
    setMax(linesAmt*lineSize);
}

template <typename T>
void byteLog<T>::push(T data)
{
    if (size_ != maximum_)                                      //в случае, если элемент влезает
    {
        data_ = static_cast<T*>( realloc(data_, ++size_) );     //просто расширяем массив на один элемент
    }
    else //иначе сдвигаем элементы в массиве влево на "строку" (n позиций)
    {
        T* tmp = static_cast<T*>( malloc((size_ - lineSize_)*sizeof(T)));       //создаём такой же по размеру массив
        memmove( tmp, data_+sizeof(T), (size_-lineSize_)*sizeof(T) );           // переносим все элементы начиная с n-того, наким образом последний элемент свободен
        free(data_);                                                            //удаляем старый массив
        data_ = tmp;                                                            //присваиваем новый
        size_-=lineSize_+1;                                                       //Не забываем указать в size_, что массив стал на строку легче. Плюс один элемент.
    }
    data_[size_-1] = data;                                                      //в последнюю ячейку кладём элемент
}

template <typename T>
int byteLog<T>::push(T* data, int amt)
{
    if (amt > maximum_)                                                             //возвращаем -1, если вталкваемое значение больше максимума.
        return -1;
    if (amt <= maximum_-size_)                                                      //если обрезать ничего не потребуется,
    {
        data_ = static_cast<T*>(realloc(data_,static_cast<size_t>(size_+amt)));     //расширяем массив с содержимым лога
        memmove(data_+static_cast<size_t>(size_), data, static_cast<size_t>(amt));  //и кладём в конец новые данные
        size_+=amt;                                                                 //увеличиваем размер на число новых значений
        return 0;                                                                   //Сдвигать ничего не потребовалось.
    }
    else //если сдвигать всё же требуется
    {
        int shifts = amt / lineSize_;   //здесь учитываем, сколько сдвигов строк нам нужно
        int k = size_ % lineSize_;      //сколько в последней строке элементов и так находится
        int l = amt % lineSize_;        //сколько элементов в последней строке нового сообщения
        if (k+l > lineSize_ || size_ == maximum_)            //Если суммарно элементов в последних строках оказалось больше, производим ещё один сдвиг
            shifts++;
        T* tmp = static_cast<T*>( malloc(static_cast<size_t>(size_ + amt - shifts * lineSize_)) ); //создаём такой же массив
        memmove(tmp, data_ + (shifts) * lineSize_, static_cast<size_t>(size_ - shifts * lineSize_)); //переносим туда лог, но без последних строк
        memmove(tmp+(size_ - shifts * lineSize_), data, static_cast<size_t>(amt));                       //переносим новую информацию
        size_ += amt - shifts * lineSize_; //пересчитываем размер
        free(data_); //удаляем старый массив
        data_ = tmp; //меняем на новый
        return shifts; //возвращаем число строк, на которые, в случае чего, требуется сдвинуть курсор
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
T* byteLog<T>::getLine(int lineNum)
{
    T* tmp = static_cast<T>( malloc(lineSize_*sizeof(T)) );
    memmove(tmp, data_+lineNum*lineSize_*sizeof(T), lineSize_*sizeof(T));
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
