#ifndef QBYTEVIEW_H
#define QBYTEVIEW_H

#include <QMouseEvent>

#include <QWidget>
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
    QGraphicsSimpleTextItem**matrix;
    QGraphicsSimpleTextItem**asciiMatrix;
    QFont fnt;
    QTimer* resizeTimer,* updateTimer;
    bool wasResized;
    bool shouldUpdate;
    int shiftsCnt;
    int asciiShiftsCnt;
    bool isTextDisplayed_;
    bool enableHighlight;
    bool chooseFirst;
    byteLog* log;
    unsigned char* buf_;
    int bsize_;
    bool viewRaw_;
    int pointSys_;
    int linesAmt_;
    int bytesInLine_;
    int shifts;
    int lineSz;
    int dispLines_;

    void redraw();
    void rewrite();

    void redrawHex();
    void rewriteHex();

    void redrawAscii();
    void rewriteAscii();

    void resizeEvent(QResizeEvent *e);
    void contextMenuEvent( QContextMenuEvent * e );
    void mousePressEvent(QMouseEvent *event);
public:
    QByteView(QGroupBox *parent = 0);

//================================================================
//================================================================

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

    bool isRawDisplayed();
    /*
     * Вернёт true, если выведено байтовое представление,
     * или false - если выведено текстовое представление.
     */

    void setMaxLines(int maxLines, int bytesInLine_ = 20);
    /*
     * Настраивает кол-во элементов для байтового представления.
     * bytesInLine_ лучше пока не менять.
     */

    void clear();
    /*
     * Очищает лог.
     */

    int linesAmt();
    /*
     * Возвращает число заполненных строк (в байтовом представлении) лога.
     */

    int bytesInLine();
    /*
     * Возвращает максимально возможное число байтов в строке (в байтовом
     * представлении) лога.
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
     * Возвращает данные из лога в виде QString.
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
     *
     */
//================================================================
//================================================================

    ~QByteView();
protected slots:
    void scrMoved(int);
    void ShowContextMenu(const QPoint &pos);
    void slotSwitchViews();
    void slotClear();
    void slotCopy();
    void slotChooseFirst();
    void slotChooseSecond();
    void slotScrDwn();
    void slotGoToHighlighted();
    void slotEnableHighlighting();
    void slotExportSelected();
    void slotSelectAll();
    void slotResizeTimeout();
    void slotUpdateTimeout();
};



#endif // QBYTEVIEW_H
