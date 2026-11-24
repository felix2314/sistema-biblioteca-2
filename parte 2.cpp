/* ---------------------------------------------------------------------
   SISTEMA DE GESTION DE BIBLIOTECA UNIVERSITARIA - GRUPO 2
   Programacion de Computadoras I - UNMSM - 2026-2
   Version en un solo archivo. Bibliotecas: iostream (entrada/salida),
   cstdio (archivos binarios FILE*) y cstdlib (system y exit).
   Las cadenas y el formato de salida se resuelven con funciones propias.
   Compilar: g++ -Wall -g biblioteca_grupo2.cpp -o biblioteca
   --------------------------------------------------------------------- */
#include <iostream>
#include <cstdio>   /* FILE, fopen, fread, fwrite, fseek, ftell, rewind, fclose */
#include <cstdlib>  /* system, exit */
using namespace std;

const int MAX_LIBROS = 500;
const int MAX_USUARIOS = 300;
const int MAX_PRESTAMOS = 1000;
const int MAX_RESERVAS = 300;
const int PLAZO_DIAS = 7;  /* dias de prestamo permitidos */

struct Fecha {
    int dia;
    int mes;
    int anio;
};

struct Libro {
    char codigo[15];
    char titulo[100];
    char autor[60];
    int anio;  /* anio de publicacion */
    int totales;  /* ejemplares totales */
    int disponibles;  /* ejemplares disponibles */
    int vecesPrestado;  /* para el reporte de mas prestados */
    bool activo;  /* false = baja logica */
    Fecha fechaAlta;  /* registro anidado */
};

struct Usuario {
    char codigo[15];
    char nombres[80];
    char tipo[15];  /* "Estudiante" o "Docente" */
    bool sancionado;
};

struct Prestamo {
    int idPrestamo;
    char codLibro[15];
    char codUsuario[15];
    Fecha fechaSalida;  /* dia en que se presta */
    Fecha fechaLimite;  /* fechaSalida + PLAZO_DIAS */
    Fecha fechaDevolucion;  /* fecha real (0/0/0 si no se devolvio) */
    bool devuelto;
    float multa;  /* monto cobrado por retraso */
};

struct Reserva {  /* innovacion: lista de espera */
    int idReserva;  /* crece en orden de llegada (cola FIFO) */
    char codLibro[15];
    char codUsuario[15];
    Fecha fechaReserva;
    bool atendida;  /* false = sigue en espera */
};

/* ---------- PROTOTIPOS ---------- */
/* Pantalla y menus (main.cpp - Guizado) */
void iniciarSistema();
void limpiarPantalla();
void pausar();
void mostrarMenuPrincipal(int &opcion);
void menuLibros(Libro libros[], int &cantL);
void menuUsuarios(Usuario usuarios[], int &cantU);
void menuBusquedas(Libro libros[], int cantL);
void menuPrestamos(Prestamo prestamos[], int &cantP, Libro libros[], int cantL,
                   Usuario usuarios[], int cantU, Reserva reservas[], int cantR, float tarifa);
void menuReportes(Libro libros[], int cantL, Prestamo prestamos[], int cantP, Usuario usuarios[], int cantU);
void menuArchivos(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                  Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR);
void menuReservas(Reserva reservas[], int &cantR, Libro libros[], int cantL, Usuario usuarios[], int cantU);

/* Cadenas propias (reemplazan a cstring) */
int longitudTexto(const char t[]);
void copiarTexto(char destino[], const char origen[]);
void concatenarTexto(char destino[], const char origen[]);
int compararTexto(const char a[], const char b[]);
bool contieneTexto(const char texto[], const char buscado[]);

/* Entrada y salida con formato (reemplazan a iomanip) */
void escribirCampoTexto(const char texto[], int ancho);
void escribirCampoNumero(int n, int ancho);
void mostrarMonto(float monto);
void mostrarFecha(Fecha f);

/* Libros (libros.cpp - Guizado) */
void registrarLibro(Libro libros[], int &cantL);
void bajaLogicaLibro(Libro libros[], int cantL);
void modificarLibro(Libro libros[], int cantL);
void consultarLibros(Libro libros[], int cantL);
void ordenarLibrosPorCodigo(Libro libros[], int cantL);

/* Usuarios (usuarios.cpp - Huiza) */
void registrarUsuario(Usuario usuarios[], int &cantU);
void actualizarSancionUsuario(Usuario usuarios[], int cantU);
int buscarUsuarioPorCodigo(Usuario usuarios[], int cantU, char cod[]);

/* Busquedas (busquedas.cpp - Chavez / busqueda_texto.cpp - Huiza) */
int busquedaBinariaRecursiva(Libro libros[], int inicio, int fin, char cod[]);
void buscarLibrosPorAutor(Libro *ptrLibros, int cantL, char autor[]);
void buscarLibrosPorTitulo(Libro *ptrLibros, int cantL, char palabra[]);

/* Utilidades: tarifa, fechas y multas (utilidades.cpp - Huiza) */
void configurarTarifaMulta(float &tarifa);
bool validarFecha(Fecha f);
int diferenciaDias(Fecha f1, Fecha f2);
void calcularFechaLimite(Fecha salida, int plazo, Fecha &limite);
float calcularMulta(int diasRetraso, float tarifa);
bool esBisiesto(int a);
int diasEnMes(int m, int a);
int diasAbsolutos(Fecha f);
void limpiarLinea();
void salirSiFin();
int leerEntero(const char mensaje[], int minimo, int maximo);
void leerTexto(const char mensaje[], char destino[], int tam);
void leerFecha(const char mensaje[], Fecha &f);

/* Prestamos y devoluciones (prestamos.cpp - Chavez) */
void registrarPrestamo(Prestamo prestamos[], int &cantP, Libro libros[], int cantL,
                       Usuario usuarios[], int cantU, Reserva reservas[], int cantR);
void registrarDevolucion(Prestamo prestamos[], int cantP, Libro libros[], int cantL,
                         Usuario usuarios[], int cantU, Reserva reservas[], int cantR, float tarifa);

/* Reservas (reservas.cpp - Chavez) */
void registrarReserva(Reserva reservas[], int &cantR, Libro libros[], int cantL, Usuario usuarios[], int cantU);
int siguienteEnEspera(Reserva reservas[], int cantR, char codLibro[]);
void consultarListaEspera(Reserva reservas[], int cantR, char codLibro[], Usuario usuarios[], int cantU);

/* Reportes (reportes.cpp - Guerrero) */
void reporteLibrosMasPrestados(Libro *ptrLibros, int cantL);
void reportePrestamosVencidos(Prestamo prestamos[], int cantP, Fecha hoy);
void historialPrestamosUsuario(Prestamo prestamos[], int cantP, char codUsuario[]);

/* Archivos (archivos.cpp - Guerrero) */
void leerArchivo(const char nombre[], char *datos, int tam, int max, int &cant, int &ok);
void escribirArchivo(const char nombre[], const char *datos, int tam, int cant, int &ok);
void copiarArchivo(const char origen[], const char destino[], int &ok);
void nombreArchivo(int k, const char sufijo[], char destino[]);
void cargarDatos(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                 Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR, int &ok);
void guardarDatos(Libro libros[], int cantL, Usuario usuarios[], int cantU,
                  Prestamo prestamos[], int cantP, Reserva reservas[], int cantR, int &ok);
void crearRespaldo(int &ok);
void restaurarRespaldo(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                       Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR, int &ok);

/* Nombres base de los cuatro archivos de datos */
const char *NOMBRES[4] = {"libros", "usuarios", "prestamos", "reservas"};

int main() {
    iniciarSistema();
    return 0;
}

/* ---------- PROGRAMA PRINCIPAL Y MENUS (Guizado) ---------- */

/** @brief Crea los arreglos, carga los archivos y ejecuta el menu principal. */
void iniciarSistema() {
    Libro libros[MAX_LIBROS];          int cantLibros = 0;
    Usuario usuarios[MAX_USUARIOS];    int cantUsuarios = 0;
    Prestamo prestamos[MAX_PRESTAMOS]; int cantPrestamos = 0;
    Reserva reservas[MAX_RESERVAS];    int cantReservas = 0;
    float tarifaMulta = 1.50;  /* soles por dia de retraso (configurable) */
    int opcion, ok;

    cargarDatos(libros, cantLibros, usuarios, cantUsuarios, prestamos, cantPrestamos, reservas, cantReservas, ok);
    if (ok == 0) {
        cout << "No se pudo cargar la informacion. El programa se cierra para no perder datos." << endl;
        pausar();
        return;
    }
    do {
        limpiarPantalla();
        mostrarMenuPrincipal(opcion);
        switch (opcion) {
            case 1: menuLibros(libros, cantLibros); break;
            case 2: menuUsuarios(usuarios, cantUsuarios); break;
            case 3: menuBusquedas(libros, cantLibros); break;
            case 4: menuPrestamos(prestamos, cantPrestamos, libros, cantLibros, usuarios, cantUsuarios, reservas, cantReservas, tarifaMulta); break;
            case 5: menuReportes(libros, cantLibros, prestamos, cantPrestamos, usuarios, cantUsuarios); break;
            case 6: menuArchivos(libros, cantLibros, usuarios, cantUsuarios, prestamos, cantPrestamos, reservas, cantReservas); break;
            case 7: configurarTarifaMulta(tarifaMulta); pausar(); break;
            case 8: menuReservas(reservas, cantReservas, libros, cantLibros, usuarios, cantUsuarios); break;
        }
    } while (opcion != 0);
    guardarDatos(libros, cantLibros, usuarios, cantUsuarios, prestamos, cantPrestamos, reservas, cantReservas, ok);
    if (ok == 1) cout << "Datos guardados. Hasta pronto." << endl;
}

/** @brief Limpia la pantalla de la consola (Windows o Linux). */
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/** @brief Detiene el programa hasta que el usuario presione Enter (Windows o Linux). */
void pausar() {
    cout << "Presione Enter para continuar...";
    cin.get();
    salirSiFin();
}

/** @brief Muestra el menu principal y captura la opcion validada (0 a 8).
 *  @param opcion (por referencia) opcion elegida. */
void mostrarMenuPrincipal(int &opcion) {
    cout << "======================================================" << endl;
    cout << "   SISTEMA DE GESTION DE BIBLIOTECA UNIVERSITARIA     " << endl;
    cout << "======================================================" << endl;
    cout << "1. Libros" << endl;
    cout << "2. Usuarios" << endl;
    cout << "3. Busquedas" << endl;
    cout << "4. Prestamos y devoluciones" << endl;
    cout << "5. Reportes" << endl;
    cout << "6. Archivos y respaldo" << endl;
    cout << "7. Configuracion (tarifa de multa)" << endl;
    cout << "8. Reservas" << endl;
    cout << "0. Salir" << endl;
    cout << "======================================================" << endl;
    opcion = leerEntero("Opcion: ", 0, 8);
}

void menuLibros(Libro libros[], int &cantL) {
    int op;
    do {
        limpiarPantalla();
        cout << "--- LIBROS ---" << endl;
        cout << "1. Registrar libro" << endl << "2. Modificar libro" << endl;
        cout << "3. Dar de baja libro" << endl << "4. Consultar libros" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 4);
        if (op == 1) registrarLibro(libros, cantL);
        else if (op == 2) modificarLibro(libros, cantL);
        else if (op == 3) bajaLogicaLibro(libros, cantL);
        else if (op == 4) consultarLibros(libros, cantL);
        if (op != 0) pausar();
    } while (op != 0);
}

void menuUsuarios(Usuario usuarios[], int &cantU) {
    int op;
    do {
        limpiarPantalla();
        cout << "--- USUARIOS ---" << endl;
        cout << "1. Registrar usuario" << endl << "2. Actualizar sancion" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 2);
        if (op == 1) registrarUsuario(usuarios, cantU);
        else if (op == 2) actualizarSancionUsuario(usuarios, cantU);
        if (op != 0) pausar();
    } while (op != 0);
}

void menuBusquedas(Libro libros[], int cantL) {
    int op;
    char texto[100];
    do {
        limpiarPantalla();
        cout << "--- BUSQUEDAS ---" << endl;
        cout << "1. Por codigo" << endl << "2. Por autor" << endl << "3. Por palabra del titulo" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            leerTexto("Codigo del libro: ", texto, 15);
            int pos = busquedaBinariaRecursiva(libros, 0, cantL - 1, texto);
            if (pos == -1) cout << "Libro no encontrado." << endl;
            else if (!libros[pos].activo) cout << "El libro existe pero esta dado de baja." << endl;
            else cout << libros[pos].codigo << " | " << libros[pos].titulo << " | " << libros[pos].autor << " | " << libros[pos].anio
                      << " | disponibles " << libros[pos].disponibles << "/" << libros[pos].totales << " (posicion " << pos << ")" << endl;
        } else if (op == 2) {
            leerTexto("Autor: ", texto, 60);
            buscarLibrosPorAutor(libros, cantL, texto);
        } else if (op == 3) {
            leerTexto("Palabra del titulo: ", texto, 100);
            buscarLibrosPorTitulo(libros, cantL, texto);
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menuPrestamos(Prestamo prestamos[], int &cantP, Libro libros[], int cantL,
                   Usuario usuarios[], int cantU, Reserva reservas[], int cantR, float tarifa) {
    int op;
    do {
        limpiarPantalla();
        cout << "--- PRESTAMOS Y DEVOLUCIONES ---" << endl;
        cout << "1. Registrar prestamo" << endl << "2. Registrar devolucion" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 2);
        if (op == 1) registrarPrestamo(prestamos, cantP, libros, cantL, usuarios, cantU, reservas, cantR);
        else if (op == 2) registrarDevolucion(prestamos, cantP, libros, cantL, usuarios, cantU, reservas, cantR, tarifa);
        if (op != 0) pausar();
    } while (op != 0);
}

void menuReportes(Libro libros[], int cantL, Prestamo prestamos[], int cantP, Usuario usuarios[], int cantU) {
    int op;
    char cod[15];
    Fecha hoy;
    do {
        limpiarPantalla();
        cout << "--- REPORTES ---" << endl;
        cout << "1. Libros mas prestados" << endl << "2. Prestamos vencidos" << endl << "3. Historial por usuario" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            reporteLibrosMasPrestados(libros, cantL);
        } else if (op == 2) {
            leerFecha("Fecha actual", hoy);
            reportePrestamosVencidos(prestamos, cantP, hoy);
        } else if (op == 3) {
            leerTexto("Codigo del usuario: ", cod, 15);
            if (buscarUsuarioPorCodigo(usuarios, cantU, cod) == -1) cout << "Error: el usuario no esta registrado." << endl;
            else historialPrestamosUsuario(prestamos, cantP, cod);
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menuArchivos(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                  Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR) {
    int op, ok;
    do {
        limpiarPantalla();
        cout << "--- ARCHIVOS Y RESPALDO ---" << endl;
        cout << "1. Guardar datos" << endl << "2. Crear copia de seguridad" << endl << "3. Restaurar copia de seguridad" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            guardarDatos(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            if (ok == 1) cout << "Datos guardados correctamente." << endl;
        } else if (op == 2) {
            guardarDatos(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            if (ok == 1) crearRespaldo(ok);
        } else if (op == 3) {
            cout << "Esto reemplaza los datos actuales por los de la copia de seguridad." << endl;
            if (leerEntero("Continuar? (1 = Si, 0 = No): ", 0, 1) == 1)
                restaurarRespaldo(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            else cout << "Operacion cancelada." << endl;
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void menuReservas(Reserva reservas[], int &cantR, Libro libros[], int cantL, Usuario usuarios[], int cantU) {
    int op;
    char cod[15];
    do {
        limpiarPantalla();
        cout << "--- RESERVAS ---" << endl;
        cout << "1. Registrar reserva" << endl << "2. Consultar lista de espera" << endl << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 2);
        if (op == 1) {
            registrarReserva(reservas, cantR, libros, cantL, usuarios, cantU);
        } else if (op == 2) {
            leerTexto("Codigo del libro: ", cod, 15);
            if (busquedaBinariaRecursiva(libros, 0, cantL - 1, cod) == -1) cout << "Error: el libro no existe.";
            else consultarListaEspera(reservas, cantR, cod, usuarios, cantU);
            cout << endl;
        }
        if (op != 0) pausar();
    } while (op != 0);
}

/* ---------- CADENAS PROPIAS (reemplazan a cstring) ---------- */

/** @brief Cuenta los caracteres de una cadena (sin el '\0'). */
int longitudTexto(const char t[]) {
    int n = 0;
    while (t[n] != '\0') n++;
    return n;
}

/** @brief Copia origen en destino (incluye el '\0'). */
void copiarTexto(char destino[], const char origen[]) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

/** @brief Agrega origen al final de destino. */
void concatenarTexto(char destino[], const char origen[]) {
    copiarTexto(destino + longitudTexto(destino), origen);
}

/** @brief Compara dos cadenas: 0 si son iguales, <0 si a va antes que b, >0 si va despues. */
int compararTexto(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) i++;
    return (unsigned char)a[i] - (unsigned char)b[i];
}

/** @brief Indica si buscado aparece dentro de texto. */
bool contieneTexto(const char texto[], const char buscado[]) {
    int n = longitudTexto(texto);
    int m = longitudTexto(buscado);
    for (int i = 0; i + m <= n; i++) {
        int j = 0;
        while (j < m && texto[i + j] == buscado[j]) j++;
        if (j == m) return true;
    }
    return false;
}

/* ---------- ENTRADA Y SALIDA CON FORMATO (reemplazan a iomanip) ---------- */

/** @brief Escribe un texto y lo completa con espacios hasta ocupar "ancho" columnas. */
void escribirCampoTexto(const char texto[], int ancho) {
    cout << texto;
    for (int i = longitudTexto(texto); i < ancho; i++) cout << ' ';
}

/** @brief Escribe un entero y lo completa con espacios hasta ocupar "ancho" columnas. */
void escribirCampoNumero(int n, int ancho) {
    int digitos = 1;
    int resto = n;
    if (resto < 0) {
        digitos++;
        resto = -resto;
    }
    while (resto >= 10) {
        digitos++;
        resto = resto / 10;
    }
    cout << n;
    for (int i = digitos; i < ancho; i++) cout << ' ';
}

/** @brief Muestra un monto con dos decimales. */
void mostrarMonto(float monto) {
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << monto;
}

/** @brief Muestra una fecha con formato dd/mm/aaaa. */
void mostrarFecha(Fecha f) {
    if (f.dia < 10) cout << '0';
    cout << f.dia << "/";
    if (f.mes < 10) cout << '0';
    cout << f.mes << "/";
    if (f.anio < 1000) cout << '0';
    if (f.anio < 100) cout << '0';
    if (f.anio < 10) cout << '0';
    cout << f.anio;
}

/* ---------- LIBROS (Guizado) ---------- */

/** @brief Registra un libro nuevo y mantiene el arreglo ordenado por codigo.
 *  @param libros arreglo de libros.
 *  @param cantL (por referencia) cantidad de libros registrados. */
void registrarLibro(Libro libros[], int &cantL) {
    if (cantL >= MAX_LIBROS) {
        cout << "Error: se alcanzo la capacidad maxima de libros." << endl;
        return;
    }
    Libro nuevo = {};  /* en cero: evita grabar bytes sin inicializar en el archivo */
    leerTexto("Codigo: ", nuevo.codigo, 15);
    if (busquedaBinariaRecursiva(libros, 0, cantL - 1, nuevo.codigo) != -1) {
        cout << "Error: el codigo ya existe." << endl;
        return;
    }
    leerTexto("Titulo: ", nuevo.titulo, 100);
    leerTexto("Autor: ", nuevo.autor, 60);
    nuevo.anio = leerEntero("Anio de publicacion: ", 1000, 2100);
    nuevo.totales = leerEntero("Ejemplares (mayor que 0): ", 1, 1000);
    leerFecha("Fecha de alta", nuevo.fechaAlta);
    nuevo.disponibles = nuevo.totales;
    nuevo.vecesPrestado = 0;
    nuevo.activo = true;
    libros[cantL] = nuevo;
    cantL++;
    ordenarLibrosPorCodigo(libros, cantL);
    cout << "Libro registrado correctamente." << endl;
}

/** @brief Da de baja logica a un libro (activo = false), sin borrar el registro.
 *  @param libros arreglo de libros.
 *  @param cantL cantidad de libros. */
void bajaLogicaLibro(Libro libros[], int cantL) {
    char cod[15];
    leerTexto("Codigo del libro: ", cod, 15);
    int pos = busquedaBinariaRecursiva(libros, 0, cantL - 1, cod);
    if (pos == -1) {
        cout << "Error: el libro no existe." << endl;
    } else if (!libros[pos].activo) {
        cout << "El libro ya estaba dado de baja." << endl;
    } else {
        libros[pos].activo = false;
        cout << "Libro dado de baja correctamente." << endl;
    }
}

/** @brief Modifica titulo, autor, anio o ejemplares de un libro activo.
 *  @param libros arreglo de libros.
 *  @param cantL cantidad de libros. */
void modificarLibro(Libro libros[], int cantL) {
    char cod[15];
    leerTexto("Codigo del libro: ", cod, 15);
    int pos = busquedaBinariaRecursiva(libros, 0, cantL - 1, cod);
    if (pos == -1 || !libros[pos].activo) {
        cout << "Error: el libro no existe o esta dado de baja." << endl;
        return;
    }
    cout << "1. Titulo  2. Autor  3. Anio  4. Ejemplares" << endl;
    int op = leerEntero("Dato a cambiar: ", 1, 4);
    if (op == 1) {
        leerTexto("Nuevo titulo: ", libros[pos].titulo, 100);
    } else if (op == 2) {
        leerTexto("Nuevo autor: ", libros[pos].autor, 60);
    } else if (op == 3) {
        libros[pos].anio = leerEntero("Nuevo anio: ", 1000, 2100);
    } else {
        int nuevo = leerEntero("Nuevo total de ejemplares: ", 1, 1000);
        int prestados = libros[pos].totales - libros[pos].disponibles;
        if (nuevo < prestados) {
            cout << "Error: hay " << prestados << " ejemplares prestados; el total no puede ser menor. No se modifico." << endl;
            return;
        }
        libros[pos].totales = nuevo;
        libros[pos].disponibles = nuevo - prestados;
    }
    cout << "Libro modificado correctamente." << endl;
}

/** @brief Lista los libros activos con sus ejemplares totales y disponibles. */
void consultarLibros(Libro libros[], int cantL) {
    int n = 0;
    for (int i = 0; i < cantL; i++) {
        if (!libros[i].activo) continue;
        if (n == 0) {
            escribirCampoTexto("Codigo", 8);
            escribirCampoTexto("Titulo", 30);
            escribirCampoTexto("Autor", 14);
            escribirCampoTexto("Anio", 6);
            escribirCampoTexto("Total", 7);
            cout << "Disp" << endl;
        }
        escribirCampoTexto(libros[i].codigo, 8);
        escribirCampoTexto(libros[i].titulo, 30);
        escribirCampoTexto(libros[i].autor, 14);
        escribirCampoNumero(libros[i].anio, 6);
        escribirCampoNumero(libros[i].totales, 7);
        cout << libros[i].disponibles << endl;
        n++;
    }
    if (n == 0) cout << "No hay libros registrados." << endl;
}

/** @brief Ordena los libros por codigo (burbuja); precondicion de la busqueda binaria. */
void ordenarLibrosPorCodigo(Libro libros[], int cantL) {
    for (int i = 0; i < cantL - 1; i++) {
        for (int j = 0; j < cantL - i - 1; j++) {
            if (compararTexto(libros[j].codigo, libros[j + 1].codigo) > 0) {
                Libro aux = libros[j];
                libros[j] = libros[j + 1];
                libros[j + 1] = aux;
            }
        }
    }
}

/* ---------- USUARIOS (Huiza) ---------- */

/** @brief Registra un estudiante o docente (codigo unico, sin sancion).
 *  @param usuarios arreglo de usuarios.
 *  @param cantU (por referencia) cantidad de usuarios. */
void registrarUsuario(Usuario usuarios[], int &cantU) {
    if (cantU >= MAX_USUARIOS) {
        cout << "Error: se alcanzo la capacidad maxima de usuarios." << endl;
        return;
    }
    Usuario u = {};  /* en cero: evita grabar bytes sin inicializar en el archivo */
    leerTexto("Codigo: ", u.codigo, 15);
    if (buscarUsuarioPorCodigo(usuarios, cantU, u.codigo) != -1) {
        cout << "Error: el codigo de usuario ya existe." << endl;
        return;
    }
    leerTexto("Nombres: ", u.nombres, 80);
    bool tipoValido;
    do {
        leerTexto("Tipo (Estudiante o Docente): ", u.tipo, 15);
        tipoValido = compararTexto(u.tipo, "Estudiante") == 0 || compararTexto(u.tipo, "Docente") == 0;
        if (!tipoValido) cout << "Error: el tipo debe ser Estudiante o Docente." << endl;
    } while (!tipoValido);
    u.sancionado = false;
    usuarios[cantU] = u;
    cantU++;
    cout << "Usuario registrado correctamente." << endl;
}

/** @brief Sanciona a un usuario o le levanta la sancion.
 *  @param usuarios arreglo de usuarios.
 *  @param cantU cantidad de usuarios. */
void actualizarSancionUsuario(Usuario usuarios[], int cantU) {
    char cod[15];
    leerTexto("Codigo del usuario: ", cod, 15);
    int pos = buscarUsuarioPorCodigo(usuarios, cantU, cod);
    if (pos == -1) {
        cout << "Error: el usuario no esta registrado." << endl;
        return;
    }
    cout << usuarios[pos].nombres << " - estado actual: " << (usuarios[pos].sancionado ? "SANCIONADO" : "sin sancion") << endl;
    cout << "1. Sancionar  2. Levantar sancion" << endl;
    int op = leerEntero("Opcion: ", 1, 2);
    usuarios[pos].sancionado = (op == 1);
    cout << (op == 1 ? "Usuario sancionado." : "Sancion levantada.") << endl;
}

/** @brief Busqueda secuencial de un usuario por codigo.
 *  @return posicion del usuario o -1 si no existe. */
int buscarUsuarioPorCodigo(Usuario usuarios[], int cantU, char cod[]) {
    for (int i = 0; i < cantU; i++) {
        if (compararTexto(usuarios[i].codigo, cod) == 0) return i;
    }
    return -1;
}

/* ---------- BUSQUEDAS (Chavez y Huiza) ---------- */

/** @brief Busca un libro por codigo con busqueda binaria recursiva.
 *  @param libros arreglo ordenado ascendentemente por codigo.
 *  @param inicio primer indice del rango.
 *  @param fin ultimo indice del rango.
 *  @param cod codigo buscado.
 *  @return posicion del libro o -1 si no existe. */
int busquedaBinariaRecursiva(Libro libros[], int inicio, int fin, char cod[]) {
    if (inicio > fin) return -1;  /* caso base: no encontrado */
    int medio = inicio + (fin - inicio) / 2;
    int comparacion = compararTexto(libros[medio].codigo, cod);
    if (comparacion == 0) return medio;  /* caso base: encontrado */
    if (comparacion > 0) return busquedaBinariaRecursiva(libros, inicio, medio - 1, cod);
    return busquedaBinariaRecursiva(libros, medio + 1, fin, cod);
}

/** @brief Muestra los libros activos cuyo autor contiene el texto ingresado.
 *  @param ptrLibros apuntador al primer libro del arreglo.
 *  @param cantL cantidad de libros.
 *  @param autor texto a buscar en el autor. */
void buscarLibrosPorAutor(Libro *ptrLibros, int cantL, char autor[]) {
    int hallados = 0;
    for (Libro *p = ptrLibros; p < ptrLibros + cantL; p++) {
        if (p->activo && contieneTexto(p->autor, autor)) {
            cout << p->codigo << " | " << p->titulo << " | " << p->autor << " | " << p->anio
                 << " | disponibles " << p->disponibles << "/" << p->totales << endl;
            hallados++;
        }
    }
    if (hallados == 0) cout << "No se encontraron libros de ese autor." << endl;
}

/** @brief Muestra los libros activos cuyo titulo contiene la palabra ingresada.
 *  @param ptrLibros apuntador al primer libro del arreglo.
 *  @param cantL cantidad de libros.
 *  @param palabra texto a buscar en el titulo. */
void buscarLibrosPorTitulo(Libro *ptrLibros, int cantL, char palabra[]) {
    int hallados = 0;
    for (Libro *p = ptrLibros; p < ptrLibros + cantL; p++) {
        if (p->activo && contieneTexto(p->titulo, palabra)) {
            cout << p->codigo << " | " << p->titulo << " | " << p->autor << " | " << p->anio
                 << " | disponibles " << p->disponibles << "/" << p->totales << endl;
            hallados++;
        }
    }
    if (hallados == 0) cout << "No se encontraron libros con esa palabra en el titulo." << endl;
}

/* ---------- UTILIDADES: TARIFA, FECHAS Y MULTAS (Huiza) ---------- */

bool esBisiesto(int a) {
    return (a % 4 == 0 && a % 100 != 0) || a % 400 == 0;
}

int diasEnMes(int m, int a) {
    int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && esBisiesto(a)) return 29;
    return d[m - 1];
}

/** @brief Cantidad de dias desde el 01/01/0001 hasta la fecha (para restar fechas). */
int diasAbsolutos(Fecha f) {
    int a = f.anio - 1;
    int total = a * 365 + a / 4 - a / 100 + a / 400;
    for (int m = 1; m < f.mes; m++) total += diasEnMes(m, f.anio);
    return total + f.dia;
}

/** @brief Descarta lo que quede pendiente en la linea de entrada. */
void limpiarLinea() {
    cin.clear();
    cin.ignore(10000, '\n');
}

/** @brief Si la entrada se cerro (fin de archivo), termina el programa. */
void salirSiFin() {
    if (cin.eof()) {
        cout << endl << "Fin de la entrada. Cerrando el programa." << endl;
        exit(0);
    }
}

/** @brief Pide una nueva tarifa de multa por dia y la guarda si es valida.
 *  @param tarifa (por referencia) tarifa en soles por dia de retraso. */
void configurarTarifaMulta(float &tarifa) {
    float t;
    cout << "Tarifa actual: S/ ";
    mostrarMonto(tarifa);
    cout << endl;
    cout << "Nueva tarifa por dia de retraso: ";
    if (!(cin >> t)) {
        salirSiFin();
        limpiarLinea();
        cout << "Error: valor no numerico. Se mantiene la tarifa anterior." << endl;
        return;
    }
    cin.ignore(10000, '\n');
    if (t < 0) {
        cout << "Error: la tarifa no puede ser negativa. Se mantiene la tarifa anterior." << endl;
        return;
    }
    tarifa = t;
    cout << "Tarifa actualizada: S/ ";
    mostrarMonto(tarifa);
    cout << " por dia." << endl;
}

/** @brief Comprueba que una fecha exista (considera anios bisiestos).
 *  @param f fecha a validar (por valor).
 *  @return true si es valida. */
bool validarFecha(Fecha f) {
    if (f.anio < 1900 || f.anio > 2100) return false;
    if (f.mes < 1 || f.mes > 12) return false;
    return f.dia >= 1 && f.dia <= diasEnMes(f.mes, f.anio);
}

/** @brief Dias transcurridos desde f1 hasta f2.
 *  @return numero de dias (negativo si f2 es anterior a f1). */
int diferenciaDias(Fecha f1, Fecha f2) {
    return diasAbsolutos(f2) - diasAbsolutos(f1);
}

/** @brief Suma plazo dias a la fecha de salida.
 *  @param limite (por referencia) fecha resultante. */
void calcularFechaLimite(Fecha salida, int plazo, Fecha &limite) {
    limite = salida;
    for (int i = 0; i < plazo; i++) {
        limite.dia++;
        if (limite.dia > diasEnMes(limite.mes, limite.anio)) {
            limite.dia = 1;
            limite.mes++;
            if (limite.mes > 12) {
                limite.mes = 1;
                limite.anio++;
            }
        }
    }
}

/** @brief Multa = dias de retraso por tarifa (0 si no hay retraso). */
float calcularMulta(int diasRetraso, float tarifa) {
    if (diasRetraso <= 0) return 0;
    return diasRetraso * tarifa;
}

/** @brief Lee un entero dentro de un rango; repite hasta que sea valido.
 *  @return el numero leido. */
int leerEntero(const char mensaje[], int minimo, int maximo) {
    int n;
    while (true) {
        cout << mensaje;
        if (cin >> n) {
            cin.ignore(10000, '\n');
            if (n >= minimo && n <= maximo) return n;
            cout << "Error: ingrese un numero entre " << minimo << " y " << maximo << "." << endl;
        } else {
            salirSiFin();
            cout << "Error: ingrese un numero entero valido." << endl;
            limpiarLinea();
        }
    }
}

/** @brief Lee una linea de texto no vacia que quepa en destino.
 *  @param tam tamano del arreglo destino. */
void leerTexto(const char mensaje[], char destino[], int tam) {
    while (true) {
        cout << mensaje;
        cin.getline(destino, tam);
        if (cin.fail()) {
            salirSiFin();
            cout << "Error: texto demasiado largo (maximo " << tam - 1 << " caracteres)." << endl;
            limpiarLinea();
            continue;
        }
        int i = 0;
        while (destino[i] == ' ') i++;
        if (destino[i] == '\0') {
            salirSiFin();
            cout << "Error: el campo no puede estar vacio." << endl;
            continue;
        }
        return;
    }
}

/** @brief Pide una fecha (dd mm aaaa) y repite hasta que sea valida.
 *  @param f (por referencia) fecha leida. */
void leerFecha(const char mensaje[], Fecha &f) {
    while (true) {
        cout << mensaje << " (dd mm aaaa): ";
        if (cin >> f.dia >> f.mes >> f.anio) {
            cin.ignore(10000, '\n');
            if (validarFecha(f)) return;
            cout << "Fecha invalida, ingrese nuevamente." << endl;
        } else {
            salirSiFin();
            cout << "Error: escriba tres numeros: dd mm aaaa." << endl;
            limpiarLinea();
        }
    }
}

