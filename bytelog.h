#ifndef BYTELOG_H
#define BYTELOG_H

class byteLog
{
private:
    unsigned char* data_;
    unsigned char* buffer;
    int firstSel_;
    int secondSel_;
    int lineSize_;
    int lines_;
    int size_;
    int maximum_;
    int asciiShift_;
    int lastAsciiSel1_;
    int lastAsciiSel2_;
public:
    byteLog();
    int size();
    int max_();
    void setMax(int newMax);
    void setMax(int linesAmt, int lineSize);
    void setLineSize(int newLineSize);
    int lineSize();
    int maxLines();
    void setFirstSel(int num);
    void setSecondSel(int num);
    int firstSel();
    int secondSel();
    int lastAsciiSel1();
    int lastAsciiSel2();
    int ALNToBLN(int ALN);
    int BLNToALN(int BLN);
    void push(unsigned char data);
    int push(const unsigned char* data,int amt);
    unsigned char get(int elementNum);
    unsigned char* getLine(int lineNum);
    void exportInFile();
    void clear();
    unsigned char* data();
    unsigned char at(int pos);
    unsigned char* toText(int ps);
    int linesAmt();
    int asciiLines();
    int getFirstSymInAsciiLine(int lineNum);
    char* getHighlighted();
    int lastLineSize();
    int asciiLineLen(int lineNum);
    int asciiShift();
    unsigned char* asciiLine(int lineNum);
    ~byteLog();
};


#endif // BYTELOG_H
