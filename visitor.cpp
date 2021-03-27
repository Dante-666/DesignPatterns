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

struct Value : Expression {
    int value;
    Value(int value) : value(value) {}
};

struct Addition : Expression {
    Expression &lhs, &rhs;
    Addition(Expression &lhs, Expression &rhs) : lhs(lhs), rhs(rhs) {}
};

struct Multiplication : Expression {
    Expression &lhs, &rhs;

    Multiplication(Expression &lhs, Expression &rhs)
        : lhs(lhs), rhs(rhs) {}
};

struct Handler {
    virtual void additionHandler(Addition &expr) = 0;
    virtual void multiplicationHandler(Multiplication &expr) = 0;
    virtual void valueHandler(Value &expr) = 0;
};

struct Visitor : Handler {
    // bind virtual handlers here
    Visitor();

    std::map<std::type_index, std::function<void(Expression &)>> type_map;

    void accept(Expression &expr);

    template <typename E> void castAndHandle(Expression &expr);
};

void Visitor::accept(Expression &expr) {

    std::type_index exprInfo = std::type_index(typeid(expr));

    auto it = type_map.find(exprInfo);

    if (it != type_map.end()) {
        it->second(expr);
    }
}

template <> void Visitor::castAndHandle<Value>(Expression &expr) {
    auto valExpr = static_cast<Value &>(expr);
    valueHandler(valExpr);
}

template <>
void Visitor::castAndHandle<Addition>(Expression &expr) {
    auto addExpr = static_cast<Addition &>(expr);
    additionHandler(addExpr);
}

template <>
void Visitor::castAndHandle<Multiplication>(
    Expression &expr) {
    auto mulExpr = static_cast<Multiplication &>(expr);
    multiplicationHandler(mulExpr);
}

Visitor::Visitor() {
    type_map.insert({std::type_index(typeid(Value)),
                     std::bind(&Visitor::castAndHandle<Value>, this,
                               std::placeholders::_1)});
    type_map.insert(
        {std::type_index(typeid(Addition)),
         std::bind(&Visitor::castAndHandle<Addition>, this,
                   std::placeholders::_1)});
    type_map.insert(
        {std::type_index(typeid(Multiplication)),
         std::bind(&Visitor::castAndHandle<Multiplication>,
                   this, std::placeholders::_1)});
};

struct Printer : Visitor {
    ostringstream oss;

    Printer() : Visitor{} {};

    void additionHandler(Addition &expr) override;
    void multiplicationHandler(Multiplication &expr) override;
    void valueHandler(Value &expr) override;

    string str() const { return oss.str(); }
};

struct Evaluator : Visitor {
    int output;

    Evaluator() : Visitor{} {};

    void additionHandler(Addition &expr) override;
    void multiplicationHandler(Multiplication &expr) override;
    void valueHandler(Value &expr) override;

    int value() const { return output; }
};

void Printer::multiplicationHandler(
    Multiplication &mulExpr) {
    accept(mulExpr.lhs);
    oss << "*";
    accept(mulExpr.rhs);
}

void Printer::additionHandler(Addition &addExpr) {
    oss << "(";
    accept(addExpr.lhs);
    oss << "+";
    accept(addExpr.rhs);
    oss << ")";
}

void Printer::valueHandler(Value &valExpr) { oss << valExpr.value; }

void Evaluator::multiplicationHandler(
    Multiplication &mulExpr) {
    accept(mulExpr.lhs);
    auto temp = output;
    accept(mulExpr.rhs);
    output *= temp;
}

void Evaluator::additionHandler(Addition &addExpr) {
    accept(addExpr.lhs);
    auto temp = output;
    accept(addExpr.rhs);
    output += temp;
}

void Evaluator::valueHandler(Value &expr) {
    output = expr.value;
}

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
