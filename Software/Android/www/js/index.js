/**
  ******************************************************************************
  *  Name        : index.js
  *  Author      : Matias Micheletto
  *  Version     : 1.0
  *  Copyright   : GPLv3
  *  Description : Software para control de scanner lidar de cobertura vegetal
  ******************************************************************************
  *
  *  Copyright (c) 2018
  *  Matias Micheletto <matias.micheletto@uns.edu.ar>
  *  Departamento de Ingeniería Eléctrica - Universidad Nacional del Sur
  *  Laboratorio de Sistemas Digitales
  *
  *
  *  This program is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

var app = {
    connectedDeviceId: "", // Id del dispositivo conectado
    csvData: "", // Arreglo de datos recibidos
    listing: true, // Vble de estado para corregir el duplicado de callback onShow de M.Tabs
    scanning: false, // Vble de estado para bloquear algunas funciones durante el escaneo
    modals: "", // Vble para controlar apertura del modal

    init: function() { // Inicializar app
        // Iniciar pestañas
        M.Tabs.init(document.querySelector('.tabs'), {swipeable: false, onShow: function(current){                    
          // Al volver a la pestaña de dispositivos, actualizar lista si no esta conectado
          if(current.id == "devices_table" && app.listing && app.connectedDeviceId==""){
            app.listing = false;
            app.scan();
            setTimeout(function(){app.listing = true},500); // Rehabilitar callback despues de 500ms
          }
          // Al ir a la pestaña de controles, actualizar variables del sistema
          if(current.id == "controls" && app.connectedDeviceId != "" && !app.scanning){
            app.sendText("e"); // Velocidad de avance
            app.sendText("f"); // Periodo de muestreo
            app.sendText("g"); // Avance de carro por pasada
          }
          // Al pasar a la pestaña de registros, listar
          if(current.id == "logs_table" && app.listing){
            app.listing = false; // Deshabilitar el callback
            app.updateLogList();
            setTimeout(function(){app.listing = true},500); // Rehabilitar callback despues de 500ms
          }
        }});

        app.modals = M.Modal.init(document.querySelectorAll('.modal'), {}); // Inicializar modals
        
        app.scan(); // Buscar dispositivos BLE para conectarse  

        // Establecer callbacks para los controles
        document.getElementById("btnStart").addEventListener('click',function(){
          app.sendText("a"); // Esperar a la respuesta para confirmar que se recibio la orden
          setTimeout(function(){ // Espera de 2 segundos
            if(!app.scanning){ // Si luego de un tiempo no se recibe respuesta
              app.log("El dispositivo no responde.");
            }
          },2000);
        },false);
        
        document.getElementById("btnStop").addEventListener('click',function(){
          app.sendText("b");
          if(app.csvData != "") // Si se obtuvieron datos
            //app.saveResult(); // Guardar registro
            app.modals[0].open(); // Mostrar dialogo de confirmacion
          else
            app.log("Registro vacio!");
        },false);    
        
        document.getElementById("btnHome").addEventListener('click',function(){
          app.sendText("c");
        },false);

        document.getElementById("btnSaveResults").addEventListener('click',function(){
          app.saveResult();
        },false);

        document.getElementById("stepper_speed").addEventListener('change',function(){
          if(!app.scanning){ // No se debe configurar esta variable durante el escaneo
            var ocr1a = 16e6/1024/this.value; // Valor que va al registro de comp del arduino
            app.sendText("j"+ocr1a.toFixed(0));
            app.log("Frecuencia de motores = "+this.value+" Hz");
          }
        },false);

        document.getElementById("sample_period").addEventListener('change',function(){
          app.sendText("k"+this.value); 
          app.log("Periodo de muestreo = "+this.value+" ms");
        },false);

        document.getElementById("vert_sweep").addEventListener('change',function(){
          app.sendText("l"+this.value);
          app.log("Avance vertical = "+this.value+" steps");
        },false);

        document.getElementById("btnSendSP").addEventListener('click',function(){
          // Obtener posicion deseada de los inputs
          var spX = document.getElementById("inputSPX").value;
          var spY = document.getElementById("inputSPY").value;
          //app.log("Nuevo set point = "+spX+","+spY);
          if(!app.scanning && spX !== "" && spY !== "") // Si no esta escaneando y se ingresaron datos
            app.sendText("i"+spX+","+spY); // Enviar comando
        },false);

        document.getElementById("btnSendPos").addEventListener('click',function(){
          // Obtener posicion deseada de los inputs
          var posX = document.getElementById("inputPosX").value;
          var posY = document.getElementById("inputPosY").value;
          //app.log("Configuracion manual de pos = "+posX+","+posY);
          if(!app.scanning && posX !== "" && posY !== "") // Si no esta escaneando
            app.sendText("m"+posX+","+posY); // Enviar comando
        },false);

        // Ajustar valores de los slider de acuerdo a la configuracion del scanner
        // (La respuesta de estos comandos se procesa en el callback app.OnData())
        //app.sendText("e"); // Pedir velocidad de avance
        //app.sendText("f"); // Pedir periodo de muestreo
        //app.sendText("g"); // Pedir valor de avance de carro por pasada

        // Inicializacion lista
        app.log('Aplicacion lista!');
    },

    scan: function(){ // Escanear dispositivos BLE cercanos
        app.log("Escaneando dispositivos BLE");
        // Limpiar lista antes de hacer nueva busqueda
        var table = document.getElementById("bleDevices"); // Lista del DOM
        table.innerHTML = "<tr><th>Nombre</th><th>Dist. (m)</th><th>Acción</th></tr>";
        ble.scan([], 5, function(device){ // Buscar dispositivos durante 5 segundos y listar
            //app.log("Nuevo: "+JSON.stringify(device)); // Mostrar el nuevo dispositivo descubierto
            var deviceName = (device.name == null || device.name == "") ? "noname":device.name; // Nombre asignado
            // Calcular distancia del dispositivo a partir del RSSI                        
            var ratio = device.rssi/-65.0; // txPwr = -65 se calcula midiendo rssi con el dispositivo a 1m de distancia            
            var deviceDistance = (ratio < 1.0) ? Math.pow(ratio, 10) : (0.89976) * Math.pow(ratio, 7.7095) + 0.111;
            // var deviceDistance = Math.pow(10, (-65.0 - device.rssi)/15); // Otra formula mas compacta (no da buenos valores)
            var tr = document.createElement("tr"); // Fila para agregar a la tabla
            var tdName = document.createElement("td"); // Celda para nombre del dispositivo
            tdName.appendChild(document.createTextNode(deviceName)); // Poner nombre del dispositivo en la celda
            var btn = document.createElement("a"); // Boton para conectar con este dispositivo
            btn.innerHTML = "Conectar"; 
            btn.className = "waves-effect waves-light btn-large green";            
            btn.setAttribute("id",device.id); // Poner el id del boton como id del dispositivo
            btn.addEventListener('click', function(){
                app.toggleConnectionState(device);
            },false);
            var tdButton = document.createElement("td");
            tdButton.appendChild(btn);
            var tdRSSI = document.createElement("td");
            tdRSSI.appendChild(document.createTextNode(deviceDistance.toFixed(2)));
            tr.appendChild(tdName); // Agregar celda a la fila
            tr.appendChild(tdRSSI); // Agregar campo de rssi
            tr.appendChild(tdButton); // Agregar boton a la fila
            table.appendChild(tr); // Agregar item a la lista
        }, function(){ // Callback de operacion fallida
            //app.log("Escaneo fallido")
        });
    },

    toggleConnectionState: function(device){ // Alternar conexion/desconexion con dispositivos de la lista
      if( app.connectedDeviceId == ""){ // Si no hay dispositivos conectados, conectar con el indicado
          var btn = document.getElementById(device.id);
          btn.classList.remove("green");
          btn.classList.add("blue");
          btn.innerHTML = "Conectando...";            
          app.log("Conectando con "+device.name);
          ble.autoConnect(device.id, function(){ // Callback de operacion exitosa
              app.log("Conexión exitosa. Dispositivo: "+device.name);
              app.connectedDeviceId = device.id; // Asignar a variable global
              // Cambiar el estado del boton                
              btn.classList.remove("blue");
              btn.classList.add("red");
              btn.innerHTML = "Desconectar";            
              // Iniciar callback para escuchar mensajes
              ble.startNotification(device.id, "FFE0", "FFE1", function(buffer) {
                  var dataReceived = String.fromCharCode.apply(null, new Uint8Array(buffer));  // Convertir a string
                  app.onData(dataReceived); // Pasar el dato al callback de la app                    
              }, function(error){
                  app.log(error);
              });              
          }, function(){ // Callback de operacion fallida
              app.log("Intento de conexión fallido.");
              btn.classList.remove("blue");
              btn.classList.add("green");
              btn.innerHTML = "Conectar";        
          });
      }else{ // Si ya estaba conectado a un dispositivo, desconectar.
          app.log("Desconectando con dispositivo");
          ble.disconnect(app.connectedDeviceId, function(){
              app.log("No hay dispositivos conectados.");
              // Cambiar el estado del boton
              var btn = document.getElementById(app.connectedDeviceId);
              btn.classList.remove("red");
              btn.classList.add("green");
              btn.innerHTML = "Conectar";        
              app.connectedDeviceId = ""; // Borrar variable global
          }, function(){
              app.log("Intento de desconexión fallido.");
          });
      }
    },

    sendText: function(str){ // Enviar cadena de caracteres por bluetooth al dispositivo conectado
        if( app.connectedDeviceId != ""){ // Si hay dispositivo conectado
            var stringToSend = str+'\n'; // Agregar fin de linea
            var array = new Uint8Array(stringToSend.length); // Convertir el string a buffer array
            for (var i = 0; i < stringToSend.length; i++)
             array[i] = stringToSend.charCodeAt(i);
            ble.write(app.connectedDeviceId, "FFE0", "FFE1", array.buffer, function(){ // Callback de operacion exitosa
                //app.log("Dato enviado: "+stringToSend);
            }, function(){ // Callback de operacion fallida
                app.log("Falla al enviar dato.");
            });
        }
    },

    onData: function(str){ // Callback que se ejecuta al recibir datos
      // Extraer caracteres de comandos
      if(str.includes('a')){ // Ack inicio escaneo
        app.log('Escaneo iniciado.');
        app.scanning = true;
        return;
      }
      if(str.includes('b')){ // Ack escaneo detenido
        app.log('Escaneo detenido.'); 
        app.scanning = false;
        return;
      }
      if(str.includes('c')){ // Ack home
        app.log('Origen.'); 
        app.scanning = false;
        return;
      }
      
      // Respuesta de variables
      if(str.includes('d')){ // Limites del espacio de escaneo              
        app.log("Tope de recorrido = "+str.replace('d',''));
        return;
      }
      if(str.includes('e')){ // Velocidad de los steppers
        var v = parseInt(str.replace('e','')); // Convertir a valor numerico
        var freq = 16e6/1024/v;
        document.getElementById("stepper_speed").value = Math.round(freq);
        //app.log("Velocidad de avance = "+freq.toFixed(2)+" Hz.");
        return;
      }
      if(str.includes('f')){ // Periodo de muestreo del sensor        
        //app.log("Periodo de muestreo = "+str.replace('f','')+" ms.");
        document.getElementById("sample_period").value = Math.round(parseInt(str.replace('f','')));
        return;
      }
      if(str.includes('g')){ // Avance vertical del carro        
        //app.log("Avance de pasada = "+str.replace('g','')+" steps");
        document.getElementById("vert_sweep").value = Math.round(parseInt(str.replace('g','')));
        return;
      }
      if(str.includes('h')){ // Posicion del carro        
        app.log("Posición actual = "+str.replace('h',''));
        return;        
      }

      // Ack seteo de variables
      if(str.includes('i')){ // Agregar nuevo setpoint        
        app.log('Setpoint ingresado correctamente: '+document.getElementById("inputSPX").value+','+document.getElementById("inputSPY").value);
        document.getElementById("inputSPX").value = "";
        document.getElementById("inputSPY").value = ""; 
        return;
      }
      if(str.includes('j')){ // Cambio de velocidad de los steppers
        app.log('Velocidad de avance configurada: '+document.getElementById("stepper_speed").value+' Hz.'); 
        return;
      }
      if(str.includes('k')){ // Cambio de periodo de muestreo
        app.log('Periodo de muestreo modificado: '+document.getElementById("sample_period").value+' ms.'); 
        return;
      }
      if(str.includes('l')){ // Cambio de avance vertical por pasada
        app.log('Paso de avance vertical modificado: '+document.getElementById("vert_sweep").value+' steps.'); 
        return;
      }
      if(str.includes('m')){ // Forzar posicion actual
        app.log('Posicion actual modificada correctamente: '+document.getElementById("inputPosX").value+','+document.getElementById("inputPosY").value);
        document.getElementById("inputPosX").value = "";
        document.getElementById("inputPosY").value = "";
        return;
      }

      // En cualquier otro caso, guardar dato recibido
      app.csvData = app.csvData + str; // Adjuntar nuevo dato
      app.log(str);
    },

    saveResult: function(){ // Guardar el valor de csvData en un archivo del dispositivo
      try {
        window.requestFileSystem(LocalFileSystem.PERSISTENT, 0,function(fileSystem) {
          fileSystem.root.getDirectory('Scanner', { create: true, exclusive: false }, function(directoryEntry) {
            var d = new Date();
            var dateStr = d.getUTCFullYear()+"_"+d.getMonth()+"_"+d.getHours()+"_"+d.getMinutes()+"_"+d.getSeconds();
           directoryEntry.getFile('Sample_'+dateStr+'.csv', { create: true, exclusive: false }, function(fileEntry) {
             fileEntry.createWriter(function(fileWriter) {
              fileWriter.onwriteend = function() {                        
                app.log("Registro guardado "+fileEntry.fullPath);
                app.csvData = ""; // Limpiar registro luego de guardar
              };
              fileWriter.onerror = function (e) {
                navigator.notification.alert(e.toString(), function(){},"Error","Aceptar");                        
              };
              var dataObj = new Blob([app.csvData], { type: 'text/plain' });                   
              fileWriter.write(dataObj);
            }, function(){app.log("Error I/O");});
           }, function(){app.log("Error I/O");});
         }, function(){app.log("Error I/O");});
       }, function(){app.log("Error I/O");});
      }
      catch(e) {
        app.log(e);
      }
    },

    updateLogList: function(){ // Actualizar lista de registros presentes en el dispositivo
      try{
        window.requestFileSystem(LocalFileSystem.PERSISTENT, 0, function(fileSystem){
          fileSystem.root.getDirectory('Scanner', {create: true, exclusive: false}, function(directoryEntry){
            var directoryReader = directoryEntry.createReader();
            directoryReader.readEntries(function(entries){
              //app.log(JSON.stringify(entries[0]));
              /*{
                "isFile":true,
                "isDirectory":false,
                "name":"Sample_1527790693265.csv",
                "fullPath":"/Scanner/Sample_1527790693265.csv",
                "filesystem":"<FileSystem: persistent>",
                "nativeURL":"file:///data/user/0/com.matias.scannerapp/files/files/Scanner/Sample_1527790693265.csv"
              }*/
              var table = document.getElementById("logs"); // Lista del DOM
              table.innerHTML = "<tr><th>Nombre</th><th></th><th></th></tr>";
              for(var k = 0; k < entries.length; k++){                
                var tr = document.createElement("tr"); // Fila para agregar a la tabla
                var tdName = document.createElement("td"); // Celda para nombre del archivo
                tdName.appendChild(document.createTextNode(entries[k].name)); // Poner nombre del archivo en la celda
                var btnShare = document.createElement("a"); // Boton para compartir este archivo
                btnShare.innerHTML = "<i class='material-icons left'>share</i>"; 
                btnShare.className = "waves-effect waves-light btn-floating green";  
                btnShare.id = entries[k].nativeURL;                          
                btnShare.addEventListener('click', function(){
                    app.shareSelectedFile(this.id); // Al cliquear el boton, pasar el directorio de archivo para compartir
                },false);
                var btnDelete = document.createElement("a"); // Boton para borrar este archivo
                btnDelete.innerHTML = "<i class='material-icons left'>delete</i>"; 
                btnDelete.className = "waves-effect waves-light btn-floating red";                            
                btnDelete.id = entries[k].name;
                btnDelete.addEventListener('click', function(){
                    app.deleteSelectedFile(this.id); // Al cliquear el boton, pasar el directorio de archivo para borrar
                },false);
                var tdButtonShare = document.createElement("td");
                tdButtonShare.appendChild(btnShare);
                var tdButtonDelete = document.createElement("td");
                tdButtonDelete.appendChild(btnDelete);                
                tr.appendChild(tdName); // Agregar campo de nombre de archivo
                tr.appendChild(tdButtonShare); // Agregar boton de compartir a la fila
                tr.appendChild(tdButtonDelete); // Agregar boton de borrar a la fila
                table.appendChild(tr); // Agregar item a la lista
              }

            }, function(){app.log("Error I/O");});
          }, function(){app.log("Error I/O");});
        }, function(){app.log("Error I/O");});
      }
      catch(e) {
        app.log(e);
      }
    },

    shareSelectedFile: function(fileName){ // Abrir menu para compartir archivo que se pasa como argumento
      window.plugins.socialsharing.share("Registro de escaneo", "Registro", fileName);
    },

    deleteSelectedFile: function(fileName){ // Borrar el registro que se pasa como argumento
      try {
            window.requestFileSystem(LocalFileSystem.PERSISTENT, 0,function(fileSystem) {
                   fileSystem.root.getDirectory('Scanner', { create: true, exclusive: false }, function(directoryEntry) {
                      directoryEntry.getFile(fileName, { create: false, exclusive: false }, function(fileEntry) {
                        fileEntry.remove(app.updateLogList(), function(){app.log("Error I/O");});
                      }, function(){app.log("Error I/O");});
                   }, function(){app.log("Error I/O");});
               }, function(){app.log("Error I/O");});
       }
       catch(e) {
           app.log(e);
       }
    },

    log: function(msg){ // Mostrar mensajes por consola y en barra de estado de la app        
        console.log(document.getElementById('statusMsg').innerHTML = msg);
    }
};

document.addEventListener('deviceready', function(){
    app.init();
}, false);

