#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"
#include "pila.h"

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
    if(!comp) return actual;

    *padre = actual;
    if(comp < 0) return buscar_hijo(actual->der, padre, clave, cmp);
    return buscar_hijo(actual->izq, padre, clave, cmp);
}

/* ******************************************************************
 *                      FUNCIONES PRIVADAS
 * *****************************************************************/

//Borrar para cero o un hijo
void _borrar_(abb_t *arbol, abb_nodo_t* hijo, abb_nodo_t* padre, abb_nodo_t* nuevo_hijo){
    if(padre){
        if ((arbol->cmp(hijo->clave, padre->clave)) > 0){
            padre->der = nuevo_hijo;
        } else {
            padre->izq = nuevo_hijo;
        }
    }else{
        arbol->raiz = nuevo_hijo;
    }
    if (arbol->destruir_dato) arbol->destruir_dato(hijo->dato);
    free((char*)hijo->clave);
    free(hijo);
}

abb_nodo_t* buscar_reemplazante(abb_nodo_t* actual){
    if(!actual->izq) return actual;
    return buscar_reemplazante(actual->izq);
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
        arbol->cantidad ++;
    }
    else{
        if(arbol->destruir_dato) arbol->destruir_dato(actual->dato);
        actual->dato = dato;
    }

    return true;
}

void *abb_borrar(abb_t *arbol, const char *clave){
    abb_nodo_t* padre = NULL;
    abb_nodo_t* hijo = buscar_hijo(arbol->raiz, &padre, clave, arbol->cmp);
    if (!hijo) return NULL;

    void* dato = hijo->dato;

    if (hijo->izq){
        if (hijo->der){ //CON DOS HIJOS
            abb_nodo_t* reemplazante = buscar_reemplazante(hijo->der);
            char* copia_clave = malloc(sizeof(char)*(strlen(reemplazante->clave)+1));
            strcpy(copia_clave, reemplazante->clave);
            void* copia_dato = abb_borrar(arbol, reemplazante->clave);
            hijo->clave = copia_clave;
            hijo->dato = copia_dato;
            return dato;
        }
        else _borrar_(arbol, hijo, padre, hijo->izq); //CON UN HIJO IZQUIERDO
    } else if(hijo->der) _borrar_(arbol, hijo, padre, hijo->der); //CON UN HIJO DERECHO
    else _borrar_(arbol, hijo, padre, NULL); //SIN HIJOS

    arbol->cantidad --;
    return dato;
}

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

/* ******************************************************************
 *                PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

void in_order(abb_nodo_t* actual, bool visitar(const char *, void *, void*), void *extra){
    if(!actual) return;
    in_order(actual->izq, visitar, extra);
    if(!visitar(actual->clave, actual->dato, extra)) return;
    in_order(actual->der, visitar, extra);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
    return in_order(arbol->raiz, visitar, extra);
}

/* ******************************************************************
 *                PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

typedef struct abb_iter abb_iter_t;

struct abb_iter{
    pila_t *pila;
    abb_t *arbol;
};

void apilar_izquierdos(abb_iter_t *iter, abb_nodo_t *actual){
    if(!actual) return;
    pila_apilar(iter->pila, actual); //Chequeamos si falla?
    return apilar_izquierdos(iter, actual->izq);
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
    abb_iter_t *iter = malloc(sizeof(abb_iter_t));
    if(!iter) return NULL;
    pila_t *pila = pila_crear();
    if(!pila){
        free(iter);
        return NULL;
    }
    iter->arbol = (abb_t*)arbol;
    iter->pila = pila;

    apilar_izquierdos(iter, iter->arbol->raiz);

    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
    if(pila_esta_vacia(iter->pila)) return false;
    abb_nodo_t* ultimo = pila_desapilar(iter->pila);
    apilar_izquierdos(iter, ultimo->der);
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
    abb_nodo_t *actual = pila_ver_tope(iter->pila);
    if(!actual) return NULL;
    return actual->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
    pila_destruir(iter->pila);
    free(iter);
}
