#ifndef BYTELOG_H
#define BYTELOG_H
template <typename T>
class byteLog
{
private:
    T* data_;
    int size_;
    int maximum_;
public:
    byteLog();
    int size();
    int max();
    void changeMax(int newMax);
    void push(T data);
    int push(T* data,int amt); // returns shift
    T get(int elementNum);
    T* get(int lineNum, int lineSize);
    void exportInFile();
    void clear();
    ~byteLog();
};
#endif // BYTELOG_H
