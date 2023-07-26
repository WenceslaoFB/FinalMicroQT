#include "qform1.h"
#include "ui_qform1.h"

QForm1::QForm1(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QForm1)
{
    ui->setupUi(this);

    QSerialPort1=new QSerialPort(this);
    connect(QSerialPort1,&QSerialPort::readyRead,this,&QForm1::OnQSerialPortRx);

    QSerialPort1->setPortName("COM5");
    QSerialPort1->setBaudRate(9600);    //velocidad de comunicacion
    QSerialPort1->open(QSerialPort::ReadWrite);

    QTimer2 =new QTimer(this);
    connect(QTimer2, &QTimer::timeout, this, &QForm1::OnQTimer2);

    QUdpSocket1=new QUdpSocket(this);
    connect(QUdpSocket1,&QUdpSocket::readyRead,this,&QForm1::onQUdpSocket1Rx);

}

QForm1::~QForm1()
{
    if(QUdpSocket1->isOpen())
           QUdpSocket1->abort();
    delete QUdpSocket1;

    delete QTimer2;
    delete QSerialPort1;
    delete  QPaintBox1;
    delete ui;
}

void QForm1::HeaderToTX(){
    TX[0]='U';
    cks='U';

    TX[1]='N';
    cks^='N';

    TX[2]='E';
    cks^='E';

    TX[3]='R';
    cks^='R';

    TX[4]=Nd.value;
    cks^=Nd.value;

    TX[5]=':';
    cks^=':';
}

void QForm1::onQUdpSocket1Rx(){
    int count;
    uint8_t *buf;


    while(QUdpSocket1->hasPendingDatagrams()){
        count=QUdpSocket1->pendingDatagramSize();
        buf=new uint8_t[count];
        QUdpSocket1->readDatagram((char *)buf,count,&hostAddress,&hostPort);
        for (uint8_t j=0;j<count;j++) {
            buff_UDP[indW_UDP]=buf[j];
            indW_UDP++;
        }
        ui->lineEdit_2->setText((char *)buf);
        while(indW_UDP != indR_UDP){
            switch(status_UDP){
                case 1:
                    if(DecodificarHeader(buff_UDP,&indR_UDP,&indW_UDP,&cksUDP ))
                        status_UDP++;
                    break;
                case 2:
                    if(CksVerif(buff_UDP,&indR_UDP,&indW_UDP,&cksUDP))
                        status_UDP++;
                    else
                        status_UDP=1;
                    break;
                case 3:
                    indR_UDP++;
                    CMD(buff_UDP);
                    status_UDP=1;
                    break;
            }
        }
        delete [] buf;
    }
}
void QForm1::QUdpSocket1Tx()//transmitir
{


    switch(comando){

        case 0xF0:      //ALIVE

            Nd.value=0x02;
            HeaderToTX();
            TX[7]= 0xF0;
            cks^=0xF0;
            TX[8]=cks;
            break;
        case 0xD0: //MOVER MOTORES
            PWM_motor1.i32=200;
            PWM_motor2.i32=200;
            jobTime.i32=5000;//5 seg

            Nd.value=14;
            HeaderToTX();
            TX[6]= 0xD0;
            cks^=0xD0;
            TX[7]=PWM_motor1.u8[0];
            cks^=TX[7];
            TX[8]=PWM_motor1.u8[1];
            cks^=TX[8];
            TX[9]=PWM_motor1.u8[2];
            cks^=TX[9];
            TX[10]=PWM_motor1.u8[3];
            cks^=TX[10];

            TX[11]=PWM_motor2.u8[0];
            cks^=TX[11];
            TX[12]=PWM_motor2.u8[1];
            cks^=TX[12];
            TX[13]=PWM_motor2.u8[2];
            cks^=TX[13];
            TX[14]=PWM_motor2.u8[3];
            cks^=TX[14];

            TX[15]=jobTime.u8[0];
            cks^=TX[15];
            TX[16]=jobTime.u8[1];
            cks^=TX[16];
            TX[17]=jobTime.u8[2];
            cks^=TX[17];
            TX[18]=jobTime.u8[3];
            cks^=TX[18];

            TX[19]=cks;
            break;
        case 0xA0:
            Nd.value=0x04;
            HeaderToTX();

            TX[7]= comando;
            cks^=comando;

            TX[8]= TimeInterval;
            cks^=TimeInterval;

            TX[9]= TimeInterval>>8;
            cks^=TimeInterval>>8;

            TX[10]=cks;
            break;

        case 0xA1:

            Nd.value=0x04;
            HeaderToTX();

            TX[7]= comando;
            cks^=comando;

            TX[8]= TimeInterval;
            cks^=TimeInterval;

            TX[9]= TimeInterval>>8;
            cks^=TimeInterval>>8;

            TX[10]=cks;
            break;

        case 0xA4:
            Nd.value=0x08;
            HeaderToTX();

            TX[7]= comando;
            cks^=comando;

            TX[8]= TimeInterval;
            cks^=TimeInterval;

            TX[9]= TimeInterval>>8;
            cks^=TimeInterval>>8;


            TX[10]=RPM_DER;
            cks^=RPM_DER;

            TX[11]=RPM_DER>>8;
            cks^=(RPM_DER>>8);

            TX[12]=RPM_IZQ;
            cks^=RPM_IZQ;

            TX[13]=RPM_IZQ>>8;
            cks^=(RPM_IZQ>>8);

            TX[14]=cks;
            break;
        case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4:
            Nd.value=18;
            HeaderToTX();

            TX[7]= comando;
            cks^=comando;

            TX[8]= maxS1;
            cks^=maxS1;

            TX[9]= (maxS1>>8);
            cks^=(maxS1>>8);

            TX[10]= minS1;
            cks^=minS1;

            TX[11]=  (minS1>>8);
            cks^=(minS1>>8);

            TX[12]= maxS2;
            cks^=maxS2;

            TX[13]=  (maxS2>>8);
            cks^=(maxS2>>8);

            TX[14]= minS2;
            cks^=minS2;

            TX[15]=  (minS2>>8);
            cks^=(minS2>>8);

            TX[16]= maxS3;
            cks^=maxS3;

            TX[17]=  (maxS3>>8);
            cks^=(maxS3>>8);

            TX[18]= minS3;
            cks^=minS3;

            TX[19]=  (minS3>>8);
            cks^=(minS3>>8);

            TX[20]= maxS4;
            cks^=maxS4;

            TX[21]=  (maxS4>>8);
            cks^=(maxS4>>8);

            TX[22]= minS4;
            cks^=minS4;

            TX[23]=  (minS4>>8);
            cks^=(minS4>>8);

            TX[24]= cks;
            break;

    }
    //QSerialPort1->write((char *) TX, TX[4]+7 );
    QUdpSocket1->writeDatagram(TX,20,hostAddress,hostPort);
    ui->plainTextEdit->setPlainText(TX);
}


void QForm1::OnQSerialPortRx()
{
    qint64 count;
    quint8 *buf;
    QString strhex,s;

    count=QSerialPort1->bytesAvailable();
    if(count<=0)
        return;

    buf=new quint8[count];
    QSerialPort1->read((char *)buf,count);

    strhex="<-- ";

    for (int i=0;i<count;i++){
        buff_Serial[indW_Serial]=buf[i];
        indW_Serial++;
//        s.append(buf[i]);
    }
//    ui->plainTextEdit->appendPlainText(s);
    while(indW_Serial != indR_Serial){
        switch(status){
            case 1:
                if(DecodificarHeader(buff_Serial,&indR_Serial,&indW_Serial,&cksSerial))
                    status++;
                break;
            case 2:
                switch(CksVerif(buff_Serial,&indR_Serial,&indW_Serial,&cksSerial)){
                    case 1:
                        status++;
                        break;
                    case 2:
                        status=1;
                        break;
                }
                break;
            case 3:
                indR_Serial++;
                CMD(buff_Serial);
                status=1;
                break;
        }
    }
    delete[] buf;
}

void QForm1::CMD(uint8_t *buff)
{
    comando=buff[cmdPos_inBuff];
    switch(buff[cmdPos_inBuff]){
        case 0xF0:
            ui->plainTextEdit->setPlainText("ALIVE");
            break;
        case 0xA1:
           QString s,s2;

           Sensor[0].u32=0;
           Sensor[0].u8[0]=buff[cmdPos_inBuff+1];
           Sensor[0].u8[1]=buff[cmdPos_inBuff+2];

           s2=s2.number(Sensor[0].u32);
           s="Sensor 1:\t";
           s.append(s2);

           Sensor[1].u32=0;
           Sensor[1].u8[0]=buff[cmdPos_inBuff+3];
           Sensor[1].u8[1]=buff[cmdPos_inBuff+4];
           s2=s2.number(Sensor[1].u32);
           s.append("\r\nSensor 2:\t");
           s.append(s2);


           Sensor[2].u32=0;
           Sensor[2].u8[0]=buff[cmdPos_inBuff+5];
           Sensor[2].u8[1]=buff[cmdPos_inBuff+6];
           s2=s2.number(Sensor[2].u32);
           s.append("\r\nSensor 3:\t");
           s.append(s2);

           Sensor[3].u32=0;
           Sensor[3].u8[0]=buff[cmdPos_inBuff+7];
           Sensor[3].u8[1]=buff[cmdPos_inBuff+8];
           s2=s2.number(Sensor[3].u32);
           s.append("\r\nSensor 4:\t");
           s.append(s2);

           Sensor[4].u32=0;
           Sensor[4].u8[0]=buff[cmdPos_inBuff+9];
           Sensor[4].u8[1]=buff[cmdPos_inBuff+10];
           s.append("\r\nSensor 5:\t");
           s2=s2.number(Sensor[4].u32);
           s.append(s2);

           Sensor[5].u32=0;
           Sensor[5].u8[0]=buff[cmdPos_inBuff+11];
           Sensor[5].u8[1]=buff[cmdPos_inBuff+12];
           s.append("\r\nSensor 6:\t");
           s2=s2.number(Sensor[5].u32);
           s.append(s2);

           Sensor[6].u32=0;
           Sensor[6].u8[0]=buff[cmdPos_inBuff+13];
           Sensor[6].u8[1]=buff[cmdPos_inBuff+14];
           s.append("\r\nSensor 7:\t");
           s2=s2.number(Sensor[6].u32);
           s.append(s2);

           Sensor[7].u32=0;
           Sensor[7].u8[0]=buff[cmdPos_inBuff+15];
           Sensor[7].u8[1]=buff[cmdPos_inBuff+16];
           s.append("\r\nSensor 8:\t");
           s2=s2.number(Sensor[7].u32);
           s.append(s2);
           ui->plainTextEdit->setPlainText(s);

           break;
    }
}

void QForm1::OnQTimer2()
{
    if(status==0)
        resetDecode=1;
    QTimer2->stop();
}

uint8_t QForm1::CksVerif(uint8_t * buff,uint8_t * indR,uint8_t * indW,uint8_t * cksRX)
{
    static uint8_t p=1;

    if((*indR!=*indW)&&(p<cantBytes)){
        if(p==1)
            cmdPos_inBuff=*indR;

        *cksRX^=buff[*indR];
        *indR+=1;
        p++;
    }

    if((*indR!=*indW)&&(p==cantBytes)){
        p=1;
        if(*cksRX==buff[*indR]){
            return 1;
        }
        return 2;
    }
    return 0;

}



uint8_t QForm1::DecodificarHeader(uint8_t * buff,uint8_t * indR,uint8_t * indW,uint8_t * cksRX)
{
    static uint8_t indexHeader=0;



    switch(indexHeader){
        case 0:
            if(buff[*indR]=='U'){
                *cksRX=buff[*indR];
            }
            else {
                indexHeader=0;
                *indR=*indW;
                status=1;
                return 0;
            }
            break;

        case 1:
            if(buff[*indR]=='N'){
                *cksRX^=buff[*indR];
            }
            else {
                indexHeader=0;
                *indR=*indW;
                status=1;
                return 0;
            }
            break;

        case 2:
            if(buff[*indR]=='E'){
                *cksRX^=buff[*indR];
            }
            else {
                indexHeader=0;
                *indR=*indW;
                status=1;
                return 0;
            }
            break;

        case 3:
            if(buff[*indR]=='R'){
                *cksRX^=buff[*indR];
            }
            else {
                indexHeader=0;
                *indR=*indW;
                status=1;
                return 0;
            }
            break;

        case 4:

            cantBytes= buff[*indR];
            *cksRX^= buff[*indR];

            break;

        case 5:
            if(buff[*indR]==':'){
                *cksRX^=buff[*indR];
                indexHeader=0;
                *indR+=1;
                return 1;
            }
            else{
                indexHeader=0;
                *indR=*indW;
                status=1;
                return 0;
            }
            break;
    }
    *indR+=1;
    indexHeader++;
    return 0;
}

void QForm1::on_pushButton_clicked()
{
    switch(ui->comboBox->currentIndex()){
        case 0:
            comando=0xD0;
            PWM_motor1.i32=ui->spinBox_11->value();
            PWM_motor2.i32=ui->spinBox_12->value();
            jobTime.i32=ui->spinBox->value();
        break;
        case 1:
            comando=0xA0;
            break;

    }
    QUdpSocket1Tx();
}



void QForm1::on_pushButton_3_clicked()
{
    quint16 port;
    bool ok;

    if(QUdpSocket1->isOpen()){
        QUdpSocket1->abort();
        ui->pushButton_3->setText("OPEN");
    }
    else{
        port =ui->lineEdit->text().toInt(&ok);
        if(!ok)
            return;
        if(!QUdpSocket1->bind(port)){
               QMessageBox::warning(this,"UDP PORT","Can't BIND PORT");
               return;
        }

        QUdpSocket1->open(QUdpSocket::ReadWrite);

        ui->pushButton_3->setText("CLOSE");
    }
}
