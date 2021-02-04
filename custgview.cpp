#include "custgview.h"
#include <iostream>
using namespace std;
custGView::custGView()
{
    isTextDisplayed_ = false;
    for (int i = 0; i < 2; i++)
    {
        tab[i] = 0;
        col[i] = 0;
        col2[i] = 0;
    }

}

void custGView::switchViews(bool val)
{
    isTextDisplayed_ = val;
}

void custGView::clear()
{
    qreal w = this->scene()->width();
    qreal h = this->scene()->height();
    this->scene()->clear();
    this->scene()->setSceneRect(0,0,w,h);
    this->initRect();
}

void custGView::initRect()
{
    if (!isTextDisplayed_)
        hexChoice = new QGraphicsRectItem(0,0,20,12);
    asciiChoice = new QGraphicsRectItem(0,0,7,12);
    printRects();
    if (!isTextDisplayed_)
        this->scene()->addItem(hexChoice);
    this->scene()->addItem(asciiChoice);
}

void custGView::printRects()
{
    if(!isTextDisplayed_)
    {
        hexChoice->setX(col[0]);
        hexChoice->setY(tab[0]);
    }
    asciiChoice->setX(col[1]);
    asciiChoice->setY(tab[1]);
}

qreal custGView::getHexX()
{
    return hexChoice->x();
}

qreal custGView::getHexY()
{
    return hexChoice->y();
}

qreal custGView::getX()
{
    if (isTextDisplayed_)
        return(col[1] / 8);
    else
        return(col[0] / 30);
}

qreal custGView::getY()
{
    if (isTextDisplayed_)
        return(tab[1] / 20);
    else
        return(tab[0] / 20);
}

void custGView::mouseMoveEvent(QMouseEvent *event)
{
    if (!isTextDisplayed_)
    {
        if (event->pos().x() < 600)
        {
            col[0] = event->pos().x() - event->pos().x() % 30;
            col[1] = static_cast<int>((col[0] - 50)/4.3+(this->width() > 755 ? 615 : 755));
            hexChoice->setX(col[0]);
            asciiChoice->setX(col[1]);
        }
        tab[0] = event->pos().y() - event->pos().y() % 20;
        tab[1] = tab[0];
        hexChoice->setY(tab[0]);
        asciiChoice->setY(tab[1]);
        printRects();
    }
    else
    {
        if (event->pos().x() > 5)
            col[1] = event->pos().x() - event->pos().x() % 8;
        tab[1] = event->pos().y() - event->pos().y() % 20;
        printRects();
    }
}

custGView::~custGView()
{
    this->scene()->clear();
}
