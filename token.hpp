//
// Created by MIKE on 11.02.2021.
//

#ifndef SYNTAX_ANALYZER_TOKEN_HPP
#define SYNTAX_ANALYZER_TOKEN_HPP

enum tokenType {
    datatype, // <T>
    plus, // <+>
    minus, // <->
    multiply, // <*>
    divide, // </>
    assignment, // <=>
    equal, // <==>
    not_equal, // <!=>
    greater, // <>>
    less, // <<>
    greater_or_equal, // <>=>
    less_or_equal, // <<=>
    semicolon, // <;>
    bracket_left, // <(>
    bracket_right, // <)>
    bracket_curly_left, // <{>
    bracket_curly_right, // <}>
    if_, // <if>
    else_, // <else>
    while_, // <while>
    for_, // <for>
    do_, // <do>
    return_, // <return>
    comma, // <,>
    id, // <ID, n>
    number // <D, n>
};

class token {

private:
    tokenType type;
    std::string value;

public:
    token(tokenType type, std::string value): type(type), value(std::move(value)) {
    }

    bool operator< (const token &token1) const {
        return value < token1.get_value();
    }

    [[nodiscard]] const tokenType &get_type() const {
        return this->type;
    }

    [[nodiscard]] const std::string &get_value() const{
        return this->value;
    }
};

#endif //SYNTAX_ANALYZER_TOKEN_HPP

/*
[[nodiscard]] указывает на обязательность использования результата при возврате из функции. Может быть применим как к типу (при объявлении класса или перечисления), так и непосредственно к возвращаемому типу функции. Пример:

[[nodiscard]] int f() { return 42; }
...
f(); // сообщение о том, что результат функции не использован

Явное приведение результата к void подавляет действие атрибута
*/