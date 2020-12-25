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

#include <QMenu>

#include <QPushButton>
#include <QGroupBox>

#include "bytelog.h"

class QByteView : public QGroupBox
{
Q_OBJECT
private:

protected:
    QScrollBar* scroller;
    QHBoxLayout* mainLayout;
    QPlainTextEdit* field;
    QTimer *timer;
    byteLog* log;
    unsigned char* buf_;
    int bsize_;
    bool viewRaw_;
    int pointSys_;
    int linesAmt_;
    int bytesInLine_;
    int shifts;
public:
    QByteView(QGroupBox *parent = 0); //PointSys = 16, linesAmt = 10000, bytesInLine = 16
    void updateAscii();
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
    void slotTimerAlarm();
    void ShowContextMenu(const QPoint &pos);
};
#endif // QBYTEVIEW_H
