#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <cstddef>

// Динамический массив целых чисел.
// Инвариант класса: все хранимые значения лежат в промежутке [MIN_VALUE, MAX_VALUE].
//
// Часть 2: все проверки выбрасывают исключения конкретного типа:
//   std::bad_alloc        — не удалось выделить память;
//   std::out_of_range     — индекс вне границ массива;
//   std::invalid_argument — значение вне промежутка [-100; 100].
// Если метод выбросил исключение, массив остаётся в прежнем состоянии.
class DynamicArray {
public:
    static constexpr int MIN_VALUE = -100;
    static constexpr int MAX_VALUE = 100;

    // Конструктор по размеру (элементы заполняются нулями).
    // Бросает std::bad_alloc, если память выделить не удалось.
    explicit DynamicArray(std::size_t size);
    ~DynamicArray();

    // Конструктор копирования (глубокое копирование данных).
    // Бросает std::bad_alloc, если память выделить не удалось.
    DynamicArray(const DynamicArray& other);

    // Оператор присваивания реализуется в части 5 (правило трёх/пяти).
    // Пока он запрещён, чтобы случайное присваивание не привело к двойному освобождению памяти.
    DynamicArray& operator=(const DynamicArray& other) = delete;

    std::size_t size() const;

    // Сеттер. Бросает std::out_of_range при неверном индексе
    // и std::invalid_argument при значении вне [-100; 100].
    void set(std::size_t index, int value);

    // Геттер. Бросает std::out_of_range при неверном индексе.
    int get(std::size_t index) const;

    // Вывод всех значений массива.
    void print() const;

    // Добавление значения в конец массива с расширением размера.
    // Бросает std::invalid_argument при значении вне [-100; 100]
    // и std::bad_alloc, если не удалось выделить память под новый размер.
    void pushBack(int value);

    // Поэлементное сложение и вычитание массивов.
    // Размер текущего массива не меняется; недостающие элементы other считаются нулями.
    // Бросает std::invalid_argument, если хотя бы один результат выходит за [-100; 100]
    // (массив при этом не изменяется).
    void add(const DynamicArray& other);
    void sub(const DynamicArray& other);

private:
    int* data_;
    std::size_t size_;

    // Выделяет память под count элементов, заполненных нулями. Бросает std::bad_alloc.
    static int* allocate(std::size_t count);

    static void checkValue(int value, const char* operationName);
    void checkIndex(std::size_t index, const char* operationName) const;

    // Общая реализация add/sub: sign = +1 для сложения, -1 для вычитания.
    void combine(const DynamicArray& other, int sign, const char* operationName);
};

#endif // DYNAMIC_ARRAY_H
