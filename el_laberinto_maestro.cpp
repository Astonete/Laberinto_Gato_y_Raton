#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <queue>// para bfs
#include <random>
#include <set>
#include <utility>
#include <vector>
#include <thread>

using namespace std;
using namespace std::chrono;

// alias para facilitar
using Tablero = vector<vector<char>>; // matriz de caracter
using Coordenada = pair<int, int>;    // posicion (fila,columna en tablero)

// constantes para el dibujo en tablero
const char PARED ='#';
const char PASILLO = ' ';
const char ENTRADA = 'I'; // inicio
const char SALIDA = 'S';  // salida
const char HUELLA = '.';

// desplazamiento Arriba abajo
const int FILA[] = {-1, 1, 0, 0};
const int COL[] = {0, 0, -1, 1};

void mostrar_laberinto(const Tablero &tablero, const string &titulo);

// creacion del laberinto dfs con backtracking
// valida si se puede hacer el movimiento teneindo en cuenta las filas y columnas
bool celda_valida(int fila, int col, int total_filas, int total_cols)
{ // en vez de total fila pasar laberinto.size experimentar
    if (fila > 0 && fila < total_filas - 1 && col > 0 && col < total_cols - 1)
    {
        return true;
    }
    return false;
}

// dfs recursivo abre camino desde la celda (fila columna)
void dfs_abrir_caminos(Tablero &tablero, int fila, int col, mt19937 &rng)
{
    int total_fila = tablero.size();
    int total_col = tablero[0].size();

    vector<Coordenada> vecinos = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};

    // mezcla para que sea aleatorio
    shuffle(vecinos.begin(), vecinos.end(), rng);

    for (auto [dir_fila, dir_col] : vecinos)
    {
        int nueva_fila = fila + dir_fila;
        int nueva_col = col + dir_col;
        // celdas que dentro y sean pared
        if (celda_valida(nueva_fila, nueva_col, total_fila, total_col) && tablero[nueva_fila][nueva_col] == PARED)
        { // romper la pared
            tablero[fila + dir_fila / 2][col + dir_col / 2] = PASILLO;
            // marcar la celda nueva como pasillo
            tablero[nueva_fila][nueva_col] = PASILLO;
            // seguir explorando desde celda (recursion = backtracking)
            dfs_abrir_caminos(tablero, nueva_fila, nueva_col, rng);
        }
    }
    // cuando no hay vecinos sin visitar, la funcion regresa al paso anterior (backtrack)
}

// inicializa el tablero y llamar al dfs para genera el laberinto
void crear_laberinto(Tablero &tablero)
{ // llena todo con paredes
    int total_filas = tablero.size();
    int total_cols = tablero[0].size();

    for (int itfila = 0; itfila < total_filas; itfila++)
    {
        for (int itcol = 0; itcol < total_cols; itcol++)
        {
            tablero[itfila][itcol] = PARED;
        }
    }

    mt19937 rng(random_device{}()); // generador numero aleatorio()
    // marca entrada y salida en esquina opuesta
    tablero[1][1] = PASILLO;
    dfs_abrir_caminos(tablero, 1, 1, rng);

    tablero[1][1] = ENTRADA;
    tablero[total_filas - 2][total_cols - 2] = SALIDA;
}

//// Solucion del tablero (BFS)
bool Transitable(const Tablero &tablero, int fila, int col)
{
    int total_filas = tablero.size();
    int total_cols = tablero[0].size();

    if (fila >= 0 && fila < total_filas && col >= 0 && col < total_cols && tablero[fila][col] != PARED)
    {
        return true;
    }
    return false;
}

void marca_camino_solucion(Tablero &tablero, const vector<vector<Coordenada>> &padre, Coordenada fin)
{
    Coordenada sin_padre = {-1, -1}; // valor sin padre entrada
    Coordenada actual = fin;

    while (padre[actual.first][actual.second] != sin_padre)
    {
        // solo marcamos los que no son ni entrada ni salida
        char &celda = tablero[actual.first][actual.second];
        if (celda != ENTRADA && celda != SALIDA)
            celda = HUELLA;
        // retrocede al padre de donde viene
        actual = padre[actual.first][actual.second];
    }
}

// bfs principal: busca el camino desde la entrada hasta la salida
bool resolver_laberinto_bfs(Tablero &tablero)
{
    int total_filas = tablero.size();
    int total_colmn = tablero[0].size();

    Coordenada inicio = {1, 1};
    Coordenada fin = {total_filas - 2, total_colmn - 2};

    // registro de celdas ya visitadas para no procesar 2 veces
    vector<vector<bool>> visitado(total_filas, vector<bool>(total_colmn, false));

    // Matriz de padres: indica desde qué celda llegamos a cada posición
    // Se inicializa con {-1,-1} que significa "aún no visitado"
    vector<vector<Coordenada>> padres(total_filas, vector<Coordenada>(total_colmn, {-1, -1}));

    // cola bfs: contiene las celdas por explorar primero las mas cercanas
    queue<Coordenada> cola;
    cola.push(inicio);
    visitado[inicio.first][inicio.second] = true;

    while (!cola.empty())
    {
        auto [fila, colu] = cola.front();
        cola.pop();

        // si llegamos a la salidas recostruir y maracar con huellas
        if (fila == fin.first && colu == fin.second)
        {
            marca_camino_solucion(tablero, padres, fin);
            return true;
        }

        // explorar los 4 vecinos(arriba, abajo, izquierda, derecha)
        for (int repite = 0; repite < 4; repite++)
        {
            int agre_fila = fila + FILA[repite];
            int agre_colu = colu + COL[repite];

            if (Transitable(tablero, agre_fila, agre_colu) && !visitado[agre_fila][agre_colu])
            {
                visitado[agre_fila][agre_colu] = true;
                padres[agre_fila][agre_colu] = {fila, colu}; // guarda de donde viene
                cola.push({agre_fila, agre_colu});
            }
        }
    }
    // Si la cola se vacía sin encontrar la salida no hay solución
    return false;
}

bool animar_bfs(Tablero & tablero)
{
    int total_filas=tablero.size();
    int total_colmn=tablero[0].size();

    Coordenada inicio = {1,1};
    Coordenada fin = {total_filas - 2, total_colmn - 2};

    vector<vector<bool>> visitado(total_filas, vector<bool>(total_colmn, false));
    vector<vector<Coordenada>> padres(total_filas, vector<Coordenada>(total_colmn, {-1,-1}));

    queue<Coordenada> cola;
    cola.push(inicio);
    visitado[inicio.first][inicio.second]=true;

    while (!cola.empty())
    {
        auto [fila, colu] = cola.front();
        cola.pop();

        if (fila == fin.first && colu == fin.second)
        {
            marca_camino_solucion(tablero, padres, fin);
            system("cls"); // limpiar consola (Windows)
            mostrar_laberinto(tablero,"Laberinto Resuelto");
            return true;
        }
        // marcar celda explorada
        if (tablero [fila][colu]!= ENTRADA && tablero[fila][colu]!= SALIDA)
            tablero[fila][colu]= HUELLA;

            //redibujar y pausar para la animacion
        system("cls"); //limpieza
        mostrar_laberinto(tablero,"Resolviendo laberinto en proceso bfs ...");
        this_thread::sleep_for(chrono::milliseconds(100)); // pausa para animacion

        for (int repite = 0; repite < 4; repite++)
        {
            int agre_fila = fila + FILA[repite];
            int agre_colu = colu + COL[repite];

            if (Transitable(tablero, agre_fila, agre_colu) && !visitado[agre_fila][agre_colu])
            {
                visitado[agre_fila][agre_colu] = true;
                padres[agre_fila][agre_colu] = {fila, colu};
                cola.push({agre_fila, agre_colu});
                // Animacion: marcar la celda actual como visitada
                if (tablero[agre_fila][agre_colu] != ENTRADA && tablero[agre_fila][agre_colu] != SALIDA)
                    tablero[agre_fila][agre_colu] = HUELLA;
                mostrar_laberinto(tablero,"BFS en Proceso...");
                this_thread::sleep_for(chrono::milliseconds(100)); // pausa para animacion
            }
        }
    }
    cout<<"No se encontro solucion (esto no deberia Ocurrir).\n";
    return false;
}
// imprime el laberinto por caracterer con titulo en consola
void mostrar_laberinto(const Tablero &tablero, const string &titulo)
{
    cout << "\n\n=== ___" << titulo << "___ ===\n\n";

    for (const auto &fila : tablero)
    {
        for (char celda : fila){
            cout << celda;
        }
        cout << "\n";
    }
    cout << "\n";
}

// Funcion Principal
int main()
{
    int num_fi;
    int num_co;

    cout<<"|=-=-= ( Laberinto ) =-=-=|\n";
    cout<<"-> Cuantas Filas va a Tener   (Minimo 10): ";
    cin>>num_fi;
    cout<<"-> Cuantas columnas columnas (Minimo 10): ";
    cin>>num_co;

    // Validar tamaño minimo
    if (num_fi < 10 || num_co < 10){
        cout<< "Error el tamaño del laberinto debe ser almenos de 10x10 celdas.\n";
        return 1;
    }
    //calcula dimensiones reales de la matriz
    int total_filas=2*num_fi+1;
    int total_colum=2*num_co+1;

    cout<<"\n-> Tamaño: "<<num_fi<<" X "<< num_co<<" celdas\n";
    cout<<"-> Tamaño Real: "<<total_filas<<" X "<< total_colum<<" (incluye paredes)\n";

    //crea el tablero vacio
    Tablero laberinto(total_filas,vector<char>(total_colum,PARED));

//--Generar el laberinto con DFS
    auto t_inicio_dfs= high_resolution_clock::now();

    crear_laberinto(laberinto);

    auto t_fin_dfs=high_resolution_clock::now();
    auto tiempo_dfs= duration_cast<microseconds>(t_fin_dfs - t_inicio_dfs);

    mostrar_laberinto(laberinto,"Laberinto Generado (^_^) /\n");
    cout<<"Tiempo de generacion (DFS): "<<tiempo_dfs.count()<<" Milisegundos\n";

// resolver el laberito con BFS
    auto t_inicio_bfs= high_resolution_clock::now();

    bool encontrado = animar_bfs(laberinto);

    auto t_fin_bfs = high_resolution_clock::now();
    auto tiempo_bfs= duration_cast<microseconds>(t_fin_bfs - t_inicio_bfs);

    if (encontrado)
    {
        mostrar_laberinto(laberinto,"Laberinto Resuelto (camino = '.')");
    }
    else{
        cout<<"No se encontro solucion (esto no deberia Ocurrir).\n";
    }
    cout<<"Tiempo de resolucion (BFS): " << tiempo_bfs.count() << " Microsegundos\n";
    return 0;
}
