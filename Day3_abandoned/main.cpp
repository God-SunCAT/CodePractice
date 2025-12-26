#include <iostream>
#include "Serializable.h"
class User : public Serializable {
    std::string name;
    int age;
    public:
    User(std::string name, int age) : name(std::move(name)), age(age) {}
    // 用于恢复原状态
    void recovery(void** p) override {};

    // BinaryFormat
    [[nodiscard]] const void* startMem() const override {
        return &name;
    };
    [[nodiscard]] const void* endMem() const override {
        return &age;
    };

    // JSONFormat
    [[nodiscard]] int memberNum() const override {
        return 2;
    };
    [[nodiscard]] StructInfo* structInfo() const override {
        return new StructInfo[2]{{3, static_cast<const void*>(&name)}, {1, static_cast<const void*>(&age)}};
    };
};
int main() {
    std::cout << Serializer<JSONFormat>().serialize(User("Alice", 14)) << std::endl;
    return 0;
}