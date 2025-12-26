//
// Created by Ayr on 2025/12/23.
//

#ifndef DAY3_SERIALIZABLE_H
#define DAY3_SERIALIZABLE_H

// 运行时多态用于创建可序列化的对象
// 编译时多态用于以特定格式序列化对象

// 如果是我，我会怎么写？
// 对于结构体而言，成员内存是最重要的一项，那么，实现对成员内存的保留便可
// 成员内存的处理 + 恢复指令
// 对于序列化器，只需要提供信息便可
#include <string>
#include <sstream>

struct StructInfo {
    // 1 - int 2 - double 3 - string 4 - serializable
    char type;
    const void* data;
};

class Serializable {
    public:
    virtual ~Serializable() = default;

    // 用于恢复原状态
    // 我真搞不清了，提供序列化器的意义是什么？？？
    // 为了统一这点代码浪费这么多资源？？？？
    virtual void recovery() = 0;

    // BinaryFormat
    [[nodiscard]] virtual const void* startMem() const = 0;
    [[nodiscard]] virtual const void* endMem() const = 0;

    // JSONFormat
    [[nodiscard]] virtual int memberNum() const = 0;
    [[nodiscard]] virtual StructInfo* structInfo() const = 0;
};

struct JSONFormat {};
struct BinaryFormat {};

template <typename Format>
class Serializer {
    public:
    std::string serialize(const Serializable& obj) {
        if constexpr (std::is_same_v<Format, JSONFormat>) {
            // p[type, address] POD + serializable
            // 1 - int 2 - double 3 - string 4 - serializable
            int num = obj.memberNum();
            StructInfo* p = obj.structInfo();
            if (p == nullptr) {
                return std::string{"Error"};
            }

            std::string s = "{";
            for (int i = 0; i < num; i++) {
                switch (p[i].type) {
                    case 1:
                        s += std::to_string(*static_cast<const int*>(p[i].data));
                        break;
                    case 2:
                        s += std::to_string(*static_cast<const double*>(p[i].data));
                        break;
                    case 3:
                        s += *static_cast<const std::string*>(p[i].data);
                        break;
                    case 4:
                        s += Serializer<JSONFormat>().serialize(
                        *static_cast<const Serializable*>(p[i].data)
                        );
                        break;
                    default:
                        break;
                }
                if (i != num - 1) s += ", ";
            }
            s += "}";

            delete p;
            return s;
        }
        else if constexpr (std::is_same_v<Format, BinaryFormat>) {
            return std::string{"Binary"};
        }
        else {
            static_assert("Unknown format");
        }
    }

    template <typename T>
    T deserialize(const std::string& data) {
        if constexpr (std::is_same_v<T, JSONFormat>) {
            std::string token;
            while (std::getline(std::stringstream(data), token, ',')) {

            }
            return T();
        }
        else if constexpr (std::is_same_v<T, BinaryFormat>) {
            return T();
        }
        else {
            static_assert("Unknown format");
        }
    }

};


#endif //DAY3_SERIALIZABLE_H