#pragma once
#include <Arduino.h>
#include <queue>

// Photodiodes Sensing Pins:
#define P0 A0
#define P1 A1
#define P2 A2
#define P3 A3
#define P4 A4
#define P5 A5
#define P6 A6
#define P7 A7

// LED Control Pin
#define LED 9
#define LED_ON_TIME 50 //ms

// Data structure
struct HeliosData
{
    uint16_t p0;
    uint16_t p1;
    uint16_t p2;
    uint16_t p3;
};

union HeliosData_union
{
   HeliosData data;   
   char data_bytes[sizeof(data)];
};

class MovingAverage {
public:
    // Constructor que toma el tamaño de la ventana como argumento.
    // El tamaño de la ventana se almacena como un valor uint8_t.
    MovingAverage(uint8_t window_size): window_size_(window_size) {}

    // Agrega un valor a la cola de datos y actualiza la suma.
    void add(uint16_t val) {
        // Añade el valor a la cola de datos.
        data_.push(val);

        // Si la cola de datos tiene más elementos que el tamaño de la ventana,
        // se elimina el elemento más antiguo de la cola de datos y se actualiza la suma.
        if (data_.size() > window_size_) {
            sum_ -= data_.front();
            data_.pop();
        }

        // Se agrega el nuevo valor a la suma total.
        sum_ += val;
    }

    // Calcula y devuelve la media móvil actual.
    uint16_t get_average() const {
        // Si la cola de datos está vacía, devuelve 0.
        if (data_.empty()) {
            return 0;
        } else {
            // Calcula la media móvil como la suma total dividida por el número de elementos en la cola de datos.
            return sum_ / static_cast<uint16_t>(data_.size());
        }
    }

private:
    // Tamaño de la ventana para el cálculo de la media móvil.
    // Se almacena como un valor uint8_t.
    uint8_t window_size_;

    // Cola de datos que almacena los últimos valores para el cálculo de la media móvil.
    std::queue<uint16_t> data_;

    // Suma total de los valores en la cola de datos.
    // Se actualiza cada vez que se agrega un nuevo valor.
    uint16_t sum_ = 0;
};


// Communication Functions
void initCommunications();
boolean commandAvailable();
byte readCommand();

// Sensing Functions
void heliosInit();
HeliosData readSensors();