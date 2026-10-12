#include "../include/DynamicArray.h"

#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

static void printTitle(const char* title) {
    std::cout << "\n===== " << title << " =====\n";
}

static void report(bool ok) {
    std::cout << "  -> " << (ok ? "ok" : "отклонено") << '\n';
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // чтобы русский текст корректно выводился в консоли Windows
#endif

    // ---------------------------------------------------------------
    printTitle("Задание 1: конструктор, сеттер, геттер, вывод");

    DynamicArray arr(5);
    std::cout << "Новый массив из 5 элементов: ";
    arr.print();

    for (std::size_t i = 0; i < arr.size(); ++i) {
        arr.set(i, static_cast<int>(i) * 10); // 0, 10, 20, 30, 40
    }
    std::cout << "После set(i, i*10): ";
    arr.print();

    std::cout << "get(2) = " << arr.get(2) << '\n';

    std::cout << "\nПроверки сеттера и геттера:\n";
    std::cout << "set(10, 5) — индекс за границей:\n";
    report(arr.set(10, 5));
    std::cout << "set(0, 150) — значение больше 100:\n";
    report(arr.set(0, 150));
    std::cout << "set(0, -101) — значение меньше -100:\n";
    report(arr.set(0, -101));
    std::cout << "set(0, -100) — граница промежутка, допустимо:\n";
    report(arr.set(0, -100));
    std::cout << "set(1, 100) — граница промежутка, допустимо:\n";
    report(arr.set(1, 100));
    std::cout << "get(7) — индекс за границей:\n";
    const int value = arr.get(7);
    std::cout << "  -> get вернул " << value << '\n';
    std::cout << "Массив после проверок: ";
    arr.print();

    // ---------------------------------------------------------------
    printTitle("Задание 2: конструктор копирования");

    DynamicArray copy(arr);
    std::cout << "Копия:    ";
    copy.print();

    copy.set(0, 77);
    std::cout << "После copy.set(0, 77):\n";
    std::cout << "Копия:    ";
    copy.print();
    std::cout << "Оригинал: ";
    arr.print();
    std::cout << "Оригинал не изменился — у копии своя память (глубокое копирование).\n";

    // ---------------------------------------------------------------
    printTitle("Задание 3: добавление в конец с расширением");

    DynamicArray grow(0);
    std::cout << "Пустой массив, size = " << grow.size() << ": ";
    grow.print();

    grow.pushBack(1);
    grow.pushBack(-50);
    grow.pushBack(99);
    std::cout << "После pushBack(1), pushBack(-50), pushBack(99), size = " << grow.size() << ": ";
    grow.print();

    std::cout << "pushBack(200) — значение вне промежутка:\n";
    report(grow.pushBack(200));
    std::cout << "Массив не изменился, size = " << grow.size() << ": ";
    grow.print();

    // ---------------------------------------------------------------
    printTitle("Задание 4: сложение и вычитание массивов");

    DynamicArray a(4);
    a.set(0, 1);
    a.set(1, 2);
    a.set(2, 3);
    a.set(3, 4);

    DynamicArray b(2); // короче, чем a: недостающие элементы считаются нулями
    b.set(0, 10);
    b.set(1, 20);

    std::cout << "a = ";
    a.print();
    std::cout << "b = ";
    b.print();

    a.add(b);
    std::cout << "a.add(b) -> a = ";
    a.print();

    a.sub(b);
    std::cout << "a.sub(b) -> a = ";
    a.print();

    DynamicArray c(6); // длиннее, чем a: лишние элементы c игнорируются
    for (std::size_t i = 0; i < c.size(); ++i) {
        c.set(i, 5);
    }
    std::cout << "c = ";
    c.print();
    a.add(c);
    std::cout << "a.add(c) -> a = ";
    a.print();
    std::cout << "Размер a не изменился: " << a.size() << '\n';

    DynamicArray big(1);
    big.set(0, 100);
    std::cout << "\nbig = ";
    big.print();
    std::cout << "big.add(big) — результат 200 вышел бы за промежуток:\n";
    report(big.add(big));
    std::cout << "big = ";
    big.print();

    return 0;
}
