import copy # para no repetir codigo y simula el siguiente movimento
import os # limpia la pantalla y luego actualiza os.system("cls" if os.name == "nt" else "clear")
import math # operaciones matematicas para calcular distancias y evaluar movimientos

# Diccionario que mapea teclas a desplazamientos en el tablero (fila, columna)
mover = {
            'W':[-1,0],# mueve hacia arriba fila -1
    'A':[0,-1],'S':[1,0],'D':[0,1]
}
#--------------------------------------------------
# FUNCIONES AUXILIARES
# QUE LINDA TE VES TRAPIANDO ESPERANCITA PERO... Te falto aqui
# LIMPIA pantalla
def limpiaPantalla():
    os.system("cls" if os.name == "nt" else "clear")
#------------------------------------------------------

# REGLAS DE DESPLASAMIENTO POR FILA Y COLUNNA ALTO Y ANCHO EVITA SALIR DEL LABERINTO
def moverEn(fila,columna,alto,ancho):
    aqui=[] # lista que almacena las posiciones válidas
    direccion = [(-1,0),(1,0),(0,-1),(0,1)] # Las cuatro direcciones ortogonales (arriba, abajo, izquierda, derecha)
    #            sube=w|baj=s|izq=a|der=d
    
    for dirFi,dirCo in direccion: # para recorrer cada direccion
        nuevaFila=fila+dirFi
        nuevaColumna=columna+dirCo
        # controla que no salga del labernito
        if 0<= nuevaFila < alto and 0 <= nuevaColumna < ancho: # Verificamos que la nueva celda esté dentro de los límites del tablero
            aqui.append((nuevaFila,nuevaColumna))# si pasa la verificacion agrega a la lista aqui
    return aqui

#-Calcula la distancia Manhattan entre dos posiciones en el tablero.----------------------------------------------------------------------------------------------
def calculaAcercamiento (posicion1,posicion2):
    distanciaMahattan=math.fabs(posicion1[0]-posicion2[0])+math.fabs(posicion1[1]-posicion2[1]) #distancia manhattan
    return distanciaMahattan

#Evalúa qué tan buena es la posición del ratón. Se basa en la distancia al gato y a la salida.
def evalua_posicion(situa):
    if situa['gato']==situa['raton']: # Si están en la misma celda -> gato gana
        return -1000 #Gana gato muy malo para el raton
    
    if situa['raton']==situa['salida']:
        return 1000 #Gana Raton escapando optimo
    
    distancia_gato= calculaAcercamiento(situa['gato'],situa['raton'])
    distancia_salida= calculaAcercamiento(situa['raton'],situa['salida'])
    
    seguir_presa = distancia_gato - distancia_salida # HEURISTICA 
    
    return seguir_presa
#-------------------------------------------------------------------------------------

# actualiza el movimiento en el juego con la nueva posicion del gato o raton
def simula_movimiento(situacion, jugador, nueva_posicion):
    nueva_situacion = copy.deepcopy(situacion) # copia la situacion para no modificar la original clonamos todo
    if jugador == 'G': # si es el gato
        nueva_situacion['gato'] = list(nueva_posicion)# actualiza la posicion de gato
    else:# si es el raton
        nueva_situacion['raton'] = list(nueva_posicion)
    return nueva_situacion

#████████████████████████████████████████ MINIMAX ███████████████████████████████████████ explora el árbol de juego hasta la profundidad indicada.
def minimax (estado,profundidad,turno_maximizador,alfa=float('-inf'), beta=float('inf')):
    # Caso base: se alcanzó la profundidad máxima o el gato ya atrapó al ratón
    if profundidad == 0 or estado ['gato'] == estado ['raton'] or estado ['raton']== estado['salida']:
        return evalua_posicion(estado)
    
    if turno_maximizador: # turno del raton
        mejor_para_raton = float('-inf') # inicia con un numero muy bajo
#        Obtenemos todos los movimientos legales del ratón
        movimiento= moverEn(
            estado['raton'][0],estado['raton'][1],# posicion del raton actual
            estado['alto'],estado['ancho']
            )
        for mov in movimiento:# # Calcula la distancia Manhattan entre dos posiciones en el tablero Simula el movimiento en un nuevo estado arbol de desicion
            nueva_situacion = simula_movimiento(estado,'R',mov)
        # Llamada recursiva: ahora turno del gato (minimizador)
            valor= minimax(nueva_situacion,profundidad -1,False,alfa,beta)# recursividad
            mejor_para_raton = max(mejor_para_raton,valor)# compara y guarda el nuevo valor maximo
            alfa = max(alfa,valor)
            if beta<= alfa: # #  evita evaluar ramas innecesarias
                break
        return mejor_para_raton

    else: # le toca al gato
        mejor_para_gato = float('inf')# empieza con el numero mas alto posible
        movimiento= moverEn(
            estado['gato'][0],estado['gato'][1],
            estado['alto'],estado['ancho']
        )
        for mov in movimiento:
            # Simulamos el movimiento del gato
            nueva_situacion = simula_movimiento(estado,'G',mov)
            # Llamada recursiva: ahora turno del ratón (maximizador)
            valor= minimax(nueva_situacion,profundidad -1,True,alfa,beta)# Llamada recursiva: ahora turno del ratón (maximizador)
            mejor_para_gato = min(mejor_para_gato,valor)# compara y guarda el nuevo valor minimo
            beta = min (beta,valor)
            if beta <=alfa:
                break
        return mejor_para_gato

def estrategia_mover_gato(estado):
    """
    Elige el mejor movimiento para el gato usando el algoritmo Minimax Devuelve la nueva posición (fila, columna) a la que debe moverse el gato.
    """
    mejorJugada=None
    menor_distancia_encontrada=float('inf')  # El gato busca minimizar, así que empezamos con infinito
    
    opciones = moverEn( # Movimientos legales reales del gato en el estado actual
        estado['gato'][0],estado['gato'][1],
        estado['alto'],estado['ancho']
    )
    
    for mov in opciones:
        #Guarda en esta variable que simula el movimiento del gato
        clon=simula_movimiento(estado,'G',mov)
    # evalúa posibles jugadas usando minimax con profundidad 7 (siguiente turno del ratón)
        chake_Gato=minimax(clon,7,True)
        # Si este movimiento produce un valor menor (mejor para el gato), lo guardamos
        if chake_Gato < menor_distancia_encontrada:
            menor_distancia_encontrada = chake_Gato
            mejorJugada = mov
    # Devolvemos la nueva posición (convertida a lista) o la posición actual si no hay movimientos
    return list(mejorJugada) if mejorJugada else estado['gato']

#--------Aqui vamos dibujando en pantalla como queda el laberinto-------------------------------------------------------------------

def crea_Laberinto(estado): 

# # Crea el tablero (matriz) donde se representa el juego lista de listas de 5x5 
    laberinto = []
    
    for _ in range(estado['alto']):      # bucle externo → filas
        fila = []
        for _ in range(estado['ancho']): # bucle interno → columnas
            fila.append(" · ")
        laberinto.append(fila)
        
    # la salida del laberinto lo representamos con un emoji de puerta
    laberinto[estado['salida'][0]][estado['salida'][1]]="🚪"
        
    gato  = estado['gato']
    raton = estado['raton']
    salida= estado['salida']

    
    if gato == raton:
        laberinto[gato[0]][gato[1]] = "☠️ "

    elif raton == salida:
        laberinto[raton[0]][raton[1]] = "✅"

    else:
        laberinto[gato[0]][gato[1]]  = "😼"
        laberinto[raton[0]][raton[1]] = "🐭"    
        
# Impresión del laberinto con formato
    print("|-----------------------------------------------------------------------------|")
    print("| ((( Laberinto The Dive ayuda al <<🐁 Raton>> a escapar del <<😼 Gato>> ))) |" )
    print("|-----------------------------------------------------------------------------|")
    print("                                  REGLAS                                       ")
    print("         Moverte hacia Arriba presiona la Tecla (W)")
    print("                                                 ⬆️")
    print("Moverte a la izquierda presiona  la tecla (A) ⬅️    ➡️ (D) Para moverte a la derecha")
    print("  Para moverte hacia abajo presiona la tecla (S) ⬇️")
    for fila in laberinto:
        print("".join(fila))
    print("|-----------------------------------------------------------------------------|")
    
def jugar():
# Diccionario con todas las cosas escenciales estado inicial posiciones y tamaño
    area= {
        'alto':5,
        'ancho':5,
        'gato':[0,4], # esquina superior derecha
        'raton':[4,4],# esquina inferior derecha
        'salida':[0,0],# esquina superior izquierda
    }
    mensaje = ""  # Para mostrar mensajes información
    limpiaPantalla()
    
    while True:
        limpiaPantalla()
        crea_Laberinto(area) #dibuja el tablero

        if mensaje:
            print(f"SISTEMA: {mensaje}")
            mensaje = "" #Lo limpiamos para la próxima iteración
        
        tecla= input("¿a donde vas?").upper()

# Procesar movimiento del ratón (jugador humano)
        if tecla in mover:
            # Calculamos la posición tentativa
            desplazamiento = mover[tecla]
            nueva_fila = area['raton'][0]+desplazamiento[0]
            nueva_columna = area['raton'][1]+desplazamiento[1]
            nueva_posicion = (nueva_fila,nueva_columna)

            # Obtenemos los movimientos legales del ratón desde su posición actual
            moverse = moverEn(
            area['raton'][0], area['raton'][1],
            area['alto'],area['ancho']
        )

            if nueva_posicion in moverse:
                area ['raton']=[nueva_fila,nueva_columna]
            else:
                mensaje="🚫 MOVIMIENTO FUERA DE LA ZONA 🙅"
                continue
        else:
            mensaje=" ⌨️ Tecla inválida, usa solo W, A, S o D  "

    # controlar que el raton llego a la salida y gano
        if area['raton']== area['salida']:
            limpiaPantalla()
            crea_Laberinto(area)
            print("Una Victoria Magistral Para La 🐭 Rata de 2 Patas")
            break #fin del juego
        
        # Turno del gato (IA)
        mejorJugada = estrategia_mover_gato(area)
        area['gato'] = mejorJugada

        # verifica si el gato atrapo al ratón (derrota del jugador)
        if area ['gato']== area ['raton']:
            limpiaPantalla()
            crea_Laberinto(area)
            print("🩸 FATALITY")
            print("El gato volador atrapo a la Raton ☠️")
            break  # Termina el juego

jugar()