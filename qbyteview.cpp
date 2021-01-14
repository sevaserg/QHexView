#include "qbyteview.h"
#include <iostream>
using namespace std;

QByteView::QByteView(QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    setMouseTracking(true);
    chooseFirst = true;
    enableHighlight = true;
    dispLines_ = 21;
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
    connect(scroller, SIGNAL(valueChanged(int)), this, SLOT(scrMoved(int)));
    field->setVerticalScrollBar(vscroller);
    field->setHorizontalScrollBar(hscroller);
    field->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    field->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    field->setLineWrapMode(QPlainTextEdit::NoWrap);
    field->setAttribute(Qt::WA_TransparentForMouseEvents);
    data = new QGraphicsScene;
    dataView = new custGView;
    dataView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dataView->setScene(data);
    dataView->initRect();
    dataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dataView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    dataView->setMouseTracking(true);
    pointSys_ = 16;
    linesAmt_ = 20000;
    bytesInLine_ = 20;
    log->setMax(linesAmt_, bytesInLine_);
    setByteLines();
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    buf_ = static_cast<unsigned char *>(malloc(0));
    shifts = 0;
    mainLayout->addWidget(field);
    mainLayout->addWidget(dataView);
    dataView->hide();
    isTextDisplayed_ = true;
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    this->show();
    dataView->printRects();
    num = new QGraphicsSimpleTextItem[dispLines_];
    asciiMatrix = new QGraphicsSimpleTextItem[20*dispLines_];
    matrix = new QGraphicsSimpleTextItem[20*dispLines_];
    redraw();
}

bool QByteView::isTextDisplayed()
{
    return isTextDisplayed_;
}

inline int QByteView::symsInPS()
{
    return (pointSys_ >= 16 ? 2 : pointSys_ >= 8 ? 4 : pointSys_ >= 4 ? 6 : 8);
}

void QByteView::redraw()
{
    delete[]asciiMatrix;
    delete[]matrix;
    delete[]num;
    dataView->clear();
    int offset = 650, strOffset = 50;
    dataView->resize();
    char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    num = new QGraphicsSimpleTextItem[dispLines_];
    asciiMatrix = new QGraphicsSimpleTextItem[20*dispLines_];
    matrix = new QGraphicsSimpleTextItem[20*dispLines_];

    for (int i = 0; i < dispLines_; i++)
    {
        num[i].setText(QString::number(scroller->value()+i+1));
        data->addItem(&num[i]);
        num[i].moveBy(5, 20*i);
        for (int j = 0; j < 320 / pointSys_; j++)
        {
            if(log->linesAmt() * 20 > (scroller->value()+i)*20+j)
            {
                int curElem = 20*(scroller->value()+i)+j;
                QString str = "";
                unsigned char info =((log->data()[curElem]));
                if (
                    (
                        (
                            (curElem > min(log->firstSel(), log->secondSel()))
                            &&
                            (curElem < max(log->firstSel(), log->secondSel()))
                        )
                        &&
                        (log->firstSel() >= 0 && log->secondSel() >= 0)
                    )
                    /*||
                    (log->secondSel() < 0 && curElem == log->firstSel())*/

                )
                {
                    asciiMatrix[20*i+j].setBrush(QBrush(Qt::red));
                    matrix[20*i+j].setBrush(QBrush(Qt::red));
                }
                else if (curElem == log->firstSel())
                {
                    asciiMatrix[20*i+j].setBrush(QBrush(Qt::green));
                    matrix[20*i+j].setBrush(QBrush(Qt::green));
                }
                else if (curElem == log->secondSel())
                {
                    asciiMatrix[20*i+j].setBrush(QBrush(Qt::darkCyan));
                    matrix[20*i+j].setBrush(QBrush(Qt::darkCyan));
                }
                else
                {
                    asciiMatrix[20*i+j].setBrush(QBrush(Qt::black));
                    matrix[20*i+j].setBrush(QBrush(Qt::black));

                }
                if (info >= 32 && info < 127)
                    asciiMatrix[20*i+j].setText(QString(info));
                else
                    asciiMatrix[20*i+j].setText(".");
                for (int i = 0; i < symsInPS();i++)
                {
                    str = alphabet[info%pointSys_]+str;
                    info /= pointSys_;
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
    dataView->printRects();
    data->addLine(strOffset, 0, strOffset,dataView->height() - 20);
    data->addLine(0, 0, 0,dataView->height() - 20);
    data->addLine(offset, 0, offset,dataView->height() - 20);
    data->addLine(offset+1, 0, offset+1,dataView->height() - 20);
    dataView->items().clear();
}

void QByteView::updateAscii()
{
    int shouldHighlight1 = -1;
    int shouldHighlight2 = -1;
    field->clear();
    field->appendPlainText(QString::number(scroller->value()) + " / " + QString::number(log->asciiLines()) + ":\n\n");

    for(int i = 0; i < dispLines_; i++)
    {
        unsigned char*tmp = log->asciiLine(scroller->value()+i);
        QString str(reinterpret_cast<char *>(tmp));
        field->textCursor().movePosition(QTextCursor::End);
        field->textCursor().insertText(str);
        free(tmp);
        if (log->lastAsciiSel1() >= 0)
            shouldHighlight1 = log->lastAsciiSel1();
    }
    field->textCursor().deletePreviousChar();
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
    log->clear();
    if (isTextDisplayed_)
        updateAscii();
    else
        redraw();
    putData(QString(""));
}

bool QByteView::isRawDisplayed()
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
        bool isMax = scroller->value() == scroller->maximum();
        int shift =log->push(reinterpret_cast<const unsigned char *>(arr.data()), arr.size());
        int newMaxLines;
        if (field->isHidden())
            newMaxLines = log->linesAmt()-dispLines_;
        else
            newMaxLines = log->asciiLines()-dispLines_;
        int tmp2 = scroller->value();
        scroller->setMaximum(newMaxLines <= 0 ? 0 : newMaxLines);
        if (field->isHidden())
        {
            log->asciiShift();
        }
        else
        {
            shift = log->asciiShift();
        }
        if (isMax)
            scroller->setValue(scroller->maximum());
        else
           scroller->setValue((tmp2 <= shift ? 0 : tmp2-shift));
        if (!isTextDisplayed_ && scroller->value() == scroller->maximum() && shifts == 0)
            redraw();
        if (isTextDisplayed_)
            updateAscii();
}

void QByteView::putData(const QString & str)
{
    putData(str.toLatin1());
}

QByteArray QByteView::getQByteArray()
{
    QByteArray qba(reinterpret_cast<const char*>(log->data()));
    return qba;
}

QString QByteView::getQString()
{
    QString qs(reinterpret_cast<const char*>(log->data()));
    return qs;
}

QByteView::~QByteView()
{
    free(buf_);
    delete[]matrix;
    delete[]asciiMatrix;
    delete[]num;
    dataView->clear();
    delete mainLayout;
    delete dataView;
    delete data;
    delete scroller;
    delete vscroller;
    delete hscroller;
    delete log;
    delete field;
}

void QByteView::ShowContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Remove Data Point", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action1);
   contextMenu.exec(mapToGlobal(pos));
}

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
    log->setLineSize(bytesInLine_);
    if (field->isHidden())
    {
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
    updateAscii();
}

void QByteView::switchViews()
{
    int tempVal = scroller->value();
    if (isTextDisplayed_)
    {
        dispLines_ = 36;
        if (log->linesAmt() - dispLines_ >= 0)
            scroller->setMaximum(log->linesAmt() - dispLines_);
        else
            scroller->setMaximum(0);
        scroller->setMinimum(0);
        int byteLineNum = log->ALNToBLN(tempVal);
        if (scroller->value() != scroller->maximum())
        {
            if (byteLineNum + dispLines_ < log->linesAmt())
            {
                scroller->setValue(byteLineNum);
            }
            else
            {
                scroller->setValue(scroller->maximum());
            }
        }
        field->hide();
        dataView->show();
        redraw();
    }
    else
    {
        dispLines_ = 21;
        if (log->asciiLines() - dispLines_ >= 0)
            scroller->setMaximum(log->asciiLines() - dispLines_);
        else
            scroller->setMaximum(0);
        scroller->setMinimum(0);
        int asciiLineNum = log->BLNToALN(tempVal);
        if (scroller->value() != scroller->maximum())
        {
            if (asciiLineNum + dispLines_ < log->asciiLines())
            {
                scroller->setValue(asciiLineNum + 1);
            }
            else
            {
                scroller->setValue(scroller->maximum());
            }
        }
        dataView->hide();
        field->show();
        updateAscii();
    }
    isTextDisplayed_ = !isTextDisplayed_;
}


void QByteView::slotCopy()
{
    if (log->firstSel() >= 0 && log->secondSel() >= 0)
    {
        QClipboard* c = QApplication::clipboard();
        char* tmp = log->getHighlighted();
        c->setText(tmp);
        free(tmp);
    }
}

void QByteView::slotSwitchViews()
{
    switchViews();
}

void QByteView::slotClear()
{
    clear();
}

void QByteView::slotChooseFirst()
{
    chooseFirst = true;
}

void QByteView::slotChooseSecond()
{
    chooseFirst = false;
}

void QByteView::slotScrDwn()
{
    scroller->setValue(scroller->maximum());
}

void QByteView::slotGoToHighlighted()
{
    int ln = min(log->firstSel(), log->secondSel()) / log->lineSize();
    if (ln > scroller->maximum())
        scroller->setValue(scroller->maximum());
    else
        scroller->setValue(ln);
}

void QByteView::slotEnableHighlighting()
{
    enableHighlight = !enableHighlight;
}

void QByteView::contextMenuEvent( QContextMenuEvent * e )
{
    QMenu* pContextMenu = new QMenu( this);
    QAction *pSwitchAction = new QAction("Switch view",this);
    connect(pSwitchAction,SIGNAL(triggered()),this,SLOT(slotSwitchViews()));
    QAction *pClearAction = new QAction("Clear",this);
    connect(pClearAction ,SIGNAL(triggered()),this,SLOT(slotClear()));
    QAction *pScrDwnAction = new QAction("Scroll down",this);
    connect(pScrDwnAction ,SIGNAL(triggered()),this,SLOT(slotScrDwn()));
    pContextMenu->addAction(pSwitchAction);
    pContextMenu->addAction(pClearAction);
    if (!isTextDisplayed_)
    {
        pContextMenu->addSeparator();
        QAction *pCopy = new QAction("Copy",this);
        connect(pCopy ,SIGNAL(triggered()),this,SLOT(slotCopy()));
        QAction *pEnableHighlighting = new QAction("Enable highlight editing",this);
        connect(pEnableHighlighting ,SIGNAL(triggered()),this,SLOT(slotEnableHighlighting()));
        pEnableHighlighting->setCheckable(true);
        pEnableHighlighting->setChecked(enableHighlight);
        QAction *pChooseFirst = new QAction("Choose green element",this);
        connect(pChooseFirst ,SIGNAL(triggered()),this,SLOT(slotChooseFirst()));
        pChooseFirst->setCheckable(true);
        pChooseFirst->setChecked(chooseFirst);
        pChooseFirst->setEnabled(enableHighlight);
        QAction *pChooseSecond = new QAction("Choose cyan element",this);
        connect(pChooseSecond ,SIGNAL(triggered()),this,SLOT(slotChooseSecond()));
        pChooseSecond->setCheckable(true);
        pChooseSecond->setChecked(!chooseFirst);
        pChooseSecond->setEnabled(log->firstSel() >= 0 && enableHighlight);
        pContextMenu->addAction(pCopy);
        pContextMenu->addSeparator();
        pContextMenu->addAction(pEnableHighlighting);
        pContextMenu->addAction(pChooseFirst);
        pContextMenu->addAction(pChooseSecond);
        pContextMenu->addSeparator();
        QAction *pGoToHighlighted = new QAction("Go to highlighted",this);
        connect(pGoToHighlighted ,SIGNAL(triggered()),this,SLOT(slotGoToHighlighted()));
        pContextMenu->addAction(pGoToHighlighted);
        pContextMenu->addSeparator();
    }
    pContextMenu->addAction(pScrDwnAction);
    pContextMenu->exec(e->globalPos());
    delete pContextMenu;
}

void QByteView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (isTextDisplayed_)
        {
            int rw = event->y() / 30 - 2;
            int cl = event->x() / 30;
            if (rw >= 0 && cl <= log->asciiLineLen(scroller->value() + rw))
            {
                cout << "Button pressed! X = " << event->x() << ", Y = " << event->y() << endl;
                cout << "first element was chosen! Row " << event->y() / 30 - 2 <<
                        ", column: " << event->x() / 30 << endl;
                log->setFirstSel(log->getFirstSymInAsciiLine(scroller->value() + cl));
            }
        }
        else
        {
            if (event->pos().x() > 50 && event->pos().x() < 650 && enableHighlight)
            {
                if (chooseFirst)
                    log->setFirstSel(static_cast<int>((scroller->value() + dataView->getHexY() / 20) * 20 + (dataView->getHexX()-50) / 30));
                else
                    log->setSecondSel(static_cast<int>((scroller->value() + dataView->getHexY() / 20) * 20 + (dataView->getHexX()-50) / 30));
                redraw();
            }
        }
    }
}
