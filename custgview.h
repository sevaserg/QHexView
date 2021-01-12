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
    QGraphicsRectItem* colChoice, *colChoice2, *tabChoice, *hexChoice, *asciiChoice;
public:
    int col[2];
    int col2[2];
    int tab[2];
    custGView();
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
