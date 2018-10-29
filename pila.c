#include "pila.h"
#include <stdlib.h>
#include <stdio.h>

#define TAMANIO_PILA 10

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t* pila_crear(void){  
      
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL){
        return NULL;
    }

    pila->datos = malloc(sizeof(void*)*TAMANIO_PILA);
    if (pila->datos == NULL){
        free(pila);
        return NULL;
    }
    
    pila->cantidad = 0;
    pila->capacidad = TAMANIO_PILA;
    return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
    return (pila->cantidad == 0);
}

void* pila_ver_tope(const pila_t *pila){
    if (pila_esta_vacia(pila)) return NULL;
    return pila->datos[pila->cantidad -1];
}

// Redimensiona la pila diez veces menos que el número indicado por parámetro
// Pre: la pila fue creada
// Post: devuelve el puntero de la pila redimensionada
bool _redimensionar_pila(pila_t *pila, int num){
    void** nuevos_datos = realloc(pila->datos, (pila->capacidad * num / 10)*sizeof(void*));
    if (!nuevos_datos) return false;
    pila->datos = nuevos_datos;
    pila->capacidad = pila->capacidad * num / 10;
    return true;
}

bool pila_apilar(pila_t *pila, void* valor){
    //Si está completa hay que redimensionar
    if (pila->capacidad == pila->cantidad){
        if (!_redimensionar_pila(pila,20)) return false;
    }

    pila->datos[pila->cantidad] = valor;
    pila->cantidad += 1;
    return true;
}

void* pila_desapilar(pila_t *pila){
    if (pila_esta_vacia(pila)) return NULL;
    
    void* elemento = pila->datos[pila->cantidad -1];
    pila->cantidad -= 1;

    //Si se usa un 25% de capacidad, hay que redimensionar
    if((pila->capacidad > TAMANIO_PILA) && (pila->capacidad == (pila->cantidad*4))){
        if (!_redimensionar_pila(pila,5)) return false;
    }

    return elemento;
}