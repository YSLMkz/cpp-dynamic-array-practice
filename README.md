
# Динамический массив на C++

Учебная реализация `DynamicArray<T>` на C++17. Заголовок класса находится в `include/DynamicArray.h`, примеры использования — в `src/main.cpp`.

| Часть | Что реализовано |
|---|---|
| [1](https://github.com/YSLMkz/cpp-dynamic-array-practice/pull/1) | Массив целых чисел, глубокое копирование, добавление элементов, сложение и вычитание массивов |
| [2](https://github.com/YSLMkz/cpp-dynamic-array-practice/pull/2) | Исключения при неверном индексе, недопустимом входном значении и ошибке выделения памяти |
| [3](https://github.com/YSLMkz/cpp-dynamic-array-practice/pull/3) | Шаблон класса, вывод в поток и евклидово расстояние для числовых массивов |
| [4](https://github.com/YSLMkz/cpp-dynamic-array-practice/tree/part4-operators) | Обращение по индексу, сравнение, операции `+=` и `-=` с массивами и скалярами, обход и изменение элементов в цикле |
| [5](https://github.com/YSLMkz/cpp-dynamic-array-practice/tree/part5-rule-of-five) | Правило пяти, передача массива по значению, проверка владения памятью |

Для целых чисел `set` и `pushBack` принимают значения от −100 до 100 включительно в пределах типа. Результаты арифметики могут выходить за этот интервал. Например, сумма 80 и 50 даёт 130 в массиве типа `int`. Если целочисленный результат не помещается в тип элемента, операция бросает `std::overflow_error`.

## Сборка и запуск

```sh
g++ -std=c++17 -Wall -Wextra src/main.cpp -o main
./main
```

В PowerShell исполняемый файл запускается командой `.\main.exe`.

## Проверка памяти

В Linux команды ниже собирают программу и тесты с AddressSanitizer. Проверка утечек включена через `detect_leaks`. Параметр `allocator_may_return_null` позволяет демонстрации обработать намеренно неудачный запрос памяти через `std::bad_alloc`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
cmake --build build
export ASAN_OPTIONS=detect_leaks=1:allocator_may_return_null=1
./build/main
(cd build && ctest --output-on-failure)
```
