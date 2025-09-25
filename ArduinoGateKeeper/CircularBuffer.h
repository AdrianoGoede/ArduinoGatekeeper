#include <algorithm>
#ifndef __ARDUINO_GATEKEEPER_CIRC_BUFFER__
#define __ARDUINO_GATEKEEPER_CIRC_BUFFER__

#include <Arduino.h>

template<typename T>
class CircularBuffer {
private:
  const size_t maxSize;
  T* buffer;
  size_t head = 0, tail = 0, count = 0;
public:
  CircularBuffer(size_t maxSize) : maxSize(maxSize) {
    this->buffer = new T[maxSize];
  };

  ~CircularBuffer() {
    delete [] this->buffer;
  };

  bool isEmpty() const { return (this->count == 0); }

  bool isFull() const { return (this->count == this->maxSize); }
  
  void push(const T& element) {
    this->buffer[this->head] = element;
    this->head = ((this->head + 1) % this->maxSize);
    if (this->isFull())
      this->tail = ((this->tail + 1) % this->maxSize);
    else
      this->count++;
  }

  T pop() {
    if (this->isEmpty()) return T{};
      
    T element = this->buffer[this->tail];
    this->tail = ((this->tail + 1) % this->maxSize);
    this->count--;
    return element;
  }
};

#endif