#ifndef QBYTEVIEW_H
#define QBYTEVIEW_H

/*
 * Собственно, виджет. Подключать надо
 * именно этот файл!
 */

#include <QMouseEvent>
#include <QWheelEvent>

#include <QWidget>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QString>
#include <fstream>
#include <QFont>
#include <QElapsedTimer>
#include <QTimer>
#include <QScrollBar>
#include <QGraphicsTextItem>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QGroupBox>
#include <QShortcut>
#include <QDateTime>
#include <QTextStream>
#include <QThread>
#include <QBrush>
#include <QByteArray>
#include "bytelog.h"
#include "custgview.h"

class QByteView : public QGroupBox
{
Q_OBJECT
private:

protected:

    QScrollBar  *hscroller, *vscroller, *scroller;
    QHBoxLayout* mainLayout;
    QGraphicsScene* data;
    custGView* dataView;
    QString dir;
    QGraphicsTextItem**matrix; //Для байтового предст.
    QGraphicsTextItem**asciiMatrix; //Для текстового предст.
    QFont fnt;
    /*
     * при изменении размеров окна происходит
     * полная перерисовка сцены. Процесс затяжной, поэтому перерисовывание
     * при каждом масштабировании может привести к достаточно неприятным
     * коллизиям. Чтобы этого не происходило, перерисовка запускается
     * по таймеру, а ивент на масштабирование как раз и указывает, надо
     * ли в следующий раз при срабатывании таймера перерисовывать сцену.
     */
    QTimer *resizeTimer;
    /*
     * Перерисовка при загрузке новых значений или при использовани
     * скроллбара - процедура очень затратная и зависящая от размера
     * виджета. Поэтому кол-во миллисекунд до таймаута равно высоте виджета
     * в пикселях. Размер таймаута обновляется со срабатыванием предыдущего таймера.
     */
    QTimer *updateTimer;
    //Что подсвечивать при поиске
    QByteArray searchQuery;
    bool wasResized;//true - при следующем таймауте resizeTimer виджет будет перерисован.
    bool shouldUpdate;//true - при следующем таймауте updateTimer текст в виджете будет переписан.
    bool shouldRewrite; //true - при следующем таймауте updateTimer виджет будет перерисован.
    int shiftsCnt; // число сдвигов в байтовом представлении, чтобы знать, насколько сдвигать
                   //скроллбар при следующей отрисовке
    int asciiShiftsCnt;// число сдвигов в текстовом представлении, чтобы знать, насколько сдвигать
                       //скроллбар при следующей отрисовке
    bool isTextDisplayed_; //True - текстовое представление. False - байтовое.
    bool enableHighlight;  //True - ползователь может выделять текст. False - не может.
    bool chooseFirst; //True - пользователь выбирает первый элемент. False - второй.
    bool isShiftPressed;
    byteLog* log; //Тут будем хранить данные.
    int linesAmt_; //максимум строк в байтовом представлении
    int lineSz; //размер байтовой строки
    int dispLines_; //Число видимых (отрисовываемых) строк

    void redraw(); //перерисовывает ascii либо hex представление целиком
    void rewrite();//переписывает информацию в ascii либо hex представлении

    void redrawHex();
    void rewriteHex();

    void redrawAscii();
    void rewriteAscii();

    void resizeEvent(QResizeEvent *e);
    void contextMenuEvent( QContextMenuEvent * e );
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);

public:
    QByteView(int linesAmt, QGroupBox *parent = nullptr);
    /*
     * Максимальное количество строк по 20 символов -
     * 835575.
     */

//================================================================
//================================================================

    /*
     * _______________________
     * |        ВИД          |
     * =======================
     */

    void switchViews();
    /*
     * Переключает представления. Было байтовое - стало
     * текстовое. Было текстовое - стало байтовое.
     */

    bool isTextDisplayed();
    /*
     * Вернёт true, если выведено текстовое представление,
     * или false - если выведено байтовое представление.
     */

    int bytesInLine();
    /*
     * Возвращает максимально возможное число байтов в строке (в байтовом
     * представлении) лога.
     */

    int linesAmt();
    /*
     * Возвращает число заполненных строк (в байтовом представлении) лога.
     */

    /*
     * _______________________
     * |       ДАННЫЕ        |
     * =======================
     */

    void setMaxLines(int maxLines, int bytesInLine_ = 20);
    /*
     * Настраивает кол-во элементов для байтового представления.
     * bytesInLine_ менять не надо.
     * Максимальное количество строк по 20 символов -
     * 835575.
     */

    void clear();
    /*
     * Очищает лог.
     */

    void putData(const QByteArray & arr);
    /*
     * Кладёт данные в лог.
     */

    void putData(const QString & str);
    /*
     * Кладёт данные в лог.
     */

    QByteArray getQByteArray();
    /*
     * Возвращает данные из лога в виде QByteArray.
     */

    QString getQString();
    /*
     * Возвращает данные из лога
     * в виде QString.
     */

    void exportSelected(int first = -1, int last = -1);
    /*
     * Экспортирует выделенное в файл.
     * Структура названия: log-t-час-минута-секунда-d-день-месяц-год.txt
     * Пример: log-t-14-47-11-d-18-01-2021.txt
     * Выделено всё - экспортирует всё.
     * Не выделено ничего - не экспортируется ничего.
     * Если first или last будут меньше нуля, значение будет браться
     * по указателям из лога. Если в логе при этом значение
     * одной из двух меток будет меньше нуля,
     * не экспортируется ничего.
     */

    /*
     * _______________________
     * |        ПОИСК        |
     * =======================
     */

    int search(QByteArray query);
    /*
     * Ищет фразы по запросу и подсвечивает их.
     * символы \r и \n при поиске считаются за пробелы.
     * То есть, для подсветки того, что отмечено
     * квадратными скобками
     * some data 1
     * some da[ta 2
     * so]me data 3
     * можно искать "ta 2  so" - между '2' и 's' два
     * пробела.
     */

//================================================================
//================================================================

    ~QByteView();
protected slots:
    void scrMoved(int); //Слот для скроллбара. При его перемещении
    //shouldUpdate становится true.
    //============================================================
    //                    КОНТЕКСТНОЕ МЕНЮ
    //============================================================
    void slotSwitchViews(); //вызывает switchViews()
    void slotClear(); //вызывает switchViews()
    void slotCopy(); //копирует выделенный текст
    void slotScrDwn(); //присваивает скроллбару максимальное
    //значение и переписывает текст
    void slotGoToHighlighted(); // двигает скроллбар к первому
    //выделенному элементу
    void slotEnableHighlighting(); //включает/отключает выделение
    //посредством enableHighlight
    void slotExportSelected(); //вызывает exportSelected()
    void slotChooseDirectory();
    void slotSelectAll(); //включает/отключает режим "выбрать
    //всё". При выделении пользователем одного из элементов режим
    //отключается.
    //============================================================
    void slotResizeTimeout();
    void slotUpdateTimeout();
};



#endif // QBYTEVIEW_H
