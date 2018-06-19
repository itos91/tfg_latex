#include "Menu.h"
#include "ModbusTCP.h"
#include "Consola.h"
#include <Arduino.h>

//Variables para la clase ModbusTCP
char ssid[] = "HUAWEI-Cw4a";
char pass[] = "9RR3VAxS";
//char ssid[] = "Quesada";
//char pass[] = "987ingenieros";
//char ssid[] = "LRA";
//char pass[] = "android2016";
//Datos para la conexión a la red
int puerto = 5333; //Puerto para la conexión al servidor
//byte ip[]={192,168,1,116};
byte ip[]={192,168,100,3};
//int puerto = 502; //Puerto para la conexión al servidor
//byte ip[]={192,168,0,203};
//byte que contiene la direccion ip del servidor a conectarnos

//Variable que reconoce el menu que está 1=Menú principal, 2=Regulador PID, 3=Ciclo de regulación
String listaMenuPrincipal[] = {"Parametros PI", "Activar/Desactivar PI" , "Introducir consigna"};
String listaMenuPI[] = {"Introducir parametros", "Visualizar parametros"};
char tituloMenuPrincipal[] = "- Menu Principal -";
char tituloMenuPI[] = "- PI -";

ModbusTCPClass Mb(ssid,pass,puerto,ip);
//Constructor de la clase ModbusTCP de parametros ssid, password, puerto e ip

//Variables de registros para mandar al autómata
float consigna;

//Variables para las clase Teclado4x4 y Pantalla Midas
char caracter;

//Variable bool que representa si se activó ciclo de regulación o no true=activado, false=desactivado
bool activacion;

//Serial.println("caca");

Consola consola(32, 33, 34, 35, 36, 37, 38, 22, 23, 24, 25, 26, 27, 28, 29, "789s456-123d.0ei");

Menu menuPrincipal (tituloMenuPrincipal, listaMenuPrincipal, 3, true);
Menu menuPI (tituloMenuPI, listaMenuPI, 2, false);


// Construye un objeto para manejar una pantalla utilizando las señales Arduino 32 a 38
// y para manejar el teclado usando las señales del 22 a 29 y la cadena que representa el teclado.
void setup() {
  // put your setup code here, to run once:
  consola.configura();
  // Configura las señales Arduino y envía la secuencia de órdenes de inicialización
  // a la pantalla y al teclado

  consola.visualizaCadena(1,1,Mb.configura());
  //Nos devuelve un string si se conectó a la red especificada
  consola.visualizaCadena(2,1,Mb.connectServer());
  //Nos devuelve un String si se conectó al servidor

  consola.visualizaCadena(4,1,"Pulsa ENT para continuar");

  activacion = Mb.leeBool(2);

  while(consola.introduceCaracter() != 'e');

  menuPrincipal.activarMenu();
}

/*#################### Funciones #######################*/

void introducirDatosPI() {
  float dato;

  consola.borraPantalla();
  consola.visualizaCadena(1,1,"Introduce Kp: ");
  dato = consola.introduceReal(1,14,6);
  Mb.escribeReal(10,dato);

  consola.visualizaCadena(2,1,"Introduce Ti: ");
  dato = consola.introduceReal(2,14,6);
  Mb.escribeReal(12,dato);

  /*consola.visualizaCadena(3,1,"Introduce Actuacion Minima: ");
  dato = consola.introduceReal(3,22,6);
  Mb.escribeReal(14,dato);

  consola.visualizaCadena(4,1,"Introduce Actuacion Maxima: ");
  dato = consola.introduceReal(4,22,6);
  Mb.escribeReal(16,dato);*/
}

void visualizaDatosPI() {
  consola.borraPantalla();

  consola.visualizaCadena(1,1,"Kp:");
  //consola.visualizaEntero(1,14,holdingRegisters[0]);
  consola.visualizaReal(1,4,Mb.leeReal(10),2,2);

  consola.visualizaCadena(2,1,"Ti:");
  consola.visualizaReal(2,4,Mb.leeReal(12),2,2);

  consola.visualizaCadena(3,1,"Salida Minima:");
  consola.visualizaReal(3,16,Mb.leeReal(14),4,2);

  consola.visualizaCadena(4,1,"Salida Maxima:");
  consola.visualizaReal(4,16,Mb.leeReal(16),4,2);

  consola.visualizaCadena(1,35,"<--");

  while(consola.introduceCaracter() != 'i');
}

void activarCicloReg() {
  //introduceConsigna();

  activacion = true;
  Mb.escribeBool(2,1);
}

void visualizarDatosAutomata() {
  consola.visualizaCadena(1,30,"DATOS");

  consola.visualizaCadena(2,26,"Consigna: ");
  consola.visualizaReal(2,36,Mb.leeReal(3),4,2);

  consola.visualizaCadena(3,26,"Salida: ");
  consola.visualizaReal(3,36,Mb.leeReal(24),4,2);

  consola.visualizaCadena(4,26,"Actuacion: ");
  consola.visualizaReal(4,36,Mb.leeReal(18),4,2);
}

void introduceConsigna() {
  float consigna;

  consola.borraPantalla();

  consola.visualizaCadena(1,1,"Introduce temperatura: ");
  consigna = consola.introduceReal(1,23,6);

  Mb.escribeReal(3,consigna);
}

/* ###################################################### */



void loop() {
  // put your main code here, to run repeatedly:


  if (menuPrincipal.getActivo()) {
    menuPrincipal.visualiza(consola);
    visualizarDatosAutomata();
    if (menuPrincipal.compruebaTecla(consola)) { //mira si se pulsó la tecla ENT o -->
      switch(menuPrincipal.getSel()) { //nos devuelve la posicion del menu
        case 0:
          //Parametros PI
          menuPI.activarMenu();           //Activamos el menu del regulador PI
          menuPrincipal.desactivarMenu(); //Desactivamos el menu principal
          break;
        case 1:
          //Activar/Desactivar PI
          activarCicloReg();          //Activamos regulador PI
          break;
        case 2:
          //Introducir Consigna
          introduceConsigna();
          break;
        }
      }
    }

    if (menuPI.getActivo()) {
      menuPI.visualiza(consola);
      visualizarDatosAutomata();
      if (menuPI.compruebaTecla(consola)) { //mira si se pulsó la tecla ENT o -->
        switch(menuPI.getSel()) { //nos devuelve la posicion del menu
          case 0:
            //Introducir parámetros PI
            introducirDatosPI();
            break;
          case 1:
            //Visualizar parametros PI
            visualizaDatosPI();
            break;
          case -1:
            //Si se pulsó tecla <--
            menuPI.desactivarMenu();           //Desactivamos el menu del regulador PI
            menuPrincipal.activarMenu();      //Activamos el menu principal
            break;
        }
      }
    }
}
