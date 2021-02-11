//
// Created by MIKE on 10.02.2021.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

#include "token.hpp"


using namespace std;


const string inputFilePath = "D:\\github-reps\\syntax-analyzer\\testFragment.cpp";


string normalizeProgram(string program) {
    /*
        R - отключает ескейп последовательности. \n? - отступ строки 0 ил и1 раз.
        //.* - однострочные комментарии | - или многострочные комментарии (* нужно экранировать т.к. служебный символ)
    */
    regex regexComments(R"(\n?//.*|/\*[\W\w]*?\*/)");
    regex regexSpaces(R"([ \t]*(\W)[ \t]*)");

    stringstream result;

    if (regex_search(program, regexComments)) {
        smatch smatchComments;
        while (regex_search(program, smatchComments, regexComments)) {
            result << smatchComments.prefix(); // Все начало строки, до первого сопоставления регулярки
            program = smatchComments.suffix(); // Оставшаяся часть строки, которую regex_search еще не проверил
        }
    }
    result << program; // Дописываем остольную часть строки, в которой regex_search ничего не находит

    program = result.str();
    result.clear();

    if (regex_search(program, regexSpaces)) {
        program = regex_replace(program, regexSpaces, "$1"); // $n - ссылка на n-ую скобочную группу. Т.е. здесь меняем на (\W) если она есть
    }

    return program;
}


tuple<vector<token>, set<token>, vector<token>> tokenize_program(const string &program) {
    vector<string> lexemes;

    regex regex_tokens(R"(<=|>=|==|!=|[(){};=+\-*/<>=!,]|[^ \n\t(){};=+\-*/<>=!,]+)");

    sregex_iterator sregex_iterator(program.begin(), program.end(), regex_tokens);
    for (auto it = sregex_iterator, sregex_iterator_end = std::sregex_iterator(); it != sregex_iterator_end; it++) {
        lexemes.emplace_back(it->str());
    }

    map<string, token> tokens_instances;
    tokens_instances.emplace("(", token(tokenType::bracket_left, "("));
    tokens_instances.emplace(")", token(tokenType::bracket_right, ")"));
    tokens_instances.emplace("{", token(tokenType::bracket_curly_left, "{"));
    tokens_instances.emplace("}", token(tokenType::bracket_curly_right, "}"));
    tokens_instances.emplace(";", token(tokenType::semicolon, ";"));
    tokens_instances.emplace("=", token(tokenType::assignment, "="));
    tokens_instances.emplace("+", token(tokenType::plus, "+"));
    tokens_instances.emplace("-", token(tokenType::minus, "-"));
    tokens_instances.emplace("*", token(tokenType::multiply, "*"));
    tokens_instances.emplace("/", token(tokenType::divide, "/"));
    tokens_instances.emplace("return", token(tokenType::return_, "return"));
    tokens_instances.emplace("if", token(tokenType::if_, "if"));
    tokens_instances.emplace("else", token(tokenType::else_, "else"));
    tokens_instances.emplace("for", token(tokenType::for_, "for"));
    tokens_instances.emplace("do", token(tokenType::do_, "do"));
    tokens_instances.emplace("while", token(tokenType::while_, "while"));
    tokens_instances.emplace("<", token(tokenType::less, "<"));
    tokens_instances.emplace(">", token(tokenType::greater, ">"));
    tokens_instances.emplace("<=", token(tokenType::less_or_equal, "<="));
    tokens_instances.emplace(">=", token(tokenType::greater_or_equal, ">="));
    tokens_instances.emplace("!=", token(tokenType::not_equal, "!="));
    tokens_instances.emplace("==", token(tokenType::equal, "=="));
    tokens_instances.emplace(",", token(tokenType::comma, ","));

}


int main() {

    // Открываем файл
    ifstream input(inputFilePath);
    if (!input) {
        cerr << "Can not open the file" << endl;
        exit(1);
    }

    // Считываем файл
    stringstream input_ss;
    for (string line; getline(input, line); input_ss << line << endl);
    input.close();

    string program = input_ss.str();
    cout << "Stage 1. Input program code:" << endl << program << endl;
    cout << "===========================================================" << endl;

    program = normalizeProgram(program);
    cout << "Stage 2. Normalized program code:" << endl << program << endl;
    cout << "===========================================================" << endl;

    tuple<vector<token>, set<token>, vector<token>> tokens;

    try {
        tokens = tokenizeProgram(program);
    } catch (const string &exception) {
        cerr << exception << endl;
        exit(1);
    }

    cout << "Stage 3. Lexer:" << endl;
    for (const auto &token : get<0>(tokens)) {
        cout << "<" << token.get_value() << ">, ";
    }
    cout << endl;
    cout << "===========================================================" << endl;

    cout << "Stage 4. Tables:" << endl;
    cout << "Identifiers:" << endl;
    for (const auto &id : get<1>(tokens)) {
        cout << id.get_value() << ", ";
    }
    cout << endl;
    cout << "Numbers:" << endl;
    for (const auto &number : get<2>(tokens)) {
        cout << number.get_value() << ", ";
    }
    cout << endl;
    cout << "===========================================================" << endl;


    return 0;
}





token::token(tokenType type, std::string value): type(type), value(std::move(value)) {
}

bool token::operator< (const token &token1) const {
    return value < token1.get_value();
}

[[nodiscard]] const tokenType &token::get_type() const {
    return this->type;
}

[[nodiscard]] const std::string &token::get_value() const {
    return this->value;
}

