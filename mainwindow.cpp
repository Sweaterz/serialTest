#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSerial();
    this->move(560, 280);

//    receiveThread *receive_t = new receiveThread;
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openSerial);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearInfo);
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::initSerial);
//    connect(ui->receiveDataArea, &QListWidget::mousePressEvent, this, &MainWindow::rightClickClear);


    ui->receiveDataArea->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->receiveDataArea, &QListWidget::customContextMenuRequested, this, &MainWindow::slotCustomMenuRequested);

}

void MainWindow::initSerial()
{
    std::vector<serial::PortInfo> allPortList = serial::list_ports();
    QStringList serialPorts;
    for(auto port: allPortList)
    {
        if(port.hardware_id != "n/a")
        {
//            qDebug() << port.port.c_str() << port.description.c_str() << port.hardware_id.c_str();

            serialPorts.append(port.port.c_str());
        }
    }
    if(serialPorts.size() != 0)
    {
        this->ui->serialBox->clear();
        this->ui->serialBox->addItems(serialPorts);

    }
    else
    {
        this->ui->serialBox->clear();
        this->ui->serialBox->addItem("None");
    }

}

void MainWindow::openSerial()
{
    std::string port = this->ui->serialBox->currentText().toStdString();
    uint32_t baudrate = std::stoi(this->ui->baudrateBox->currentText().toStdString());
    std::string dataBits = this->ui->dataBitsBox->currentText().toStdString();
    std::string stopBits = this->ui->stopBitsBox->currentText().toStdString();
    std::string parity = this->ui->parityBox->currentText().toStdString();

    static receiveThread *receive_t;

    if (this->ui->openButton->text() == "打开")
    {

        std::cout << "open serial " << std::endl;
        try
        {
            serial::Serial openedPort(port, baudrate, serial::Timeout::simpleTimeout(1000));
        }
        catch(serial::IOException e)
        {
            //        std::cout << e.what() << std::endl;
            QMessageBox::warning(NULL, "Title", e.what());
            return;
        }
        this->ui->openButton->setText("关闭");

        receive_t = new receiveThread();
        connect(receive_t, &receiveThread::receiveData, this, &MainWindow::showData);
        std::cout << baudrate << std::endl;
        receive_t->set(port, baudrate);
        receive_t->start();


    }
    else if (this->ui->openButton->text() == "关闭")
    {
        receive_t->stop();
        this->ui->openButton->setText("打开");
        disconnect(receive_t, &receiveThread::receiveData, this, &MainWindow::showData);

        delete receive_t;
    }


}

// 显示接收数据
void MainWindow::showData(const QByteArray& data)
{
//    std::cout << data.toStdString() << std::endl;
    int length = data.size();
    QString dataRemoveN;

    if(this->ui->hexOutput->isChecked())
    {
        std::cout << "十六进制输出模式" << std::endl;
        bool ok;

        std::cout << data.size() << std::endl;

        std::string strData(data.data(), data.size());
        std::vector<uint8_t> hexData;
        std::vector<std::string> hexStrData;
        for(int i = 0; i < strData.size(); i++)
        {
            std::stringstream hexStr;
            uint8_t hex_value = static_cast<uint8_t>(strData[i]);
            hexStr << std::hex << static_cast<uint32_t>(hex_value);
            hexData.push_back(hex_value);
            hexStrData.push_back(hexStr.str());
//            std::cout << static_cast<uint32_t>(hex_value) <<std::endl;
//            std::cout << hexStr.str() << std::endl;
        }

        std::string content;
        for(auto hexStr: hexStrData)
        {
            if (hexStr.size() == 1)
                content += "0";
            content += hexStr;
            content += " ";
        }
        content.pop_back();
        dataRemoveN = content.c_str();
        this->ui->receiveDataArea->addItem(dataRemoveN);
        this->parseData(hexData);


    }
    else
    {
        dataRemoveN = data.left(length - 1);
        std::cout << "原始文本输出模式" << std::endl;
        this->ui->receiveDataArea->addItem(dataRemoveN);
    }

    this->ui->receiveDataArea->setCurrentRow(this->ui->receiveDataArea->count() - 1);
}

int MainWindow::parseData(std::vector<uint8_t> &hexData)
{
    int length = hexData.size();
    std::string content;
    if(length < 17)
        return -1;
    // 心跳包
    if(hexData[0]==255 && hexData[1]==12 && hexData[2]==170)
    {
        bool HB_flag = false;
        int timeStamp[4];
        int xorByte = hexData[0] ^ hexData [1] ^ hexData[2];
        for(int i = 3; i < length; i++)
        {
            if(i < 11){
                if(hexData[i] != 0){
                    HB_flag = false;
                    std::cout << "心跳包 报文头异常"<< std::endl;
                    content += "心跳包 报文头异常";
                    this->ui->parseDataArea->addItem(content.c_str());
                    return -1;
                }
            }
            else if(i < 15){
                timeStamp[i-11] = hexData[i];
            }
            else if(i == length - 2){
                if(hexData[i] != 3){
                    HB_flag = false;
                    std::cout << "心跳包 结束位异常"<<std::endl;
                    content += "心跳包 结束位异常";
                    this->ui->parseDataArea->addItem(content.c_str());
                    return -1;
                }
            }
            else if(i == length - 1){
                if (hexData[i] == xorByte){
                    HB_flag = true;}
                else{
                    HB_flag = false;
                    std::cout << "心跳包 校验位异常" <<std::endl;
                    std::cout << xorByte << std::endl;
                    content += "心跳包 校验位异常";
                    this->ui->parseDataArea->addItem(content.c_str());
                    return -1;
                }
                break;
            }
            xorByte ^= hexData[i];
        }
        if (HB_flag)
        {
            content += "时间戳：";
            for(int i = 0; i < 4; i++)
            {
                content += std::to_string(timeStamp[i]);
                content += " ";
            }
            this->ui->parseDataArea->addItem(content.c_str());
        }
    }
    else if(hexData[0]==255 && hexData[1]==13 && hexData[2]==63)
    {
        bool flag = false;
        int timeStamp[4];
        int xorByte = hexData[0] ^ hexData [1] ^ hexData[2];
        int vehType;
        if(hexData[length - 2] != 3)
        {
            flag = false;
            std::cout << "停止位异常"<<std::endl;
            content += "停止位异常";
            this->ui->parseDataArea->addItem(content.c_str());
            return -1;
        }

        for (int i = 3; i < length - 1; ++i) {
           xorByte ^= hexData[i];
        }
        if (hexData[length - 1] == xorByte){
            flag = true;}
        else{
            flag = false;
            std::cout << "心跳包 校验位异常"<<std::endl;
            content += "心跳包 校验位异常";
            std::cout << xorByte << std::endl;
            this->ui->parseDataArea->addItem(content.c_str());
            return -1;
        }

        if(hexData[6] == 255)
        {
            content += "多轴: ";
            content += std::to_string(hexData[5]);
            content += "轴车";
        }
        else{
            content += "轴型： ";
            content += std::to_string(hexData[4] + 256* hexData[5]);
        }
//        content += " ";
        this->ui->parseDataArea->addItem(content.c_str());

    }
    else if(hexData[0]==255 && hexData[2]==3)
    {
        std::vector<uint8_t> sliced_vec(hexData.begin(), hexData.end() - 2);  // 切片向量
        uint16_t checkData = getCRC16(sliced_vec, sliced_vec.size());
        uint8_t check1 = checkData >> 8;
        uint8_t check2 = checkData & 0x00ff;
        std::cout << "校验位：" << static_cast<int>(check1) << " " << static_cast<int>(check2) << std::endl;
        if(hexData[length-2] != check1 || hexData[length-1] != check2)
        {
            content += "CRC校验位出错";
            std::cout << "CRC校验位出错" << std::endl;
            this->ui->parseDataArea->addItem(content.c_str());
            return -1;
        }
        else{
            uint8_t devNum = hexData[1];
            uint8_t number = hexData[3];                        //序号 1~10循环
            uint8_t dataFrameLength = hexData[4];               //数据帧长度 单位字节
            uint16_t timeYear = hexData[5] * 256 + hexData[6];  //年
            uint8_t timeMonth = hexData[7];
            uint8_t timeDay = hexData[8];
            uint8_t timeHour = hexData[9];
            uint8_t timeMinute = hexData[10];
            uint8_t timeSecond = hexData[11];
            uint8_t axleNum = hexData[12];
            uint16_t axleType = hexData[13] * 256 + hexData[14];
            uint16_t driveShaftLoc = hexData[15] * 256 + hexData[16];
            uint8_t driveShaftNum = hexData[17];
            uint8_t confidenceOfAxle = 100;
            uint16_t totalMassLimit = hexData[19] * 256 + hexData[20];   //总质量限值  单位10kg
//            std::string originalAxle(hexData.begin()+21, hexData.size()-23);
            std::string originalAxle;
            for(int i = 21; i < hexData.size() - 2; ++i)
            {
                char value = hexData[i];
                if(value == 0)
                    break;
                else
                    originalAxle += value;
            }
            std::cout << originalAxle << std::endl;

            content += "轴数：";
            content += std::to_string(axleNum);
            content += "  ";

            content += "轴型：";
            content += std::to_string(axleType);
            content += "  ";

            content += "驱动轴位置：";
            content += std::to_string(driveShaftLoc);
            content += "  ";

            content += "驱动轴个数：";
            content += std::to_string(driveShaftNum);
            content += "  ";

            content += "车辆原始轴型:";
            content += originalAxle;
            this->ui->parseDataArea->addItem(content.c_str());

        }
    }
    std::cout << content << std::endl;
    return 1;
}

void MainWindow::clearInfo()
{
    qDebug() << "clear info";
    this->ui->receiveDataArea->clear();
}

//实现右键菜单清除功能
void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction* clearInfo = new QAction("清除信息", this);
    connect(clearInfo, &QAction::triggered, this, &MainWindow::clearInfo);
    menu->addAction(clearInfo);
//    menu->addAction(new QAction("Action 2", this));
    menu->popup(ui->receiveDataArea->mapToGlobal(pos));
}



MainWindow::~MainWindow()
{
    delete ui;
}

