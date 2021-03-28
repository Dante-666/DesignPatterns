/* Copyright, All Rights Reserved
 * Siddharth J Singh, siddharthjsingh@protonmail.com
 */

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <typeindex>
#include <typeinfo>

using namespace std;

struct Expression {
    virtual ~Expression() {}
};

// Value.cpp
struct Value : Expression {
    int value;
    Value(int value) : value(value) {}
};

// Addition.cpp
struct Addition : Expression {
    Expression &lhs, &rhs;
    Addition(Expression &lhs, Expression &rhs) : lhs(lhs), rhs(rhs) {}
};

// Multiplication.cpp
struct Multiplication : Expression {
    Expression &lhs, &rhs;

    Multiplication(Expression &lhs, Expression &rhs)
        : lhs(lhs), rhs(rhs) {}
};

// Vistor.cpp
struct Handler {
    // Insert new polymorphic handles here
    virtual void handle(Addition &expr) = 0;
    virtual void handle(Multiplication &expr) = 0;
    virtual void handle(Value &expr) = 0;
};

struct Visitor : Handler {
    std::map<std::type_index, std::function<void(Expression &)>> type_map;
    // bind virtual handlers here
    Visitor();

    void accept(Expression &expr);
    template <typename E> void castAndHandle(Expression &expr);
};

void Visitor::accept(Expression &expr) {
    std::type_index exprInfo = std::type_index(typeid(expr));
    auto it = type_map.find(exprInfo);
    if (it != type_map.end()) {
        it->second(expr);
    } else {
	std::cerr<<"New unhandled type introduced: "<<it->first.name()<<std::endl;
    }
}

template <typename T> void Visitor::castAndHandle(Expression &expr) {
    auto casted = static_cast<T &>(expr);
    handle(casted);
}

Visitor::Visitor() {
    // Insert new composite types here
    type_map.insert({std::type_index(typeid(Value)),
                     std::bind(&Visitor::castAndHandle<Value>, this,
                               std::placeholders::_1)});
    type_map.insert({std::type_index(typeid(Addition)),
                     std::bind(&Visitor::castAndHandle<Addition>, this,
                               std::placeholders::_1)});
    type_map.insert({std::type_index(typeid(Multiplication)),
                     std::bind(&Visitor::castAndHandle<Multiplication>, this,
                               std::placeholders::_1)});
};

// Printer.cpp
struct Printer : Visitor {
    ostringstream oss;

    Printer() : Visitor{} {};

    void handle(Addition &expr) override;
    void handle(Multiplication &expr) override;
    void handle(Value &expr) override;

    string str() const { return oss.str(); }
};

void Printer::handle(Multiplication &mulExpr) {
    accept(mulExpr.lhs);
    oss << "*";
    accept(mulExpr.rhs);
}

void Printer::handle(Addition &addExpr) {
    oss << "(";
    accept(addExpr.lhs);
    oss << "+";
    accept(addExpr.rhs);
    oss << ")";
}

void Printer::handle(Value &valExpr) { oss << valExpr.value; }

// Evaluator.cpp
struct Evaluator : Visitor {
    int output;

    Evaluator() : Visitor{} {};

    void handle(Addition &expr) override;
    void handle(Multiplication &expr) override;
    void handle(Value &expr) override;

    int value() const { return output; }
};

void Evaluator::handle(Multiplication &mulExpr) {
    accept(mulExpr.lhs);
    auto temp = output;
    accept(mulExpr.rhs);
    output *= temp;
}

void Evaluator::handle(Addition &addExpr) {
    accept(addExpr.lhs);
    auto temp = output;
    accept(addExpr.rhs);
    output += temp;
}

void Evaluator::handle(Value &expr) {
    output = expr.value;
}

//In Client.cpp
int main() {
    Value v1{6};
    Value v2{3};
    Value v3{7};
    Value v4{5};
    Value v5{2};

    Expression *add1 = new Addition{v1, v2};
    Expression *add2 = new Addition{v3, v5};
    Expression *mul1 = new Multiplication{v4, *add1};
    Expression *mul2 = new Multiplication{*add1, *add2};
    Expression *add3 = new Addition{*mul1, *mul2};
    Expression *mul3 = new Multiplication{*add1, *add3};

    Printer ep;
    ep.accept(*mul3);

    Evaluator ev;
    ev.accept(*mul3);

    std::cout << ep.str() << std::endl;
    std::cout << ev.value() << std::endl;

    return 0;
}
