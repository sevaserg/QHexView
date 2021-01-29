#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "qhexview.h"
#include "qbyteview.h"
#include "comport_wgt.h"
#include "srch.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    srch *search_wgt;
    QByteView *byteview_wgt;
    ComPortWgt *com_wgt;
public slots:
    void read_from_com ( const QByteArray & );
    void search_exec ( const QByteArray & );
};

#endif // MAINWINDOW_H
