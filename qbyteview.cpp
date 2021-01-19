#include "qbyteview.h"
#include <iostream>
using namespace std;

QByteView::QByteView(QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    setMouseTracking(true);


    fnt.setFamily("Courier New");
    fnt.setPixelSize(11);

    chooseFirst = true;
    enableHighlight = true;
    dispLines_ = 36;
    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    log = new byteLog;

    vscroller = new QScrollBar;
    hscroller = new QScrollBar;
    scroller = new QScrollBar;
    scroller->setMaximum(0);
    connect(scroller, SIGNAL(valueChanged(int)), this, SLOT(scrMoved(int)));
    data = new QGraphicsScene;
    dataView = new custGView;
    dataView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dataView->setScene(data);
    dataView->initRect();
    dataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataView->setMouseTracking(true);
    pointSys_ = 16;
    linesAmt_ = 15000000 / 20;
    bytesInLine_ = 20;
    log->setMax(linesAmt_, bytesInLine_);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    buf_ = static_cast<unsigned char *>(malloc(0));
    shifts = 0;
    mainLayout->addWidget(dataView);;
    isTextDisplayed_ = true;
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    dataView->printRects();
    //num = new QGraphicsSimpleTextItem[dispLines_];
    asciiMatrix = new QGraphicsSimpleTextItem*[dispLines_];
    matrix = new QGraphicsSimpleTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsSimpleTextItem[20];
        matrix[i] = new QGraphicsSimpleTextItem[20];
    }
    redraw();
    //rewrite();
    this->show();
    this->setMinimumWidth(780);
    wasResized = false;
    resizeTimer = new QTimer;
    updateTimer = new QTimer;
    connect(resizeTimer, SIGNAL(timeout()), this, SLOT(slotResizeTimeout()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateTimeout()));
    resizeTimer->start(50);
    updateTimer->start(1000);
}

bool QByteView::isTextDisplayed()
{
    return isTextDisplayed_;
}

void QByteView::rewriteHex()
{
    char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    bool isMax = scroller->value() == scroller->maximum();
    scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
    if (isMax)
        scroller->setValue(scroller->maximum());
    for (int i = 0; i < dispLines_; i++)
    {
        //num[i].setText(QString::number(scroller->value()+i+1));
        for (int j = 0; j < 20; j++)
        {
            //if(log->linesAmt() * 20 >= (scroller->value()+i)*20+j)
            {
                if (log->size() > 20*(scroller->value()+i)+j)
                {
                int curElem = 20*(scroller->value()+i)+j;
                QString str = "";
                unsigned char info =((log->data()[curElem]));
                //cout << "Element #" << curElem << ": " << info << endl;
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
                )
                {
                    asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                    matrix[i][j].setBrush(QBrush(Qt::red));
                }
                else if (curElem == log->firstSel())
                {
                    asciiMatrix[i][j].setBrush(QBrush(Qt::green));
                    matrix[i][j].setBrush(QBrush(Qt::green));
                }
                else if (curElem == log->secondSel())
                {
                    asciiMatrix[i][j].setBrush(QBrush(Qt::cyan));
                    matrix[i][j].setBrush(QBrush(Qt::cyan));
                }
                else
                {
                    asciiMatrix[i][j].setBrush(QBrush(Qt::black));
                    matrix[i][j].setBrush(QBrush(Qt::black));

                }
                if (info >= 32 && info < 127)
                    asciiMatrix[i][j].setText(QString(info));
                else
                    asciiMatrix[i][j].setText(".");
                for (int i = 0; i < 2;i++)
                {
                    str = alphabet[info%pointSys_]+str;
                    info /= pointSys_;
                }
                matrix[i][j].setText(str);
                }
                else
                {
                    asciiMatrix[i][j].setText("?");
                    matrix[i][j].setText("??");
                }
            }
            /*else
            {
                asciiMatrix[i][j].setText("?");
                matrix[i][j].setText("??");
            }*/
        }
    }
}

void QByteView::redrawHex()
{
    for (int i = 0; i < dispLines_; i++)
    {
        delete[]asciiMatrix[i];
        delete[]matrix[i];
    }
    delete[]matrix;
    delete[]asciiMatrix;
    dataView->clear();
    data->clear();
    dataView->switchViews(false);
    int offset = 600, strOffset = 0;
    lineSz = 20;
    dispLines_ = this->height()/20;
    scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
    dataView->resize();
    asciiMatrix = new QGraphicsSimpleTextItem*[dispLines_];
    matrix = new QGraphicsSimpleTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsSimpleTextItem[20];
        matrix[i] = new QGraphicsSimpleTextItem[20];
    }
    for (int i = 0; i < dispLines_; i++)
    {
        for (int j = 0; j < 320 / pointSys_; j++)
        {
            matrix[i][j].setFont(fnt);
            data->addItem(&matrix[i][j]);
            data->addItem(&asciiMatrix[i][j]);
            matrix[i][j].moveBy(strOffset+5+30*j, 20*i);
            asciiMatrix[i][j].moveBy(offset+5+7*j, 20*i);
        }
    }
    dataView->initRect();
    dataView->printRects();
    //data->addLine(strOffset, 0, strOffset,dataView->height() - 20);
    //data->addLine(0, 0, 0,dataView->height() - 20);
    data->addLine(offset, 0, offset,dataView->height());
    data->addLine(offset+1, 0, offset+1,dataView->height());
    rewriteHex();
}

void QByteView::rewriteAscii()
{
    for (int i = 0; i < dispLines_; i++)
    {
        //num[i].setText(QString::number(scroller->value()+i+1));
        unsigned char* ln = log->asciiLine(scroller->value()+i+1);
        QString d = QString(reinterpret_cast<char*>(ln));
        for (int j = 0; j < lineSz; j++)
        {
            if (j < d.length())
            {
                asciiMatrix[i][j].setBrush(QBrush(Qt::black));
                asciiMatrix[i][j].setText(d.at(j));
                if (log->asciiSel1Active() >= 0 && log->asciiSel2Active() >= 0)
                {
                    //cout << "Current " << scroller->value() + i << ", left "  << log->asciiSel1Line() << ", right " << log->asciiSel2Line() << endl;
                    if((scroller->value() + i  > log->asciiSel1Line() && scroller->value() + i  < log->asciiSel2Line()) || (scroller->value() + i  > log->asciiSel2Line() && scroller->value() + i  < log->asciiSel1Line()))
                    {
                        asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                    }
                    if (log->asciiSel1Line() < log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (j > log->asciiSel1Sym())
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                        if (scroller->value() + i  == log->asciiSel2Line())
                        {
                            if (j < log->asciiSel2Sym())
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                    }
                    if (log->asciiSel1Line() > log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (j < log->asciiSel1Sym())
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                        if (scroller->value() + i  == log->asciiSel2Line())
                        {
                            if (j > log->asciiSel2Sym())
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                    }
                    if (log->asciiSel1Line() == log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (log->asciiSel1Sym() < log->asciiSel2Sym())
                            {
                                if (j > log->asciiSel1Sym() && j < log->asciiSel2Sym())
                                    asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                            }
                            if (log->asciiSel1Sym() > log->asciiSel2Sym())
                            {
                                if (j < log->asciiSel1Sym() && j > log->asciiSel2Sym())
                                    asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                            }
                        }
                    }
                }
                if (log->asciiSel1Active() >= 0)
                {
                    if (scroller->value() + i == log->asciiSel1Line() && j == log->asciiSel1Sym())
                    {
                        asciiMatrix[i][j].setBrush(QBrush(Qt::green));
                    }
                }
                if (log->asciiSel2Active() >= 0)
                {
                    if (scroller->value() + i == log->asciiSel2Line() && j == log->asciiSel2Sym())
                    {
                        asciiMatrix[i][j].setBrush(QBrush(Qt::cyan));
                    }
                }

            }
            else
                asciiMatrix[i][j].setText("");
            }
        free(ln);
    }
}

void QByteView::redrawAscii()
{
    for (int i = 0; i < dispLines_; i++)
    {
        delete[]asciiMatrix[i];
        delete[]matrix[i];
    }
    delete[]asciiMatrix;
    delete[]matrix;
    dataView->clear();
    data->clear();
    dataView->switchViews(true);
    lineSz = this->width()/8;
    dispLines_ = this->height()/20;
    scroller->setMaximum(max(log->asciiLines() - dispLines_, 0));
    int strOffset = 0;
    dataView->resize();
    asciiMatrix = new QGraphicsSimpleTextItem*[dispLines_];
    matrix = new QGraphicsSimpleTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
            asciiMatrix[i] = new QGraphicsSimpleTextItem[lineSz];
            matrix[i] = new QGraphicsSimpleTextItem[1];
    }

    for (int i = 0; i < dispLines_; i++)
    {
        for (int j = 0; j < lineSz;j++)
        {
            data->addItem(&asciiMatrix[i][j]);
            asciiMatrix[i][j].moveBy(strOffset+8*j, 20*i);
        }
    }
    //data->addLine(strOffset, 0, strOffset,dataView->height() - 20);
    dataView->initRect();
    dataView->printRects();
    rewriteAscii();
}

void QByteView::rewrite()
{
    if (isTextDisplayed_)
        rewriteAscii();
    else
        rewriteHex();
}

void QByteView::redraw()
{
    if (isTextDisplayed_)
        redrawAscii();
    else
        redrawHex();
}

void QByteView::exportSelected(int first, int last)
{
    int firstData, lastData;
    QDateTime dt = QDateTime::currentDateTime();
    QString yr = QString::number(dt.date().year());
    QString mn = QString::number(dt.date().month() / 10)+QString::number(dt.date().month() % 10);
    QString dy = QString::number(dt.date().day() / 10)+QString::number(dt.date().day() % 10);
    QString hr = QString::number(dt.time().hour() / 10)+QString::number(dt.time().hour() % 10);
    QString mt = QString::number(dt.time().minute() / 10)+QString::number(dt.time().minute() % 10);
    QString sc = QString::number(dt.time().second() / 10)+QString::number(dt.time().second() % 10);
    QString res = "log-t-" + hr + "-" + mt + "-" + sc + "-d-" + dy + "-" + mn + "-" + yr + ".txt";
    if (first < 0 && last < 0)
    {
        firstData = min(log->firstSel(), log->secondSel());
        lastData = max(log->firstSel(), log->secondSel());
    }
    else
    {
        firstData = min(first, last);
        lastData = max(first, last);
    }
    if (firstData >= 0 && lastData >= 0 && firstData < log->size())
    {
        QDateTime dt = QDateTime::currentDateTime();
        QString yr = QString::number(dt.date().year());
        QString mn = QString::number(dt.date().month() / 10)+QString::number(dt.date().month() % 10);
        QString dy = QString::number(dt.date().day() / 10)+QString::number(dt.date().day() % 10);
        QString hr = QString::number(dt.time().hour() / 10)+QString::number(dt.time().hour() % 10);
        QString mt = QString::number(dt.time().minute() / 10)+QString::number(dt.time().minute() % 10);
        QString sc = QString::number(dt.time().second() / 10)+QString::number(dt.time().second() % 10);
        QString res = "log-t-" + hr + "-" + mt + "-" + sc + "-d-" + dy + "-" + mn + "-" + yr + ".txt";
        QFile file("log-t-" + hr + "-" + mt + "-" + sc + "-d-" + dy + "-" + mn + "-" + yr + ".txt");
        QTextStream stream(&file);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            char*data = log->getData(firstData, lastData);
            stream << data;
            free(data);
        }
    }
}

void QByteView::setMaxLines(int maxLines, int bytesInLine)
{
    bytesInLine_ = bytesInLine;
    linesAmt_ = maxLines;
    log->setMax(linesAmt_, bytesInLine_);
}

void QByteView::clear()
{
    log->clear();
    scroller->setValue(0);
    scroller->setMaximum(0);
    redraw();
    //putData(QString(""));
}

bool QByteView::isRawDisplayed()
{
    return viewRaw_;
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
        if (!isTextDisplayed_)
            newMaxLines = log->linesAmt()-dispLines_;
        else
            newMaxLines = log->asciiLines()-dispLines_;
        int tmp2 = scroller->value();
        scroller->setMaximum(newMaxLines <= 0 ? 0 : newMaxLines);
        if (!isTextDisplayed_)
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
        if (log->selectAll())
        {
            log->setFirstSel(0);
            log->setSecondSel(log->size() - 1);
        }
        //==================================================
        if (shift > 0)
        {
            cout <<"Full!"<<endl;
        }
        //==================================================
        //rewrite();
        shouldUpdate = true;
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
    delete resizeTimer;
    free(buf_);
    for (int i = 0; i < dispLines_; i++)
    {
        delete[]asciiMatrix[i];
        delete[]matrix[i];
    }
    delete[]matrix;
    delete[]asciiMatrix;
    //delete[]num;
    dataView->clear();
    delete mainLayout;
    delete dataView;
    delete data;
    delete scroller;
    delete vscroller;
    delete hscroller;
    delete log;
    delete updateTimer;
}

void QByteView::ShowContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);
   QAction action1("Remove Data Point", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));
   contextMenu.addAction(&action1);
   contextMenu.exec(mapToGlobal(pos));
}

void QByteView::scrMoved(int)
{
    //rewrite();
    //redraw();
    shouldUpdate = true;
    if (scroller->maximum() == scroller->value())
    {
        //cout << "scrollbar: " << scroller->value()+1 << "-" << scroller->value()+dispLines_+1
        //     << "; last ASCII string: " << log->asciiLines()+1 << ", value: " << log->asciiLine(log->asciiLines()+1) << endl;
    }
}

void QByteView::resizeEvent(QResizeEvent*)
{
    wasResized = true;
    //redraw();
}

void QByteView::switchViews()
{
    int tempVal = scroller->value();
    bool isMax = scroller->value() == scroller->maximum();
    if (isTextDisplayed_)
    {
        //if (log->linesAmt() - dispLines_ >= 0)
            //scroller->setMaximum(log->linesAmt() - dispLines_);
            scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
        //else
            //scroller->setMaximum(0);
        scroller->setMinimum(0);
        if (isMax)
        {
            scroller->setValue(scroller->maximum());
        }
        else
        {
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
        }
    }
    else
    {
        if (log->asciiLines() - dispLines_ >= 0)
            scroller->setMaximum(log->asciiLines() - dispLines_);
        else
            scroller->setMaximum(0);
        scroller->setMinimum(0);
        if (isMax)
        {
            scroller->setValue(scroller->maximum());
        }
        else
        {
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
        }
        rewrite();
    }
    dataView->show();
    isTextDisplayed_ = !isTextDisplayed_;
    redraw();
    rewrite();
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

void QByteView::slotExportSelected()
{
    exportSelected();
}

void QByteView::slotSelectAll()
{
    log->setSelectAll(true);
    log->setFirstSel(0);
    log->setSecondSel(log->size()-1);
    rewrite();
}

void QByteView::slotResizeTimeout()
{
    if (wasResized)
    {
        redraw();
        wasResized = false;
    }
}

void QByteView::slotUpdateTimeout()
{
    if (shouldUpdate)
    {
        bool isMax = scroller->maximum() == scroller->value();
        redraw();
        if (isMax)
            scroller->setValue(scroller->maximum());
        shouldUpdate = false;
    }
}

void QByteView::contextMenuEvent(QContextMenuEvent *e)
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
    //if (!isTextDisplayed_)
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
        QAction *pSelectAll = new QAction("Select all",this);
        connect(pSelectAll ,SIGNAL(triggered()),this,SLOT(slotSelectAll()));
        pContextMenu->addAction(pSelectAll);
        pContextMenu->addSeparator();
        QAction *pGoToHighlighted = new QAction("Go to highlighted",this);
        connect(pGoToHighlighted ,SIGNAL(triggered()),this,SLOT(slotGoToHighlighted()));
        pContextMenu->addAction(pGoToHighlighted);
        pContextMenu->addSeparator();
        QAction *pExport = new QAction("Export selected",this);
        connect(pExport ,SIGNAL(triggered()),this,SLOT(slotExportSelected()));
        pContextMenu->addAction(pExport);
        pContextMenu->addSeparator();
    }
    pContextMenu->addAction(pScrDwnAction);
    pContextMenu->exec(e->globalPos());
    delete pContextMenu;
}

void QByteView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton && enableHighlight)
    {
        int rw = static_cast<int>(dataView->getY());
        int cl = static_cast<int>(dataView->getX());
        if (isTextDisplayed_)
        {
            if (cl <= log->asciiLineLen(scroller->value() + rw) && enableHighlight)
            {
                if (chooseFirst)
                    log->setFirstSel(log->getFirstSymInAsciiLine(scroller->value() + rw) + min(log->asciiLineLen(scroller->value() + rw), cl));
                else
                    log->setSecondSel(log->getFirstSymInAsciiLine(scroller->value() + rw) + min(log->asciiLineLen(scroller->value() + rw), cl));
            }
        }
        else
        {
            log->setSelectAll(false);
            if (event->pos().x() < 600)
            {
                if (chooseFirst)
                    log->setFirstSel(static_cast<int>((scroller->value() + rw) * 20 + cl));
                else
                    log->setSecondSel(static_cast<int>((scroller->value() + rw) * 20 + cl));

            }
        }
        rewrite();
    }
}

