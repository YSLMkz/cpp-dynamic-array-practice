#include "../include/DynamicArray.h"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <typeinfo>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static void printTitle(const char* title) {
    std::cout << "\n===== " << title << " =====\n";
}

// Выполняет действие и показывает, какое именно исключение было поймано.
template <typename Action>
static void attempt(const char* description, Action action) {
    std::cout << description << '\n';
    try {
        action();
        std::cout << "  -> выполнено без исключений\n";
    } catch (const std::out_of_range& e) {
        std::cout << "  -> поймано std::out_of_range: " << e.what() << '\n';
    } catch (const std::invalid_argument& e) {
        std::cout << "  -> поймано std::invalid_argument: " << e.what() << '\n';
    } catch (const std::bad_alloc& e) {
        std::cout << "  -> поймано std::bad_alloc: " << e.what() << '\n';
    } catch (const std::bad_typeid& e) {
        std::cout << "  -> поймано std::bad_typeid: " << e.what() << '\n';
    } catch (const std::exception& e) {
        std::cout << "  -> поймано другое исключение (не ожидалось!): " << e.what() << '\n';
    }
}

// Удобное заполнение массива несколькими значениями через pushBack.
template <typename T>
static void append(DynamicArray<T>& array, std::initializer_list<T> values) {
    for (const T& value : values) {
        array.pushBack(value);
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // чтобы русский текст корректно выводился в консоли Windows
#endif

    // ---------------------------------------------------------------
    printTitle("Задание 1: шаблонный класс, проверка только для целых чисел");

    std::cout << "--- DynamicArray<int> (целочисленный: значения проверяются) ---\n";
    DynamicArray<int> ints(3);
    ints.set(0, -100);
    ints.set(1, 42);
    ints.set(2, 100);
    std::cout << "ints = " << ints << '\n';
    attempt("ints.set(1, 150)", [&] { ints.set(1, 150); });
    attempt("ints.pushBack(-101)", [&] { ints.pushBack(-101); });
    attempt("ints.get(3)", [&] { ints.get(3); });

    std::cout << "\n--- DynamicArray<unsigned int> (тоже целочисленный) ---\n";
    DynamicArray<unsigned int> unsignedInts(2);
    attempt("unsignedInts.set(0, 100u)", [&] { unsignedInts.set(0, 100u); });
    attempt("unsignedInts.set(1, 101u)", [&] { unsignedInts.set(1, 101u); });
    std::cout << "unsignedInts = " << unsignedInts << '\n';

    std::cout << "\n--- DynamicArray<double> (числовой, но не целочисленный: без проверки значения) ---\n";
    DynamicArray<double> doubles(2);
    attempt("doubles.set(0, 250.5)", [&] { doubles.set(0, 250.5); });
    attempt("doubles.set(1, -1e6)", [&] { doubles.set(1, -1e6); });
    attempt("doubles.set(2, 1.0)  — индекс всё равно проверяется", [&] { doubles.set(2, 1.0); });
    std::cout << "doubles = " << doubles << '\n';

    std::cout << "\n--- DynamicArray<std::string> (нечисловой тип) ---\n";
    DynamicArray<std::string> words(0);
    append(words, {std::string("шаблоны"), std::string("в"), std::string("C++")});
    std::cout << "words = " << words << ", size = " << words.size() << '\n';
    attempt("words.get(10)", [&] { words.get(10); });

    DynamicArray<std::string> wordsCopy(words); // конструктор копирования для строк
    wordsCopy.set(0, "копия");
    std::cout << "wordsCopy (после set(0, \"копия\")) = " << wordsCopy << '\n';
    std::cout << "words (не изменился)              = " << words << '\n';

    DynamicArray<std::string> suffixes(0);
    append(suffixes, {std::string("!"), std::string("?")});
    words.add(suffixes); // для строк + — это конкатенация, третьему элементу добавляется ""
    std::cout << "words.add(" << suffixes << ") -> words = " << words << '\n';

    std::cout << "\n--- add/sub для чисел (логика из части 1) ---\n";
    DynamicArray<int> a(0);
    append(a, {1, 2, 3, 4});
    DynamicArray<int> b(0);
    append(b, {10, 20});
    std::cout << "a = " << a << ", b = " << b << '\n';
    a.add(b);
    std::cout << "a.add(b) -> a = " << a << '\n';
    a.sub(b);
    std::cout << "a.sub(b) -> a = " << a << '\n';
    attempt("ints.add(ints)  (-100 + (-100) = -200)", [&] { ints.add(ints); });
    std::cout << "ints не изменился: " << ints << '\n';

    DynamicArray<double> d1(0);
    append(d1, {0.5, 1.5});
    DynamicArray<double> d2(0);
    append(d2, {0.25});
    d1.sub(d2);
    std::cout << "{0.5, 1.5}.sub({0.25}) -> " << d1 << '\n';

    // ---------------------------------------------------------------
    printTitle("Задание 2: операция вывода <<");

    std::cout << "ints    = " << ints << '\n'
              << "doubles = " << doubles << '\n'
              << "words   = " << words << '\n'
              << "пустой  = " << DynamicArray<int>(0) << '\n';
    std::cout << "print() тоже использует <<: ";
    ints.print();

    // ---------------------------------------------------------------
    printTitle("Задание 3: расстояние между массивами");

    DynamicArray<int> p(0);
    append(p, {0, 0, 0});
    DynamicArray<int> q(0);
    append(q, {3, 4, 0});
    std::cout << "p = " << p << ", q = " << q << '\n';
    std::cout << "p.distance(q) = " << p.distance(q) << "  (sqrt(9 + 16) = 5)\n";

    DynamicArray<double> x(0);
    append(x, {1.0, 2.0, 3.0, 4.0});
    DynamicArray<double> y(0);
    append(y, {2.0, 3.0, 4.0, 5.0});
    std::cout << "x = " << x << ", y = " << y << '\n';
    std::cout << "x.distance(y) = " << x.distance(y) << "  (sqrt(1 + 1 + 1 + 1) = 2)\n";
    std::cout << "x.distance(x) = " << x.distance(x) << '\n';

    DynamicArray<unsigned int> u1(0);
    append(u1, {1u, 10u});
    DynamicArray<unsigned int> u2(0);
    append(u2, {4u, 6u});
    std::cout << "u1 = " << u1 << ", u2 = " << u2
              << ", u1.distance(u2) = " << u1.distance(u2) << "  (sqrt(9 + 16) = 5)\n";

    attempt("p.distance(a)  — размеры 3 и 4", [&] { p.distance(a); });
    attempt("words.distance(wordsCopy)  — std::string не число", [&] { words.distance(wordsCopy); });

    // ---------------------------------------------------------------
    printTitle("Исключение std::bad_alloc из части 2 сохранилось");

    const std::size_t hugeSize =
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) / sizeof(int) / 2;
    attempt("DynamicArray<int> huge(~4 эксабайта)", [&] {
        DynamicArray<int> huge(hugeSize);
        std::cout << "  huge.size() = " << huge.size() << '\n';
    });

    return 0;
}
