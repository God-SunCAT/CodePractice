#include <iostream>
#include "Serializer.h"

class User : public Serializable {
public:
    std::string name;
    int age;

    User(std::string name = "", int age = 0) : name(std::move(name)), age(age) {}

    [[nodiscard]] int StructNum() const final {
        return 2;
    };
    [[nodiscard]] Pair* StructInfo() const final {
        // 看好了，这是const接口，成员是const的，那么这个const要一直继承下去
        return new Pair[2]{{3, &name}, {1, &age}};
    };
};
int main() {
    std::cout << Serializer<JSONFormat>().serialize(User("Alice", 14)) << std::endl;
    try {
        std::cout << Serializer<JSONFormat>().deserialize<User>("{\"Alice\", 14}").name << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}