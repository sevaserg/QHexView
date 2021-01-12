#include "mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QVBoxLayout *main_lay = new QVBoxLayout;

    com_wgt = new ComPortWgt( "COM7");
    byteview_wgt = new QByteView;

    main_lay->addWidget( com_wgt );
    main_lay->addWidget( byteview_wgt );
    byteview_wgt->show();
    QWidget *central_wgt = new QWidget;
    central_wgt->setLayout( main_lay );

    this->setCentralWidget( central_wgt );
    this->resize( 1000,800 );

    connect( com_wgt, &ComPortWgt::send_data_from_com, this, &MainWindow::read_from_com );
}

MainWindow::~MainWindow()
{

}

void MainWindow::read_from_com ( const QByteArray & data )
{
    byteview_wgt->putData( data ) ;
}
