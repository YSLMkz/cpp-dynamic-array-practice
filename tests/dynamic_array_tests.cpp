#include "DynamicArray.h"
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

static void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template<class Error, class Action>
static void expectError(Action action) {
    try {
        action();
    } catch (const Error&) {
        return;
    }
    throw std::runtime_error("expected exception");
}

static void checkArithmetic() {
    DynamicArray<int> a(2), b(1);
    a[0] = 80;
    a[1] = a[0];
    b[0] = 50;
    DynamicArray<int> original(a);
    a += b;
    require(a[0] == 130 && a[1] == 80 && a != original, "unequal-length addition");
    a -= b;
    require(a == original, "subtraction");
    a += a.get(0);
    require(a[0] == 160 && a[1] == 160, "scalar referring to array storage");
    expectError<std::invalid_argument>([&]{ a[0] = 101; });
    expectError<std::out_of_range>([&]{ (void)a[2]; });
    a.begin()[1] = std::numeric_limits<int>::max();
    expectError<std::overflow_error>([&]{ a += 1; });
    require(a[0] == 160, "overflow changed a preceding element");

    DynamicArray<double> x(1), zero(1);
    for (double value : {1e200, 1e-200}) {
        x[0] = value;
        require(x.distance(zero) == value, "distance to zero");
    }
}

static void checkOwnership() {
    using Array = DynamicArray<std::string>;
    static_assert(std::is_nothrow_move_constructible_v<Array>);
    static_assert(std::is_nothrow_move_assignable_v<Array>);
    Array source(1);
    source[0] = "source";
    Array copy(source), dest(2);
    (dest = copy) = source;
    dest[0] = "changed";
    require(source[0] == "source" && copy[0] == "source", "copies share storage");
    auto& alias = source;
    const auto* data = source.begin();
    source = alias;
    source = std::move(alias);
    require(source.begin() == data && source[0] == "source", "self-assignment");
    Array moved(std::move(source));
    require(moved.begin() == data && source.begin() == nullptr && source.size() == 0,
            "move construction");
    source.pushBack("reused");
    dest = std::move(moved);
    require(dest.begin() == data && moved.begin() == nullptr && moved.size() == 0,
            "move assignment");
    std::vector<Array> arrays;
    arrays.push_back(dest);
    arrays.push_back(std::move(dest));
    require(arrays[0].begin() != data && arrays[1].begin() == data && dest.size() == 0,
            "copy and move into a container");
    moved = dest;
    require(moved.begin() == moved.end(), "copy from empty");
    arrays[1] = std::move(moved);
    require(arrays[1].size() == 0, "move from empty");
}

struct CopyFailure {};
struct Item {
    int value = 0;
    Item& operator=(const Item& other) {
        if (other.value == 2) throw CopyFailure{};
        value = other.value;
        return *this;
    }
};

static void checkFailedCopy() {
    DynamicArray<Item> source(2), target(1);
    source[0].value = 1;
    source[1].value = 2;
    target[0].value = 42;
    expectError<CopyFailure>([&]{ target = source; });
    require(target.size() == 1 && target[0].value == 42, "failed copy modified target");
}

int main() {
    try {
        checkArithmetic();
        checkOwnership();
        checkFailedCopy();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
