#include "qbyteview.h"
#include <iostream>
using namespace std;

QByteView::QByteView(QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    dispLines_ = 20;
    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    log = new byteLog;
    field = new QPlainTextEdit;
    f.setFamily("Courier New");
    f.setPointSize(20);
    field->setFont(f);
    vscroller = new QScrollBar;
    hscroller = new QScrollBar;
    scroller = new QScrollBar;
    scroller->setMaximum(0);
    //connect(scroller, SIGNAL(sliderMoved(int)), this, SLOT(scrMoved(int)));
    connect(scroller, SIGNAL(valueChanged(int)), this, SLOT(scrMoved(int)));
    field->setVerticalScrollBar(vscroller);
    field->setHorizontalScrollBar(hscroller);

    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    field->setLineWrapMode(QPlainTextEdit::NoWrap);

    data = new QGraphicsScene;

    dataView = new QGraphicsView(data);

    dataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dataView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    pointSys_ = 16;
    linesAmt_ = 20000;
    bytesInLine_ = 20;
    log->setMax(linesAmt_, bytesInLine_);

    setByteLines();

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

    //field->setVisible(true);
    //timer->start(1000);
    buf_ = static_cast<unsigned char *>(malloc(0));
    shifts = 0;

    mainLayout->addWidget(dataView);

    //mainLayout->addWidget(field);
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    this->show();
    redraw();
}

inline int QByteView::symsInPS()
{
    return (pointSys_ >= 16 ? 2 : pointSys_ >= 8 ? 4 : pointSys_ >= 4 ? 6 : 8);
}


void QByteView::redraw()
{
    matrix = new QGraphicsSimpleTextItem[20*dispLines_];
    asciiMatrix = new QGraphicsSimpleTextItem[20*dispLines_];
    num = new QGraphicsSimpleTextItem[dispLines_];
    int offset = 650, strOffset = 50;
    data->clear();
    dataView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //item1->setBrush(QBrush(Qt::red));
    //item2->setBrush(QBrush(Qt::green));
    //item3->setBrush(QBrush(Qt::blue));
    //item4->setBrush(QBrush(Qt::yellow));
    char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for (int i = 0; i < dispLines_; i++)
    {
        num[i].setText(QString::number(scroller->value()+i+1));
        data->addItem(&num[i]);
        num[i].moveBy(5, 20*i);
        for (int j = 0; j < 320 / pointSys_; j++)
        {
            if(log->linesAmt() > scroller->value()+20*i+j)
            {
                QString str = "";
                char info =(static_cast<char>((log->data()[scroller->value()+20*i+j])));
                asciiMatrix[20*i+j].setText(QString(info));
                for (int i = 0; i < symsInPS();i++)
                {
                    str = alphabet[info%pointSys_]+str;
                }
                matrix[20*i+j].setText(str);
            }
            else
            {
                asciiMatrix[20*i+j].setText("?");
                matrix[20*i+j].setText("??");
            }
            data->addItem(&matrix[20*i+j]);
            data->addItem(&asciiMatrix[20*i+j]);
            matrix[20*i+j].moveBy(strOffset+5+30*j, 20*i);
            asciiMatrix[20*i+j].moveBy(offset+5+7*j, 20*i);
        }
    }
    data->addLine(strOffset, 0, strOffset,dataView->height() - 20);
    data->addLine(0, 0, 0,dataView->height() - 20);
    data->addLine(offset, 0, offset,dataView->height() - 20);
    data->addLine(offset+1, 0, offset+1,dataView->height() - 20);
}

void QByteView::updateAscii()
{
    field->clear();
    field->appendPlainText(QString::number(scroller->value()) + " / " + QString::number(log->asciiLines()) + ":\n\n");
    /*
    if (log->size() > 0)
    {
        int lines = min(dispLines_, log->linesAmt());
        for (int i = 0; i < lines; i++)
        {
            unsigned char *out = static_cast<unsigned char*>(malloc(static_cast<size_t>(bytesInLine_)));

            memcpy(out, log->data()+bytesInLine_*(scroller->value()+i), static_cast<size_t>(log->lineSize()));
            //int symsamt = (scroller->value()+i == log->linesAmt() ? log->lastLineSize() : bytesInLine_);
            for (int j = 0; j < bytesInLine_; j++)
            {
                if (out[j] < 32 || out[j] > 126)
                    out[j] = '.';
            }

            if (i != lines-1)
            {
                QString s(reinterpret_cast<char*>(out));
                s.resize(bytesInLine_);
                field->appendPlainText(s);
            }
            free(out);
        }
    }*/
    if (log->size() > 0)
    for(int i = 0; i < dispLines_; i++)
    {
        unsigned char*t = log->asciiLine(scroller->value()+i);
        QString s(reinterpret_cast<char *>(t));
        s = s.split('\n').at(0);
//        s.chop(1);
        field->textCursor().movePosition(QTextCursor::End);
        field->textCursor().insertText(s);
//        field->appendPlainText(s);
        free(t);
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
    int shift =log->push(reinterpret_cast<const unsigned char *>(arr.data()), arr.size());

    //tmp = log->linesAmt()-dispLines_;
    int tmp;
    if (field->isHidden())
        tmp = log->linesAmt()-dispLines_;
    else
        tmp = log->asciiLines()-dispLines_;
    int tmp2 = scroller->value();
    scroller->setMaximum(tmp <= 0 ? 0 : tmp);
    if (isMax)
        scroller->setValue(scroller->maximum());
    else
        scroller->setValue((tmp2 <= shift ? 0 : tmp2-shift));
    updateAscii();
    redraw();
}

void QByteView::putData(const QString & str)
{
    bool isMax = scroller->isMaximized();
    int shifts = log->push(reinterpret_cast<const unsigned char *>(str.data()), str.size());
    if (!isMax)
    {
        scroller->setValue(scroller->value() - shifts);
    }
    else
    {
        scroller->setValue(scroller->maximum());
    }
}

QByteView::~QByteView()
{
    free(buf_);
    delete log;
    delete field;
    //delete timer;
    delete mainLayout;
    delete scroller;
    delete vscroller;
    delete hscroller;
    delete data;
    delete dataView;
    delete[]matrix;
    delete[]asciiMatrix;
    delete tabChoice;
    delete colChoice;
    delete colChoice2;
    delete num;
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

void QByteView::setAsciiLines()
{
    bytesInLine_ = field->width() / 18;
    dispLines_ = field->height() / 35;
    log->setLineSize(bytesInLine_);
    scroller->setMaximum(log->linesAmt() - (log->linesAmt() < dispLines_ ? 0 : dispLines_));
    this->updateAscii();
}

void QByteView::setByteLines()
{
    bytesInLine_ = dataView->width() / 18;
    dispLines_ = dataView->height() / 21;
    log->setLineSize(bytesInLine_);
    if (field->isHidden())
    {
        this->redraw();
        scroller->setMaximum(log->linesAmt() - (log->linesAmt() < dispLines_ ? 0 : dispLines_));
    }
    else
    {
        this->updateAscii();
        scroller->setMaximum(log->asciiLines() - (log->asciiLines() < dispLines_ ? 0 : dispLines_));
    }


}

void QByteView::scrMoved(int val)
{
    redraw();
    updateAscii();
}

void QByteView::resizeEvent(QResizeEvent*)
{
    setByteLines();
    redraw();
}
