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

struct AdditionExpression : Expression {
    Expression &lhs, &rhs;
    AdditionExpression(Expression &lhs, Expression &rhs) : lhs(lhs), rhs(rhs) {}
};

struct MultiplicationExpression : Expression {
    Expression &lhs, &rhs;

    MultiplicationExpression(Expression &lhs, Expression &rhs)
        : lhs(lhs), rhs(rhs) {}
};

struct ExpressionHandler {
    virtual void additionHandler(AdditionExpression &expr) = 0;
    virtual void multiplicationHandler(MultiplicationExpression &expr) = 0;
    virtual void valueHandler(Value &expr) = 0;
};

struct ExpressionVisitor : ExpressionHandler {
    // bind virtual handlers here
    ExpressionVisitor();
    std::map<std::type_index, std::function<void(Expression &)>> type_map;

    void accept(Expression &expr);

    void castValue(Expression& expr);
    void castAddition(Expression& expr);
    void castMultiplication(Expression& expr);
};

void ExpressionVisitor::accept(Expression &expr) {

    std::type_index exprInfo = std::type_index(typeid(expr));

    auto it = type_map.find(exprInfo);

    if (it != type_map.end()) {
        it->second(expr);
    }
}

void ExpressionVisitor::castValue(Expression& expr) {
    auto valExpr = static_cast<Value&>(expr);
    valueHandler(valExpr);
}

void ExpressionVisitor::castAddition(Expression& expr) {
    auto addExpr = static_cast<AdditionExpression&>(expr);
    additionHandler(addExpr);
}

void ExpressionVisitor::castMultiplication(Expression& expr) {
    auto mulExpr = static_cast<MultiplicationExpression&>(expr);
    multiplicationHandler(mulExpr);
}

ExpressionVisitor::ExpressionVisitor() {
    type_map.insert({std::type_index(typeid(Value)),
                     std::bind(&ExpressionVisitor::castValue, this,
                               std::placeholders::_1)});
    type_map.insert({std::type_index(typeid(AdditionExpression)),
                     std::bind(&ExpressionVisitor::castAddition, this,
                               std::placeholders::_1)});
    type_map.insert({std::type_index(typeid(MultiplicationExpression)),
                     std::bind(&ExpressionVisitor::castMultiplication, this,
                               std::placeholders::_1)});
};

struct ExpressionPrinter : ExpressionVisitor {
    ostringstream oss;

    ExpressionPrinter() : ExpressionVisitor{} {};

    void additionHandler(AdditionExpression &expr) override;
    void multiplicationHandler(MultiplicationExpression &expr) override;
    void valueHandler(Value &expr) override;

    string str() const { return oss.str(); }
};

struct ExpressionEvaluator : ExpressionVisitor {
    int output;

    ExpressionEvaluator() : ExpressionVisitor{} {};

    void additionHandler(AdditionExpression &expr) override;
    void multiplicationHandler(MultiplicationExpression &expr) override;
    void valueHandler(Value &expr) override;

    int value() const { return output; }
};

void ExpressionPrinter::multiplicationHandler(MultiplicationExpression &mulExpr) {
    accept(mulExpr.lhs);
    oss << "*";
    accept(mulExpr.rhs);
}

void ExpressionPrinter::additionHandler(AdditionExpression &addExpr) {
    oss << "(";
    accept(addExpr.lhs);
    oss << "+";
    accept(addExpr.rhs);
    oss << ")";
}

void ExpressionPrinter::valueHandler(Value &valExpr) {
    oss << valExpr.value;
}

void ExpressionEvaluator::multiplicationHandler(MultiplicationExpression &mulExpr) {
    accept(mulExpr.lhs);
    accept(mulExpr.rhs);
}

void ExpressionEvaluator::additionHandler(AdditionExpression &addExpr) {
    accept(addExpr.lhs);
    accept(addExpr.rhs);
}

void ExpressionEvaluator::valueHandler(Value &expr) {
}

int main() {
    Value v1{1};
    Value v2{2};

    Expression *add = new AdditionExpression{v1, v2};
    Expression *mul = new MultiplicationExpression{v1, *add};

    ExpressionPrinter ep;
    ep.accept(*mul);

    /*ExpressionEvaluator ev;
    ev.accept(*mul);*/

    std::cout << ep.str() << std::endl;
    //std::cout << ev.value() << std::endl;

    return 0;
}
