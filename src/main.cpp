#include "../include/DynamicArray.h"

#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static void printTitle(const char* title) {
    std::cout << "\n===== " << title << " =====\n";
}

// Выполняет действие и показывает, какое именно исключение было поймано.
// Обработчики идут от конкретных типов к общему: если бы класс бросал
// «не тот» тип, сработал бы последний catch и это было бы видно в выводе.
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
    } catch (const std::exception& e) {
        std::cout << "  -> поймано другое исключение (не ожидалось!): " << e.what() << '\n';
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // чтобы русский текст корректно выводился в консоли Windows
#endif

    // ---------------------------------------------------------------
    printTitle("Функции из части 1 (без ошибок)");

    DynamicArray arr(5);
    for (std::size_t i = 0; i < arr.size(); ++i) {
        arr.set(i, static_cast<int>(i) * 10);
    }
    std::cout << "arr = ";
    arr.print();
    std::cout << "arr.get(3) = " << arr.get(3) << '\n';

    DynamicArray copy(arr);
    copy.set(0, -7);
    std::cout << "copy (после copy.set(0, -7)) = ";
    copy.print();
    std::cout << "arr  (не изменился)          = ";
    arr.print();

    arr.pushBack(55);
    std::cout << "arr.pushBack(55) -> arr = ";
    arr.print();

    DynamicArray b(2);
    b.set(0, 1);
    b.set(1, 2);
    std::cout << "b = ";
    b.print();
    arr.add(b);
    std::cout << "arr.add(b) -> arr = ";
    arr.print();
    arr.sub(b);
    std::cout << "arr.sub(b) -> arr = ";
    arr.print();

    // ---------------------------------------------------------------
    printTitle("std::out_of_range — индекс вне границ");

    attempt("arr.get(10)", [&] { arr.get(10); });
    attempt("arr.set(6, 1)", [&] { arr.set(6, 1); });
    attempt("arr.get(-1)  (−1 превращается в огромный size_t)", [&] { arr.get(static_cast<std::size_t>(-1)); });

    // ---------------------------------------------------------------
    printTitle("std::invalid_argument — значение вне −100…100");

    attempt("arr.set(0, 150)", [&] { arr.set(0, 150); });
    attempt("arr.set(0, -101)", [&] { arr.set(0, -101); });
    attempt("arr.pushBack(1000)", [&] { arr.pushBack(1000); });

    DynamicArray big(2);
    big.set(0, 90);
    big.set(1, -90);
    std::cout << "big = ";
    big.print();
    attempt("big.add(big)  (90 + 90 = 180)", [&] { big.add(big); });
    attempt("big.sub(arr)  (90 - 0 = 90; -90 - 10 = -100 — граница, допустимо)", [&] { big.sub(arr); });

    std::cout << "После всех ошибок массивы не испорчены:\n";
    std::cout << "arr = ";
    arr.print();
    std::cout << "big = ";
    big.print();

    // ---------------------------------------------------------------
    printTitle("std::bad_alloc — не удалось выделить память");

    // Запрашиваем блок в половину PTRDIFF_MAX байт (~4 эксабайта на 64-битной системе):
    // формально размер допустимый, но столько памяти нет ни у одной машины,
    // поэтому allocate() получает nullptr и сам бросает std::bad_alloc.
    const std::size_t hugeSize =
        static_cast<std::size_t>(std::numeric_limits<std::ptrdiff_t>::max()) / sizeof(int) / 2;
    std::cout << "Пытаемся создать массив из " << hugeSize << " элементов\n";
    attempt("DynamicArray huge(hugeSize)", [&] {
        DynamicArray huge(hugeSize);
        std::cout << "  huge.size() = " << huge.size() << '\n';
    });

    // ---------------------------------------------------------------
    printTitle("Корректные граничные значения не бросают исключений");

    attempt("arr.set(0, -100); arr.set(1, 100); arr.get(5)", [&] {
        arr.set(0, -100);
        arr.set(1, 100);
        std::cout << "  arr.get(5) = " << arr.get(5) << '\n';
    });
    std::cout << "arr = ";
    arr.print();

    return 0;
}
