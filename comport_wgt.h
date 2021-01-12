#ifndef COMPORT_WGT_H
#define COMPORT_WGT_H

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QMessageBox>
#include <QSerialPort>

class ComPortWgt : public QGroupBox
{
    Q_OBJECT

public:
    ComPortWgt  ( QString com_name, QGroupBox *parent = nullptr ) ;
    ~ComPortWgt ( ) ;

protected:
    QGroupBox        *gbox_comport   ;
    QLabel           *lbl_com        ;
    QLineEdit        *le_com_name    ;
    QPushButton      *btn_open_com   ;
    QPushButton      *btn_close_com  ;
    QHBoxLayout      *layout_gbox    ;

    QSerialPort      *m_serial ;

public slots:

    //
    void    write_to_com        ( QByteArray ) ;
    //
    void    open_com_port       ( ) ;
    //
    void    close_com_port      ( ) ;
    //
    void    read_from_com       ( ) ;
    //
    QString    get_name         ( ) ;
    //
    void       set_name         ( const QString & ) ;
    //
    bool       is_open          ( ) ;
    //
    QSerialPort*    get_serial      ( ) ;

signals:
    //
    void    com_port_opening ( ) ;
    //
    void    com_port_closed ( ) ;
    //
    void    send_data_from_com ( const QByteArray & ) ;
};

#endif // COMPORT_WGT_H
