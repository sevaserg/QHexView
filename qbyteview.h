#ifndef QBYTEVIEW_H
#define QBYTEVIEW_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QString>
#include <fstream>
#include <QFont>
#include <QElapsedTimer>
#include <QTimer>
#include <QScrollBar>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>

#include <QMenu>

#include <QGroupBox>
#include <QShortcut>
#include "bytelog.h"

class QByteView : public QGroupBox
{
Q_OBJECT
private:

protected:
    inline int symsInPS();
    QScrollBar *vscroller, *hscroller, *scroller;
    QHBoxLayout* mainLayout;
    QPlainTextEdit* field;
    QGraphicsScene* data;
    QGraphicsView* dataView;
    QGraphicsSimpleTextItem*num;
    QGraphicsSimpleTextItem*matrix;
    QGraphicsSimpleTextItem*asciiMatrix;
    QGraphicsRectItem* colChoice, *colChoice2, *tabChoice;
    QFont f;
    // QTimer *timer;
    byteLog* log;
    unsigned char* buf_;
    int bsize_;
    bool viewRaw_;
    int pointSys_;
    int linesAmt_;
    int bytesInLine_;
    int shifts;

    int dispLines_;

    void redraw();
    void updateAscii();
    void resizeEvent(QResizeEvent*);
    void setAsciiLines();
    void setByteLines();
public:
    QByteView(QGroupBox *parent = 0); //PointSys = 16, linesAmt = 10000, bytesInLine = 16

    void setMaxLines(int maxLines, int bytesInLine_);
    bool setPS(int ps);
    void clear();

    bool isRaw();
    int pointSys();
    int linesAmt();
    int bytesInLine();

    void putData(const QByteArray & arr);
    void putData(const QString & str);

    ~QByteView();
protected slots:
    void scrMoved(int val);
    //void slotTimerAlarm();
    void ShowContextMenu(const QPoint &pos);

};
#endif // QBYTEVIEW_H
