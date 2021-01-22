#include "bytelog.h"
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <iostream>
#include <QElapsedTimer>

using namespace std;

byteLog::byteLog()
{
    selectAll_ = false;
    lineSize_ = 20;
    maximum_ = 1500;
    this->FSIAS_ = static_cast<int*>(malloc(0));
    this->data_ = static_cast<unsigned char*>(malloc(0));
    this->size_ = 0;
    this->FSIASSize_ = 0;
    asciiShift_ = 0;
    firstSel_ = -1;
    secondSel_ = -1;
    asciiSel1_ = new int[3];
    asciiSel2_ = new int[3];
    asciiSel1_[0] = -1;
    asciiSel2_[0] = -1;
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
    clear();
    maximum_ = newMax;
}

void byteLog::setMax(int linesAmt, int lineSize)
{
    lineSize_ = lineSize;
    lines_ = linesAmt;
    clear();
    size_ = 0;
    maximum_ = linesAmt*lineSize;

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
    redoAsciiChoice(firstSel_, asciiSel1_);
}

void byteLog::setSecondSel(int num)
{
    secondSel_ = num;
    redoAsciiChoice(secondSel_, asciiSel2_);
}

int byteLog::firstSel()
{
    return firstSel_;
}

int byteLog::secondSel()
{
    return secondSel_;
}

int byteLog::asciiSel1Line()
{
    return asciiSel1_[1];
}

int byteLog::asciiSel2Line()
{
    return asciiSel2_[1];
}

int byteLog::asciiSel1Sym()
{
    return asciiSel1_[2];
}

int byteLog::asciiSel2Sym()
{
    return asciiSel2_[2];
}

int byteLog::asciiSel1Active()
{
    return asciiSel1_[0];
}

int byteLog::asciiSel2Active()
{
    return asciiSel2_[0];
}

void byteLog::redoAsciiChoice(int sel, int *asel)
{
    int line = 0, sym = 0, avail = 0;
    if (sel < 0)
    {
        avail = -1;
    }
    else
    {
        for (int i = 0; i < sel; i++)
        {
            sym++;
            if (data_[i] == '\n')
            {
                line++;
                sym = 0;
            }
        }
    }
    asel[0] = avail;
    asel[1] = line;
    asel[2] = sym;
}

void byteLog::setSelectAll(bool sel)
{
    selectAll_ = sel;
}

bool byteLog::selectAll()
{
    return selectAll_;
}

int byteLog::ALNToBLN(int ALN) // ASCII Line Number to Byte Line Number
{
    return(FSIAS_[ALN] / lineSize_);
}

int byteLog::BLNToALN(int BLN) // Byte Line Number to ASCII Line Number
{
    for (int i = 0; i < FSIASSize_; i++)
    {
        if (FSIAS_[i] >= BLN*lineSize_)
        {
            return i;
        }
    }
    return 0;
}

int byteLog::push(const unsigned char* data, int amt)
{
    if (amt > maximum_) return -1;
    if (amt == 0) return 0;
    if (FSIASSize_ == 0)
    {
        FSIAS_ = static_cast<int*>(malloc(sizeof(int)));
        FSIASSize_++;
        FSIAS_[0] = 0;
    }
    else
    {
        if(data_[size_-1] == '\n')
        {
            FSIASSize_++;
            FSIAS_ = static_cast<int*>(realloc(FSIAS_, sizeof(int)*static_cast<size_t>(FSIASSize_)));
            FSIAS_[FSIASSize_-1] = size_;
        }
    }
    int tmpvl = FSIAS_[FSIASSize_-1];
    for (int i = 1; i < amt; i++)
    {
        if (data[i-1] == '\n')
        {
//=================================
            FSIASSize_++;
            FSIAS_ = static_cast<int*>(realloc(FSIAS_, sizeof(int)*static_cast<size_t>(FSIASSize_)));
            FSIAS_[FSIASSize_-1] = i+tmpvl;
//=================================
        }
    }
    if (amt <= maximum_-size_)                                                      //если обрезать ничего не потребуется,
    {
        data_ = static_cast<unsigned char *>(realloc(data_,static_cast<size_t>(size_+amt)));     //расширяем массив с содержимым лога
        memmove(data_+static_cast<size_t>(size_), data, static_cast<size_t>(amt));  //и кладём в конец новые данные
        size_+=amt;                                                                 //увеличиваем размер на число новых значений



        //updateFSIAS();
        return 0;
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
        unsigned char *delTmp = static_cast<unsigned char*>(malloc(static_cast<size_t>(shifts * lineSize_)));
        memmove(delTmp, data_, static_cast<size_t>(shifts * lineSize_));
        for (int i = 0; i < shifts * lineSize_; i++) //считаем число сдвигов по ascii (число выдвинутых \n)
        {
            if (delTmp[i] == '\n')
                asciiShift_++;
        }
        free(delTmp);
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
        FSIASSize_ -= asciiShift_;
        int *temp = static_cast<int*>(malloc(sizeof(int) * static_cast<size_t>(FSIASSize_)));
        memmove(temp, FSIAS_+ asciiShift_, static_cast<size_t>(FSIASSize_));
        //cout << "Copied! asciiShift = " << asciiShift_ <<", FSIASSize_ = " << FSIASSize_ << endl;
        free(FSIAS_);
        FSIAS_ = temp;
        //cout << "tmpval = " << FSIAS_[0] << endl;
        int tmpval = FSIAS_[0];
        //можно попробовать ускорить с помощью OpenMP.
        for (int i = 0; i < FSIASSize_; i++)
            FSIAS_[i]-=tmpval;
        updateFSIAS();
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
    unsigned char *buffer = static_cast<unsigned char*>(malloc(static_cast<size_t>(lineSize_+1)));
    memmove(buffer, data_+lineNum*lineSize_, static_cast<size_t>(lineSize_));
    if (lineNum < (size_ / lineSize_))
        buffer[lineSize_] = '\n';
    return buffer;
}

void byteLog::clear()
{
    free(data_);
    data_ = static_cast<unsigned char*>(malloc(0));
    FSIAS_ = static_cast<int*>(malloc(0));
    FSIASSize_ = 0;
    size_ = 0;
}

unsigned char* byteLog::data()
{
    return data_;
}

unsigned char byteLog::at(int pos)
{
    if (pos < 0 || pos >= size_)
        return '\0';
    return data_[pos];
}

int byteLog::asciiLines()
{
    return FSIASSize_;
}

unsigned char* byteLog::asciiLine(int lineNum)
{

    size_t pos1 = 0, pos2 = 0;
    unsigned char* ret;
    if (lineNum > FSIASSize_ || size_ == 0 || FSIASSize_ == 0)
    {
        ret = static_cast<unsigned char*>(malloc(1));
        ret[0] = '\0';
        return ret;
    }
    pos1 = static_cast<size_t>(FSIAS_[lineNum-1]);
    if(FSIASSize_ <= lineNum)
    {
        pos2 = static_cast<size_t>(size_);
    }
    else
    {
         pos2 = static_cast<size_t>(FSIAS_[lineNum]);
    }
    ret = static_cast<unsigned char*>(malloc(static_cast<size_t>(pos2-pos1)));
    //cout << "==================="<<endl;
    //cout << "FSIAS:" << endl;
    //for(int i = 0; i < FSIASSize_; i++)
    {
        //cout << i + 1<< ":\t" << FSIAS_[i] << endl;
    }
    //cout << "==================="<<endl;
    //cout << "pos1 = FSIAS_[" << lineNum-1 << "], pos2 = FSIAS_[" << lineNum << "], FSIASSize_ = " << FSIASSize_<< endl;
    //cout << "pos1 = " << pos1 << ", pos2 = " << pos2 << endl;
    memmove(ret, data_+pos1, pos2-1-pos1);
    ret[pos2-pos1-1] = '\0';
    return ret;
}

int byteLog::asciiLineLen(int lineNum)
{
    if (lineNum >= FSIASSize_)
        return 0;
    else
        if (lineNum < FSIASSize_-1)
        {
            return (FSIAS_[lineNum+1] - FSIAS_[lineNum]);
        }
        else
        {
            return (size_ - FSIAS_[lineNum]);
        }
}

int byteLog::getFirstSymInAsciiLine(int lineNum)
{
    if (lineNum >= FSIASSize_)
        return -1;
    else return FSIAS_[lineNum];
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

char* byteLog::getData(int first, int last)
{
    int op = min(first, size_);
    int ed = min(last, size_)+1;
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
    delete[]asciiSel1_;
    delete[]asciiSel2_;
    free(data_);
    free(FSIAS_);
}

void byteLog::updateFSIAS()
{
    free(FSIAS_);
    FSIASSize_ = 1;
    FSIAS_ = static_cast<int*>(malloc(sizeof(int)));
    FSIAS_[0] = 0;
    if (size_ > 0)
    {
        for (int i = 1; i < size_; i++)
        {
            if (data_[i-1] == '\n')
            {
                FSIASSize_++;
                FSIAS_ = static_cast<int*>(realloc(FSIAS_, FSIASSize_*sizeof(int)));
                FSIAS_[FSIASSize_-1] = i;
            }
        }

    }

}
