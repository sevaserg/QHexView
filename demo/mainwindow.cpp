#include "mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QVBoxLayout *main_lay = new QVBoxLayout;

    com_wgt = new ComPortWgt( "COM11");
    search_wgt = new srch;
    byteview_wgt = new QByteView(250);

    main_lay->addWidget( com_wgt );
    main_lay->addWidget( search_wgt );
    main_lay->addWidget( byteview_wgt );
    byteview_wgt->show();
    QWidget *central_wgt = new QWidget;
    central_wgt->setLayout( main_lay );

    this->setCentralWidget( central_wgt );
    this->resize( 1000,800 );

    connect( search_wgt, &srch::start_search, this, &MainWindow::search_exec );
    connect( com_wgt, &ComPortWgt::send_data_from_com, this, &MainWindow::read_from_com );

}

MainWindow::~MainWindow()
{

}

void MainWindow::search_exec(const QByteArray & QBA)
{
    byteview_wgt->search(QBA);
}

void MainWindow::read_from_com ( const QByteArray & data )
{
    byteview_wgt->putData( data ) ;
}
