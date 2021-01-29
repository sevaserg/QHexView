#ifndef BYTELOG_H
#define BYTELOG_H

/*
 * Этот класс нужен для хранения данных,
 * выдачи этих же данных, выделения,
 * поиска, учёта числа строк, и так далее.
 */

#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <iostream>
#include <QElapsedTimer>

using namespace std;

class byteLog
{
private:
    bool selectAll_; //True - считается, что выбрано всё.
    //False - ориентироваться касаемо выделения на отдельные
    //переменные.
    unsigned char* data_; //тут хранятся данные.
    int *FSIAS_; //First Symbols In ASCII Strings. В этом массиве
    //хранятся все номера первых символов (идущим после \n) ascii-строк.
    //Нужно, чтобы не искать начало строк в цикле.
    int FSIASSize_;//размер массива FSIAS
    int firstSel_; //первый выделенный символ.
    int secondSel_; //второй выделенный символ.
    //"первый" и "второй" - условное деление. порядковый номер второго
    //символа может быть меньше порядкового номера первого.
    int lineSize_; //размер строки в байтовом представлении. Она фиксирована.
    int lines_; //максимальное число строк в байтовом представлении.
    int size_; //текущее число элементов массива.
    int asciiShift_; //число удалённых ASCII-строк. Сбрасывается при считывании
    int *asciiSel1_; //выбор первого элемента.
    // 0 - активен выбор или нет
    // 1 - строка
    // 2 - номер элемента
    int *asciiSel2_; //выбор второго элемента.
public:
    // Конструктор. Без комментариев.
    byteLog();
    // Проверяет, есть ли в последовательности seq длиной len символ
    // номер symNum
    bool isInSeq(int symNum, char* seq, int len);
    // Возвращает текущий размер массива
    int size();
    //очищает лог и меняет максимум на число
    //строк*число символов в строке. Соответствующие
    //переменные в самом логе меняются.
    void setMax(int linesAmt, int lineSize);
    //возвращает размер строки в байтовом представлении
    int lineSize();
    //Возвращает максимальное число строк
    int maxLines();
    //Выбирает первый элемент
    void setFirstSel(int num);
    //Выбирает второй элемент
    void setSecondSel(int num);
    //Возвращает номер первого выделенного элемента
    int firstSel();
    //Возвращает номер второго выделенного элемента
    int secondSel();
    //Возвращает номер ASCII-строки выделенного первого элемента.
    int asciiSel1Line();
    //Возвращает номер ASCII-строки выделенного второго элемента.
    int asciiSel2Line();
    //Возвращает номер символа в ASCII-строке выделенного первого элемента.
    int asciiSel1Sym();
    //Возвращает номер символа в ASCII-строке выделенного второго элемента.
    int asciiSel2Sym();
    //Возвращает false, если выбор первого элемента неактивен (элемент не выделен),
    //или true, если активен.
    bool asciiSel1Active();
    //Возвращает false, если выбор второго элемента неактивен (элемент не выделен),
    //или true, если активен.
    bool asciiSel2Active();
    //переделывает номер выделенного элемента в номер строки и номер элемента
    //в строке. Если нужно сделать неактивным, шлём sel < 0.
    void redoAsciiChoice(int sel, int *asel);
    //true - режим "выделить всё", false - надо смотреть по переменным выбора
    void setSelectAll(bool sel);
    //сообщает, включен ли режим "выделить всё"
    bool selectAll();
    // ASCII Line Number to Byte Line Number
    //преобразует номер ASCII-строки в номер строки в байтовом представлении
    //Полезно для того, чтобы переключать вид, сохраняя в окне ту же информацию
    int ALNToBLN(int ALN);
    // Byte Line Number to ASCII Line Number
    //преобразует номер байтовой строки в номер ASCII-строки
    int BLNToALN(int BLN);
    //заталкивает данные в конец лога. Если данных по итогу оказалось больше максимума, происходит
    //смещение
    int push(const unsigned char* data,int amt);
    //Чистит массив и переменные, в которых обозначен его размер. Убирает выделение
    void clear();
    //Возвращает весь массив целиком
    unsigned char* data();
    //Возвращает выделенный фрагмент данных
    //Возвращает выбранное. FREE НЕ ЗАБЫВАЕМ!
    char* getData(int first, int last);
    //Возвращает число строк
    int linesAmt();
    //Возвращает число ASCII-строк
    int asciiLines();
    //Возвращает номер первого символа в ASCII-строке номер lineNum
    int getFirstSymInAsciiLine(int lineNum);
    //Возвращает выбранное. ВЫЗЫВАТЬ FREE ПОТОМ ДЛЯ ТОГО МЕСТА, КУДА КЛАДЁТЕ, ОБЯЗАТЕЛЬНО!
    char* getHighlighted();
    //Возвращает размер последней строки в байтовом представлении
    int lastLineSize();
    //Возвращает длину ASCII-строки
    int asciiLineLen(int lineNum);
    //Возвращает число удалённых ASCII-строк, сбрасывает соответствующую переменную.
    int asciiShift();
    //Возвращает ASCII-строку. ДЛЯ ЭТОГО ГЕНЕРИРУЕТСЯ НОВАЯ СТРОКА MALLOC'ОМ, ОБЯЗАТЕЛЬНО
    //НАДО ВЫЗЫВАТЬ FREE ДЛЯ ТОГО УКАЗАТЕЛЯ, В КОТОРЫЙ КЛАДЁТСЯ ЗНАЧЕНИЕ!
    unsigned char* asciiLine(int lineNum);
    //Деструктор.
    ~byteLog();
};


#endif // BYTELOG_H
