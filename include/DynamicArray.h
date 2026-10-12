#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <cstddef>

// Динамический массив целых чисел.
// Инвариант класса: все хранимые значения лежат в промежутке [MIN_VALUE, MAX_VALUE].
class DynamicArray {
public:
    static constexpr int MIN_VALUE = -100;
    static constexpr int MAX_VALUE = 100;

    // Задание 1: конструктор по размеру (элементы заполняются нулями) и деструктор.
    explicit DynamicArray(std::size_t size);
    ~DynamicArray();

    // Задание 2: конструктор копирования (глубокое копирование данных).
    DynamicArray(const DynamicArray& other);

    // Оператор присваивания реализуется в части 5 (правило трёх/пяти).
    // Пока он запрещён, чтобы случайное присваивание не привело к двойному освобождению памяти.
    DynamicArray& operator=(const DynamicArray& other) = delete;

    std::size_t size() const;

    // Задание 1: сеттер.
    // Проверяет индекс на выход за границы и значение на принадлежность [-100; 100].
    // Возвращает true, если значение записано, и false при ошибке (сообщение печатается в std::cerr).
    bool set(std::size_t index, int value);

    // Задание 1: геттер.
    // Проверяет индекс на выход за границы. При ошибке печатает сообщение и возвращает 0.
    int get(std::size_t index) const;

    // Задание 1: вывод всех значений массива.
    void print() const;

    // Задание 3: добавление значения в конец массива с расширением размера.
    // Значение проверяется на принадлежность [-100; 100]. Возвращает false при ошибке.
    bool pushBack(int value);

    // Задание 4: поэлементное сложение и вычитание массивов.
    // Размер текущего массива не меняется; недостающие элементы other считаются нулями.
    // Если хотя бы один результат выходит за [-100; 100], массив не изменяется и возвращается false.
    bool add(const DynamicArray& other);
    bool sub(const DynamicArray& other);

private:
    int* data_;
    std::size_t size_;

    static bool isValidValue(int value);
    bool isValidIndex(std::size_t index) const;

    // Общая реализация add/sub: sign = +1 для сложения, -1 для вычитания.
    bool combine(const DynamicArray& other, int sign, const char* operationName);
};

#endif // DYNAMIC_ARRAY_H
