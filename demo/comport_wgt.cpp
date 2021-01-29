#include "comport_wgt.h"

ComPortWgt::ComPortWgt ( QString name, QGroupBox *parent ) : QGroupBox ( parent )
{
    lbl_com         = new QLabel      ( "Name:"       ) ;
    le_com_name     = new QLineEdit   ( name          ) ;
    btn_open_com    = new QPushButton ( "Open"        ) ;
    btn_close_com   = new QPushButton ( "Close"       ) ;
    layout_gbox     = new QHBoxLayout ;

    layout_gbox -> addWidget ( lbl_com       ) ;
    layout_gbox -> addWidget ( le_com_name   ) ;
    layout_gbox -> addWidget ( btn_open_com  ) ;
    layout_gbox -> addWidget ( btn_close_com ) ;

    layout_gbox     -> addStretch   ( 1 ) ;
    btn_close_com   -> setEnabled   ( false ) ;

    this    -> setLayout        ( layout_gbox ) ;
    this    -> setMaximumHeight ( 70          ) ;
    this    -> setStyleSheet    ( "QGroupBox { border: 2px }" ) ;

    m_serial = new QSerialPort ( ) ;


    //
    connect ( btn_open_com,     SIGNAL  ( clicked ( ) ),
              this,             SLOT    ( open_com_port( ) ) ) ;

    connect ( btn_close_com,    SIGNAL  ( clicked ( ) ),
              this,             SLOT    ( close_com_port ( ) ) ) ;

    connect ( m_serial,         SIGNAL  ( readyRead ( ) ),
              this,             SLOT    ( read_from_com ( ) ) ) ;
}

ComPortWgt::~ComPortWgt ()
{
    m_serial->close ( ) ;
    delete m_serial ;
}

//
void ComPortWgt::open_com_port ()
{
    m_serial -> setPortName ( le_com_name->text() ) ;
    m_serial -> setBaudRate ( QSerialPort::Baud115200 ) ;
    m_serial -> setDataBits ( QSerialPort::Data8 ) ;

    if ( !m_serial->open ( QSerialPort::ReadWrite ) )
    {
        QMessageBox message ;
        message.setText ( "Не удалось открыть ком порт" ) ;
        message.exec ( ) ;
    }
    else
    {
        btn_open_com    -> setEnabled ( false ) ;
        btn_close_com   -> setEnabled ( true  ) ;
        emit com_port_opening ( ) ;
        m_serial -> clear ( ) ;
    }

}

//
void ComPortWgt::close_com_port ( )
{
    if ( m_serial -> isOpen ( ) )
    {
        m_serial        -> close ( ) ;
        btn_open_com    -> setEnabled ( true  ) ;
        btn_close_com   -> setEnabled ( false ) ;
    }
    emit com_port_closed ( ) ;
}

//
void ComPortWgt::write_to_com ( QByteArray by )
{
        m_serial -> write ( by ) ;
        m_serial->setDataTerminalReady ( true ) ;
}

//
void ComPortWgt::read_from_com ( )
{
    QByteArray by ;
    by.append ( m_serial -> readAll ( ) ) ;
    emit send_data_from_com ( by ) ;
}

QString ComPortWgt::get_name ( )
{
    return le_com_name->text() ;
}

void ComPortWgt::set_name( const QString & name )
{
    le_com_name->setText( name ) ;
}

bool ComPortWgt::is_open()
{
    return m_serial->isOpen() ;
}

QSerialPort* ComPortWgt::get_serial ( )
{
    return m_serial ;
}
