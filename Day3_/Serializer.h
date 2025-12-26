//
// Created by Ayr on 2025/12/23.
//

#ifndef DAY3__SERIALIZER_H
#define DAY3__SERIALIZER_H
#include <stdexcept>
#include <string>
#include <sstream>

// 好好切分运行时多态和编译时多态
// 运行时多态是用于继承可序列化能力
// 编译时多态纯为了简化代码

// 目的，完成对嵌套的序列化以及POD+String类型的序列化
struct Pair {
    int type;
    const void *p;
};

class Serializable {
    public:
    virtual ~Serializable() = default;
    [[nodiscard]] virtual int StructNum() const = 0;
    [[nodiscard]] virtual Pair* StructInfo() const = 0;
};

struct JSONFormat {};

template<class Format>
class Serializer {
    public:
    [[nodiscard]] std::string serialize(const Serializable& obj) const {
        if constexpr (std::is_same_v<Format, JSONFormat>) {
            int num = obj.StructNum();
            Pair* info = obj.StructInfo();

            std::string str = "{";
            for (int i = 0; i < num; i++) {
                switch (info[i].type) {
                    case 1:
                        // int
                        str += std::to_string(*static_cast<const int *>(info[i].p));
                        break;
                    case 2:
                        // double
                        str += std::to_string(*static_cast<const double *>(info[i].p));
                        break;
                    case 3:
                        // std::string
                        str += "\"" + *static_cast<const std::string *>(info[i].p) + "\"";
                        break;
                    case 4:
                        // 内嵌
                        str += Serializer<JSONFormat>().serialize(*static_cast<const Serializable *>(info[i].p));
                        break;
                    default:
                        break;
                }
                if (i != num - 1) str += ", ";
            }
            str += "}";

            delete[] info;
            return str;
        }
        else {
            throw std::logic_error{"Unknown format"};
        }
    }
    template<class T>
    [[nodiscard]] T deserialize(const std::string& str) {
        if constexpr (std::is_same_v<Format, JSONFormat>) {
            T obj;
            int num = obj.StructNum();
            Pair* info = obj.StructInfo(); // 用RAII不然抛异常会泄露

            std::string token;
            std::stringstream ss{str.substr(1, str.length() - 2)};
            for (int i = 0; std::getline(ss, token, ','); i++) {
                if (i >= num) throw std::logic_error{"Too many tokens"};
                switch (info[i].type) {
                    case 1:
                        // int
                        *static_cast<int *>(const_cast<void *>(info[i].p)) = std::stoi(token);
                        break;
                    case 2:
                        // double
                        *static_cast<double *>(const_cast<void *>(info[i].p)) = std::stod(token);
                        break;
                    case 3:
                        // std::string
                        *static_cast<std::string *>(const_cast<void *>(info[i].p)) = token.substr(1, token.length() - 2);
                        break;
                    case 4:
                        // 嵌套怎么处理？无法判断嵌套类型具体是哪个。总不能跑过去用模板吧
                        // *static_cast<T *>(const_cast<void *>(info[i].p)) = Serializer<JSONFormat>().deserialize<T>(token);
                        break;
                    default:
                        break;
                }
            }
            return obj;
        }
        else {
            throw std::logic_error{"Unknown format"};
        }
    }

};


#endif //DAY3__SERIALIZER_H