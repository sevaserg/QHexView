#include "bytelog.h"
#include <stdlib.h>
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
    asciiShift_ = 0;
    firstSel_ = -1;
    secondSel_ = -1;
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

int byteLog::max_()
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

void byteLog::setFirstSel(int num)
{
    firstSel_ = num;
}

void byteLog::setSecondSel(int num)
{
    secondSel_ = num;
}

int byteLog::firstSel()
{
    return firstSel_;
}

int byteLog::secondSel()
{
    return secondSel_;
}

int byteLog::lastAsciiSel1()
{
    return lastAsciiSel1_;
}

int byteLog::lastAsciiSel2()
{
    return lastAsciiSel2_;
}

int byteLog::ALNToBLN(int ALN) // ASCII Line Number to Byte Line Number
{
    int i = 0, cntr = 0;
    for (i = 0; i < size_; i++)
    {
        if (data_[i] == '\n')
            cntr++;
        if (cntr == ALN)
            break;
    }
    return i / lineSize_;
}

int byteLog::BLNToALN(int BLN) // Byte Line Number to ASCII Line Number
{
    int i = 0, cntr = 0;
    for (i = 0; i < BLN * lineSize_; i++)
    {
        if (data_[i] == '\n')
            cntr++;
    }
    return cntr;
}

int byteLog::push(const unsigned char* data, int amt)
{

    if (amt > maximum_)                                                             //возвращаем -1, если вталкваемое значение больше максимума.
    {

       return -1;
    }
    if (amt <= maximum_-size_)                                                      //если обрезать ничего не потребуется,
    {
        data_ = static_cast<unsigned char *>(realloc(data_,static_cast<size_t>(size_+amt)));     //расширяем массив с содержимым лога

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
        unsigned char* tmp = static_cast<unsigned char*>( malloc(static_cast<size_t>(size_ + amt - shifts * lineSize_)) ); //создаём такой же массив
        memmove(tmp, data_ + (shifts) * lineSize_, static_cast<size_t>(size_ - shifts * lineSize_)); //переносим туда лог, но без последних строк
        memmove(tmp+(size_ - shifts * lineSize_), data, static_cast<size_t>(amt));                       //переносим новую информацию

        for (int i = 0; i < (shifts) * lineSize_; i++) //считаем число сдвигов по ascii (число выдвинутых \n)
        {
            if (data_[i] == '\n')
                asciiShift_++;
        }

        size_ += amt - shifts * lineSize_; //пересчитываем размер
        free(data_); //удаляем старый массив
        data_ = tmp; //меняем на новый
        if (firstSel_ >= 0)
        {
            firstSel_ -= shifts*lineSize_;
            if (firstSel_ < 0)
                firstSel_ = 0;
        }
        if (secondSel_ >= 0)
        {
            secondSel_ -= shifts*lineSize_;
            if (secondSel_ < 0)
                secondSel_ = 0;
        }
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
    size_t pos1 = 0, pos2 = 0;
    unsigned char* ret;
    int cnt = 0;
    for (int i = 0; i < size_; i++)
    {
        if ((data_[i] == '\n') || (i == size_-1))
        {
            pos1 = pos2;
            pos2 = static_cast<size_t>(i);
            cnt++;
        }
        if (cnt == lineNum)
            break;
    }
    if (pos1 == pos2 || pos1 == pos2-1)
        return NULL;
    if ((pos1 >= static_cast<size_t>(firstSel_) && pos2 <= static_cast<size_t>(firstSel_)) || (pos1 <= static_cast<size_t>(firstSel_) && pos2 >= static_cast<size_t>(firstSel_)))
    {
        lastAsciiSel1_ = firstSel_ - static_cast<int>(pos1);
    }
    else
    {
        lastAsciiSel1_ = -1;
    }
    if ((pos1 >= static_cast<size_t>(secondSel_) && pos2 <= static_cast<size_t>(secondSel_)) || (pos1 <= static_cast<size_t>(secondSel_) && pos2 >= static_cast<size_t>(secondSel_)))
    {
        lastAsciiSel2_ = secondSel_ - static_cast<int>(pos1);
    }
    else
    {
        lastAsciiSel2_ = -1;
    }
    ret = static_cast<unsigned char*>(malloc(static_cast<size_t>(pos2-pos1)));
    memcpy(ret, data_+pos1+(pos1 == 0 ? 0:1), pos2-pos1);
    ret[pos2-pos1-1] = '\0';

    return ret;
}

int byteLog::asciiLineLen(int lineNum)
{
    size_t pos1 = 0, pos2 = 0;
    int cnt = 0;
    for (int i = 0; i < size_; i++)
    {
        if ((data_[i] == '\n') || (i == size_-1))
        {
            pos1 = pos2;
            pos2 = static_cast<size_t>(i);
            cnt++;
        }
        if (cnt == lineNum)
            break;
    }
    return static_cast<int>(pos2-pos1);
}

int byteLog::getFirstSymInAsciiLine(int lineNum)
{
    if (lineNum == 0)
        return -1;
    size_t pos = 0;
    int cnt = 0;
    for (int i = 0; i < size_; i++)
    {
        if ((data_[i] == '\n') || (i == size_-1))
        {
            pos = static_cast<size_t>(i+1);
            cnt++;
        }
        if (cnt == lineNum)
            break;
    }
    return static_cast<int>(pos);
}

char* byteLog::getHighlighted()
{
    int op = min(firstSel_, secondSel_);
    int ed = max(firstSel_, secondSel_)+1;
    char* rt = static_cast<char*>(malloc(static_cast<size_t>(ed-op+1)*sizeof(char)));
    memcpy(rt,data_+op, static_cast<size_t>(ed-op));
    rt[ed-op] = '\0';
    return rt;
}

int byteLog::linesAmt()
{
    return size_ / lineSize_;
}

int byteLog::lastLineSize()
{
    return size_ % lineSize_;
}

int byteLog::asciiShift()
{
    int tmp = asciiShift_;
    asciiShift_ = 0;
    return tmp;
}

byteLog::~byteLog()
{
    free(data_);
}
