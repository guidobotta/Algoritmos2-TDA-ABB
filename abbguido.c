#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct abb_nodo abb_nodo_t;

struct abb_nodo{
    const char* clave;
    void* dato;
    abb_nodo_t *izq;
    abb_nodo_t *der;
};

struct abb{
    abb_nodo_t *raiz;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir_dato;
    size_t cantidad;
};

/* ******************************************************************
 *                      PRIMITIVAS DEL NODO
 * *****************************************************************/

abb_nodo_t* crear_nodo(const char* clave, void* dato){
    abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
    if(!nodo) return NULL;
    nodo->dato = dato;
    nodo->clave = clave;
    nodo->izq = NULL;
    nodo->der = NULL;

    return nodo;
}

abb_nodo_t* buscar_hijo(abb_nodo_t* actual, abb_nodo_t** padre, const char* clave, abb_comparar_clave_t cmp){
    if(!actual) return NULL;

    int comp = cmp(actual->clave, clave);
    if(!cmp) return actual;

    *padre = actual;
    if(comp < 0) return buscar_hijo(actual->der, padre, clave, cmp);
    return buscar_hijo(actual->izq, padre, clave, cmp);
}

/* ******************************************************************
 *                      PRIMITIVAS DEL ABB
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
    if(!cmp) return NULL;
    abb_t *abb = malloc(sizeof(abb_t));
    if(!abb) return NULL;

    abb->cmp = cmp;
    abb->destruir_dato = destruir_dato;
    abb->raiz = NULL;
    abb->cantidad = 0;

    return abb;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
    abb_nodo_t* padre = NULL;
    abb_nodo_t* actual = buscar_hijo(arbol->raiz, &padre, clave, arbol->cmp);

    if(!actual){
        char* copia_clave = malloc(sizeof(char)*(strlen(clave)+1));
        strcpy(copia_clave, clave);

        abb_nodo_t* nodo_b = crear_nodo(copia_clave, dato);
        if(!nodo_b) return false;

        if (!padre){
            arbol->raiz = nodo_b;
        } else if ((arbol->cmp(clave, padre->clave)) > 0){
            padre->der = nodo_b;
        } else {
            padre->izq = nodo_b;
        }
    }
    else{
        if(arbol->destruir_dato) arbol->destruir_dato(actual->dato);
        actual->dato = dato;
    }

    arbol->cantidad ++;
    return true;
}

void *abb_borrar(abb_t *arbol, const char *clave);

void *abb_obtener(const abb_t *arbol, const char *clave){
    abb_nodo_t* padre = NULL;
    abb_nodo_t* hijo = buscar_hijo(arbol->raiz, &padre, clave, arbol->cmp);
    if (!hijo) return NULL;

    return hijo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
    abb_nodo_t* padre = NULL;
    abb_nodo_t* hijo = buscar_hijo(arbol->raiz, &padre, clave, arbol->cmp);

    return hijo;
}

size_t abb_cantidad(abb_t *arbol){
    return arbol->cantidad;
}

void destruir_arbol(abb_nodo_t *actual, abb_destruir_dato_t destruir_dato){
    if(!actual) return;
    if(destruir_dato) destruir_dato(actual->dato);
    free((char*)actual->clave);
    destruir_arbol(actual->izq, destruir_dato);
    destruir_arbol(actual->der, destruir_dato);
    free(actual);
}

void abb_destruir(abb_t* arbol){
    if(!arbol) return;
    destruir_arbol(arbol->raiz, arbol->destruir_dato);
    free(arbol);
}