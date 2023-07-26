#ifndef QFORM1_H
#define QFORM1_H

#include <QMainWindow>
#include <QTimer>
#include <math.h>
#include <QtSerialPort/QSerialPort>
#include "qpaintbox.h"
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

namespace Ui {
class QForm1;
}

class QForm1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit QForm1(QWidget *parent = nullptr);
    ~QForm1();

private slots:

    void onQUdpSocket1Rx();

    void on_pushButton_clicked();

    void QUdpSocket1Tx();

    void OnQSerialPortRx();

    uint8_t DecodificarHeader(uint8_t * buff,uint8_t * indR,uint8_t * indW,uint8_t * cksRX);

    uint8_t CksVerif(uint8_t * buff,uint8_t * indR,uint8_t * indW,uint8_t * cksRX);

    void CMD(uint8_t *buff);

    void OnQTimer2();

    void HeaderToTX();

    void on_pushButton_3_clicked();

private:
    Ui::QForm1 *ui;
    QTimer *QTimer2;
    QSerialPort *QSerialPort1;

    QUdpSocket *QUdpSocket1;
    QHostAddress hostAddress;
    quint16 hostPort;
    uint8_t *buf;

    QPaintBox *QPaintBox1,*QPaintBox2,*QPaintBox3;

    //comunicacion serial
    uint8_t buff_Serial[256];
    uint8_t indW_Serial=0,indR_Serial=0;

    uint8_t buff_UDP[256];
    uint8_t indW_UDP=0,indR_UDP=0;


    uint8_t cksUDP,cksSerial;
    uint8_t status=1,status_UDP=1;
    uint8_t cmdPos_inBuff;
    uint8_t resetDecode=0;
    uint8_t comando;

    uint16_t TimeInterval;

    int anchoFrame,largoFrame,x=0,xAntes=0,yAntes=0,yAntes2=0,yAntes3=0,yAntes4=0,yAntes5=0,yAntes6=0,yAntes7=0,yAntes8=0;
    int y,y2,y3,y4,y5,y6,y7,y8;
    typedef union{
        uint16_t value;
        uint8_t v[2];
    }unionNd;

    typedef union{
        uint8_t u8[4];
        int8_t  i8[4];

        uint16_t u16[2];
        int16_t  i16[2];

        uint32_t u32;
        int32_t  i32;

        float    f;
    }_sWork;

    _sWork PWM_motor1,PWM_motor2,jobTime;
    _sWork Sensor[8];

    unionNd Nd,Nd2;

    uint8_t cantBytes;
    uint8_t cks;


    uint8_t indR_IR=0,indW_IR=0,primero=0;
    int xActual=0;

    uint16_t maxS1,minS1,maxS2,minS2,maxS3,minS3,maxS4,minS4;

    uint16_t RPM_DER,RPM_IZQ;
    char TX[50];
};

#endif // QFORM1_H
