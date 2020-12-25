#include "qbyteview.h"
#include <iostream>
using namespace std;
bool lastState = true;

QByteView::QByteView(QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    mainLayout = new QHBoxLayout;
    log = new byteLog;
    field = new QPlainTextEdit;
    scroller = new QScrollBar;
    field->setVerticalScrollBar(scroller);
    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    field->setVisible(false);
    mainLayout->addWidget(field);
    pointSys_ = 16;
    linesAmt_ = 2000;
    bytesInLine_ = 17;
    log->setMax(linesAmt_, bytesInLine_);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    timer->start(250);
    field->setVisible(true);
    buf_ = static_cast<unsigned char *>(malloc(0));
    shifts = 0;
}

void QByteView::updateAscii()
{
    if (log->size() > 0)
    {
        field->clear();
        field->setPlainText(QString(reinterpret_cast<char *>(log->data())));
    }
}

void QByteView::setMaxLines(int maxLines, int bytesInLine)
{
    bytesInLine_ = bytesInLine;
    linesAmt_ = maxLines;
    log->setMax(linesAmt_, bytesInLine_);
}

bool QByteView::setPS(int ps)
{
    if (ps == 0 || ps ==2 || ps == 4 || ps == 8 || ps == 16)
    {
        pointSys_ = ps;
        return true;
    }
    return false;
}

void QByteView::clear()
{
    field->clear();
    log->clear();
}

bool QByteView::isRaw()
{
    return viewRaw_;
}

int QByteView::pointSys()
{
    return pointSys_;
}

int QByteView::linesAmt()
{
    return linesAmt_;
}

int QByteView::bytesInLine()
{
    return bytesInLine_;
}

void QByteView::putData(const QByteArray & arr)
{
    int tmp =log->push(reinterpret_cast<const unsigned char *>(arr.data()), arr.size());
    shifts+=tmp;
    lastState = tmp >= 0;
}

void QByteView::putData(const QString & str)
{
    log->push(reinterpret_cast<const unsigned char *>(str.data()), str.size());

}

QByteView::~QByteView()
{
    free(buf_);
    delete log;
    delete field;
    delete timer;
    delete mainLayout;
    delete scroller;
}

void QByteView::ShowContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);

   QAction action1("Remove Data Point", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action1);

   contextMenu.exec(mapToGlobal(pos));
}

void QByteView::slotTimerAlarm()
{
    for (int i = log->size()-log->lineSize(); i < log->size();i++)
    cout << log->at(i);
    cout << '\t' << log->size() << "/" <<log->max()<< '\t' << (lastState ? "true" : "false")<< endl;
    if (scroller->maximum() == scroller->value() || (shifts == 0))
    {

        int scrVal = scroller->value();
        bool isMax = (scroller->value() == scroller->maximum() );
        this->updateAscii();
        if (isMax)
            scroller->setValue(scroller->maximum());
        else
            scroller->setValue(scrVal);
        shifts = 0;
    }
}
