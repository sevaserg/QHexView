#ifndef BYTELOG_H
#define BYTELOG_H

class byteLog
{
private:
    unsigned char* data_;
    unsigned char* buffer;
    int lineSize_;
    int lines_;
    int size_;
    int maximum_;
    int asciiShift_;
public:
    byteLog();
    int size();
    int max();
    void setMax(int newMax);
    void setMax(int linesAmt, int lineSize);
    void setLineSize(int newLineSize);
    int lineSize();
    int maxLines();
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
    int lastLineSize();
    int asciiShift();
    unsigned char* asciiLine(int lineNum);
    ~byteLog();
};


#endif // BYTELOG_H
