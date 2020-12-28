#include "bytelog.h"
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>


#include <iostream>
    using namespace std;


byteLog::byteLog()
{
    lineSize_ = 20;
    maximum_ = 200000;
    this->data_ = static_cast<unsigned char*>(malloc(0));
    this->size_ = 0;
    buffer = static_cast<unsigned char*>(malloc(0));
    asciiShift = 0;
}

int byteLog::size()
{
    return size_;
}

int byteLog::lineSize()
{
    return lineSize_;
}

void byteLog::setLineSize(int newLineSize)
{
    lineSize_ = newLineSize;
}

int byteLog::max()
{
    return maximum_;
}

void byteLog::setMax(int newMax)
{
    if (size_ > newMax)                                                 // Если новый максимальный размер меньше числа элементов
    {
        unsigned char* tmp = static_cast<unsigned char *>( malloc(static_cast<size_t>(newMax)) );           //создаём массив tmp величиной с новый максимум
        memmove(tmp,data_+(maximum_ - newMax - 1), static_cast<size_t>(newMax));   //переносим все последние влезающие значения из data_
        size_ = newMax;                                                 //меняем размер на новый максимум
        free(data_);                                                    //меняем старый "большой" массив
        data_ = tmp;                                                    //на новый
    }
    maximum_ = newMax;                                                  //максимум меняем
}

void byteLog::setMax(int linesAmt, int lineSize)
{
    lineSize_ = lineSize;
    setMax(linesAmt*lineSize);
}

int byteLog::maxLines()
{
    return lines_;
}

void byteLog::push(unsigned char data)
{
    if (size_ != maximum_)                                      //в случае, если элемент влезает
    {
        data_ = static_cast<unsigned char*>( realloc(data_, static_cast<size_t>(++size_)) );     //просто расширяем массив на один элемент
    }
    else //иначе сдвигаем элементы в массиве влево на "строку" (n позиций)
    {
        unsigned char* tmp = static_cast<unsigned char*>( malloc(static_cast<size_t>(size_ - lineSize_)));       //создаём такой же по размеру массив
        memmove( tmp, data_+sizeof(unsigned char), static_cast<size_t>(size_-lineSize_) );           // переносим все элементы начиная с n-того, наким образом последний элемент свободен
        free(data_);                                                            //удаляем старый массив
        data_ = tmp;                                                            //присваиваем новый
        size_-=lineSize_+1;                                                       //Не забываем указать в size_, что массив стал на строку легче. Плюс один элемент.
    }
    data_[size_-1] = data;                                                      //в последнюю ячейку кладём элемент
}

int byteLog::push(const unsigned char* data, int amt)
{

    if (amt > maximum_)                                                             //возвращаем -1, если вталкваемое значение больше максимума.
    {

       return -1;
    }
    cout << "data accepted!"<< endl;
    if (amt <= maximum_-size_)                                                      //если обрезать ничего не потребуется,
    {
        cout << "NO SHIFT. SIZE: " << size_ << endl;
        cout << "============-------------------" << endl;
        data_ = static_cast<unsigned char *>(realloc(data_,static_cast<size_t>(size_+amt)));     //расширяем массив с содержимым лога

        memmove(data_+static_cast<size_t>(size_), data, static_cast<size_t>(amt));  //и кладём в конец новые данные
        size_+=amt;                                                                 //увеличиваем размер на число новых значений
        return 0;                                                                   //Сдвигать ничего не потребовалось.

    }

    else //если сдвигать всё же требуется
    {
        cout << "SHIFT. SIZE: " << size_ << endl;
        cout << "============-----------==========" << endl;

        int shifts = amt / lineSize_;   //здесь учитываем, сколько сдвигов строк нам нужно
        int k = size_ % lineSize_;      //сколько в последней строке элементов и так находится
        int l = amt % lineSize_;        //сколько элементов в последней строке нового сообщения
        if (k+l > lineSize_ || size_ == maximum_)            //Если суммарно элементов в последних строках оказалось больше, производим ещё один сдвиг
            shifts++;
        unsigned char* tmp = static_cast<unsigned char*>( malloc(static_cast<size_t>(size_ + amt - shifts * lineSize_)) ); //создаём такой же массив
        memmove(tmp, data_ + (shifts) * lineSize_, static_cast<size_t>(size_ - shifts * lineSize_)); //переносим туда лог, но без последних строк
        memmove(tmp+(size_ - shifts * lineSize_), data, static_cast<size_t>(amt));                       //переносим новую информацию
        size_ += amt - shifts * lineSize_; //пересчитываем размер
        free(data_); //удаляем старый массив
        data_ = tmp; //меняем на новый

        //Считаем сдвиги по ASCII

        return shifts; //возвращаем число строк, на которые, в случае чего, требуется сдвинуть курсор
    }
}

unsigned char byteLog::get(int elementNum)
{
    if (elementNum >= size_)
        return 0;
    return(data_[elementNum]);
}

unsigned char* byteLog::getLine(int lineNum)
{
    buffer = static_cast<unsigned char*>(malloc(static_cast<size_t>(lineSize_+1)));
    memmove(buffer, data_+lineNum*lineSize_, static_cast<size_t>(lineSize_));
    if (lineNum < (size_ / lineSize_))
        buffer[lineSize_] = '\n';
    return buffer;
}

void byteLog::clear()
{
    free(data_);
    data_ = static_cast<unsigned char*>(malloc(0));
    size_ = 0;
}

unsigned char* byteLog::data()
{
    return data_;
}

unsigned char byteLog::at(int pos)
{
    if (pos < 0 || pos >= size_)
        return NULL;
    return data_[pos];
}

unsigned char* byteLog::toText(int ps)
{
    unsigned char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    //if (ps == 0)
    {
        unsigned char* tmp = static_cast<unsigned char *>(malloc((static_cast<size_t>(size_ + lines_ ))));
        for(int i = 0; i < lines_; i++)
        {
            memcpy(tmp+i*(lineSize_+1),data_+i*lineSize_,static_cast<size_t>(lineSize_));
            for (int j = i*lineSize_-1; j < ( i != lines_-1 ? (i+1)*lineSize_ : i + size_%lineSize_); j++)
            {
                if (tmp[j] >= 126 && tmp[j] < 30)
                    tmp[j] = '.';
            }
            tmp[lineSize_*i] = '\n';
        }
        tmp = static_cast<unsigned char *>(realloc(tmp, static_cast<size_t>(size_ + lines_ -1)));
        return tmp;
    }
}

int byteLog::asciiLines()
{
    int lns = 0;
    for (int i = 0; i < size_; i++)
        if (data_[i] == '\n')
            lns++;
    return lns;
}

unsigned char* byteLog::asciiLine(int lineNum)
{
    int pos1 = 0, pos2 = 0;
    unsigned char* ret;
    int cnt = 0;
    for (int i = 0; i < size_; i++)
    {
        if ((data_[i] == '\n') || (i == size_-1))
        {
            pos1 = pos2;
            pos2 = i;
            cnt++;
        }
        if (cnt == lineNum)
            break;
    }
    if (pos1 == pos2)
        return NULL;
    ret = static_cast<unsigned char*>(malloc(static_cast<size_t>(pos2-pos1)));
    memcpy(ret, data_+pos1+1, pos2-pos1);
    /*for (int i = pos1+1, j = 0; i < pos2; i++, j++)
    {
        ret[j] = data_[i];
    }*/
    return ret;
}

int byteLog::linesAmt()
{
    return size_ / lineSize_;
}

int byteLog::lastLineSize()
{
    return size_ % lineSize_;
}

byteLog::~byteLog()
{
    free(data_);
}
