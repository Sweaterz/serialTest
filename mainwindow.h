#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <cstdlib>
#include <iomanip>      // std::put_time
#include <ctime>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "serial/serial.h"
#include "receivethread.h"
#include "crc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void initSerial();
    void openSerial();
    void updateReceiveAreaCurrentRow();
    void updateParseAreaCurrentRow();

    void showData(const QByteArray&);
    void slotCustomMenuRequested(QPoint pos);
    int parseData(std::vector<uint8_t>&);
    void clearSelectedInfo();
    void clearInfo();
    std::string getCurrentTime();



};
#endif // MAINWINDOW_H
