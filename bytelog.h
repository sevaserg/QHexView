#ifndef BYTELOG_H
#define BYTELOG_H
template <typename T>
class byteLog
{
private:
    T* data_;
    int size_;
    int maximum_;
    int lineSize_;
    int lines_;
public:
    byteLog();
    int size();
    int max();
    void setMax(int newMax);
    void setMax(int linesAmt, int lineSize);
    void setLineSize(int newLineSize);
    int lineSize();
    void push(T data);
    int push(T* data,int amt);
    T get(int elementNum);
    T* getLine(int lineNum);
    void exportInFile();
    void clear();
    ~byteLog();
};

#endif // BYTELOG_H
