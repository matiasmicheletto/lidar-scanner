#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QComboBox>
#include <QTextEdit>
#include <QDial>
#include <QInputDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <QErrorMessage>
#include <QFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QTimer>

#include "qextserialport.h"
#include "qextserialenumerator.h"

enum ControlMode{AUTO, MANUAL};
enum MotorState{STOP, LEFT, RIGHT};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Funciones de control para el micro
    void getSystemState();
    void toggleManualAuto();
    void moveDomeRight();
    void moveDomeLeft();
    void stopDome();    

    ControlMode controlMode;
    MotorState motorState;
    int actualStep;
    int setpoint;

signals:
    void updateDisplay(QString);

private slots:
    void onDataAvailable();
    void onDeviceDiscovered();
    void onDeviceRemoved();
    void connectBtnClicked();
    void forcePosBtnClicked();
    void changeSetpoint(int val);
    void onTimerTimeout();

private:
    void loadPortConfiguration();
    void changeActualPos(int val);

    bool strAppending;
    QextSerialPort *serialPort;
    QString strReceived;
    QComboBox *portSelect;
    QStatusBar *statusBar;
    QErrorMessage *errorMessage;
};


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(MainWindow *mainW, QWidget *parent = 0);
    ~MainWidget();

public slots:
    void updateDisplay(QString val);

private slots:
    void dialChanged(int val);
    void modeBtnPressed();

private:
    MainWindow *mainWindow;
    QTextEdit *display;
};


#endif // MAINWINDOW_H
