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


const static string inputFilePath = "D:\\testFragment.cpp";

// Определение регулярных выражений
/*
        R - отключает ескейп последовательности. \n? - отступ строки 0 ил и1 раз.
        //.* - однострочные комментарии | - или многострочные комментарии (* нужно экранировать т.к. служебный символ)
*/
const static regex commentsRegEx(R"(\n?//.*|/\*[\W\w]*?\*/)");
const static regex spacesRegEx(R"([ \t]*(\W)[ \t]*)");
const static regex tokensRegEx(R"(<=|>=|==|!=|[(){};=+\-*/<>=!,]|[^ \n\t(){};=+\-*/<>=!,]+)");
//const static regex numberRegEx(R"(^[0-9]+$)");
const static regex numberRegEx(R"(^[0-9]+|[0-9]+.[0-9]*$)");
const static regex idRegEx(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");

// Определение прототипов функций

string normalizeProgramCode(string program);
tuple<vector<token>, set<token>, vector<token>> tokenizeProgram(const string&);
string syntaxAnalyzer(const vector<token> &tokens);
bool recognizeExpectedToken(const vector<token> &tokens, size_t index, const tokenType &expected_type);
string getErrorMessage(const vector<token>&, size_t, string);
pair<string, size_t> rule_s(const vector<token>&, size_t);
pair<string, size_t> rule_h(const vector<token>&, size_t);
pair<string, size_t> rule_i(const vector<token>&, size_t);
pair<string, size_t> rule_n(const vector<token>&, size_t);
pair<string, size_t> rule_m(const vector<token>&, size_t);
pair<string, size_t> subrule_m_1(const vector<token>&, size_t);
pair<string, size_t> subrule_m_2(const vector<token>&, size_t);
pair<string, size_t> subrule_m_3(const vector<token>&, size_t);
pair<string, size_t> subrule_m_4(const vector<token>&, size_t);
pair<string, size_t> subrule_m_5(const vector<token>&, size_t);
pair<string, size_t> subrule_m_6(const vector<token>&, size_t);
pair<string, size_t> subrule_m_7(const vector<token> &, size_t);
pair<string, size_t> subrule_m_8(const vector<token>&, size_t);
pair<string, size_t> rule_a(const vector<token>&, size_t);
pair<string, size_t> subrule_a_1(const vector<token>&, size_t);
pair<string, size_t> subrule_a_2(const vector<token>&, size_t);
pair<string, size_t> subrule_a_3(const vector<token>&, size_t);
pair<string, size_t> rule_o(const vector<token>&, size_t);
pair<string, size_t> subrule_o_1(const vector<token>&, size_t);
pair<string, size_t> subrule_o_2(const vector<token>&, size_t);
pair<string, size_t> subrule_o_3(const vector<token>&, size_t);
pair<string, size_t> rule_d(const vector<token>&, size_t);
pair<string, size_t> rule_q(const vector<token>&, size_t);
pair<string, size_t> rule_b(const vector<token>&, size_t);
pair<string, size_t> rule_f(const vector<token>&, size_t);
pair<string, size_t> subrule_f_1(const vector<token>&, size_t);
pair<string, size_t> subrule_f_2(const vector<token>&, size_t);


// Реализация прототипов функций

string normalizeProgramCode(string program) {
    stringstream result;

    if (regex_search(program, commentsRegEx)) {
        smatch smatchComments;
        while (regex_search(program, smatchComments, commentsRegEx)) {
            result << smatchComments.prefix(); // Все начало строки, до первого сопоставления регулярки
            program = smatchComments.suffix(); // Оставшаяся часть строки, которую regex_search еще не проверил
        }
    }
    result << program; // Дописываем остольную часть строки, в которой regex_search ничего не находит

    program = result.str();
    result.clear();

    if (regex_search(program, spacesRegEx)) {
        program = regex_replace(program, spacesRegEx, "$1"); // $n - ссылка на n-ую скобочную группу. Т.е. здесь меняем на (\W) если она есть
    }

    return program;
}


tuple<vector<token>, set<token>, vector<token>> tokenizeProgram(const string &program) {
    vector<string> lexemes;

    sregex_iterator sregex_iterator(program.begin(), program.end(), tokensRegEx);
    for (auto it = sregex_iterator, sregex_iterator_end = std::sregex_iterator(); it != sregex_iterator_end; it++) {
        lexemes.emplace_back(it->str());
    }

    map<string, token> tokens_instances;
    tokens_instances.emplace("+", token(tokenType::plus, "+"));
    tokens_instances.emplace("-", token(tokenType::minus, "-"));
    tokens_instances.emplace("*", token(tokenType::multiply, "*"));
    tokens_instances.emplace("/", token(tokenType::divide, "/"));
    tokens_instances.emplace("=", token(tokenType::assignment, "="));
    tokens_instances.emplace("==", token(tokenType::equal, "=="));
    tokens_instances.emplace("!=", token(tokenType::not_equal, "!="));
    tokens_instances.emplace(">", token(tokenType::greater, ">"));
    tokens_instances.emplace("<", token(tokenType::less, "<"));
    tokens_instances.emplace(">=", token(tokenType::greater_or_equal, ">="));
    tokens_instances.emplace("<=", token(tokenType::less_or_equal, "<="));
    tokens_instances.emplace(";", token(tokenType::semicolon, ";"));
    tokens_instances.emplace("(", token(tokenType::bracket_left, "("));
    tokens_instances.emplace(")", token(tokenType::bracket_right, ")"));
    tokens_instances.emplace("{", token(tokenType::bracket_curly_left, "{"));
    tokens_instances.emplace("}", token(tokenType::bracket_curly_right, "}"));
    tokens_instances.emplace("if", token(tokenType::if_, "if"));
    tokens_instances.emplace("else", token(tokenType::else_, "else"));
    tokens_instances.emplace("while", token(tokenType::while_, "while"));
    tokens_instances.emplace("for", token(tokenType::for_, "for"));
    tokens_instances.emplace("do", token(tokenType::do_, "do"));
    tokens_instances.emplace("return", token(tokenType::return_, "return"));
    tokens_instances.emplace(",", token(tokenType::comma, ","));

    set<string> tokens_datatypes{"bool",
                                  "signed char", "unsigned char", "char",
                                  "signed short", "unsigned short", "short",
                                  "signed int", "unsigned int", "int",
                                  "signed long", "unsigned long", "long",
                                  "float",
                                  "double",
                                  "void"
    };

    vector<token> tokens;
    set<token> ids;
    vector<token> numbers;

    for (auto it = lexemes.begin(); it != lexemes.end(); it++) {
        string lexeme = *it;
        auto tokens_instances_it = tokens_instances.find(lexeme);
        if (tokens_instances_it != tokens_instances.end()) {
            tokens.emplace_back(tokens_instances_it->second);
            continue;
        }
        if (lexeme == "signed" || lexeme == "unsigned") {
            it++;
            lexeme += ' ' + *it;
        }
        if (tokens_datatypes.find(lexeme) != tokens_datatypes.end()) {
            tokens.emplace_back(token(datatype, lexeme));
            continue;
        }
        if (regex_match(lexeme, numberRegEx)) {
            token numberToken = token(number, lexeme);
            tokens.emplace_back(numberToken);
            numbers.emplace_back(numberToken);
            continue;
        }
        if (regex_match(lexeme, idRegEx)) {
            token idToken = token(id, lexeme);
            tokens.emplace_back(idToken);
            ids.emplace(idToken);
            continue;
        }

        throw "Can not recognize the lexeme: " + lexeme; // Если лексема не распознается
    }

    return make_tuple(tokens, ids, numbers);
}


string syntaxAnalyzer(const vector<token> &tokens) {
    string successMessage = "Program is correct.";
    pair<string, size_t> result;
    result = rule_s(tokens, 0);

    if (!result.first.empty()) {
        return result.first + " at pointer index = " + to_string(result.second);
    }
    if (result.second < tokens.size()) {
        return "Error: incorrect token at the end " + tokens[result.second].get_value();
    }

    return successMessage;
}


bool recognizeExpectedToken(const vector<token> &tokens, size_t index, const tokenType &expected_type) {
    return index < tokens.size() && tokens[index].get_type() == expected_type;
}


string getErrorMessage(const vector<token> &tokens, size_t index, string expectedToken) {
    string errorMessage = "Error: " + (index < tokens.size() ? "incorrect token near " + tokens[index].get_value() + ", expected token " + expectedToken : "wrong end of program");
    return errorMessage;
}


pair<string, size_t> rule_s(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, datatype)) {
        return make_pair(getErrorMessage(tokens, index, "<T>"), index);
    }
    index++;

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;
    /*
    resultOfSubRules = rule_h(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;
    */

    return make_pair("", index);
}

/*
pair<string, size_t> rule_h(const vector<token> &tokens, size_t index) {
    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_s(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return make_pair("", index);
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}
*/

pair<string, size_t> rule_i(const vector<token> &tokens, size_t index) {
    if (!recognizeExpectedToken(tokens, index, id)) {
        return make_pair(getErrorMessage(tokens, index, "<ID, n>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_n(const vector<token> &tokens, size_t index) {
    if (!recognizeExpectedToken(tokens, index, number)) {
        return make_pair(getErrorMessage(tokens, index, "<D, n>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_m(const vector<token> &tokens, size_t index) {
    do {

        pair<string, size_t> resultOfSubRules;

        resultOfSubRules = subrule_m_1(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_2(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_3(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_4(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_5(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_6(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_7(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        resultOfSubRules = subrule_m_8(tokens, index);
        if (resultOfSubRules.first.empty()) {
            index = resultOfSubRules.second;
            continue;
        }

        break;
    } while (true);

    return make_pair("", index);
}


pair<string, size_t> subrule_m_1(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, datatype)) {
        return make_pair(getErrorMessage(tokens, index, "<T>"), index);
    }
    index++;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_2(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_3(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, for_)) {
        return make_pair(getErrorMessage(tokens, index, "<for>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    resultOfSubRules = rule_b(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_4(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, while_)) {
        return make_pair(getErrorMessage(tokens, index, "<while>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_b(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_5(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, do_)) {
        return make_pair(getErrorMessage(tokens, index, "<do>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, while_)) {
        return make_pair(getErrorMessage(tokens, index, "<while>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_b(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_6(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, if_)) {
        return make_pair(getErrorMessage(tokens, index, "<if>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_b(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    resultOfSubRules = rule_f(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_7(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, return_)) {
        return make_pair(getErrorMessage(tokens, index, "<return>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_m_8(const vector<token> &tokens, size_t index) {
    if (!recognizeExpectedToken(tokens, index, semicolon)) {
        return make_pair(getErrorMessage(tokens, index, "<;>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> rule_a(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = subrule_a_1(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_a_2(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_a_3(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);

}


pair<string, size_t> subrule_a_1(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, comma)) {
        return make_pair( getErrorMessage(tokens, index, "<,>"), index);
    }
    index++;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_a_2(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, assignment)) {
        return make_pair(getErrorMessage(tokens, index, "<=>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, comma)) {
        return make_pair( getErrorMessage(tokens, index, "<,>"), index);
    }
    index++;

    resultOfSubRules = rule_a(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_a_3(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_i(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, assignment)) {
        return make_pair(getErrorMessage(tokens, index, "<=>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> rule_o(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = subrule_o_1(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_o_2(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_o_3(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = rule_d(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_o_1(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    resultOfSubRules = rule_q(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> subrule_o_2(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_o_3(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_d(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;


    resultOfSubRules = rule_q(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> rule_d(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_i(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = rule_n(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> rule_q(const vector<token> &tokens, size_t index) {
    if (recognizeExpectedToken(tokens, index, tokenType::plus) | recognizeExpectedToken(tokens, index, tokenType::minus) |
        recognizeExpectedToken(tokens, index, multiply) | recognizeExpectedToken(tokens, index, divide)) {
        return make_pair("", index + 1);
    }

    //return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <+> or <-> or <*> or </>" : "wrong end of program"), index);
    return make_pair(getErrorMessage(tokens, index, "<+> or <-> or <*> or </>"), index);
}


pair<string, size_t> rule_b(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, tokenType::equal) && !recognizeExpectedToken(tokens, index, not_equal) &&
        !recognizeExpectedToken(tokens, index, tokenType::greater) && !recognizeExpectedToken(tokens, index, tokenType::less) &&
        !recognizeExpectedToken(tokens, index, greater_or_equal) && !recognizeExpectedToken(tokens, index, less_or_equal)){

        //return make_pair("Error. " + (index < tokens.size() ? "Incorrect token " + tokens[index].get_value() + ", expected token <==> or <!=> or <>> or <<> or <>=> or <<=>" : "wrong end of program"), index);
        return make_pair(getErrorMessage(tokens, index, "<==> or <!=> or <>> or <<> or <>=> or <<=>"), index);
    }
    index++;

    resultOfSubRules = rule_o(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
}


pair<string, size_t> rule_f(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    resultOfSubRules = subrule_f_1(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    resultOfSubRules = subrule_f_2(tokens, index);
    if (resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }

    return make_pair("", index);
}


pair<string, size_t> subrule_f_1(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, else_)) {
        return make_pair(getErrorMessage(tokens, index, "<else>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    return make_pair("", index);
}


pair<string, size_t> subrule_f_2(const vector<token> &tokens, size_t index) {

    pair<string, size_t> resultOfSubRules;

    if (!recognizeExpectedToken(tokens, index, else_)) {
        return make_pair(getErrorMessage(tokens, index, "<else>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, if_)) {
        return make_pair(getErrorMessage(tokens, index, "<if>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_left)) {
        return make_pair(getErrorMessage(tokens, index, "<(>"), index);
    }
    index++;

    resultOfSubRules = rule_b(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_right)) {
        return make_pair(getErrorMessage(tokens, index, "<)>"), index);
    }
    index++;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_left)) {
        return make_pair(getErrorMessage(tokens, index, "<{>"), index);
    }
    index++;

    resultOfSubRules = rule_m(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    if (!recognizeExpectedToken(tokens, index, bracket_curly_right)) {
        return make_pair(getErrorMessage(tokens, index, "<}>"), index);
    }
    index++;

    resultOfSubRules = rule_f(tokens, index);
    if (!resultOfSubRules.first.empty()) {
        return resultOfSubRules;
    }
    index = resultOfSubRules.second;

    return make_pair("", index);
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
    cout << "Input program code:" << endl << program << endl;
    cout << "===========================================================" << endl << endl;

    program = normalizeProgramCode(program);
    cout << "Stage 1. Normalized program code:" << endl << program << endl;
    cout << "===========================================================" << endl << endl;

    tuple<vector<token>, set<token>, vector<token>> tokens;

    try {
        tokens = tokenizeProgram(program);
    } catch (const string &exception) {
        cerr << exception << endl;
        exit(1);
    }

    cout << "Stage 2. Lexer:" << endl;
    for (const auto &token : get<0>(tokens)) {
        cout << "<" << token.get_value() << "> ";
    }
    cout << endl;
    cout << "===========================================================" << endl << endl;

    cout << "Stage 3. Tables:" << endl;
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
    cout << "===========================================================" << endl << endl;

    cout << "Stage 4. Errors checking:" << endl;

    cout << syntaxAnalyzer(get<0>(tokens));

    cout << endl;
    cout << "===========================================================" << endl << endl;

    return 0;
}




/*
// Доопределение класса token

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
*/

/*
[[nodiscard]] указывает на обязательность использования результата при возврате из функции. Может быть применим как к типу (при объявлении класса или перечисления), так и непосредственно к возвращаемому типу функции. Пример:

[[nodiscard]] int f() { return 42; }
...
f(); // сообщение о том, что результат функции не использован

Явное приведение результата к void подавляет действие атрибута
*/

