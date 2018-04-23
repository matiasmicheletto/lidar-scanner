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

    // Selector de setpoint
    QLabel *spLabel = new QLabel("Setpoint:");
    QSpinBox *spSpinBox = new QSpinBox;
    spSpinBox->setRange(0,359);
    spSpinBox->setToolTip("Setpoint");
    spSpinBox->setMaximumWidth(70);
    connect(spSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeSetpoint(int)));

    // Boton para corregir la posicion de la cupula
    QPushButton *forcePosBtn = new QPushButton("Posición...");
    forcePosBtn->setToolTip("Corregir posición");
    connect(forcePosBtn,SIGNAL(clicked(bool)), this, SLOT(forcePosBtnClicked()));

    // Layout para los controles de seleccion de puerto
    QToolBar *toolBar = new QToolBar("Controles");
    toolBar->addWidget(portSelect);
    toolBar->addWidget(connectBtn);
    toolBar->addSeparator();
    toolBar->addWidget(spLabel);
    toolBar->addWidget(spSpinBox);
    toolBar->addSeparator();
    toolBar->addWidget(forcePosBtn);

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


void MainWindow::getSystemState()
// Solicitar variables del sistema
{
    if(serialPort != 0)
        serialPort->write("a\n");
}

void MainWindow::toggleManualAuto()
// Alternar modo de control manual o automatico 
{
    if(serialPort != 0)
        serialPort->write("b\n");
}

void MainWindow::moveDomeRight()
// Activar motor de la cupula
{
    if(serialPort != 0)
        serialPort->write("c\n");
}

void MainWindow::moveDomeLeft()
// Activar motor de la cupula
{
    if(serialPort != 0)
        serialPort->write("d\n");
}

void MainWindow::stopDome()
// Apagar motor
{
    if(serialPort != 0)
        serialPort->write("e\n");
}

void MainWindow::changeSetpoint(int val)
// Actualizar el setpoint
{
    if(serialPort != 0){
        if(val < 0) val = 0;
        else if( val > 360 ) val = 359;
        QString strToSend = "f"+QString().setNum(val)+"\n";
        serialPort->write(strToSend.toStdString().c_str());
        statusBar->showMessage("Cambiando setpoint: "+QString().setNum(val)+"...");
    }
}

void MainWindow::changeActualPos(int val)
// Forzar valor de la posicion actual
{
    if(serialPort != 0){
        if(val < 0) val = 0;
        else if( val > 360 ) val = 359;
        QString strToSend = "g"+QString().setNum(val)+"\n";
        serialPort->write(strToSend.toStdString().c_str());
        statusBar->showMessage("Cambiando posicion: "+QString().setNum(val)+"...");
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
            if( strAppending ){ // Si se estaba recibiendo una cadena de caracteres
                emit updateDisplay(strReceived);
                strReceived.clear(); // Borrar string
                strAppending = false; // Marcar fin de cadena de caracteres
            }
        }else{ // Si no es fin de linea
            if( strAppending ) // Si se esta recibiendo variables del sistema de control
                strReceived.append(c); // Agregar caracteres a una cadena
            else
                switch(c){
                case 'a': // Ack, variables del sistema de control
                    strAppending = true; // Marcar para concatenar los siguientes caracteres
                    break;
                case 'b': // Ack cambio de modo automatico/manual
                    controlMode = controlMode == AUTO ? MANUAL : AUTO; // Alternar modo
                    if(controlMode == AUTO)
                        statusBar->showMessage("Cambio a modo automático.");
                    else
                        statusBar->showMessage("Cambio a modo manual.");
                    break;
                case 'c': // Ack mover hacia la derecha
                    statusBar->showMessage("Moviendo cúpula hacia la derecha.");
                    break;
                case 'd': // Ack mover hacia la izquierda
                    statusBar->showMessage("Moviendo cúpula hacia la izquierda.");
                    break;
                case 'e': // Ack detener motor
                    statusBar->showMessage("Deteniendo motor.");
                    break;
                case 'f': // Ack cambio de setpoint
                    statusBar->showMessage("Nuevo setpoint ingresado.");
                    break;
                case 'g':
                    statusBar->showMessage("Posicion actualizada.");
                    break;
                case '#': // Error en el comando
                    break;
                }
        }
    }
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


void MainWindow::forcePosBtnClicked()
// SLOT: Boton para corregir la posicion de la cupula
{
    // Mostrar dialogo para ingresar el valor de la posicion deseada
    bool ok;
    int val = QInputDialog::getInt(this, tr("Corregir posición"),
                                    tr("Posición actual:"), 0, 0, 359, 1, &ok);
    if (ok && val >= 0 && val < 360)
       changeActualPos(val);
    else
       statusBar->showMessage("No se puede corregir la posición");
}

void MainWindow::onTimerTimeout()
// SLOT: Disparo del timer
{
    getSystemState();
}


/// MAINWIDGET ///

MainWidget::MainWidget(MainWindow *mainW, QWidget *parent) : QWidget(parent)
{
    mainWindow = mainW; // Ventana principal

    /// CONTROLES ///
    // Estilo de los controles
    this->setStyleSheet("QPushButton{background:#12BA1A;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QPushButton:pressed{background:#0A700F;"
                            "border-radius:15px;"
                            "border: 3px solid grey}"
                        "QDial{background-color:red}"
                        "QTextEdit{background:#68D651;"
                            "color:black}");

    // Selección de tipo de modo
    QPushButton *modeBtn = new QPushButton;
    modeBtn->setFixedSize(30,30);
    modeBtn->setToolTip("Alternar modo automático/manual");
    connect(modeBtn, SIGNAL(clicked(bool)), this, SLOT(modeBtnPressed()));

    // Switch de control manual
    QDial *dial = new QDial;
    dial->setSingleStep(1);
    dial->setRange(-1,1);
    dial->setFixedSize(50,50);
    dial->setNotchesVisible(true);
    dial->setToolTip("Control manual del motor");
    connect(dial, SIGNAL(valueChanged(int)), this, SLOT(dialChanged(int)));

    // Texto informativo
    display = new QTextEdit;
    display->setFixedSize(300,130);
    display->setFont(QFont("Courier",18));
    display->setTextInteractionFlags(Qt::NoTextInteraction);

    /// LAYOUT ///
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(modeBtn);
    vBoxLayout->addWidget(dial);
    vBoxLayout->setAlignment(modeBtn, Qt::AlignHCenter);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(vBoxLayout);
    layout->addWidget(display);

    this->setLayout(layout);
    updateDisplay("t 85 110 r"); // Quitar luego
}


MainWidget::~MainWidget()
// Destructor de la clase
{
    delete display;
}

void MainWidget::updateDisplay(QString val)
// SLOT: Actualizar variables de estado y texto informativo.
// Formato argumento: modo pos sp motor. Ejemplo: t 120 120 s
// modo: t->AUTO, m->MANUAL
// pos: int(0..359)
// sp: int(0..359)
// motor: s->apagado, r->derecha, l->izquierda
{
    QStringList args = val.split(" "); // Separar argumentos
    QStringList lines; // Lineas del display

    // Modo de control
    if(args.at(0) == "t"){
        mainWindow->controlMode = AUTO;
        lines.append("Modo: Automático");
    }else{
        mainWindow->controlMode = MANUAL;
        lines.append("Modo: Manual");
    }
    mainWindow->actualStep = args.at(1).toInt(); // Posicion actual
    lines.append("Posición: "+args.at(1));
    mainWindow->setpoint = args.at(2).toInt(); // Setpoint
    lines.append("Setpoint: "+args.at(2));
    // Motor
    if(args.at(3) == "s"){
        mainWindow->motorState = STOP;
        lines.append("Detenido");
    }else{
        if(args.at(3) == "r"){
            mainWindow->motorState = RIGHT;
            lines.append("Rot. derecha");
        }else{
            mainWindow->motorState = LEFT;
            lines.append("Rot. izquierda");
        }
    }

    display->clear();
    for(int k = 0; k < 4; k++)
        display->append(lines.at(k));
}

void MainWidget::dialChanged(int val)
// SLOT: Cambio de la posicion de la llave de control del motor
{
    if(val == -1)
        mainWindow->moveDomeLeft();
    else if(val == 1)
        mainWindow->moveDomeRight();
    else
        mainWindow->stopDome();
}

void MainWidget::modeBtnPressed()
// SLOT: Boton de cambio de modo de control manual/automatico
{
    mainWindow->toggleManualAuto();
}


