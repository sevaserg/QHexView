#include "qbyteview.h"
#include <iostream>
using namespace std;

QByteView::QByteView(QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    dispLines_ = 50;


    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    log = new byteLog;
    field = new QPlainTextEdit;
    vscroller = new QScrollBar;
    hscroller = new QScrollBar;
    scroller = new QScrollBar;
    scroller->setMaximum(0);
    connect(scroller, SIGNAL(sliderMoved(int value)), this, SLOT(scrMoved(int val)));
    field->setVerticalScrollBar(vscroller);
    field->setHorizontalScrollBar(hscroller);

    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    pointSys_ = 16;
    linesAmt_ = 20000;
    bytesInLine_ = 17;
    log->setMax(linesAmt_, bytesInLine_);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

    //field->setVisible(true);
    //timer->start(1000);
    buf_ = static_cast<unsigned char *>(malloc(0));
    shifts = 0;

    mainLayout->addWidget(field);
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    this->show();
}

void QByteView::updateAscii()
{
    field->clear();
    field->appendPlainText(QString::number(scroller->value()) + " / " + QString::number(log->linesAmt()) + ":\n");
    if (log->size() > 0)
    {
        int lines = min(dispLines_, log->linesAmt());
        for (int i = 0; i < lines; i++)
        {
            unsigned char *out = static_cast<unsigned char*>(malloc(static_cast<size_t>(bytesInLine_)));

            memcpy(out, log->data()+bytesInLine_*(scroller->value()+i), static_cast<size_t>(log->lineSize()));
            int symsamt = (scroller->value()+i == log->linesAmt() ? log->lastLineSize() : bytesInLine_);
            for (int j = 0; j < bytesInLine_; j++)
            {
                if (out[j] < 32 || out[j] > 126)
                    out[j] = '.';
            }

            if (i != lines-1)
            {
                field->appendPlainText(QString(reinterpret_cast<char*>(out)));
            }
            free(out);
        }
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
    cout << "data pushed!"<< endl;
    bool isMax = scroller->isMaximized();
    int tmp =log->push(reinterpret_cast<const unsigned char *>(arr.data()), arr.size());
    shifts+=tmp;
    tmp = log->linesAmt()-dispLines_;
    int tmp2 = scroller->value();
    scroller->setMaximum(tmp <= 0 ? 0 : tmp);
    if (isMax)
        scroller->setValue(scroller->maximum());
    else
        scroller->setValue(tmp2);
    updateAscii();
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
    //delete timer;
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



/*
void QByteView::slotTimerAlarm()
{
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
    updateAscii();
}*/

void QByteView::scrMoved(int val)
{
    updateAscii();
}
