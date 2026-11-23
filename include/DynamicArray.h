#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <algorithm>   // std::min
#include <cmath>       // std::hypot
#include <cstddef>     // std::size_t
#include <cstdint>     // std::uintmax_t
#include <iostream>    // std::ostream, std::cout
#include <limits>
#include <new>         // std::bad_alloc, std::nothrow
#include <stdexcept>   // std::out_of_range, std::invalid_argument
#include <string>
#include <type_traits> // std::is_integral_v, std::is_arithmetic_v, std::is_signed_v
#include <typeinfo>    // std::bad_typeid
#include <utility>     // std::swap

// Шаблонный динамический массив элементов типа T.
//
// Шаблон целиком находится в заголовке: компилятор должен видеть реализацию
// в момент подстановки конкретного типа, поэтому отдельного DynamicArray.cpp больше нет.
//
// Исключения:
//   std::bad_alloc        — не удалось выделить память;
//   std::out_of_range     — индекс вне границ массива;
//   std::invalid_argument — для целочисленных T: входное значение вне [-100; 100]
//                           (для беззнаковых T — вне [0; 100]);
//                           в distance(): массивы разного размера;
//   std::bad_typeid       — distance() для нечислового T.
//   std::overflow_error   — целочисленный результат add/sub не помещается в T.
template <typename T>
class DynamicArray {
public:
    // Допустимый промежуток значений — проверяется только для целочисленных T.
    static constexpr int MIN_VALUE = -100;
    static constexpr int MAX_VALUE = 100;

    // Конструктор по размеру: элементы создаются значением по умолчанию
    // (0 для чисел, пустая строка для std::string и т.д.).
    explicit DynamicArray(std::size_t size);
    ~DynamicArray();

    // Конструктор копирования (глубокое копирование данных).
    DynamicArray(const DynamicArray& other);

    // Оператор присваивания реализуется в части 5 (правило трёх/пяти).
    DynamicArray& operator=(const DynamicArray& other) = delete;

    std::size_t size() const;

    // Сеттер: проверка индекса, а для целочисленных T — ещё и значения.
    void set(std::size_t index, const T& value);

    // Геттер: проверка индекса.
    const T& get(std::size_t index) const;

    // Ссылка для записи в целочисленный массив: arr[i] = x вызывает set().
    class ElementReference {
        friend class DynamicArray;
        DynamicArray& array_;
        std::size_t index_;

        ElementReference(DynamicArray& array, std::size_t index)
            : array_(array), index_(index) {}

    public:
        operator const T&() const { return array_.get(index_); }

        ElementReference& operator=(const T& value) {
            array_.set(index_, value);
            return *this;
        }

        ElementReference& operator=(const ElementReference& other) {
            return *this = static_cast<const T&>(other);
        }
    };

    using reference = std::conditional_t<std::is_integral_v<T>, ElementReference, T&>;
    reference operator[](std::size_t index);
    const T& operator[](std::size_t index) const { return get(index); }

    bool operator==(const DynamicArray& other) const;
    bool operator!=(const DynamicArray& other) const { return !(*this == other); }

    DynamicArray& operator+=(const DynamicArray& other);
    DynamicArray& operator-=(const DynamicArray& other);
    DynamicArray& operator+=(const T& value);
    DynamicArray& operator-=(const T& value);

    T* begin() noexcept { return data_; }
    T* end() noexcept { return size_ ? data_ + size_ : data_; }
    const T* begin() const noexcept { return data_; }
    const T* end() const noexcept { return size_ ? data_ + size_ : data_; }

    // Вывод всех значений массива (через operator<<).
    void print() const;

    // Добавление значения в конец массива с расширением размера.
    void pushBack(const T& value);

    // Поэлементное сложение/вычитание. Размер не меняется,
    // недостающие элементы other считаются равными T{} (0 для чисел).
    // add работает для любого T с operator+ (для std::string — конкатенация),
    // sub — для любого T с operator-.
    void add(const DynamicArray& other);
    void sub(const DynamicArray& other);

    // Часть 3, задание 3: евклидово расстояние между массивами
    //   dist = sqrt( sum (a[i] - b[i])^2 ).
    // Только для числовых T, иначе std::bad_typeid.
    // Размеры должны совпадать, иначе std::invalid_argument.
    double distance(const DynamicArray& other) const;

private:
    T* data_;
    std::size_t size_;

    // Выделяет память под count элементов. Бросает std::bad_alloc.
    static T* allocate(std::size_t count);

    // Новый блок на newCount элементов с копией первых count элементов source.
    // При исключении во время копирования освобождает блок и пробрасывает исключение дальше.
    static T* copyOf(const T* source, std::size_t count, std::size_t newCount);

    // Часть 3, задание 1: отдельная проверка данных для целочисленных типов.
    static void checkValue(const T& value, const char* operationName);

    void checkIndex(std::size_t index, const char* operationName) const;

    // Результаты записываются в копию и применяются после успешного расчёта.
    template <typename Operation>
    void transformPrefix(std::size_t count, Operation operation);
    static T addValues(const T& a, const T& b);
    static T subValues(const T& a, const T& b);
};

// ---------------------------------------------------------------------------
// Часть 3, задание 2: операция вывода <<.
// Формат: [1, 2, 3]; строки выводятся в кавычках, чтобы были видны пустые.
// ---------------------------------------------------------------------------
template <typename T>
std::ostream& operator<<(std::ostream& out, const DynamicArray<T>& array) {
    out << '[';
    for (std::size_t i = 0; i < array.size(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        if constexpr (std::is_same_v<T, std::string>) {
            out << '"' << array.get(i) << '"';
        } else {
            out << array.get(i);
        }
    }
    return out << ']';
}

// ---------------------------------------------------------------------------
// Реализация
// ---------------------------------------------------------------------------

template <typename T>
T* DynamicArray<T>::allocate(std::size_t count) {
    if (count == 0) {
        return nullptr;
    }
    // new(std::nothrow), как malloc, при нехватке памяти возвращает nullptr —
    // в этом случае явно бросаем std::bad_alloc.
    T* memory = new (std::nothrow) T[count]();
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

template <typename T>
T* DynamicArray<T>::copyOf(const T* source, std::size_t count, std::size_t newCount) {
    T* memory = allocate(newCount);
    try {
        for (std::size_t i = 0; i < count; ++i) {
            memory[i] = source[i]; // копирование элемента может бросить (например, у std::string)
        }
    } catch (...) {
        delete[] memory;
        throw;
    }
    return memory;
}

template <typename T>
DynamicArray<T>::DynamicArray(std::size_t size)
    : data_(allocate(size)),
      size_(size) {
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data_;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other)
    : data_(copyOf(other.data_, other.size_, other.size_)),
      size_(other.size_) {
}

template <typename T>
std::size_t DynamicArray<T>::size() const {
    return size_;
}

template <typename T>
void DynamicArray<T>::checkValue([[maybe_unused]] const T& value,
                                 [[maybe_unused]] const char* operationName) {
    // Проверка выполняется только для целочисленных T (int, long, short, unsigned, char...).
    // Для double, std::string и других типов эта ветка даже не компилируется.
    if constexpr (std::is_integral_v<T>) {
        constexpr int low = std::is_signed_v<T> ? MIN_VALUE : 0;
        constexpr int high = MAX_VALUE;
        bool outOfRange = value > high;
        if constexpr (std::is_signed_v<T>) {
            outOfRange = outOfRange || value < low;
        }
        if (outOfRange) {
            throw std::invalid_argument(
                std::string(operationName) + ": значение " + std::to_string(value) +
                " вне промежутка [" + std::to_string(low) + "; " + std::to_string(high) + "]");
        }
    }
}

template <typename T>
void DynamicArray<T>::checkIndex(std::size_t index, const char* operationName) const {
    if (index >= size_) {
        throw std::out_of_range(
            std::string(operationName) + ": индекс " + std::to_string(index) +
            " вне границ массива размера " + std::to_string(size_));
    }
}

template <typename T>
void DynamicArray<T>::set(std::size_t index, const T& value) {
    checkIndex(index, "set");
    checkValue(value, "set");
    data_[index] = value;
}

template <typename T>
const T& DynamicArray<T>::get(std::size_t index) const {
    checkIndex(index, "get");
    return data_[index];
}

template <typename T>
typename DynamicArray<T>::reference DynamicArray<T>::operator[](std::size_t index) {
    checkIndex(index, "operator[]");
    if constexpr (std::is_integral_v<T>) {
        return ElementReference(*this, index);
    } else {
        return data_[index];
    }
}

template <typename T>
bool DynamicArray<T>::operator==(const DynamicArray& other) const {
    if (size_ != other.size_) {
        return false;
    }
    for (std::size_t i = 0; i < size_; ++i) {
        if (!(data_[i] == other.data_[i])) {
            return false;
        }
    }
    return true;
}

template <typename T>
void DynamicArray<T>::print() const {
    std::cout << *this << '\n';
}

template <typename T>
void DynamicArray<T>::pushBack(const T& value) {
    checkValue(value, "pushBack");

    T* newData = copyOf(data_, size_, size_ + 1);
    try {
        newData[size_] = value;
    } catch (...) {
        delete[] newData;
        throw;
    }

    delete[] data_;
    data_ = newData;
    ++size_;
}

template <typename T>
template <typename Operation>
void DynamicArray<T>::transformPrefix(std::size_t count, Operation operation) {
    if (count == 0) {
        return;
    }
    DynamicArray result(*this);
    for (std::size_t i = 0; i < count; ++i) {
        result.data_[i] = operation(i);
    }
    std::swap(data_, result.data_);
}

template <typename T>
T DynamicArray<T>::addValues(const T& a, const T& b) {
    if constexpr (std::is_integral_v<T>) {
        using Limits = std::numeric_limits<T>;
        if ((b > 0 && a > Limits::max() - b) || (b < 0 && a < Limits::lowest() - b)) {
            throw std::overflow_error("add: результат не помещается в тип элемента");
        }
    }
    return a + b;
}

template <typename T>
T DynamicArray<T>::subValues(const T& a, const T& b) {
    if constexpr (std::is_integral_v<T>) {
        using Limits = std::numeric_limits<T>;
        if ((b > 0 && a < Limits::lowest() + b) || (b < 0 && a > Limits::max() + b)) {
            throw std::overflow_error("sub: результат не помещается в тип элемента");
        }
    }
    return a - b;
}

template <typename T>
void DynamicArray<T>::add(const DynamicArray& other) {
    *this += other;
}

template <typename T>
void DynamicArray<T>::sub(const DynamicArray& other) {
    *this -= other;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator+=(const DynamicArray& other) {
    transformPrefix(std::min(size_, other.size_), [&](std::size_t i) {
        return addValues(data_[i], other.data_[i]);
    });
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator-=(const DynamicArray& other) {
    transformPrefix(std::min(size_, other.size_), [&](std::size_t i) {
        return subValues(data_[i], other.data_[i]);
    });
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator+=(const T& value) {
    transformPrefix(size_, [&](std::size_t i) { return addValues(data_[i], value); });
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator-=(const T& value) {
    transformPrefix(size_, [&](std::size_t i) { return subValues(data_[i], value); });
    return *this;
}

template <typename T>
double DynamicArray<T>::distance([[maybe_unused]] const DynamicArray& other) const {
    if constexpr (!std::is_arithmetic_v<T>) {
        // Для нечисловых типов расстояние не определено.
        throw std::bad_typeid();
    } else {
        if (size_ != other.size_) {
            throw std::invalid_argument(
                "distance: размеры массивов не совпадают (" + std::to_string(size_) +
                " и " + std::to_string(other.size_) + ")");
        }
        long double norm = 0;
        for (std::size_t i = 0; i < size_; ++i) {
            const T& a = data_[i];
            const T& b = other.data_[i];
            long double diff;
            if constexpr (std::is_integral_v<T>) {
                // Разность по модулю в беззнаковом типе точна даже на концах диапазона T.
                const auto ua = static_cast<std::uintmax_t>(a);
                const auto ub = static_cast<std::uintmax_t>(b);
                diff = static_cast<long double>(a < b ? ub - ua : ua - ub);
            } else {
                diff = static_cast<long double>(a) - static_cast<long double>(b);
            }
            norm = std::hypot(norm, diff);
        }
        return static_cast<double>(norm);
    }
}

#endif // DYNAMIC_ARRAY_H
