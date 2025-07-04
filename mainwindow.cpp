#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QSerialPort>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort(this))
{
    ui->setupUi(this);

    // Updated widget signal connections
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &MainWindow::refreshPorts);
    connect(ui->pushButtonConnect, &QPushButton::clicked, this, &MainWindow::connectPort);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::startReading);
    connect(ui->pushButtonStop, &QPushButton::clicked, this, &MainWindow::stopReading);

    refreshPorts();
    ui->label->setText("Disconnected");
}

MainWindow::~MainWindow()
{
    stopReading();
    if (serialPort->isOpen())
        serialPort->close();
    delete ui;
}

void MainWindow::refreshPorts()
{
    ui->comboBox->clear();
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        ui->comboBox->addItem(info.portName());
    }
}

void MainWindow::connectPort()
{
    if (serialPort->isOpen())
        serialPort->close();

    QString portName = ui->comboBox->currentText();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "Error", "No COM port selected.");
        return;
    }

    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::Baud9600); // Adjust as needed
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Failed to open serial port.");
        ui->label->setText("Disconnected");
        return;
    }

    connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readData);
    ui->label->setText("Connected → " + portName);
}

void MainWindow::startReading()
{
    if (!serialPort->isOpen()) {
        QMessageBox::warning(this, "Error", "Serial port not connected.");
        return;
    }
    if (isReading) return;

    QString folderPath = desktopAppFolder();
    QDir().mkpath(folderPath);

    QString fileName = QString("%1/data_%2.bin")
                           .arg(folderPath)
                           .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    outputFile.setFileName(fileName);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "File Error", "Could not open output file.");
        return;
    }

    isReading = true;
    ui->plainTextEdit->appendPlainText("▶ Reading started...");
}

void MainWindow::stopReading()
{
    if (!isReading) return;

    isReading = false;
    outputFile.close();
    ui->plainTextEdit->appendPlainText("⏹ Reading stopped.");
}

void MainWindow::readData()
{
    if (!isReading) return;

    QByteArray data = serialPort->readAll();
    outputFile.write(data);

    QString hexData;
    for (auto byte : data)
        hexData += QString::asprintf("%02X ", static_cast<unsigned char>(byte));

    ui->plainTextEdit->appendPlainText(hexData.trimmed());
}

QString MainWindow::desktopAppFolder() const
{
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    return desktop + "/SerialReaderApp";
}
