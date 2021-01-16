#ifndef CUSTGVIEW_H
#define CUSTGVIEW_H

#include <QMouseEvent>

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>

class custGView : public QGraphicsView
{
private:
    bool isTextDisplayed_;
    int col[2];
    int col2[2];
    int tab[2];
    QGraphicsRectItem* colChoice, *colChoice2, *tabChoice, *hexChoice, *asciiChoice;
public:
    custGView();
    void switchViews(bool val);
    void clear();
    void initRect();
    void resize();
    void printRects();
    qreal getHexX();
    qreal getHexY();
    void mouseMoveEvent(QMouseEvent *event);
    void resetRectCoords();
    void upd();
    void diag();
    ~custGView();
};

#endif // CUSTGVIEW_H
