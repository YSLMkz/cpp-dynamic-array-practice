#include "../include/DynamicArray.h"

#include <iostream>

DynamicArray::DynamicArray(std::size_t size)
    : data_(size > 0 ? new int[size]() : nullptr), // () — заполнение нулями
      size_(size) {
}

DynamicArray::~DynamicArray() {
    delete[] data_;
}

DynamicArray::DynamicArray(const DynamicArray& other)
    : data_(other.size_ > 0 ? new int[other.size_] : nullptr),
      size_(other.size_) {
    // Копируем сами значения, а не указатель: у копии своя память.
    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}

std::size_t DynamicArray::size() const {
    return size_;
}

bool DynamicArray::isValidValue(int value) {
    return value >= MIN_VALUE && value <= MAX_VALUE;
}

bool DynamicArray::isValidIndex(std::size_t index) const {
    // index беззнаковый, поэтому отрицательный индекс превращается в очень большое число
    // и тоже отсекается этой проверкой.
    return index < size_;
}

bool DynamicArray::set(std::size_t index, int value) {
    if (!isValidIndex(index)) {
        std::cerr << "Ошибка set: индекс " << index
                  << " вне границ массива размера " << size_ << '\n';
        return false;
    }
    if (!isValidValue(value)) {
        std::cerr << "Ошибка set: значение " << value
                  << " вне промежутка [" << MIN_VALUE << "; " << MAX_VALUE << "]\n";
        return false;
    }
    data_[index] = value;
    return true;
}

int DynamicArray::get(std::size_t index) const {
    if (!isValidIndex(index)) {
        std::cerr << "Ошибка get: индекс " << index
                  << " вне границ массива размера " << size_ << '\n';
        return 0;
    }
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

bool DynamicArray::pushBack(int value) {
    if (!isValidValue(value)) {
        std::cerr << "Ошибка pushBack: значение " << value
                  << " вне промежутка [" << MIN_VALUE << "; " << MAX_VALUE << "]\n";
        return false;
    }

    // Выделяем новый блок на 1 элемент больше, переносим старые данные, освобождаем старый блок.
    int* newData = new int[size_ + 1];
    for (std::size_t i = 0; i < size_; ++i) {
        newData[i] = data_[i];
    }
    newData[size_] = value;

    delete[] data_;
    data_ = newData;
    ++size_;
    return true;
}

bool DynamicArray::combine(const DynamicArray& other, int sign, const char* operationName) {
    // Первый проход: только проверяем, что все результаты останутся в допустимом промежутке.
    // Так массив не окажется изменённым «наполовину», если где-то в середине будет ошибка.
    for (std::size_t i = 0; i < size_; ++i) {
        const int rhs = (i < other.size_) ? other.data_[i] : 0; // недостающие элементы = 0
        const int result = data_[i] + sign * rhs;
        if (!isValidValue(result)) {
            std::cerr << "Ошибка " << operationName << ": результат " << result
                      << " для элемента " << i << " вне промежутка ["
                      << MIN_VALUE << "; " << MAX_VALUE << "], массив не изменён\n";
            return false;
        }
    }

    // Второй проход: применяем изменения. Размер массива не меняется.
    for (std::size_t i = 0; i < size_; ++i) {
        const int rhs = (i < other.size_) ? other.data_[i] : 0;
        data_[i] += sign * rhs;
    }
    return true;
}

bool DynamicArray::add(const DynamicArray& other) {
    return combine(other, +1, "add");
}

bool DynamicArray::sub(const DynamicArray& other) {
    return combine(other, -1, "sub");
}
