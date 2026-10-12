#include "../include/DynamicArray.h"

#include <iostream>
#include <new>       // std::bad_alloc, std::nothrow
#include <stdexcept> // std::out_of_range, std::invalid_argument
#include <string>

int* DynamicArray::allocate(std::size_t count) {
    if (count == 0) {
        return nullptr;
    }
    // Обычный new сам бросает std::bad_alloc. Здесь используется new(std::nothrow),
    // который при нехватке памяти возвращает nullptr (как malloc в C), —
    // и тогда исключение выбрасывается явно, чтобы проверка была видна в коде.
    int* memory = new (std::nothrow) int[count](); // () — заполнение нулями
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

DynamicArray::DynamicArray(std::size_t size)
    : data_(allocate(size)),
      size_(size) {
}

DynamicArray::~DynamicArray() {
    delete[] data_;
}

DynamicArray::DynamicArray(const DynamicArray& other)
    : data_(allocate(other.size_)),
      size_(other.size_) {
    // Копируем сами значения, а не указатель: у копии своя память.
    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}

std::size_t DynamicArray::size() const {
    return size_;
}

void DynamicArray::checkValue(int value, const char* operationName) {
    if (value < MIN_VALUE || value > MAX_VALUE) {
        throw std::invalid_argument(
            std::string(operationName) + ": значение " + std::to_string(value) +
            " вне промежутка [" + std::to_string(MIN_VALUE) + "; " +
            std::to_string(MAX_VALUE) + "]");
    }
}

void DynamicArray::checkIndex(std::size_t index, const char* operationName) const {
    // index беззнаковый, поэтому отрицательный индекс превращается в очень большое число
    // и тоже отсекается этой проверкой.
    if (index >= size_) {
        throw std::out_of_range(
            std::string(operationName) + ": индекс " + std::to_string(index) +
            " вне границ массива размера " + std::to_string(size_));
    }
}

void DynamicArray::set(std::size_t index, int value) {
    checkIndex(index, "set");
    checkValue(value, "set");
    data_[index] = value;
}

int DynamicArray::get(std::size_t index) const {
    checkIndex(index, "get");
    return data_[index];
}

void DynamicArray::print() const {
    std::cout << '[';
    for (std::size_t i = 0; i < size_; ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << data_[i];
    }
    std::cout << "]\n";
}

void DynamicArray::pushBack(int value) {
    checkValue(value, "pushBack");

    // Выделяем новый блок на 1 элемент больше, переносим старые данные, освобождаем старый блок.
    // Если allocate бросит std::bad_alloc, старые данные останутся нетронутыми.
    int* newData = allocate(size_ + 1);
    for (std::size_t i = 0; i < size_; ++i) {
        newData[i] = data_[i];
    }
    newData[size_] = value;

    delete[] data_;
    data_ = newData;
    ++size_;
}

void DynamicArray::combine(const DynamicArray& other, int sign, const char* operationName) {
    // Первый проход: только проверяем, что все результаты останутся в допустимом промежутке.
    // Так массив не окажется изменённым «наполовину», если где-то в середине будет ошибка.
    for (std::size_t i = 0; i < size_; ++i) {
        const int rhs = (i < other.size_) ? other.data_[i] : 0; // недостающие элементы = 0
        checkValue(data_[i] + sign * rhs, operationName);
    }

    // Второй проход: применяем изменения. Размер массива не меняется.
    for (std::size_t i = 0; i < size_; ++i) {
        const int rhs = (i < other.size_) ? other.data_[i] : 0;
        data_[i] += sign * rhs;
    }
}

void DynamicArray::add(const DynamicArray& other) {
    combine(other, +1, "add");
}

void DynamicArray::sub(const DynamicArray& other) {
    combine(other, -1, "sub");
}
