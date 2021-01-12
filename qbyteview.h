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

#include <QGroupBox>
#include <QShortcut>
#include "bytelog.h"
#include "custgview.h"

class QByteView : public QGroupBox
{
Q_OBJECT
private:

protected:
    inline int symsInPS();
    QScrollBar  *hscroller, *vscroller, *scroller;
    QHBoxLayout* mainLayout;
    QPlainTextEdit* field;
    QGraphicsScene* data;
    custGView* dataView;
    QGraphicsSimpleTextItem*num;
    QGraphicsSimpleTextItem*matrix;
    QGraphicsSimpleTextItem*asciiMatrix;
    QFont f;

    bool isTextDisplayed_;
    bool chooseFirst;
    int chosenFirst;
    byteLog* log;
    unsigned char* buf_;
    int bsize_;
    bool viewRaw_;
    int pointSys_;
    int linesAmt_;
    int bytesInLine_;
    int shifts;

    int dispLines_;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void redraw();
    void updateAscii();
    void resizeEvent(QResizeEvent*);
    void contextMenuEvent( QContextMenuEvent * e );
    void mousePressEvent(QMouseEvent *event);
    void setAsciiLines();
    void setByteLines();
public:
    QByteView(QGroupBox *parent = 0); //PointSys = 16, linesAmt = 10000, bytesInLine = 20

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

    void setMaxLines(int maxLines, int bytesInLine_);
    /*
     * Настраивает кол-во элементов для байтового представления.
     */

    bool setPS(int ps);
    /*
     * выбор системы счисления (2 двоичная, 4 четвертичная,
     * 8 восьмеричная и 16 шестнадцатеричная) байтового
     * представления.
     * ПОКА НЕ РЕАЛИЗОВАНО!
     */

    void clear();
    /*
     * Очищает лог.
     */

    bool isRawDisplayed();
    /*
     * Вернёт true, если выведено байтовое представление,
     * или false - если выведено текстовое представление.
     */

    int pointSys();
    /*
     * Вернёт выбранную систему счисления (2 двоичная, 4 четвертичная,
     * 8 восьмеричная и 16 шестнадцатеричная) байтового
     * представления.
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
     * кладёт данные в лог.
     */

    void putData(const QString & str);
    /*
     * кладёт данные в лог.
     */

    //unsigned char* getC();

    QByteArray getQByteArray();
    /*
     * Возвращает данные из лога в виде QByteArray.
     */

    QString getQString();
    /*
     * Возвращает данные из лога в виде QString.
     */

//================================================================
//================================================================

    ~QByteView();
protected slots:
    void scrMoved(int val);
    //void slotTimerAlarm();
    void ShowContextMenu(const QPoint &pos);
    void slotSwitchViews();
    void slotClear();
};



#endif // QBYTEVIEW_H
