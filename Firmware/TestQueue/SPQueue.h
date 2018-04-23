#ifndef SPQUEUE_h
#define SPQUEUE_h

#include <Arduino.h>

class SPQueue {
public:
    SPQueue(int size);
    ~SPQueue();
    boolean push(int spX, int spY); // Nuevo vector a la cola
    int* pop();                     // Quitar vector de la cola y retornar
    int itemCount();                // Cantidad de elementos en la cola

private:
    int **setpoints;        // Cola de vectores
    int tail;               // Puntero al primero en salir
    int head;               // Puntero al ultimo en entrar
    int items;              // Cantidad de vectores en la cola
    int size;               // Espacio de memoria asignado
};

SPQueue::SPQueue(int size)
// Constructor de la clase
{
  // Asignar memoria para una matriz de 2 x size.
  this->setpoints = (int**) malloc (sizeof (int*) * size);
  for(int i = 0; i < size; i++)
    this->setpoints[i] = (int*) malloc (sizeof (int) * 2);
  // Iniciar atributos
  this->tail = 0; // First out
  this->head = 0; // First in
  this->items = 0; // Cantidad de items en la cola
  this->size = size; // Espacio de memoria asignado
}

SPQueue::~SPQueue()
// Destructor de la clase
{
  // Liberar memoria ocupada por la cola
  for(int i = 0; i < size; i++)
    free(this->setpoints[i]);
  free(this->setpoints);
}

boolean SPQueue::push(int spX, int spY)
{
    if(items < size) // Verificar que haya espacio para mas elementos
    {
      setpoints[head][0] = spX;
      setpoints[head][1] = spY;
      head++;
      items++;
      return true;
    }
    else
      return false;
}

int* SPQueue::pop()
{
  if(items > 0) // Si todavia hay elementos en la cola
  {
    tail++; // Quitar primero
    items--; // Decrementar numero de items
    return setpoints[tail-1]; // Retornar puntero
  }
  else
    return 0;
}

int SPQueue::itemCount()
{ 
  return items;
}


#endif //SPQUEUE
