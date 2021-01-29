#include "qbyteview.h"
#include <iostream>
using namespace std;

QByteView::QByteView(int linesAmt, QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    searchQuery.clear();
    setMouseTracking(true);
    shouldUpdate = false;
    shouldRewrite = false;
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
    linesAmt_ = linesAmt;
    lineSz = 20;
    log->setMax(linesAmt_, lineSz);
    //cout << "max lines: " << log->maxLines();
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
    mainLayout->addWidget(dataView);;
    isTextDisplayed_ = true;
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    dataView->printRects();
    asciiMatrix = new QGraphicsSimpleTextItem*[dispLines_];
    matrix = new QGraphicsSimpleTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsSimpleTextItem[20];
        matrix[i] = new QGraphicsSimpleTextItem[20];
    }
    this->show();
    this->setMinimumWidth(780);
    wasResized = false;
    updateTimer = new QTimer;
    resizeTimer = new QTimer;
    connect(resizeTimer, SIGNAL(timeout()), this, SLOT(slotResizeTimeout()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateTimeout()));
    resizeTimer->start(50);
    updateTimer->start(500);
    redraw();
}

bool QByteView::isTextDisplayed()
{
    return isTextDisplayed_;
}

void QByteView::rewriteHex()
{
    char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    //переделываем скроллбар. При этом если он был в самом низу, он внизу так и остаётся.
    bool isMax = scroller->value() == scroller->maximum();
    scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
    if (isMax)
        scroller->setValue(scroller->maximum());
    //Проходимся по каждому элементу
    for (int i = 0; i < dispLines_; i++)
    {
        for (int j = 0; j < lineSz; j++)
        {
            {
                if (log->size() > lineSz*(scroller->value()+i)+j) //если в данной
                //позиции+смещении скроллбара существует символ, то пишем и пытаемся его
                //выделить.
                {
                int curElem = lineSz*(scroller->value()+i)+j;
                QString str = "";
                unsigned char info =((log->data()[curElem]));
                if (log->isInSeq(curElem, searchQuery.data(), searchQuery.length()))//Если лежит в последовательности, красим в оранжевый.
                {
                    if (asciiMatrix[i][j].brush() !=QBrush(QColor(0xff, 0x9b, 0, 255)))
                        asciiMatrix[i][j].setBrush(QBrush(QColor(0xff, 0x9b, 0, 255)));
                    if (matrix[i][j].brush() !=QBrush(QColor(0xff, 0x9b, 0, 255)))
                        matrix[i][j].setBrush(QColor(0xff, 0x9b, 0, 255));
                }
                else
                {
                    //Ниже - отвратительно длинный иф.
                    if
                    (
                        (
                            (
                            //Если текущий элемент лежит между минимумом и максимумом (не включительно) выделенного
                                (curElem > min(log->firstSel(), log->secondSel()))
                                &&
                                (curElem < max(log->firstSel(), log->secondSel()))
                            )
                            //и при этом оба выделения больше нуля
                            &&
                            (log->firstSel() >= 0 && log->secondSel() >= 0)
                        )
                    )//красим букву, если она уже не окрашена, в красный.
                    {
                        if (asciiMatrix[i][j].brush() != QBrush(Qt::red))
                            asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        if (matrix[i][j].brush() != QBrush(Qt::red))
                            matrix[i][j].setBrush(QBrush(Qt::red));
                    }
                    else if (curElem == log->firstSel()) //Если это первый выделенный элемент, красим в зелёный
                    {
                        if (asciiMatrix[i][j].brush() != QBrush(Qt::green))
                            asciiMatrix[i][j].setBrush(QBrush(Qt::green));
                        if (matrix[i][j].brush() != QBrush(Qt::green))
                            matrix[i][j].setBrush(QBrush(Qt::green));
                    }
                    else if (curElem == log->secondSel())//Если это второй выделенный элемент, красим в голубой
                    {
                        if (asciiMatrix[i][j].brush() != QBrush(Qt::cyan))
                            asciiMatrix[i][j].setBrush(QBrush(Qt::cyan));
                        if (matrix[i][j].brush() != QBrush(Qt::cyan))
                            matrix[i][j].setBrush(QBrush(Qt::cyan));
                    }
                    else //В противном случае красим в скучный серый
                    {
                        if (asciiMatrix[i][j].brush() != QBrush(Qt::black))
                            asciiMatrix[i][j].setBrush(QBrush(Qt::black));
                        if (matrix[i][j].brush() != QBrush(Qt::black))
                            matrix[i][j].setBrush(QBrush(Qt::black));
                    }
                }
                if (info >= 32 && info < 127) //Если символ видимый, то печатаем его в ASCII-представление
                {
                    if (asciiMatrix[i][j].text() != QString(info))
                        asciiMatrix[i][j].setText(QString(info));
                }
                else //если нет, вместо него печатаем точку
                    asciiMatrix[i][j].setText(".");
                for (int i = 0; i < 2;i++) //Пишем в байтовое представление
                {
                    str = alphabet[info%16]+str;
                    info /= 16;
                }
                if (matrix[i][j].text() != str)
                    matrix[i][j].setText(str);
                }
                else//если символа не существует, то пишем черный вопросительный знак
                {
                    if (asciiMatrix[i][j].brush() != QBrush(Qt::black))
                        asciiMatrix[i][j].setBrush(QBrush(Qt::black));
                    if (matrix[i][j].brush() != QBrush(Qt::black))
                        matrix[i][j].setBrush(QBrush(Qt::black));
                    if (asciiMatrix[i][j].text() != "?")
                        asciiMatrix[i][j].setText("?");

                    if (matrix[i][j].text() != "??")
                        matrix[i][j].setText("??");
                }
            }
        }
    }
}

void QByteView::redrawHex()
{
    //удаляем всё с dataView
    for (int i = 0; i < dispLines_; i++)
    {
        delete[]asciiMatrix[i];
        delete[]matrix[i];
    }
    delete[]matrix;
    delete[]asciiMatrix;
    dataView->clear(); //очищаем област
    data->clear();
    dataView->switchViews(false);
    int offset = 600, strOffset = 0;
    lineSz = 20;
    dispLines_ = this->height()/20;
    //переназначаем максимум скроллбара
    scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
    //создаём и размещаем элементы по новой
    asciiMatrix = new QGraphicsSimpleTextItem*[dispLines_];
    matrix = new QGraphicsSimpleTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsSimpleTextItem[20];
        matrix[i] = new QGraphicsSimpleTextItem[20];
    }
    for (int i = 0; i < dispLines_; i++)
        for (int j = 0; j < lineSz; j++)
        {
            matrix[i][j].setFont(fnt);
            data->addItem(&matrix[i][j]);
            data->addItem(&asciiMatrix[i][j]);
            matrix[i][j].moveBy(strOffset+5+30*j, 20*i);
            asciiMatrix[i][j].moveBy(offset+5+7*j, 20*i);
        }
    //размещаем выделяющие квадраты
    dataView->initRect();
    dataView->printRects();
    //Добавляем прямые
    data->addLine(offset, 0, offset,dataView->height());
    data->addLine(offset+1, 0, offset+1,dataView->height());
    rewriteHex();
}
//В целом, с текстовым представлением принцип примерно тот же.
void QByteView::rewriteAscii()
{
    for (int i = 0; i < dispLines_; i++)
    {
        unsigned char* ln = log->asciiLine(scroller->value()+i+((log->asciiLineLen(0) == 0 && scroller->value() > 0) ? 1 : 0));
        QString d = QString(reinterpret_cast<char*>(ln));
        for (int j = 0; j < lineSz; j++)
        {
            if (j < d.length())
            {
                if (asciiMatrix[i][j].brush() !=QBrush(Qt::black))
                    asciiMatrix[i][j].setBrush(QBrush(Qt::black));
                if (asciiMatrix[i][j].text() != d.at(j))
                    asciiMatrix[i][j].setText(d.at(j));
                if (log->asciiSel1Active() && log->asciiSel2Active())
                {
                    if((scroller->value() + i  > log->asciiSel1Line() && scroller->value() + i  < log->asciiSel2Line()) || (scroller->value() + i  > log->asciiSel2Line() && scroller->value() + i  < log->asciiSel1Line()))
                    {
                        if (asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                            asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                    }
                    if (log->asciiSel1Line() < log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (j > log->asciiSel1Sym() && asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                        if (scroller->value() + i  == log->asciiSel2Line() && asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                        {
                            if (j < log->asciiSel2Sym())
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                    }
                    if (log->asciiSel1Line() > log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (j < log->asciiSel1Sym() && asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                        if (scroller->value() + i  == log->asciiSel2Line())
                        {
                            if (j > log->asciiSel2Sym() &&asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                                asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                        }
                    }
                    if (log->asciiSel1Line() == log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (log->asciiSel1Sym() < log->asciiSel2Sym())
                            {
                                if (j > log->asciiSel1Sym() && j < log->asciiSel2Sym() && asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                                    asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                            }
                            if (log->asciiSel1Sym() > log->asciiSel2Sym())
                            {
                                if (j < log->asciiSel1Sym() && j > log->asciiSel2Sym() && asciiMatrix[i][j].brush() !=QBrush(Qt::red))
                                    asciiMatrix[i][j].setBrush(QBrush(Qt::red));
                            }
                        }
                    }
                }
                if (log->asciiSel1Active())
                {
                    if (scroller->value() + i == log->asciiSel1Line() && j == log->asciiSel1Sym() && asciiMatrix[i][j].brush() !=QBrush(Qt::green))
                    {
                        asciiMatrix[i][j].setBrush(QBrush(Qt::green));
                    }
                }
                if (log->asciiSel2Active())
                {
                    if (scroller->value() + i == log->asciiSel2Line() && j == log->asciiSel2Sym())
                    {
                        asciiMatrix[i][j].setBrush(QBrush(Qt::cyan));
                    }
                }
                if (searchQuery.length() > 0 && log->isInSeq(log->getFirstSymInAsciiLine(scroller->value()+i-((log->asciiLineLen(0) == 0) ? 0 : 1))+j, searchQuery.data(), searchQuery.length()))
                {
                    asciiMatrix[i][j].setBrush(QBrush(QColor(0xff, 0x9b, 0, 255)));
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
    scroller->setMaximum(max(log->asciiLines() - dispLines_+ 1, 0));
    int strOffset = 0;
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
    updateTimer->start(this->height() * (isTextDisplayed_ ? 1 : 2));
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
    clear();
    lineSz = bytesInLine;
    linesAmt_ = maxLines > 835575 ? 835575 : maxLines;
    log->setMax(linesAmt_, lineSz);
}

void QByteView::clear()
{
    log->clear();
    scroller->setValue(0);
    scroller->setMaximum(0);
    redraw();
}

int QByteView::linesAmt()
{
    return linesAmt_;
}

int QByteView::bytesInLine()
{
    return lineSz;
}

void QByteView::putData(const QByteArray & arr)
{
        bool isMax = scroller->value() == scroller->maximum(); //запоминаем, был ли скроллбар в максимуме
        int tmp2 = scroller->value(); //и его значение
        //Заталкиваем данные и запоминаем число сдвигов байтовых строк.
        int shift =log->push(reinterpret_cast<const unsigned char *>(arr.data()), arr.size());
        int newMaxLines;
        if (!isTextDisplayed_)
        {
            newMaxLines = log->linesAmt()-dispLines_;
            scroller->setMaximum(newMaxLines <= 0 ? 0 : newMaxLines);
            //в байтовом представлении сдвиг по ASCII нам не интересен, поэтому
            //сбрасываем его.
            log->asciiShift();
        }
        else
        {
            newMaxLines = log->asciiLines()-dispLines_;
            scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
            //В текстовом представлении запоминаем сдвиг по ASCII.
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
        shouldRewrite = true;

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
    for (int i = 0; i < dispLines_; i++)
    {
        delete[]asciiMatrix[i];
        delete[]matrix[i];
    }
    delete[]matrix;
    delete[]asciiMatrix;
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

int QByteView::search(QByteArray query)
{
    searchQuery = query;
    return 1;
}

void QByteView::resizeEvent(QResizeEvent*)
{
    wasResized = true;
}

void QByteView::scrMoved(int)
{
    shouldRewrite = true;
}

void QByteView::switchViews()
{
    int tempVal = scroller->value();
    bool isMax = scroller->value() == scroller->maximum();
    if (isTextDisplayed_)
    {
        scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
        scroller->setMinimum(0);
        if (isMax)
        {
            scroller->setValue(scroller->maximum());
        }
        else
        {
            scroller->setValue(log->ALNToBLN(tempVal));
        }
    }
    else
    {
        if (log->asciiLines() - dispLines_ >= 0)
            scroller->setMaximum(log->asciiLines() - dispLines_ + 1);
        else
            scroller->setMaximum(0);
        scroller->setMinimum(0);
        if (isMax)
        {
            scroller->setValue(scroller->maximum());
        }
        else
        {
            int asciiLineNum = max(log->BLNToALN(tempVal) - 1, 0);
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
    dataView->show();
    isTextDisplayed_ = !isTextDisplayed_;
    redraw();
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
    int ln;
    if (isTextDisplayed_)
        ln = min(log->asciiSel1Line(), log->asciiSel2Line());
    else
        ln = min(log->firstSel(), log->secondSel()) / log->lineSize();
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
    if (shouldRewrite)
    {
        bool isMax = scroller->maximum() == scroller->value();
        rewrite();
        if (isMax)
            scroller->setValue(scroller->maximum());
        shouldRewrite = false;
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
    pContextMenu->addAction(pScrDwnAction);
    pContextMenu->exec(e->globalPos());
    delete pContextMenu;
}

void QByteView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton && enableHighlight)
    {
        log->setSelectAll(false);
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
