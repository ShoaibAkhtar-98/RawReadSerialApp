#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshPorts();
    void connectPort();
    void startReading();
    void stopReading();
    void readData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QFile outputFile;
    bool isReading = false;

    QString desktopAppFolder() const;
};

#endif // MAINWINDOW_H
