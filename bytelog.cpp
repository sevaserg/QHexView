#include "bytelog.h"

byteLog::byteLog()
{
    selectAll_ = false;
    lineSize_ = 20;
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

void byteLog::setMax(int linesAmt, int lineSize)
{
    lineSize_ = lineSize;
    lines_ = linesAmt;
    clear();
    size_ = 0;
}

int byteLog::maxLines()
{
    return lines_;
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

bool byteLog::asciiSel1Active()
{
    return asciiSel1_[0] > 0;
}

bool byteLog::asciiSel2Active()
{
    return asciiSel2_[0] > 0;
}

void byteLog::redoAsciiChoice(int sel, int *asel)
{
    asel[0] = 1; //Активен ли
    asel[1] = 0; //Номер строки
    asel[2] = 0; //Номер символа в строке

    if (sel < 0 || sel >= size_) asel[0] = -1; //Если номер символа меньше нуля
    //или больше числа символов (то есть нет такого номера), выделение
    //перестаёт быть активным. Проще говоря, чтобы снять выделение, шлём sel,
    //равный -1
    else
        for (int i = 0; i < FSIASSize_; i++)
            if ((FSIAS_[i] <= sel && FSIAS_[i+1] > sel) || i == FSIASSize_-1)
            {
                asel[1] = i;
                asel[2] = sel - FSIAS_[i];
                break;
            }
}

void byteLog::setSelectAll(bool sel)
{
    selectAll_ = sel;
}

bool byteLog::selectAll()
{
    return selectAll_;
}

int byteLog::ALNToBLN(int ALN)
{
    return(FSIAS_[ALN] / lineSize_);
}

int byteLog::BLNToALN(int BLN)
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
    if (amt > lineSize_*lines_) return -1; // игнорируем всё, что больше лога
    if (amt == 0) return 0; //Если класть нечего, то выходим из цикла
    if (FSIASSize_ == 0) //Если FSIAS совершенно пуст, создаём хотя бы одну
    //переменную в нём. Если информация не была забракована ранее, то что-то
    //в лог будет положено, а значит минимум одна строка там точно будет.
    //Следовательно, в FSIAS_ должна быть одна переменная.
    {
        FSIAS_ = static_cast<int*>(realloc(FSIAS_, sizeof(int)));
        FSIASSize_++;
        FSIAS_[0] = 0;
    }
    else //в противном случае смотрим, является ли последний символ лога '\n'.
    // Если является, добавляем новую запись в FSIAS_ о том, что первый символ
    // вталкиваемой информации начинает новую строку.
    {
        if(data_[size_-1] == '\n')
        {
            FSIASSize_++;
            FSIAS_ = static_cast<int*>(realloc(FSIAS_, sizeof(int)*static_cast<size_t>(FSIASSize_)));
            FSIAS_[FSIASSize_-1] = size_;
        }
    }
    for (int i = 1; i < amt; i++) //добавляем в лог новые значения.
    //Их считаем по вталкиваемым данным. Находим в data новые строки,
    //прибавляем к их номеру текущий размер массива
    if (data[i-1] == '\n')
    {
        FSIASSize_++;
        FSIAS_ = static_cast<int*>(realloc(FSIAS_, sizeof(int)*static_cast<size_t>(FSIASSize_)));
        FSIAS_[FSIASSize_-1] = i+size_;
    }
    if (amt <= lineSize_*lines_-size_) //если обрезать ничего не потребуется,
    {
        data_ = static_cast<unsigned char *>(realloc(data_,static_cast<size_t>(size_+amt)));  //расширяем массив с содержимым лога
        memmove(data_+static_cast<size_t>(size_), data, static_cast<size_t>(amt));  //и кладём в конец новые данные
        size_+=amt; //увеличиваем размер на число новых значений
        return 0;
    }
    else //если сдвигать всё же требуется
    {
        int shifts = amt / lineSize_;   //здесь учитываем, сколько сдвигов строк нам нужно
        int k = size_ % lineSize_;      //сколько в последней строке элементов и так находится
        int l = amt % lineSize_;        //сколько элементов в последней строке нового сообщения
        if (k+l > lineSize_ || size_ == lineSize_*lines_)            //Если суммарно элементов в последних строках оказалось больше, производим ещё один сдвиг
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
        if (firstSel_ >= 0) //двигаем выбор, если таковой был.
        {
            firstSel_ -= shifts*lineSize_;
            if (firstSel_ < 0) //Если ушли в минус, выделяем нулевой символ.
                firstSel_ = 0;
        }
        if (secondSel_ >= 0)
        {
            secondSel_ -= shifts*lineSize_;
            if (secondSel_ < 0)
                secondSel_ = 0;
        }
        redoAsciiChoice(firstSel_, asciiSel1_);
        redoAsciiChoice(secondSel_, asciiSel2_);
        //Сдвигаем первые символы в строке.
        //0.	Считаем кол-во уехавших символов, условно переменная sa
        int sa = shifts * lineSize_;
        //1.	Делаем счетчик
        int cntr = 0;
        //2.	В цикле сравниваем sa со СЛЕДУЮЩИМ значением элемента
        for (int i = 0; i < FSIASSize_ - 1; i++)
        {
        //3.	Если СЛЕДУЮЩЕЕ значение больше, вычитаем из СЛЕДУЮЩЕГО значения sa.
            if (FSIAS_[i+1] >= sa)
                FSIAS_[i+1] -= sa;
        //4.	Иначе увеличиваем счетчик
            else
            {
                cntr++;
                FSIAS_[i+1] -= sa;
            }
        }
        //5.	Сдвигаем весь массив на число в счётчике
        int *temp = static_cast<int*>(malloc(sizeof(int) * static_cast<size_t>(FSIASSize_ - cntr + 1 )));
        memmove(temp, FSIAS_+ static_cast<size_t>(cntr) + (FSIAS_[1] == 1 ? 1 : 0), static_cast<size_t>(FSIASSize_ - cntr + 1 ) * sizeof(int));
        free(FSIAS_);
        FSIAS_ = temp;
        FSIASSize_-=cntr;
        //6.	Обнуляем первый элемент
        FSIAS_[0] = 0;
        return shifts; //возвращаем число строк, на которые, в случае чего, требуется сдвинуть курсор
    }
}

void byteLog::clear()
{
    //чистим лог
    free(data_);
    data_ = static_cast<unsigned char*>(malloc(0));
    size_ = 0;
    //чистим FSIAS_
    free(FSIAS_);
    FSIAS_ = static_cast<int*>(malloc(0));
    FSIASSize_ = 0;
    //чистим выделение
    setFirstSel(-1);
    setSecondSel(-1);

}

unsigned char* byteLog::data()
{
    return data_;
}

int byteLog::asciiLines()
{
    return FSIASSize_;
}

unsigned char* byteLog::asciiLine(int lineNum) // Нумерация начинается с 1!
{
    size_t pos1 = 0, pos2 = 0;
    unsigned char* ret;
    //Первая позиция - первый символ в указанной строке
    pos1 = static_cast<size_t>(lineNum-1 < 0 ? 0 : FSIAS_[lineNum-1]);
    //Вторая позиция - либо первый символ в следующей строке, либо, если
    //такой нет, последний символ.
    pos2 = static_cast<size_t>(lineNum < FSIASSize_ ? FSIAS_[lineNum] : size_);
   /* Кидаем пустую строку в случае, если:
    * - номер запрашиваемой строки больше, чем строк в логе есть в принципе
    * - лог пустой
    * - позиции получились равны между собой
    * - первая позиция по какой-то причине стоит дальше второй
    * - первая позиция больше, чем элементов в массиве. Вторую проверять
    *   не имеет смысла, так как либо она ещё дальше, либо уже отсеялась
    *   по предыдущему пункту
    */
    if (lineNum > FSIASSize_ || size_ == 0 || pos1 >= pos2 || static_cast<int>(pos1) > size_)
    {
        ret = static_cast<unsigned char*>(malloc(1));
        ret[0] = '\0';
        return ret;
    }
    //Иначе просто кидаем запрошенную строку.
    ret = static_cast<unsigned char*>(malloc(static_cast<size_t>(pos2-pos1)));
    memmove(ret, data_+pos1, pos2-1-pos1);
    ret[pos2-pos1-1] = '\0'; //Не забываем про EOL в конце, иначе выведется ещё и мусор
    return ret; //Ещё раз: НАДО ВЫЗЫВАТЬ FREE ДЛЯ УКАЗАТЕЛЯ, КОТОРОМУ
    //ПРИСВАИВАЕТСЯ ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ!
}

int byteLog::asciiLineLen(int lineNum)
{
    if (lineNum >= FSIASSize_) // у несуществующей строки длина 0
        return 0;
    else //Существующих строк есть два типа
    {
        if (lineNum < FSIASSize_-1)
        //не последние - их длина считается, как номер первого символа в
        //строке после текущей рассматриваемой (формулировка "следующей
        //текущей рассматриваемой" была бы более корректной, но она немного
        //путает) минус номер первого символа в текущей рассматриваемой строке.
            return (FSIAS_[lineNum+1] - FSIAS_[lineNum]);
        else
        //последние - их длина считается, как размер массива минус
        //номер первого символа в текущей рассматриваемой строке.
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
    //смотрим, где начало и конец выделения
    int op = min(firstSel_, secondSel_);
    int ed = max(firstSel_, secondSel_)+1;
    //Возвращаем всё, что находится между ними
    char* rt = static_cast<char*>(malloc(static_cast<size_t>(ed-op+1)*sizeof(char)));
    memcpy(rt,data_+op, static_cast<size_t>(ed-op));
    rt[ed-op] = '\0'; //EOL не забываем
    return rt; //И СНОВА ПОВТОРЯЮ, ВЫЗЫВАЙТЕ FREE ДЛЯ ТОГО УКАЗАТЕЛЯ, КОТОРОМУ
    //ПРИСВАИВАЕТЕ ЭТО ЗНАЧЕНИЕ!
}

char* byteLog::getData(int first, int last)
{
    //Определяем начало и конец запрошенного.
    int op = min(first, size_);
    int ed = min(last, size_)+1;
    //Выделяем память
    char* rt = static_cast<char*>(malloc(static_cast<size_t>(ed-op+1)*sizeof(char)));
    memcpy(rt,data_+op, static_cast<size_t>(ed-op));
    rt[ed-op] = '\0'; //EOL, опять
    return rt; //Я знаю, что вам надоело это читать, НО FREE!
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

bool byteLog::isInSeq(int symNum, char* seq, int len)
{
    //Идём по последовательности и пытаемся найти в ней проверяемую букву
    for (int i = 0; i < len; i++)
        if (data_[symNum] == seq[i] && symNum - i >= 0 && symNum + len - i < size_)
        //если нашли, и при этом под последовательность есть место
        {
            //проверяем каждый символ в последовательности на предмет
            //совпадения. Если хоть один не совпал, то проверяем
            //последовательность дальше. Если нет, возвращаем true
            bool isCorrect = true;
            for (int j = 0; j < len ; j++)
            {
                if (!(seq[j] == data_[symNum - i + j] || (seq[j] == ' ' &&
                    (data_[symNum - i + j] == '\n' || data_[symNum - i + j] == '\r'))))
                {

                    isCorrect = false;
                    break;
                }
            }
            if (isCorrect == true)
                return true;
        }
    return false;
}
