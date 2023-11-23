#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

using namespace std;

ifstream entrada("Dispositivos.dat");
fstream resultados("resultados.out", ios::in| ios:: out);
fstream DispositivosRespaldo("Dispositivos_resp.dat", ios::in| ios:: out);
fstream RutasRespaldo("rutas_resp.dat", ios::in| ios:: out);

int RutasEliminadas = 0;

class Dispositivo;

struct Relaciones{
    Dispositivo *router = NULL;
    int ping;
    string conexion;
};
class Dispositivo{
    public:
    string hostname;
    string IP;
    int indice;
    Relaciones Relacion[12]; 

    Dispositivo(){
        indice = 0;
    }
    ~Dispositivo(){}
};
class Pila{
    public: 
        int punteroDePila;
        Dispositivo *Apilados[1000];

        Pila(){
            punteroDePila = 0;
        }

        void insertar(Dispositivo *Router){
            Apilados[punteroDePila] = Router;
            punteroDePila++;
        }

        Dispositivo *extraer(){
            Apilados[punteroDePila] = NULL;
            punteroDePila--;
            return Apilados[punteroDePila];
        }

        bool EstaEnPila(Dispositivo *Comparacion){
            for(int i = 0; i < punteroDePila; i++){
                if(Comparacion == Apilados[i]){
                    return true;
                }
            }
            return false;
        }

        void ImprimirPila(){
            for(int i = 0; i < punteroDePila; i++){
                resultados << Apilados[i]->hostname << " ";
                cout << (*Apilados[i]).hostname << " ";
            }
        }
        void VaciarPila(){
            for(int i = 0; i < punteroDePila; i++){
                Apilados[i] = NULL;
            }
            punteroDePila = 0;
        }
        bool EstaVacia(){
            if(punteroDePila == 0){
                return true;
            }
            return false;
        }
        Dispositivo *Transferencia(int indice){
            return Apilados[indice];
        }

        Dispositivo *extraerLectura(int punteroExtraccion){
            return Apilados[punteroExtraccion];
        }

        Dispositivo *extraerCola(int indice){
            return Apilados[indice];
        }

        int ImprimirPing(){
            int aux = 0;
            for(int i = 0; i < punteroDePila; i++){
                for(int j = 0; j < 12; j++){
                    if(Apilados[i]->Relacion[j].router == Apilados[i+1]){
                        aux += Apilados[i]->Relacion[j].ping;
                    }
                }
            }
            return aux;
        }

    ~Pila(){}
};

void Backtracking(Dispositivo *Raiz, Dispositivo *Destino, int indice, Pila &Atravesados, bool &Solucion, string tipoC){
    Relaciones RelacionActual[indice];

    for(int i = 0; i < indice; i++){
        RelacionActual[i] = Raiz->Relacion[i]; 
    }

    for(int i = 0; i < indice; i++){
        if(!Atravesados.EstaEnPila(RelacionActual[i].router)){
            if(RelacionActual[i].router->hostname == Destino->hostname){
                Atravesados.insertar(Destino);
                cout << "Solucion encontrada: " << endl;
                Atravesados.ImprimirPila();
                resultados << " Ping: " << Atravesados.ImprimirPing() << " Saltos: " << Atravesados.punteroDePila << " (Relacion, " << tipoC << ")" << endl;
                cout << " Ping: " << Atravesados.ImprimirPing() << " Saltos: " << Atravesados.punteroDePila << " (Relacion, " << tipoC << ")" << endl;
                Solucion = true;
                Atravesados.extraer();
            }
            else{
                Atravesados.insertar(RelacionActual[i].router);
                Backtracking(RelacionActual[i].router, Destino, RelacionActual[i].router->indice, Atravesados, Solucion, tipoC);
            }
        }
    }
    Atravesados.extraer();
}
void BacktrackingEliminar(Dispositivo *Raiz, Dispositivo *Destino, int indice, Pila &Atravesados, Pila &Soluciones, bool &Solucion){
    Relaciones RelacionActual[indice];

    for(int i = 0; i < indice; i++){
        RelacionActual[i] = Raiz->Relacion[i]; 
    }

    for(int i = 0; i < indice; i++){
        if(!Atravesados.EstaEnPila(RelacionActual[i].router)){
            if(RelacionActual[i].router->hostname == Destino->hostname){
                Solucion = true;
                for(int i = 0; i < Atravesados.punteroDePila; i++){
                    Soluciones.insertar(Atravesados.Transferencia(i));
                }
                Soluciones.insertar(Destino);
            }
            else{
                Atravesados.insertar(RelacionActual[i].router);
                BacktrackingEliminar(RelacionActual[i].router, Destino, RelacionActual[i].router->indice, Atravesados, Soluciones, Solucion);
            }
        }
    }
    Atravesados.extraer();
}

void intercambiarDispositivos(Dispositivo Dispositivos[], int i, int j){
    Dispositivo aux;
    aux = Dispositivos[i];
    Dispositivos[i] = Dispositivos[j];
    Dispositivos[j] = aux; 
}

void OrdenamientoDispositivosNombre(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    for(int o = 0, t = cantidadDeDispositivos - 1; o < cantidadDeDispositivos; o++, t--){
        for(int i = 0; i < t; i++){
            if(Dispositivos[i].hostname > Dispositivos[i+1].hostname){
                intercambiarDispositivos(Dispositivos, i, i+1);
            }
        }
    }
}
void IntercambiarRelaciones(Relaciones Relacion[], int i, int j){
    Relaciones aux;
    aux = Relacion[i];
    Relacion[i] = Relacion[j];
    Relacion[j] = aux; 
}
void OrdenamientoRelaciones(Relaciones Relacion[]){
    for(int o = 0, t = 12 - 1; o < 12; o++, t--){
        for(int i = 0; i < t; i++){
            if(Relacion[i].router == NULL && Relacion[i + 1].router != NULL){
                IntercambiarRelaciones(Relacion, i, i+1);
            }
        }
    }
}
void AgregarDispositivo(int &PunteroDeDispositivos, Dispositivo Dispositivos[], int &cantidadDeDispositivos){
    string hostnameAgg;
    string IPAgg;

    cout << "Introduzca el nombre del dispositivo a agregar" << endl;
    cin >> hostnameAgg;
    cout << "Introduzca la direccion IP del dispositivo a agregar" << endl;
    cin >> IPAgg;

    if(hostnameAgg.at(hostnameAgg.length() - 1) == ','){
        hostnameAgg.pop_back(); 
    }
    if(IPAgg.at(IPAgg.length() - 1) == ','){
        IPAgg.pop_back(); 
    }

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivos[i].hostname == hostnameAgg || Dispositivos[i].IP == IPAgg){
            resultados << "0" << endl;
            cout << "0: El dispositivo ya existe en el sistema" << endl;
            return;
        }
    }
    Dispositivos[PunteroDeDispositivos].hostname = hostnameAgg;
    Dispositivos[PunteroDeDispositivos].IP = IPAgg;
    PunteroDeDispositivos++;
    cantidadDeDispositivos++;
    resultados << "1" << endl;
    cout << "1: El dispositivo " << hostnameAgg << " ha sido agregado." << endl;
}

void AgregarRelacionDesdeEntrada(Dispositivo Dispositivos[], int cantidadDeDispositivos, ifstream &entrada){
    string hostnameR1, hostnameR2, pingEntrada, tipoC;
    int ping = 0;
    Dispositivo *Dispositivo1, *Dispositivo2;

    entrada >> hostnameR1 >> hostnameR2 >> pingEntrada >> tipoC;
    
    if(hostnameR1.at(hostnameR1.length() - 1) == ','){
        hostnameR1.pop_back(); 
    }
    if(hostnameR2.at(hostnameR2.length() - 1) == ','){
        hostnameR2.pop_back(); 
    }
    if(pingEntrada.at(pingEntrada.length() - 1) == ','){
        pingEntrada.pop_back(); 
    }

    if(tipoC.at(tipoC.length() - 1) == ','){
        tipoC.pop_back(); 
    }

    ping = stoi(pingEntrada);

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(hostnameR1 == Dispositivos[i].hostname || hostnameR1 == Dispositivos[i].IP){Dispositivo1 = &Dispositivos[i];}

        if(hostnameR2 == Dispositivos[i].hostname || hostnameR2 == Dispositivos[i].IP){Dispositivo2 = &Dispositivos[i];}
    }
    for(int i = 0; i < 12; i++){
        if(Dispositivo1->Relacion[i].router == NULL){
            Dispositivo1->Relacion[i].router = Dispositivo2;
            Dispositivo1->Relacion[i].ping = ping;
            Dispositivo1->Relacion[i].conexion = tipoC;
            Dispositivo1->indice++;
            break;
        }  
    }
    for(int i = 0; i < 12; i++){
        if(Dispositivo2->Relacion[i].router == NULL){
            Dispositivo2->Relacion[i].router = Dispositivo1;
            Dispositivo2->Relacion[i].ping = ping;
            Dispositivo2->Relacion[i].conexion = tipoC;  
            Dispositivo2->indice++;
            break;
        }
    }
}
void AgregarRelacionDesdeConsola(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    string hostnameR1, hostnameR2, pingEntrada, tipoC;
    int ping = 0;
    Dispositivo *Dispositivo1, *Dispositivo2;

    cout << "Ingrese hostname1, hostname2, ping y tipo de conexion" << endl;
    cin >> hostnameR1 >> hostnameR2 >> pingEntrada >> tipoC;

    if(hostnameR1.at(hostnameR1.length() - 1) == ','){
        hostnameR1.pop_back(); 
    }
    if(hostnameR2.at(hostnameR2.length() - 1) == ','){
        hostnameR2.pop_back(); 
    }
    if(pingEntrada.at(pingEntrada.length() - 1) == ','){
        pingEntrada.pop_back(); 
    }

    if(tipoC.at(tipoC.length() - 1) == ','){
        tipoC.pop_back(); 
    }

    ping = stoi(pingEntrada);

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(hostnameR1 == Dispositivos[i].hostname || hostnameR1 == Dispositivos[i].IP){Dispositivo1 = &Dispositivos[i];}

        if(hostnameR2 == Dispositivos[i].hostname || hostnameR2 == Dispositivos[i].IP){Dispositivo2 = &Dispositivos[i];}
    }

    if(Dispositivo1->hostname != hostnameR1 && Dispositivo2->hostname == hostnameR2){
        resultados << "-3" << endl;
        cout << "-3: El dispositivo origen no ha sido encontrado" << endl;
        return;
    }
    else if(Dispositivo2->hostname != hostnameR2 && Dispositivo1->hostname == hostnameR1){
        resultados << "-2" << endl;
        cout << "-2: El dispositivo destino no ha sido encontrado" << endl;
        return;
    }
    else if (Dispositivo1->hostname != hostnameR1 && Dispositivo2->hostname == hostnameR2){
        resultados << "-1" << endl;
        cout << "-1: Ninguno de los dispositivos existen" << endl;
        return;
    }
    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivo1->Relacion[i].router == Dispositivo2){
            resultados << "0" << endl;
            cout << "0: Ya existe una conexion entre ambos dispositivos" << endl;
            return;
        }
    }
    for(int i = 0; i < 12; i++){
        if(Dispositivo1->Relacion[i].router == NULL){
            Dispositivo1->Relacion[i].router = Dispositivo2;
            Dispositivo1->Relacion[i].ping = ping;
            Dispositivo1->Relacion[i].conexion = tipoC;  
            Dispositivo1->indice++;
            break;
        }
    }
    for(int i = 0; i < 12; i++){
        if(Dispositivo2->Relacion[i].router == NULL){
            Dispositivo2->Relacion[i].router = Dispositivo1;
            Dispositivo2->Relacion[i].ping = ping;
            Dispositivo2->Relacion[i].conexion = tipoC; 
            Dispositivo2->indice++;
            break; 
        }
    }
    resultados << "1" << endl; 
    cout << "1: Operacion exitosa" << endl;
}

void EliminarRutaDirecta(Dispositivo *Eliminar1, Dispositivo *Eliminar2){
    RutasEliminadas++;
    for(int i = 0; i < 12; i++){
        if(Eliminar1->Relacion[i].router == Eliminar2){
            RutasRespaldo << Eliminar1->hostname << " ";
            Eliminar1->Relacion[i].router = NULL;
            Eliminar1->Relacion[i].ping = 0;
            Eliminar1->Relacion[i].conexion = "Vacio";
            Eliminar1->indice--;
        }
        if(Eliminar2->Relacion[i].router == Eliminar1){
            RutasRespaldo << Eliminar2->hostname << " " << Eliminar2->Relacion[i].ping << " " << Eliminar2->Relacion[i].conexion << endl;
            Eliminar2->Relacion[i].router = NULL;
            Eliminar2->Relacion[i].ping = 0;
            Eliminar2->Relacion[i].conexion = "Vacio";
            Eliminar2->indice--;
        }
    }

}
void EliminarRuta(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    Dispositivo *Raiz, *Destino;
    Pila Atravesados, Soluciones;
    string Dispositivo1, Dispositivo2;
    int contador = 0;
    bool Existe1 = false, Existe2 = false, Solucion = false;
    cout << "Ingrese el nombre o direccion IP (solo uno) de ambos dispositivos" << endl;
    cin >> Dispositivo1 >> Dispositivo2;

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivos[i].hostname == Dispositivo1 || Dispositivos[i].IP == Dispositivo1){
            Raiz = &Dispositivos[i];
            Existe1 = true;
        }
        if(Dispositivos[i].hostname == Dispositivo2 || Dispositivos[i].IP == Dispositivo2){
            Destino = &Dispositivos[i];
            Existe2 = true;
        }
    }

    if(Existe1 && !Existe2){
        cout << "-2: El dispositivo destino no existe" << endl;
        resultados << "-2" << endl;
        return;
    }
    else if(!Existe1 && Existe2){
        cout << "-3: El dispositivo origen no existe" << endl;
        resultados << "-3" << endl;
        return;
    }
    else if(!Existe1 && !Existe2){
        cout << "-1: Ambos dispositivos no existen" << endl;
        resultados << "-1" << endl;
        return;
    }

    Atravesados.insertar(Raiz);
    BacktrackingEliminar(Raiz, Destino, Raiz->indice, Atravesados, Soluciones, Solucion);
    if(!Solucion){
        cout << "0: No existe un camino entre los Dispositivos que  los conecte" << endl;
        resultados << "0" << endl;
    }
    Atravesados.VaciarPila();

    Dispositivo *Eliminar1, *Eliminar2;

    for(int i = 0; i < Soluciones.punteroDePila - 1; i++){
        Eliminar1 = Soluciones.extraerLectura(i);
        Eliminar2 = Soluciones.extraerLectura(i + 1);
        EliminarRutaDirecta(Eliminar1, Eliminar2);
        OrdenamientoRelaciones(Eliminar1->Relacion);
        OrdenamientoRelaciones(Eliminar2->Relacion);
    }

    cout << "Entre los dispositivos " << Dispositivo1 << " y " << Dispositivo2 << " se eliminaron las siguientes rutas: " << endl;
    for(int i = 0; i < Soluciones.punteroDePila; i++){
        cout << Soluciones.extraerCola(i)->hostname << " ";
        if(Soluciones.extraerCola(i)->hostname == Destino->hostname){
            contador++;
            cout << endl;
        }
    }
    Soluciones.VaciarPila();

    cout << "Total de rutas eliminadas: " << contador << endl; 
}

void EliminarDispositivo(Dispositivo Dispositivos[], int &cantidadDeDispositivos, int &DispositivosEliminados){
    string hostnameEl;
    Dispositivo *Dispositivo1;
    bool Existe = false;

    cout << "Ingrese el nombre del dispositivo a eliminar" << endl;
    cin >> hostnameEl;

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivos[i].hostname == hostnameEl || Dispositivos[i].IP == hostnameEl){
            Dispositivo1 = &Dispositivos[i];
            Existe = true;
            break;
        }
    }

    if(!Existe){
        cout << "El dispositivo no existe" << endl;
        resultados << "0" << endl;
        return;
    }

    for(int i = 0; i < 12; i++){
        for(int j = 0; j < cantidadDeDispositivos; j++){
            if(Dispositivos[i].Relacion[j].router == Dispositivo1){
                Dispositivos[i].Relacion[j].router = NULL;
            }
        }
    }

    for(int i = 0; i < 12; i++){
        Dispositivo1->Relacion[i].router = NULL;
    }

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivos[i].hostname == hostnameEl){
            DispositivosRespaldo << Dispositivos[i].hostname << " " << Dispositivos[i].IP << endl;
            Dispositivos[i].hostname = "{";
            break;
        }
    }

    OrdenamientoDispositivosNombre(Dispositivos, cantidadDeDispositivos);
    cantidadDeDispositivos--;
    DispositivosEliminados++;
    cout << "Se ha realizado exitosamente" << endl;
    resultados << "1" << endl;
}

void ConsultarDispositivos(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    cout << endl;
    cout <<"Ingrese el nombre o la direccion IP del dispositivo a consultar" << endl;
    string Consulta;
    cin >> Consulta;
    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Consulta == Dispositivos[i].hostname || Consulta == Dispositivos[i].IP){
            cout << Dispositivos[i].hostname << " " << Dispositivos[i].IP << endl;
            return;
        }
    }
    cout << "Dispositivo no encontrado." << endl;
}
void ListadoDeDispositivos(Dispositivo Dispositivos[], int cantidadDeDispositivos){

    OrdenamientoDispositivosNombre(Dispositivos, cantidadDeDispositivos);
    cout << "Listado de Dispositivos: " << endl;
    for(int i = 0; i < cantidadDeDispositivos; i++){
        cout << Dispositivos[i].hostname << " " << Dispositivos[i].IP << endl;
    }
}

void BuscarRuta(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    Pila Atravesados;
    bool Solucion = false, Existe1 = false, Existe2 = false;
    string dispositivo1Buscar, dispositivo2Buscar, Conexion;
    int tipoConexion;
    cout << "Indique los dispositivos y el tipo de conexion" << endl;
    cin >> dispositivo1Buscar >> dispositivo2Buscar >> tipoConexion;

    Dispositivo *Dispositivo1, *Dispositivo2;

    for(int i = 0; i < cantidadDeDispositivos; i++){
        if(Dispositivos[i].hostname == dispositivo1Buscar || Dispositivos[i].IP == dispositivo1Buscar){
            Dispositivo1 = &Dispositivos[i];
            Existe1 = true;
        }
        else if(Dispositivos[i].hostname == dispositivo2Buscar || Dispositivos[i].IP == dispositivo2Buscar){
            Dispositivo2 = &Dispositivos[i];
            Existe2 = true;
        }
    }

    if(Existe1 && !Existe2){
        cout << "-2: El dispositivo destino no existe" << endl;
        resultados << "-2" << endl;
        return;
    }
    else if(!Existe1 && Existe2){
        cout << "-3: El dispositivo origen no existe" << endl;
        resultados << "-3" << endl;
        return;
    }
    else if(!Existe1 && !Existe2){
        cout << "-1: Ambos dispositivos no existen" << endl;
        resultados << "-1" << endl;
        return;
    }

    switch(tipoConexion){
        case 1:
            Conexion = "5G";
            Atravesados.insertar(Dispositivo1);
            Backtracking(Dispositivo1, Dispositivo2, Dispositivo1->indice, Atravesados, Solucion, Conexion);
            if(!Solucion){
                resultados << "0" << endl;
                cout << endl << "No existe una relacion entre " << Dispositivo1->hostname << " y " << Dispositivo2->hostname << endl;
            }
            Atravesados.extraer();
        break;
        case 2:
            Conexion = "FibraOptica";
            Atravesados.insertar(Dispositivo1);
            Backtracking(Dispositivo1, Dispositivo2, Dispositivo1->indice, Atravesados, Solucion, Conexion);
            if(!Solucion){
                resultados << "0" << endl;
                cout << endl << "No existe una relacion entre " << Dispositivo1->hostname << " y " << Dispositivo2->hostname << endl;
            }
            Atravesados.extraer();
        break;
        case 3:
            Conexion = "Ambas";
            Atravesados.insertar(Dispositivo1);
            Backtracking(Dispositivo1, Dispositivo2, Dispositivo1->indice, Atravesados, Solucion, Conexion);
            if(!Solucion){
                resultados << "0" << endl;
                cout << endl << "No existe una relacion entre " << Dispositivo1->hostname << " y " << Dispositivo2->hostname << endl;
            }
            Atravesados.extraer();
        break;
    }
}
void DispositivosAdyacentes(Dispositivo Dispositivos[], int cantidadDeDispositivos){
    int cont = 0;
    string hostname1Ad;
    cout << "Indique el dispositivo: ";
    cin >> hostname1Ad;

    cout << "Listado de todos los dispositivos con relacion directa, ping y tipo de conexion" << endl;

    for(int i = 0; i < 12; i++){
        if(Dispositivos[i].hostname == hostname1Ad || Dispositivos[i].IP == hostname1Ad){
            for(int j = 0; j < 12; j++){
                if(Dispositivos[i].Relacion[j].router != NULL){
                    cout << Dispositivos[i].Relacion[j].router->hostname << ", ";
                    cout << Dispositivos[i].Relacion[j].ping << ", ";
                    cout << Dispositivos[i].Relacion[j].conexion;
                    cout << endl;
                    cont++;
                }
            }
            break;
        }
    }
    cout << endl << "Total de dispositivos adyacentes: " << cont << endl;
}

void ListadoDispositivosExistentes(Dispositivo Respaldo[], int cantidadDeDispositivos){
    OrdenamientoDispositivosNombre(Respaldo, cantidadDeDispositivos);
    for(int i = 0; i < cantidadDeDispositivos; i++){
        cout << Respaldo[i].hostname << endl;
    }
}

void ListadoDispositivosEliminados(int DispositivosEliminados){
    DispositivosRespaldo.seekg(0);
    string hostname;
    string IP;
    for(int i = 0; i < DispositivosEliminados; i++){
        DispositivosRespaldo >> hostname;
        DispositivosRespaldo >> IP;
        cout << hostname << " " << IP << endl;
    }
}
void ListadoRutasEliminadas(){
    RutasRespaldo.seekg(0);
    string hostname1, hostname2, conexion;
    int ping = 0;
    for(int i = 0; i < RutasEliminadas; i++){
        RutasRespaldo >> hostname1;
        RutasRespaldo >> hostname2;
        RutasRespaldo >> ping;
        RutasRespaldo >> conexion;
        cout << hostname1 << " " << hostname2 << " " << ping << " " << conexion << endl;
    }
}

void AgregarInformacion(char opcion, char &opcionPrincipal, int &PunteroDeDispositivos, int &cantidadDeDispositivos, Dispositivo Dispositivos[]){
    cout << endl;
    cout << "Universidad Central de Venezuela" << endl;
    cout << "Facultad de Ciencias" << endl;
    cout << "Escuela de Computacion" << endl;
    cout << "Orquestador de redes" << endl << endl;
    cout << "Agregar Informacion" << endl;
    cout << "1: Agregar dispositivos" << endl;
    cout << "2: Agregar ruta" << endl;
    cout << "3: Volver a Principal" << endl;
    cout << "4: Salir de la aplicacion" << endl;
    cin >> opcion;

    switch(opcion){
        case '1':
            AgregarDispositivo(PunteroDeDispositivos, Dispositivos, cantidadDeDispositivos);
        break;
        case '2':
            AgregarRelacionDesdeConsola(Dispositivos, cantidadDeDispositivos);
        break; 

        case '4':
            opcionPrincipal = '6';
            return;
        break;
    }
    return;
}

void EliminarInformacion(char opcion, char &opcionPrincipal, Dispositivo Dispositivos[], int cantidadDeDispositivos, int &DispositivosEliminados){
    cout << endl;
    cout << "Universidad Central de Venezuela" << endl;
    cout << "Facultad de Ciencias" << endl;
    cout << "Escuela de Computacion" << endl;
    cout << "Orquestador de redes" << endl << endl;
    cout << "Eliminar Informacion" << endl;
    cout << "1: Eliminar dispositivo" << endl;
    cout << "2: Eliminar ruta" << endl;
    cout << "3: Volver a Principal" << endl;
    cout << "4: Salir de la aplicacion" << endl;
    cin >> opcion;

    switch(opcion){
        case '1':
            EliminarDispositivo(Dispositivos, cantidadDeDispositivos, DispositivosEliminados);
        break;
        case '2':
            EliminarRuta(Dispositivos, cantidadDeDispositivos);
        break; 

        case '4':
            opcionPrincipal = '6';
            return;
        break;
    }
    return;
}

void BuscarYListar(char opcion, char &opcionPrincipal, Dispositivo Dispositivos[], int cantidadDeDispositivos){
    cout << endl;
    cout << "Universidad Central de Venezuela" << endl;
    cout << "Facultad de Ciencias" << endl;
    cout << "Escuela de Computacion" << endl;
    cout << "Orquestador de redes" << endl << endl;
    cout << "Buscar y Listar" << endl;
    cout << "1: Consultar dispositivo." << endl;
    cout << "2: Listado de Dispositivos." << endl;
    cout << "3: Buscar ruta(1:5G, 2: fibra optica, 3: ambas)." << endl;
    cout << "4: Dispositivos adyacentes." << endl;
    cout << "5: Volver a Principal." << endl;
    cout << "6: Salir de la aplicacion." << endl;
    cin >> opcion;

    switch(opcion){
        case '1':
            ConsultarDispositivos(Dispositivos, cantidadDeDispositivos);
        break;
        case '2':
            ListadoDeDispositivos(Dispositivos, cantidadDeDispositivos);
        break; 
        case '3':
            BuscarRuta(Dispositivos, cantidadDeDispositivos);
        break;
        case '4':
            DispositivosAdyacentes(Dispositivos, cantidadDeDispositivos);
        break;
        case '6':
            opcionPrincipal = '6';
            return;
        break;
    }
    return;
}

void MostrarRespaldos(char opcion, char &opcionPrincipal, Dispositivo Respaldo[], int cantidadDeDispositivos, int DispositivosEliminados){
    cout << endl;
    cout << "Universidad Central de Venezuela" << endl;
    cout << "Facultad de Ciencias" << endl;
    cout << "Escuela de Computacion" << endl;
    cout << "Orquestador de redes" << endl << endl;
    cout << "Mostrar Respaldos" << endl;
    cout << "1: Listado Dispositivos existentes." << endl;
    cout << "2: Listado Dispositivos eliminados." << endl;
    cout << "3: Listado rutas eliminadas." << endl;
    cout << "4: Volver a Principal." << endl;
    cout << "5: Salir de la aplicacion." << endl;
    cin >> opcion;

    switch(opcion){
        case '1':
            ListadoDispositivosExistentes(Respaldo, cantidadDeDispositivos);
        break;

        case '2':
            ListadoDispositivosEliminados(DispositivosEliminados);
        break; 

        case '3':
            ListadoRutasEliminadas();
        break;

        case '5':
            opcionPrincipal = '6';
            return;
        break;
    }
    return;
}

void MostrarCreditos(){
    cout << endl;
    cout << "Universidad Central de Venezuela" << endl;
    cout << "Facultad de Ciencias" << endl;
    cout << "Escuela de Computacion" << endl;
    cout << "Orquestador de redes" << endl << endl;
    cout << "Jhonatan Homsany C.I. 30.182.893" << endl;
    cout << "Cesar Carios C.I. 30.136.117" << endl;
    cout << "Los Teques, a los 5 dias del mes de Junio del 2023" << endl;
}

int main(){

    //Lectura de Archivo
    int CantidadDispositivos = 0;
    int PunteroDeDispositivos = 0;
    int DispositivosEliminados = 0;
    int CantidadRelaciones = 0;

    entrada >> CantidadDispositivos;
    Dispositivo Dispositivos[1000];
    Dispositivo Respaldo[CantidadDispositivos];

    while(PunteroDeDispositivos < CantidadDispositivos){
        entrada >> Dispositivos[PunteroDeDispositivos].hostname;
        entrada >> Dispositivos[PunteroDeDispositivos].IP;
        PunteroDeDispositivos++;
    }

    for(int i = 0; i < CantidadDispositivos; i++){
        if(Dispositivos[i].hostname.at(Dispositivos[i].hostname.length() - 1) == ','){
            Dispositivos[i].hostname.pop_back(); 
        }
        if(Dispositivos[i].IP.at(Dispositivos[i].IP.length() - 1) == ','){
            Dispositivos[i].IP.pop_back(); 
        }
    }

    entrada >> CantidadRelaciones;

    for(int i = 0; i < CantidadRelaciones; i++){ AgregarRelacionDesdeEntrada(Dispositivos, CantidadDispositivos, entrada); }

    for(int i = 0; i < CantidadDispositivos; i++){
        Respaldo[i] = Dispositivos[i];
    }

    //Menu Principal

    char opcionPrincipal = '0';
    char opcion;

    do{
        
        cout << endl;
        cout << "Universidad Central de Venezuela" << endl;
        cout << "Facultad de Ciencias" << endl;
        cout << "Escuela de Computacion" << endl;
        cout << "Orquestador de redes" << endl << endl;
        cout << "Principal" << endl;
        cout << "1: Agregar informacion" << endl;
        cout << "2: Eliminar informacion" << endl;
        cout << "3: Buscar y listar" << endl;
        cout << "4: Mostrar respaldos" << endl;
        cout << "5: Creditos" << endl;
        cout << "6: Salir de la aplicacion" << endl;

        cin >> opcionPrincipal;

        if(opcionPrincipal == '1'){
            AgregarInformacion(opcion, opcionPrincipal, PunteroDeDispositivos, CantidadDispositivos, Dispositivos);
        }
        else if(opcionPrincipal == '2'){
            EliminarInformacion(opcion, opcionPrincipal, Dispositivos, CantidadDispositivos, DispositivosEliminados);
        }
        else if(opcionPrincipal == '3'){
            BuscarYListar(opcion, opcionPrincipal, Dispositivos, CantidadDispositivos);
        }
        else if(opcionPrincipal == '4'){
            MostrarRespaldos(opcion, opcionPrincipal, Respaldo, CantidadDispositivos, DispositivosEliminados);
        }
        else if(opcionPrincipal == '5'){
            MostrarCreditos();
        };

    }while(opcionPrincipal != '6');

    cout << "Programa terminado";

    return 0;
}