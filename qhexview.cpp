#include "qhexview.h"
//#include "ui_qhexview.h"

#include <iostream>

using namespace std;

QHexView::QHexView(QWidget *parent)
{
    linesAmt_ = 0;
    bytesInLine_ = 0;
    divider_ = 0;
    pointSys_ = 16;
    divEnabled_ = true;
    textOnly_ = false;
    mainLayout = new QVBoxLayout;
    toolsLayout = new QHBoxLayout;
    systemChoice = new QRadioButton[5];
    systemChoice[0].setText("BIN");
    systemChoice[1].setText("QUAD");
    systemChoice[2].setText("OCT");
    systemChoice[3].setText("HEX");
    systemChoice[3].setChecked(true);
    systemChoice[4].setText("ASCII");
    radioLayout = new QVBoxLayout;
    for (int i = 0; i < 5; i++)
    {
        connect(&systemChoice[i], SIGNAL(clicked()), this, SLOT(radioHandler()));
        radioLayout->addWidget(&systemChoice[i]);
    }
    toolsLayout->addLayout(radioLayout);

    lineLbl = new QLabel("Lines: ");
    lineLbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lineField = new QLineEdit("40");
    connect(lineField, SIGNAL(editingFinished()), this, SLOT(lineFieldHandler()));
    lineField->setFixedWidth(50);
    divsLbl = new QLabel("Divide symbols by: ");
    divsLbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    divsField = new QLineEdit("4");
    connect(divsField, SIGNAL(editingFinished()), this, SLOT(divsFieldHandler()));
    divsField->setFixedWidth(50);
    settingsLayout = new QVBoxLayout[2];
    settingsLayout[0].addWidget(lineLbl);
    settingsLayout[0].addWidget(divsLbl);
    settingsLayout[0].setAlignment(Qt::AlignLeft);
    settingsLayout[1].addWidget(lineField);
    settingsLayout[1].addWidget(divsField);
    settingsLayout[1].setAlignment(Qt::AlignLeft);
    topSettingsLayout = new QHBoxLayout;
    topSettingsLayout->addLayout(&settingsLayout[0]);
    topSettingsLayout->addLayout(&settingsLayout[1]);
    topSettingsLayout->setSpacing(10);
    topSettingsLayout->setAlignment(Qt::AlignLeft);
    toolsLayout->addLayout(topSettingsLayout);

    autoBytesCheck = new QCheckBox("auto-size bytes in line", this);
    autoBytesCheck->setChecked(true);
    tableCheck = new QCheckBox("print as table", this);
    tableCheck->setChecked(false);
    checkboxesLayout = new QVBoxLayout;
    checkboxesLayout->addWidget(autoBytesCheck);
    checkboxesLayout->addWidget(tableCheck);
    toolsLayout->addLayout(checkboxesLayout);

    clearBtn = new QPushButton("Clear");
    connect(clearBtn, SIGNAL(pressed()), this, SLOT(clearHandler()));
    exportBtn = new QPushButton("Export");
    connect(exportBtn, SIGNAL(pressed()), this, SLOT(exportHandler()));
    clearBtn->setFixedHeight(25);
    clearBtn->setFixedWidth(50);
    exportBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    exportBtn->setFixedHeight(25);
    exportBtn->setFixedWidth(50);
    toolsLayout->addWidget(clearBtn);
    toolsLayout->addWidget(exportBtn);

    workLayout = new QHBoxLayout;
    rawField = new QPlainTextEdit;
    asciiField = new QPlainTextEdit;
    scroller = new QScrollBar();
    scroller->setVisible(true);
    rawField->setVerticalScrollBar(scroller);
    asciiField->setVerticalScrollBar(scroller);
    this->realloc_array(40, 4, 4);
    mainLayout->addLayout(toolsLayout);
    mainLayout->addLayout(workLayout);
    setLayout(mainLayout);
    this->setMinimumWidth(500);
    this->setMinimumHeight(250);
    this->resize(500,300);
    workLayout->setSizeConstraint(QLayout::SetMinimumSize);
    workLayout->setSpacing(0);
    this->rebuildLayout();
}

//====================================================
//          Public-методы
//====================================================


void QHexView::autoResize()
{
        QFontMetrics fm(asciiField->font());
        if (textOnly_)
        {
           realloc_array(linesAmt_, (this->width() * 0.9) / fm.width(' '), divider_);
        }
        else
        {
            realloc_array(linesAmt_, (this->width())/ (fm.width(' ') * (pointSys_ >= 16 ? 5.4 : pointSys_ >= 8 ? 4 : pointSys_ >= 4 ? 7 : 9)), divider_);
        }
        this->rebuildLayout();
}

int QHexView::realloc_array(int lines, int bytesInLine, int divider)
{
    if (lines * bytesInLine == 0)
    {
        return -1;
    }
    if (lines*bytesInLine < byteArray_.size())
    {
        byteArray_.erase(byteArray_.begin(), byteArray_.end()-lines*bytesInLine);
    }
    bytesInLine_ = bytesInLine;
    divider_ = divider;
    linesAmt_ = lines;
    this->rebuildLayout();
    return 1;
}

int QHexView::set_lines(int linesAmt)
{
    return this->realloc_array(linesAmt, bytesInLine_, divider_);
}

int QHexView::set_line_length(int bytesInLine)
{
    return this->realloc_array(linesAmt_, bytesInLine, divider_);
}

int QHexView::set_divider(int divider)
{
    return this->realloc_array(linesAmt_, bytesInLine_, divider);
}

void QHexView::clear()
{
    byteArray_.clear();
    byteArray_.erase(byteArray_.begin(), byteArray_.end());
    byteArray_.resize(0);
    this->rebuildLayout();
}

int QHexView::insert(const QByteArray& data, int pos)
{
    if (pos > byteArray_.size() || pos < -1)
        return -1;
    for (int i = 0; i < data.size(); i++)
    {
        if (pos > -1)
            byteArray_.insert(byteArray_.begin()+pos+i, data.at(i));
        else
            byteArray_.insert(byteArray_.end(), data.at(i));
    }
    realloc_array(linesAmt_, bytesInLine_, divider_);
    return 1;
}

void QHexView::div_enabled(bool en)
{
    divEnabled_ = en;
    this->reprint();
}

void QHexView::put_char(unsigned char ch)
{
    this->printChar(ch);
}

void QHexView::put_data(const QByteArray &byteOutput)
{
    for (int i = 0; i < byteOutput.size(); i++)
    {
        this->put_char(byteOutput.at(i));
    }
}

void QHexView::set_point_system(int ps)
{
    if (ps==2 || ps==4 || ps==8 || ps==16)
        pointSys_ = ps;
    this->autoResize();
    rebuildLayout();
}

//==========================================
void QHexView::rebuildLayout()
{
    this->clearLayout(workLayout);
    asciiField->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    rawField->setReadOnly(true);
    asciiField->setReadOnly(true);
    QFont f("unexistent");
    f.setStyleHint(QFont::Monospace);
    asciiField->setFont(f);
    rawField->setFont(f);

    if (textOnly_ == false)
    {
        rawField->setVisible(true);
        workLayout->addWidget(rawField, symsInPS());
    }
    else
    {
        rawField->setVisible(false);
    }
    workLayout->addWidget(asciiField, 1);
    this->reprint();

}


//==========================================

void QHexView::clearLayout( QLayout *layout) {
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        delete item;
    }
}



QHexView::~QHexView()
{
    delete scroller;
    delete mainLayout;
    delete workLayout;
    delete toolsLayout;
    delete[]systemChoice;
    delete lineField;
    delete divsField;
    delete rawField;
    delete asciiField;
    delete checkboxesLayout;
    delete clearBtn;
    delete exportBtn;
    delete topSettingsLayout;
    delete divsLbl;
    delete settingsLayout;
    delete lineLbl;
    delete radioLayout;
    delete autoBytesCheck;
    delete tableCheck;
    delete ui;
}

void QHexView::resizeEvent(QResizeEvent*)
{
    this->autoResize();
}

void QHexView::radioHandler()
{
    if (systemChoice[0].isChecked())
    {
        this->textOnly_ = false;
        this->set_point_system(2);
    }
    if (systemChoice[1].isChecked())
    {
        this->textOnly_ = false;
        this->set_point_system(4);
    }
    if (systemChoice[2].isChecked())
    {
        this->textOnly_ = false;
        this->set_point_system(8);
    }
    if (systemChoice[3].isChecked())
    {
        this->textOnly_ = false;
        this->set_point_system(16);
    }
    if (systemChoice[4].isChecked())
    {
        this->textOnly_ = true;
        this->autoResize();
        this->rebuildLayout();
    }
}

void QHexView::divsFieldHandler()
{
    if (divsField->text().toInt() > 0)
    {
        divEnabled_ = true;
        divider_ = divsField->text().toInt();
    }
    else
        divEnabled_ = false;
    this->reprint();
}

void QHexView::lineFieldHandler()
{
    linesAmt_ = lineField->text().toInt();
    realloc_array(linesAmt_,bytesInLine_,divider_);
}

void QHexView::clearHandler()
{
    this->clear();
}

void QHexView::exportHandler()
{
    this->exportFile();
}

void QHexView::exportFile(std::string filename)
{
    std::ofstream f;
    f.open(filename.c_str());
    for (int i = 0; i < byteArray_.size(); i++)
        f << byteArray_.at(i);
    f.close();
}


inline int QHexView::symsInPS()
{
    return (pointSys_ >= 16 ? 2 : pointSys_ >= 8 ? 4 : pointSys_ >= 4 ? 6 : 8);
}

//======================================================
void QHexView::removeUpperStringFromField(QPlainTextEdit* qpte)
{

    QTextCursor cursor = qpte->textCursor();
    QTextCursor buf = qpte->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 0);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    cursor.deleteChar();
    qpte->setTextCursor(buf);
}

void QHexView::appendSymbolToField(char ch, QPlainTextEdit* qpte)
{
    QTextCursor cursor = qpte->textCursor();
    qpte->moveCursor (QTextCursor::End);
    qpte->insertPlainText (QString(ch));
    qpte->setTextCursor(cursor);
}

void QHexView::appendStringToField(QString str, QPlainTextEdit* qpte)
{
    QTextCursor cursor = qpte->textCursor();
    qpte->moveCursor (QTextCursor::End);
    qpte->insertPlainText (str);
    qpte->setTextCursor(cursor);
}

int QHexView::printCharOnAscii(unsigned char ch)
{
    if (ch >= 31 && ch <= 126)
    {
        appendSymbolToField(ch, asciiField);
    }
    else
        appendSymbolToField('.', asciiField);
    if (asciiField->toPlainText().split("\n").at(asciiField->toPlainText().split("\n").size()-1).size() + 1 > bytesInLine_)
    {
        appendSymbolToField('\n', asciiField);
    }
    if (asciiField->toPlainText().split("\n").size() >= linesAmt_)
    {
        this->removeUpperStringFromField(asciiField);
        return -1;
    }
    return 0;
}

int QHexView::printCharOnRaw(unsigned char ch)
{
    char alphabet[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A','B','C','D','E','F'};

    QString num = "";
    unsigned char data = ch;
    for (int digitsCount = 0; digitsCount < this->symsInPS(); digitsCount++)
    {
        num = alphabet[data%pointSys_]+num;
        data = data / pointSys_;
    }
    if (rawField->toPlainText().split('\n').at(rawField->toPlainText().split('\n').size()-1).count(QRegExp("[ |]")) % divider_ == 0)
        appendStringToField('|'+num, rawField);
    else
        appendStringToField(' '+num, rawField);
    if (rawField->toPlainText().split("\n").at(rawField->toPlainText().split("\n").size()-1).split(QRegExp("[ |]")).size() > bytesInLine_)
    {
        appendSymbolToField('\n', rawField);
    }
    if (rawField->toPlainText().split("\n").size() > linesAmt_)
    {
        this->removeUpperStringFromField(rawField);
        return -1;
    }
    return 0;
}

void QHexView::reprint()
{
    asciiField->clear();
    rawField->clear();
    for (int i = 0; i < byteArray_.size(); i++)
    {
        printCharOnAscii(byteArray_.at(i));
        if (!textOnly_)
            printCharOnRaw(byteArray_.at(i));
    }
}

void QHexView::printChar(unsigned char ch)
{
    byteArray_.push_back(ch);
    int scroller_val = scroller->value();
    bool isMax = (scroller_val > scroller->maximum() - 1);
    int shift = printCharOnAscii(ch);
    if (!textOnly_)
        printCharOnRaw(ch);
    if (isMax)
    {
        scroller->setValue(scroller->maximum());
    }
    else
    {
        if (!isMax)
        {
            scroller->setValue(scroller_val+shift);
        }
    }
    if (shift < 0)
        byteArray_.erase(byteArray_.begin(), byteArray_.begin()+bytesInLine_);
}

void QHexView::printQString(QString str)
{
    for (int i = 0; i < str.size(); i++)
    {
        this->printChar(str.toLatin1().at(i));
    }
}

















void QHexView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_1  )
    {
        bool isMax = (scroller->value() > scroller->maximum()-1);
        for(int i = 0; i < 2000; i++)
            this->printChar((unsigned char)qrand());
       // if (isMax)
        //    scroller->setValue(scroller->maximum());
    }
    if(event->key() == Qt::Key_2)
    {
        char ch;
        ifstream f;
        f.open("test.gbc", std::ifstream::in);
        if (f.is_open())
            while(!f.eof())
            {
                f >> ch;
                this->printChar(ch);
            }
    }
    if(event->key() == Qt::Key_3)
    {

        QString str = "";
        ifstream f;
        char ch;
        QElapsedTimer t;
        f.open("test.gbc", std::ifstream::in);
        t.start();
        while(!f.eof())
        {
            f >> ch;
            this->printChar(ch);
        }
        cout << t.elapsed() << " milliseconds, " << byteArray_.size() << " bytes in log." << endl;
        f.close();
    }
    if(event->key() == Qt::Key_4)
    {
        if (pointSys_ == 16)
            this->set_point_system(2);
        else
            this->set_point_system(pointSys_ * 2);
    }
    if(event->key() == Qt::Key_5)
    {
        this->clear();
    }
    if(event->key() == Qt::Key_6)
    {
        this->realloc_array(linesAmt_,bytesInLine_+1,divider_);
    }
    if(event->key() == Qt::Key_7)
    {
        removeUpperStringFromField(asciiField);
        if(!textOnly_)
            removeUpperStringFromField(rawField);
    }
    if(event->key() == Qt::Key_8)
    {

    }

    if (event->key() < Qt::Key_1 || event->key() > Qt::Key_9)
    {
        //this->put_char(event->key());
        printChar(event->key());
        //this->updateInfo();
    }
}
