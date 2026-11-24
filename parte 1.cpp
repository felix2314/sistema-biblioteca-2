#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

const int MAX_LIBROS = 500;
const int MAX_USUARIOS = 300;
const int MAX_PRESTAMOS = 1000;
const int MAX_RESERVAS = 300;
const int PLAZO_DIAS = 7;

struct Fecha {
    int dia;
    int mes;
    int anio;
};

struct Libro {
    char codigo[15];
    char titulo[100];
    char autor[60];
    int anio;
    int totales;
    int disponibles;
    int vecesPrestado;
    bool activo;
    Fecha fechaAlta;
};

struct Usuario {
    char codigo[15];
    char nombres[80];
    char tipo[15];
    bool sancionado;
};

struct Prestamo {
    int idPrestamo;
    char codLibro[15];
    char codUsuario[15];
    Fecha fechaSalida;
    Fecha fechaLimite;
    Fecha fechaDevolucion;
    bool devuelto;
    float multa;
};

struct Reserva {
    int idReserva;
    char codLibro[15];
    char codUsuario[15];
    Fecha fechaReserva;
    bool atendida;
};

int longitudTexto(const char t[]);
int compararTexto(const char a[], const char b[]);
void escribirCampoTexto(const char texto[], int ancho);
void escribirCampoNumero(int n, int ancho);
void salirSiFin();
int leerEntero(const char mensaje[], int minimo, int maximo);
void leerTexto(const char mensaje[], char destino[], int tam);
void leerFecha(const char mensaje[], Fecha &f);

int busquedaBinariaRecursiva(Libro libros[], int inicio, int fin, char cod[]);
void buscarLibrosPorAutor(Libro *ptrLibros, int cantL, char autor[]);
void buscarLibrosPorTitulo(Libro *ptrLibros, int cantL, char palabra[]);

void registrarUsuario(Usuario usuarios[], int &cantU);
void actualizarSancionUsuario(Usuario usuarios[], int cantU);
int buscarUsuarioPorCodigo(Usuario usuarios[], int cantU, char cod[]);

void configurarTarifaMulta(float &tarifa);
void registrarPrestamo(Prestamo prestamos[], int &cantP, Libro libros[], int cantL,
                       Usuario usuarios[], int cantU, Reserva reservas[], int cantR);
void registrarDevolucion(Prestamo prestamos[], int cantP, Libro libros[], int cantL,
                         Usuario usuarios[], int cantU, Reserva reservas[], int cantR, float tarifa);

void registrarReserva(Reserva reservas[], int &cantR, Libro libros[], int cantL, Usuario usuarios[], int cantU);
void consultarListaEspera(Reserva reservas[], int cantR, char codLibro[], Usuario usuarios[], int cantU);

void reporteLibrosMasPrestados(Libro *ptrLibros, int cantL);
void reportePrestamosVencidos(Prestamo prestamos[], int cantP, Fecha hoy);
void historialPrestamosUsuario(Prestamo prestamos[], int cantP, char codUsuario[]);

void cargarDatos(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                 Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR, int &ok);
void guardarDatos(Libro libros[], int cantL, Usuario usuarios[], int &cantU,
                  Prestamo prestamos[], int cantP, Reserva reservas[], int cantR, int &ok);
void crearRespaldo(int &ok);
void restaurarRespaldo(Libro libros[], int &cantL, Usuario usuarios[], int &cantU,
                       Prestamo prestamos[], int &cantP, Reserva reservas[], int &cantR, int &ok);

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

void registrarLibro(Libro libros[], int &cantL);
void bajaLogicaLibro(Libro libros[], int cantL);
void modificarLibro(Libro libros[], int cantL);
void consultarLibros(Libro libros[], int cantL);
void ordenarLibrosPorCodigo(Libro libros[], int cantL);

int main() {
    iniciarSistema();
    return 0;
}

void iniciarSistema() {
    Libro libros[MAX_LIBROS];          int cantLibros = 0;
    Usuario usuarios[MAX_USUARIOS];    int cantUsuarios = 0;
    Prestamo prestamos[MAX_PRESTAMOS]; int cantPrestamos = 0;
    Reserva reservas[MAX_RESERVAS];    int cantReservas = 0;
    float tarifaMulta = 1.50f;
    int opcion = 0;
    int ok = 0;

    cargarDatos(libros, cantLibros, usuarios, cantUsuarios,
                prestamos, cantPrestamos, reservas, cantReservas, ok);
    if (ok == 0) {
        cout << "No se pudo cargar la informacion. El programa se cierra para no perder datos." << endl;
        pausar();
        return;
    }

    do {
        limpiarPantalla();
        mostrarMenuPrincipal(opcion);
        switch (opcion) {
            case 1:
                menuLibros(libros, cantLibros);
                break;
            case 2:
                menuUsuarios(usuarios, cantUsuarios);
                break;
            case 3:
                menuBusquedas(libros, cantLibros);
                break;
            case 4:
                menuPrestamos(prestamos, cantPrestamos, libros, cantLibros,
                              usuarios, cantUsuarios, reservas, cantReservas, tarifaMulta);
                break;
            case 5:
                menuReportes(libros, cantLibros, prestamos, cantPrestamos, usuarios, cantUsuarios);
                break;
            case 6:
                menuArchivos(libros, cantLibros, usuarios, cantUsuarios,
                             prestamos, cantPrestamos, reservas, cantReservas);
                break;
            case 7:
                configurarTarifaMulta(tarifaMulta);
                pausar();
                break;
            case 8:
                menuReservas(reservas, cantReservas, libros, cantLibros, usuarios, cantUsuarios);
                break;
            case 0:
                break;
        }
    } while (opcion != 0);

    guardarDatos(libros, cantLibros, usuarios, cantUsuarios,
                 prestamos, cantPrestamos, reservas, cantReservas, ok);
    if (ok == 1) {
        cout << "Datos guardados. Hasta pronto." << endl;
    }
}

void limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    cout << "Presione Enter para continuar...";
    cin.get();
    salirSiFin();
}

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
        cout << "1. Registrar libro" << endl;
        cout << "2. Modificar libro" << endl;
        cout << "3. Dar de baja libro" << endl;
        cout << "4. Consultar libros" << endl;
        cout << "0. Volver" << endl;
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
        cout << "1. Registrar usuario" << endl;
        cout << "2. Actualizar sancion" << endl;
        cout << "0. Volver" << endl;
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
        cout << "1. Por codigo" << endl;
        cout << "2. Por autor" << endl;
        cout << "3. Por palabra del titulo" << endl;
        cout << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            leerTexto("Codigo del libro: ", texto, 15);
            int pos = busquedaBinariaRecursiva(libros, 0, cantL - 1, texto);
            if (pos == -1) {
                cout << "Libro no encontrado." << endl;
            } else if (!libros[pos].activo) {
                cout << "El libro existe pero esta dado de baja." << endl;
            } else {
                cout << libros[pos].codigo << " | " << libros[pos].titulo
                     << " | " << libros[pos].autor << " | " << libros[pos].anio
                     << " | disponibles " << libros[pos].disponibles << "/" << libros[pos].totales
                     << " (posicion " << pos << ")" << endl;
            }
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
        cout << "1. Registrar prestamo" << endl;
        cout << "2. Registrar devolucion" << endl;
        cout << "0. Volver" << endl;
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
        cout << "1. Libros mas prestados" << endl;
        cout << "2. Prestamos vencidos" << endl;
        cout << "3. Historial por usuario" << endl;
        cout << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            reporteLibrosMasPrestados(libros, cantL);
        } else if (op == 2) {
            leerFecha("Fecha actual", hoy);
            reportePrestamosVencidos(prestamos, cantP, hoy);
        } else if (op == 3) {
            leerTexto("Codigo del usuario: ", cod, 15);
            if (buscarUsuarioPorCodigo(usuarios, cantU, cod) == -1) {
                cout << "Error: el usuario no esta registrado." << endl;
            } else {
                historialPrestamosUsuario(prestamos, cantP, cod);
            }
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
        cout << "1. Guardar datos" << endl;
        cout << "2. Crear copia de seguridad" << endl;
        cout << "3. Restaurar copia de seguridad" << endl;
        cout << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 3);
        if (op == 1) {
            guardarDatos(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            if (ok == 1) cout << "Datos guardados correctamente." << endl;
        } else if (op == 2) {
            guardarDatos(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            if (ok == 1) crearRespaldo(ok);
        } else if (op == 3) {
            cout << "Esto reemplaza los datos actuales por los de la copia de seguridad." << endl;
            if (leerEntero("Continuar? (1 = Si, 0 = No): ", 0, 1) == 1) {
                restaurarRespaldo(libros, cantL, usuarios, cantU, prestamos, cantP, reservas, cantR, ok);
            } else {
                cout << "Operacion cancelada." << endl;
            }
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
        cout << "1. Registrar reserva" << endl;
        cout << "2. Consultar lista de espera" << endl;
        cout << "0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 2);
        if (op == 1) {
            registrarReserva(reservas, cantR, libros, cantL, usuarios, cantU);
        } else if (op == 2) {
            leerTexto("Codigo del libro: ", cod, 15);
            if (busquedaBinariaRecursiva(libros, 0, cantL - 1, cod) == -1) {
                cout << "Error: el libro no existe." << endl;
            } else {
                consultarListaEspera(reservas, cantR, cod, usuarios, cantU);
                cout << endl;
            }
        }
        if (op != 0) pausar();
    } while (op != 0);
}

void registrarLibro(Libro libros[], int &cantL) {
    if (cantL >= MAX_LIBROS) {
        cout << "Error: se alcanzo la capacidad maxima de libros." << endl;
        return;
    }

    Libro nuevo = {};
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
        int nuevoTotal = leerEntero("Nuevo total de ejemplares: ", 1, 1000);
        int prestados = libros[pos].totales - libros[pos].disponibles;
        if (nuevoTotal < prestados) {
            cout << "Error: hay " << prestados
                 << " ejemplares prestados; el total no puede ser menor. No se modifico." << endl;
            return;
        }
        libros[pos].totales = nuevoTotal;
        libros[pos].disponibles = nuevoTotal - prestados;
    }
    cout << "Libro modificado correctamente." << endl;
}

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
    if (n == 0) {
        cout << "No hay libros registrados." << endl;
    }
}

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
