#include <iostream>
using namespace std;

struct Fecha {
    int dia;
    int mes;
    int anio;
};

bool esBisiesto(int a) {
    return (a % 4 == 0 && a % 100 != 0) || a % 400 == 0;
}

int diasEnMes(int m, int a) {
    int d[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && esBisiesto(a)) return 29;
    return d[m - 1];
}

int diasAbsolutos(Fecha f) {
    int a = f.anio - 1;
    int total = a * 365 + a / 4 - a / 100 + a / 400;
    for (int m = 1; m < f.mes; m++) total += diasEnMes(m, f.anio);
    return total + f.dia;
}

bool validarFecha(Fecha f) {
    if (f.anio < 1900 || f.anio > 2100) return false;
    if (f.mes < 1 || f.mes > 12) return false;
    return f.dia >= 1 && f.dia <= diasEnMes(f.mes, f.anio);
}

int diferenciaDias(Fecha f1, Fecha f2) {
    return diasAbsolutos(f2) - diasAbsolutos(f1);
}

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

int main() {
    cout << "CP-09 validarFecha:" << endl;
    Fecha f1 = {29, 2, 2024}, f2 = {29, 2, 2025}, f3 = {31, 4, 2026}, f4 = {15, 13, 2026};
    cout << "29/02/2024 -> " << (validarFecha(f1) ? "valida" : "invalida") << endl;
    cout << "29/02/2025 -> " << (validarFecha(f2) ? "valida" : "invalida") << endl;
    cout << "31/04/2026 -> " << (validarFecha(f3) ? "valida" : "invalida") << endl;
    cout << "15/13/2026 -> " << (validarFecha(f4) ? "valida" : "invalida") << endl;

    cout << "CP-10 diferenciaDias:" << endl;
    Fecha a1 = {28, 2, 2024}, a2 = {1, 3, 2024};
    Fecha b1 = {28, 2, 2026}, b2 = {1, 3, 2026};
    Fecha c1 = {30, 12, 2025}, c2 = {2, 1, 2026};
    cout << "28/02/2024 a 01/03/2024 -> " << diferenciaDias(a1, a2) << " dias" << endl;
    cout << "28/02/2026 a 01/03/2026 -> " << diferenciaDias(b1, b2) << " dias" << endl;
    cout << "30/12/2025 a 02/01/2026 -> " << diferenciaDias(c1, c2) << " dias" << endl;
    cout << "01/03/2024 a 28/02/2024 -> " << diferenciaDias(a2, a1) << " dias (orden inverso)" << endl;

    Fecha salida = {28, 12, 2026}, limite;
    calcularFechaLimite(salida, 7, limite);
    cout << "CP-11 calcularFechaLimite: salida 28/12/2026 + 7 dias -> "
         << (limite.dia < 10 ? "0" : "") << limite.dia << "/"
         << (limite.mes < 10 ? "0" : "") << limite.mes << "/" << limite.anio << endl;
    return 0;
}
