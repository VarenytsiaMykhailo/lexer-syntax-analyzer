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
    token(tokenType, std::string);
    bool operator< (const token &token1) const;
    [[nodiscard]] const tokenType &get_type() const;
    [[nodiscard]] const std::string &get_value() const;
};

#endif //SYNTAX_ANALYZER_TOKEN_HPP
