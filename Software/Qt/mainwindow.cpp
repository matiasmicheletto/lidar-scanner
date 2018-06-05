#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    serialPort = 0; // Puntero del objeto para controlar el puerto serie

    strAppending = false;
    strReceived.clear(); // String para almacenar caracteres recibidos por serie (com. asincrona)

    portSelect = new QComboBox; // Selector de puerto serie
    portSelect->setToolTip("Elija un puerto serie disponible en la lista desplegable");

    // Boton para conectar con el puerto seleccionado
    QPushButton *connectBtn = new QPushButton("Conectar");
    connectBtn->setToolTip("Conectar con el puerto seleccionado");
    connect(connectBtn,SIGNAL(clicked(bool)),this,SLOT(connectBtnClicked()));

    // Layout para los controles de seleccion de puerto
    QToolBar *toolBar = new QToolBar("Controles");
    toolBar->addWidget(portSelect);
    toolBar->addWidget(connectBtn);
    toolBar->addSeparator();

    // Barra de estado y ventana de mensajes de error
    statusBar = new QStatusBar;
    statusBar->showMessage("Seleccione un puerto disponible y presione \"Conectar\"");

    // Ventana de errores
    errorMessage = new QErrorMessage;

    // Widget central
    MainWidget *mainWidget = new MainWidget(this,this);
    connect(this, SIGNAL(updateDisplay(QString)), mainWidget, SLOT(updateDisplay(QString)));


    /// PUERTOS SERIE ///
    // Explorar puertos serie conectados al equipo
    QextSerialEnumerator *serialEnumerator = new QextSerialEnumerator;
    QList<QextPortInfo> ports = serialEnumerator->getPorts();
    foreach(QextPortInfo portInfo, ports) // Para cada puerto encontrado
        if(!portInfo.portName.isEmpty()) // Si el nombre no es nulo, agregar item a la lista de puertos
            portSelect->addItem(portInfo.portName + "-" + portInfo.friendName, portInfo.portName);
    // Habilitar notificaciones de conexion/desconexion de dispositivos
    serialEnumerator->setUpNotifications();
    connect(serialEnumerator, SIGNAL(deviceDiscovered(QextPortInfo)), this, SLOT(onDeviceDiscovered()));
    connect(serialEnumerator, SIGNAL(deviceRemoved(QextPortInfo)), this, SLOT(onDeviceRemoved()));

    /// LAYOUT ///
    this->addToolBar(toolBar);
    this->setStatusBar(statusBar);
    this->setCentralWidget(mainWidget);

    setWindowTitle("QtDomeControl");
}

MainWindow::~MainWindow()
{
    // Borrar punteros atributos
    if(serialPort != 0) delete serialPort;
    delete portSelect;
    delete statusBar;
    delete errorMessage;
}


void MainWindow::startScanning()
// Iniciar el escaneo del suelo
{
    if(serialPort != 0){
        serialPort->write("a\n");
        statusBar->showMessage("Escaneando...");
    }
}

void MainWindow::stopScanning()
// Detener el escaneo
{
    if(serialPort != 0){
        serialPort->write("b\n");
        statusBar->showMessage("Detenido");
    }
}

void MainWindow::goHome()
// Volver al inicio
{
    if(serialPort != 0){
        serialPort->write("c\n");
        statusBar->showMessage("Detenido");
    }
}


void MainWindow::loadPortConfiguration()
// Cargar configuracion del puerto si existe
{
    // Arreglos de configuracion del puerto
    #if defined(Q_OS_LINUX)
        BaudRateType baudRates[10] = {BAUD4800, BAUD9600, BAUD19200, BAUD38400,
                                      BAUD57600, BAUD115200};
        StopBitsType stopBits[3] = {STOP_1, STOP_2};
        ParityType parityTypes[5] = {PAR_NONE, PAR_EVEN, PAR_ODD, PAR_SPACE};
    #elif defined(Q_OS_WIN)
        BaudRateType baudRates[10] = {BAUD4800, BAUD9600, BAUD14400, BAUD19200, BAUD38400,
                                      BAUD56000, BAUD57600, BAUD115200, BAUD128000, BAUD256000};
        StopBitsType stopBits[3] = {STOP_1, STOP_1_5, STOP_2};
        ParityType parityTypes[5] = {PAR_NONE, PAR_EVEN, PAR_ODD, PAR_MARK, PAR_SPACE};
    #endif

    DataBitsType dataBits[4] = {DATA_5, DATA_6, DATA_7, DATA_8};
    FlowType flowTypes[3] = {FLOW_OFF, FLOW_XONXOFF, FLOW_HARDWARE};

    // Leer archivo y cargar parametros
    QFile *file = new QFile("SerialPort.conf");
    file->open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream textStream(file);
    QString actualLine; // QString temporal para lectura de lineas
    int cnt = 0; // Variable temporal que cuenta las lineas leidas
    bool ok = true; // Conversion exitosa de string a int
    uint temp; // Indice para los arreglos de configuracion
    while (!textStream.atEnd()) // Recorrer el archivo
    {
        actualLine = textStream.readLine(); // Linea actual del archivo
        // Excluir comentarios y lineas vacias
        if( actualLine != "" && !actualLine.startsWith("#"))
        {
            cnt++;
            switch (cnt) {
            case 1: // Linea 1 -> Baudrate
                temp = actualLine.toInt(&ok);
                if(ok && temp < 10) serialPort->setBaudRate(baudRates[temp]);
                else serialPort->setBaudRate( BAUD9600 );
                break;
            case 2: // Linea 2 -> Databits
                temp = actualLine.toInt(&ok);
                if(ok && temp < 4) serialPort->setDataBits(dataBits[temp]);
                else serialPort->setDataBits(DATA_8);
                break;
            case 3: // Linea 3 -> Stopbits
                temp = actualLine.toInt(&ok);
                if(ok && temp < 3) serialPort->setStopBits(stopBits[temp]);
                else serialPort->setStopBits(STOP_1);
                break;
            case 4: // Linea 4 -> Flow control
                temp = actualLine.toInt(&ok);
                if(ok && temp < 3) serialPort->setFlowControl(flowTypes[temp]);
                else serialPort->setFlowControl(FLOW_OFF);
                break;
            case 5: // Linea 5 -> Parity
                temp = actualLine.toInt(&ok);
                if(ok && temp < 5) serialPort->setParity(parityTypes[temp]);
                else serialPort->setParity(PAR_NONE);
                break;
            default:
                qWarning() << "Hay lineas adicionales en el archivo de configuracion." ;
                qDebug() << "Linea :" << cnt << "Pos. :" << textStream.pos();
                break;
            }
        }
    }

    file->close();
    delete file; // Borrar el objeto, (no el archivo)
    qDebug() << "Configuracion cargada.";
    qDebug() << "Baud =" << serialPort->baudRate();
    qDebug() << "Data =" << serialPort->dataBits();
    qDebug() << "Stop =" << serialPort->stopBits();
    qDebug() << "Flow =" << serialPort->flowControl();
    qDebug() << "Parity =" << serialPort->parity();
}




void MainWindow::onDataAvailable()
// SLOT: Bytes recibidos
{
    char c; // Temporal para almacenar caracteres recibidos por serie
    int bytesAvailable = serialPort->bytesAvailable(); // Bytes disponibles para leer
    for(int i = 0; i < bytesAvailable; i++){ // Para cada byte disponible
        c = serialPort->read(1).at(0); // Leer un byte
        if( c == '\n'){ // Fin de linea
            emit updateDisplay(strReceived);
            // Guardar dato recibido en archivo de salida
            strAppending = false; // Marcar fin de cadena de caracteres
            saveDataReceived();
            strReceived.clear(); // Borrar string
        }else // Si no es fin de linea
            strReceived.append(c); // Agregar caracteres a una cadena
    }
}

void MainWindow::saveDataReceived()
// Guardar dato recibido
{
    // Guardar en registro el dato recibido
    QString registerFileName = QDir::currentPath()+"/RegScanner.csv";
    QFile *file = new QFile(registerFileName);
    file->open(QIODevice::Append | QIODevice::Text);
    QTextStream textStream(file);
    // Guardar dato
    textStream << strReceived; // Dato con estampa de tiempo
    file->close(); // Cerrar archivo del disco
    delete(file); // Eliminar puntero
}



void MainWindow::onDeviceDiscovered()
// SLOT: Al aparecer nuevos dispositivos, actualizar lista
{
    portSelect->clear();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach(QextPortInfo portInfo, ports) // Para cada puerto encontrado
        if(!portInfo.portName.isEmpty()) // Si el nombre no es nulo, agregar item a la lista
            portSelect->addItem(portInfo.portName + "-" + portInfo.friendName, portInfo.portName);
}

void MainWindow::onDeviceRemoved()
// SLOT: Al quitar dispositivos
{
    if(!serialPort->isOpen()){ // Si el puerto serie se cerro, eliminar objeto
        delete serialPort;
        serialPort = 0;
        statusBar->showMessage("El puerto serie fue desconectado.");
    }
}

void MainWindow::connectBtnClicked()
// SLOT: Conectar con el dispositivo mediante puerto serie
{
    // Crear nuevo puerto
    if(serialPort != 0) delete serialPort;
    QString portName = portSelect->currentData().toString();
    if( portName != "" ){
        // Crear objeto puerto serie
        serialPort = new QextSerialPort( portName );

        // Leer configuracion del archivo
        loadPortConfiguration();

        // Intentar conectar con el dispositivo
        statusBar->showMessage("Conectando con puerto serie...");
        if(serialPort->open(QIODevice::ReadWrite)){
            // Iniciar timer para solicitar estado del sistema cada 1 segundo
            QTimer *timer = new QTimer;
            timer->setInterval(1000);
            connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
            qDebug() << "Puerto:" << portSelect->currentData().toString();
            qDebug() << "Baud rate:" << serialPort->baudRate();
            connect(serialPort, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
            statusBar->showMessage("Conectado");
        }else
            statusBar->showMessage("El puerto serie no se pudo abrir.");
    }
    else
        statusBar->showMessage("Debe indicar puerto!");
}


void MainWindow::onTimerTimeout()
// SLOT: Disparo del timer
{

}




/// MAINWIDGET ///

MainWidget::MainWidget(MainWindow *mainW, QWidget *parent) : QWidget(parent)
{
    mainWindow = mainW; // Ventana principal

    /// CONTROLES ///
    // Estilo de los controles
    this->setStyleSheet("QPushButton#startBtn{background:#12BA1A;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton#startBtn:pressed{background:#0A700F;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton#stopBtn{background:#FF0000;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton#stopBtn:pressed{background:#610B0B;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton#homeBtn{background:#013ADF;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton#homeBtn:pressed{background:#0A122A;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QDial{background-color:red}"
                        "QTextEdit{background:black;"
                            "color:white}");

    // Selección de tipo de modo
    QPushButton *startBtn = new QPushButton;
    startBtn->setObjectName("startBtn");
    startBtn->setFixedSize(30,30);
    startBtn->setToolTip("Iniciar escaneo");
    connect(startBtn, SIGNAL(clicked(bool)), this, SLOT(greenBtnPressed()));

    QPushButton *stopBtn = new QPushButton;
    stopBtn->setObjectName("stopBtn");
    stopBtn->setFixedSize(30,30);
    stopBtn->setToolTip("Detener escaneo");
    connect(stopBtn, SIGNAL(clicked(bool)), this, SLOT(redBtnPressed()));

    QPushButton *homeBtn = new QPushButton;
    homeBtn->setObjectName("homeBtn");
    homeBtn->setFixedSize(30,30);
    homeBtn->setToolTip("Volver al inicio");
    connect(homeBtn, SIGNAL(clicked(bool)), this, SLOT(blueBtnPressed()));


    // Texto informativo
    display = new QTextEdit;
    display->setFixedSize(300,130);
    display->setFont(QFont("Courier",18));
    display->setTextInteractionFlags(Qt::NoTextInteraction);

    /// LAYOUT ///
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(startBtn);
    vBoxLayout->addWidget(stopBtn);
    vBoxLayout->addWidget(homeBtn);
    vBoxLayout->setAlignment(startBtn, Qt::AlignHCenter);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(vBoxLayout);
    layout->addWidget(display);

    this->setLayout(layout);
}


MainWidget::~MainWidget()
// Destructor de la clase
{
    delete display;
}

void MainWidget::updateDisplay(QString val)
{
    display->clear();
    QStringList lst = val.split(",");
    display->append("X = "+lst.at(0));
    display->append("Y = "+lst.at(1));
    display->append("Z = "+lst.at(2));
}

void MainWidget::greenBtnPressed()
// Al presionar el boton verde
{
    mainWindow->startScanning();
}

void MainWidget::redBtnPressed()
{
    mainWindow->stopScanning();
}

void MainWidget::blueBtnPressed()
{
    mainWindow->goHome();
    display->clear();
    display->append("Listo");
}

