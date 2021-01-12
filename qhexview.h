#ifndef QHEXVIEW_H
#define QHEXVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QString>
#include <vector>
#include <fstream>
#include <QPushButton>
#include <QFont>
#include <QRadioButton>
#include <QKeyEvent>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QElapsedTimer>
namespace Ui {
class QHexView;

}

class QHexView : public QWidget
{
    Q_OBJECT
public:
    explicit QHexView(QWidget *parent = 0);
    void put_char(unsigned char ch);
    void put_data(const QByteArray & byteOutput);
    void get_data( QString & textOutput);
    void get_data( QByteArray & byteOutput);
    int insert(const QByteArray& data, int pos = -1);
    void clear();
    int set_lines(int linesAmt);
    int set_line_length(int bytesInLine);
    int set_divider(int divider);
    void set_point_system(int ps);
    void div_enabled(bool en);
    ~QHexView();
protected:
    //==========================================
    void keyPressEvent(QKeyEvent *event);
    //==========================================
    void autoResize();
    void resizeEvent(QResizeEvent*);
    int realloc_array(int lines, int bytesInLine, int divider);
    void clearLayout(QLayout *layout); //https://stackoverflow.com/questions/4857188/clearing-a-layout-in-qt
    void rebuildLayout();
    int updateInfo();
    int fullUpdate();
    void exportFile(std::string filename = "./log.txt");
    inline int symsInPS();
    //==========================================
    void removeUpperStringFromField(QPlainTextEdit* qpte);
    int printCharOnAscii(unsigned char ch);
    int printCharOnRaw(unsigned char ch);
    void appendSymbolToField(char ch, QPlainTextEdit* qpte);
    void appendStringToField(QString str, QPlainTextEdit* qpte);
    void printChar(unsigned char ch);
    void printQString(QString str);
    void printDividers();
    void printDividersInString(int stringNum);
    void reprint();
    //==========================================
    QMoveEvent scrollerMoved(int a, int b);
    QVBoxLayout *checkboxesLayout;
    QPushButton* clearBtn;
    QPushButton* exportBtn;
    QHBoxLayout* topSettingsLayout;
    QLabel* divsLbl;
    QVBoxLayout* settingsLayout;
    QLabel* lineLbl;
    QVBoxLayout* radioLayout;
    QCheckBox *autoBytesCheck;
    QCheckBox *tableCheck;
    QPlainTextEdit* rawField;
    QPlainTextEdit* asciiField;
    QScrollBar* scroller;
    QVBoxLayout* mainLayout;
    QHBoxLayout* workLayout;
    QHBoxLayout* toolsLayout;
    Ui::QHexView *ui;
    QRadioButton* systemChoice;
    QLineEdit* lineField;
    QLineEdit* divsField;
    std::vector<unsigned char> byteArray_;
    //QByteArray byteArray_;
    int linesAmt_;
    int bytesInLine_;
    int divider_;
    int pointSys_;
    bool divEnabled_;
    bool pullDownFlag_;
    bool textOnly_;
    QFont font;
protected slots:
    void radioHandler();
    void lineFieldHandler();
    void divsFieldHandler();
    void clearHandler();
    void exportHandler();

};

#endif // QHEXVIEW_H
