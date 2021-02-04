#include "qbyteview.h"

#include <iostream>
using namespace std;

QByteView::QByteView(int linesAmt, QGroupBox *parent) : QGroupBox ( parent )//PointSys = 16, linesAmt = 10000, bytesInLine = 16
{
    dir = "./";

    searchQuery.clear();
    setMouseTracking(true);
    shouldUpdate = false;
    shouldRewrite = false;
    isShiftPressed = false;
    fnt.setFamily("Courier New");
    fnt.setPixelSize(11);

    chooseFirst = true;
    enableHighlight = true;
    dispLines_ = 36;
    mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    log = new byteLog;
    scroller = new QScrollBar;
    scroller->setMaximum(0);
    connect(scroller, SIGNAL(valueChanged(int)), this, SLOT(scrMoved(int)));
    dscroller = new QScrollBar;
    dscroller->setOrientation(Qt::Horizontal);
    connect(dscroller, SIGNAL(valueChanged(int)), this, SLOT(scrMoved(int)));
    data = new QGraphicsScene;
    dataView = new custGView;
    dataView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    dataView->setScene(data);
    dataView->initRect();
    dataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataView->setMouseTracking(true);
    dataLayout = new QVBoxLayout;
    dataLayout->addWidget(dataView);
    dataLayout->addWidget(dscroller);
    linesAmt_ = linesAmt;
    lineSz = 20;
    log->setMax(linesAmt_, lineSz);
    mainLayout->addLayout(dataLayout);
    isTextDisplayed_ = true;
    mainLayout->addWidget(scroller);
    this->setLayout(mainLayout);
    dataView->printRects();
    asciiMatrix = new QGraphicsTextItem*[dispLines_];
    matrix = new QGraphicsTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsTextItem[20];
        matrix[i] = new QGraphicsTextItem[20];
    }
    this->show();
    this->setMinimumWidth(200);
    this->setMinimumHeight(100);
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
                if (log->size() > lineSz*(scroller->value()+i)+j + dscroller->value()) //если в данной
                //позиции+смещении скроллбара существует символ, то пишем и пытаемся его
                //выделить.
                {
                    int curElem = lineSz*(scroller->value()+i)+j + dscroller->value();
                    bool isinseq= log->isInSeq(curElem, searchQuery.data(), searchQuery.length());
                    bool ishighlighted = false;
                    //Ниже - отвратительно длинный иф.
                    if
                    (
                        (
                            (
                            //Если текущий элемент лежит между минимумом и максимумом (не включительно) выделенного
                                (curElem >= min(log->firstSel(), log->secondSel()))
                                &&
                                (curElem <= max(log->firstSel(), log->secondSel()))
                            )
                            //и при этом оба выделения больше нуля
                            &&
                            (log->firstSel() >= 0 && log->secondSel() >= 0)
                        )
                        ||
                        (
                             log->firstSel() >= 0
                             &&
                             curElem == log->firstSel()
                        )
                        ||
                        (
                             log->secondSel() >= 0
                             &&
                             curElem == log->secondSel()
                        )
                    )//выделяем
                    {
                        ishighlighted = true;
                    }

                    QString str = "";
                    unsigned char info =((log->data()[curElem]));
                    if (dataView->width() > 750)
                    {
                        if (info >= 32 && info < 127) //Если символ видимый, то печатаем его в ASCII-представление
                        {
                            //asciiMatrix[i][j].setHtml(QString("<div style='background-color:") + QString(ishighlighted ? "blue;" : "white;")+ QString("font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : ishighlighted ? "color:white;'>": "color:black;'>" ) + QString(static_cast<char>(info)) + "</div>");
                            if (info == ' ') //Если символ видимый, то печатаем его в ASCII-представление
                                asciiMatrix[i][j].setHtml(QString("<div style='background-color:") + QString(ishighlighted ? "blue;" : "white;")+ QString("font-family:Courier New;") + QString(ishighlighted ? "color:blue;'>": "color:white;'>" ) +  "_</div>");
                            else
                                asciiMatrix[i][j].setHtml(QString("<div style='background-color:") + QString(ishighlighted ? "blue;" : "white;")+ QString(" font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : ishighlighted ? "color:white;'>": "color:black;'>" ) + QString(static_cast<char>(info)) + "</div>");

                        }
                        else //если нет, вместо него печатаем точку
                        {
                            asciiMatrix[i][j].setHtml(QString("<div style='background-color:") + QString(ishighlighted ? "blue;" : "white;")+ QString(" font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : ishighlighted ? "color:white;'>": "color:black;'>")+QString(".</div>"));
                        }
                    }
                    for (int i = 0; i < 2;i++) //Пишем в байтовое представление
                    {
                        str = alphabet[info%16]+str;
                        info /= 16;
                    }
                    matrix[i][j].setHtml(QString("<div style='background-color:") + QString(ishighlighted ? "blue;" : "white;")+ QString(" font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : ishighlighted ? "color:white;'>": "color:black;'>" ) + str + "</div>");
                }
                else//если символа не существует, то пишем черный вопросительный знак
                {
                    if (dataView->width() > 750)
                        asciiMatrix[i][j].setHtml(QString("<div style='background-color:white; font-family:Courier New; color:black;'>" ) + "?</div>");
                    matrix[i][j].setHtml("<div style='background-color:white; font-family:Courier New;color:black;'>?? </div>");
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
    dataView->clear(); //очищаем область
    data->clear();
    dataView->switchViews(false);
    int offset = 600, strOffset = 0;
    if (dataView->width() > offset)
    {
        dscroller->setMaximum(0);
    }
    else
    {
        int v = dscroller->value();
        dscroller->setMaximum(20-this->width()/30);
        if (v >= dscroller->maximum())
            dscroller->setValue(dscroller->maximum());
    }
    lineSz = 20;
    dispLines_ = this->height()/20 - 2;
    //переназначаем максимум скроллбара
    scroller->setMaximum(max(log->linesAmt() - dispLines_ + 1, 0));
    //создаём и размещаем элементы по новой
    asciiMatrix = new QGraphicsTextItem*[dispLines_];
    matrix = new QGraphicsTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
        asciiMatrix[i] = new QGraphicsTextItem[20];
        matrix[i] = new QGraphicsTextItem[20];
    }
    for (int i = 0; i < dispLines_; i++)
        for (int j = 0; j < lineSz; j++)
        {
            matrix[i][j].setFont(fnt);
            data->addItem(&matrix[i][j]);
            if (dataView->width() > 750)
                data->addItem(&asciiMatrix[i][j]);
            matrix[i][j].moveBy(strOffset+30*j, 20*i-4);
            if (dataView->width() > 750)
                asciiMatrix[i][j].moveBy(offset+7*j-1, 20*i-4);
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
        unsigned char* ln = log->asciiLine(scroller->value()+i+1+((log->asciiLineLen(0) == 0 && scroller->value() > 0) ? 1 : 0)); //+((log->asciiLineLen(0) == 0 && scroller->value() > 0) ? 1 : 0)
        QString d = QString(reinterpret_cast<char*>(ln));
        for (int j = 0; j < lineSz; j++)
        {
            bool isinseq = searchQuery.length() > 0 && log->isInSeq(log->getFirstSymInAsciiLine(scroller->value()+i-((log->asciiLineLen(0) == 0) ? 0 : 1) + 1)+j, searchQuery.data(), searchQuery.length());
            if (j+dscroller->value() < d.length())
            {
                int k = j+dscroller->value();
                asciiMatrix[i][j].setTextWidth(20);
                asciiMatrix[i][j].setHtml("<div style='background-color:white; font-family:Courier New; height:20;"+ QString(isinseq ? "color:#FF4000;'>" : "color:black;'>" ) + QString(d.at(k)) + "</div>");
                if (log->asciiSel1Active() && log->asciiSel2Active())
                {
                    if((scroller->value() + i  > log->asciiSel1Line() && scroller->value() + i  < log->asciiSel2Line()) || (scroller->value() + i  > log->asciiSel2Line() && scroller->value() + i  < log->asciiSel1Line()))
                    {
                        asciiMatrix[i][j].setHtml(QString("<div style='background-color:blue; font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");
                    }
                    if (log->asciiSel1Line() < log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {

                            if (k > log->asciiSel1Sym())
                                asciiMatrix[i][j].setHtml(QString("<div style='background-color:blue; font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                        }
                        if (scroller->value() + i  == log->asciiSel2Line())
                        {
                            if (k < log->asciiSel2Sym())
                            asciiMatrix[i][j].setHtml(QString("<div style='background-color:blue;font-family:Courier New;") + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                        }
                    }
                    if (log->asciiSel1Line() > log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if (k < log->asciiSel1Sym())
                                asciiMatrix[i][j].setHtml("<div style='background-color:blue; font-family:Courier New;" + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                        }
                        if (scroller->value() + i  == log->asciiSel2Line())
                        {
                            if (k > log->asciiSel2Sym())
                                asciiMatrix[i][j].setHtml("<div style='background-color:blue; font-family:Courier New;" + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                        }
                    }
                    if (log->asciiSel1Line() == log->asciiSel2Line())
                    {
                        if (scroller->value() + i  == log->asciiSel1Line())
                        {
                            if ((k > log->asciiSel2Sym() && k < log->asciiSel1Sym()) || (k < log->asciiSel2Sym() && k > log->asciiSel1Sym()))
                                 asciiMatrix[i][j].setHtml("<div style='background-color:blue; font-family:Courier New;" + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");
                        }
                    }
                }
                if (log->asciiSel1Active())
                {
                    if (scroller->value() + i == log->asciiSel1Line() && k == log->asciiSel1Sym())
                    {
                        asciiMatrix[i][j].setHtml("<div style='background-color:blue;font-family:Courier New;" + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                    }
                }
                if (log->asciiSel2Active())
                {
                    if (scroller->value() + i == log->asciiSel2Line() && k == log->asciiSel2Sym())
                    {
                        asciiMatrix[i][j].setHtml("<div style='background-color:blue; font-family:Courier New;" + QString(isinseq ? "color:#FF4000;'>" : "color:white;'>" ) + QString(d.at(k)) + "</div>");

                    }
                }

            }
            else
            {
                asciiMatrix[i][j].setHtml("");
            }
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
    dispLines_ = this->height()/20 - 1;
    scroller->setMaximum(max(log->asciiLines() - dispLines_+ 1, 0));
    int strOffset = 0;
    asciiMatrix = new QGraphicsTextItem*[dispLines_];
    matrix = new QGraphicsTextItem*[dispLines_];
    for (int i = 0; i < dispLines_; i++)
    {
            asciiMatrix[i] = new QGraphicsTextItem[lineSz];
            matrix[i] = new QGraphicsTextItem[1];
    }

    for (int i = 0; i < dispLines_; i++)
    {
        for (int j = 0; j < lineSz;j++)
        {
            data->addItem(&asciiMatrix[i][j]);
            asciiMatrix[i][j].moveBy(strOffset+8*j-4, 20*i-4);
        }
    }
    dscroller->setMaximum(max(0, log->maxAsciiLineLen() - this->width()/8+2));
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
        QString res = dir;
        if (res.at(res.length()-1) != '/')
            res+="/";
        res += "log-t-" + hr + "-" + mt + "-" + sc + "-d-" + dy + "-" + mn + "-" + yr + ".txt";
        QFile file(res);
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
            scroller->setMaximum(max(log->asciiLines() - dispLines_ + 1, 0));
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
        if(isTextDisplayed_)
            dscroller->setMaximum(max(0, log->maxAsciiLineLen() - this->width()/8+2));
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
    delete dataView;
    delete data;
    delete scroller;
    delete log;
    delete updateTimer;
    delete dscroller;
    delete dataLayout;
    delete mainLayout;
}

int QByteView::search(QByteArray query)
{
    searchQuery = query;
    shouldUpdate = true;
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

void QByteView::slotChooseDirectory()
{

    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "./",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
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
    pContextMenu->addAction(pCopy);
    pContextMenu->addSeparator();
    QAction *pSelectAll = new QAction("Select all",this);
    connect(pSelectAll ,SIGNAL(triggered()),this,SLOT(slotSelectAll()));
    pContextMenu->addAction(pSelectAll);
    QAction *pGoToHighlighted = new QAction("Go to highlighted",this);
    connect(pGoToHighlighted ,SIGNAL(triggered()),this,SLOT(slotGoToHighlighted()));
    pContextMenu->addAction(pGoToHighlighted);
    pContextMenu->addSeparator();
    QAction *pExport = new QAction("Export selected",this);
    connect(pExport ,SIGNAL(triggered()),this,SLOT(slotExportSelected()));
    pContextMenu->addAction(pExport);
    QAction *pChooseDirectory = new QAction("Choose directory",this);
    connect(pChooseDirectory ,SIGNAL(triggered()),this,SLOT(slotChooseDirectory()));
    pContextMenu->addAction(pChooseDirectory);
    pContextMenu->addSeparator();
    pContextMenu->addAction(pScrDwnAction);
    pContextMenu->exec(e->globalPos());
    delete pContextMenu;
}

void QByteView::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        log->setSelectAll(false);
        int rw = static_cast<int>(dataView->getY());
        int cl = static_cast<int>(dataView->getX());
        int elNum;
        if (isTextDisplayed_)
            elNum = log->getFirstSymInAsciiLine(scroller->value() + rw) + min(log->asciiLineLen(scroller->value() + rw), cl+dscroller->value());
        else
            elNum = static_cast<int>((scroller->value() + rw) * 20 + cl + dscroller->value());

        if (enableHighlight)
        {
            if (isShiftPressed)
            {
                if (log->asciiSel1Active() && log->asciiSel2Active())
                {
                    if (abs(elNum - log->firstSel()) < abs(elNum - log->secondSel()))
                    {
                        log->setFirstSel(elNum);
                    }
                    else
                    {
                        log->setSecondSel(elNum);
                    }
                }
                else
                {
                    if (chooseFirst)
                        log->setFirstSel(elNum);
                    else
                        log->setSecondSel(elNum);
                    chooseFirst = !chooseFirst;
                }
            }
            else
            {

                log->setFirstSel(elNum);
                log->setSecondSel(elNum);
            }
        }
        rewrite();
    }

}

void QByteView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift)
    {
        isShiftPressed = true;
    }
}

void QByteView::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift)
    {
        isShiftPressed = false;
    }

}

void QByteView::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0)
        scroller->setValue(min(scroller->value() - e->delta() / 40, scroller->maximum()));
    else
        scroller->setValue(max(scroller->value() - e->delta() / 40, scroller->minimum()));
    shouldUpdate = true;
}
