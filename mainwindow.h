#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "serial/serial.h"
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>

#include "receivethread.h"

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
    void clearInfo();
    void showData(const QString&);

};
#endif // MAINWINDOW_H
