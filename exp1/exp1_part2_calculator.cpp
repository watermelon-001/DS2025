// exp1_part2_calculator.cpp
#include <bits/stdc++.h>
using namespace std;

// Helper: determine if a char is operator
bool isOp(char c) {
    return c=='+' || c=='-' || c=='*' || c=='/' || c=='^';
}

int prec(char op) {
    if (op=='+' || op=='-') return 1;
    if (op=='*' || op=='/') return 2;
    if (op=='^') return 3;
    return 0;
}

// apply operator to two operands (double)
double applyOp(double a, double b, char op) {
    if (op=='+') return a + b;
    if (op=='-') return a - b;
    if (op=='*') return a * b;
    if (op=='/') return a / b;
    if (op=='^') return pow(a, b);
    throw runtime_error("Unknown op");
}

// Evaluate infix expression using two stacks (supports parentheses and unary minus)
double evaluate(const string& s) {
    stack<double> values;
    stack<char> ops;
    int i = 0;
    int n = (int)s.size();
    while (i < n) {
        if (isspace(s[i])) { ++i; continue; }
        if (s[i]=='(') { ops.push(s[i]); ++i; }
        else if (isdigit(s[i]) || s[i]=='.') {
            // parse number
            int j = i;
            while (j < n && (isdigit(s[j]) || s[j]=='.')) ++j;
            double val = stod(s.substr(i, j-i));
            values.push(val);
            i = j;
        } else if (s[i]==')') {
            while (!ops.empty() && ops.top()!='(') {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(a,b,op));
            }
            if (!ops.empty() && ops.top()=='(') ops.pop();
            ++i;
        } else if (isOp(s[i])) {
            // handle unary minus
            if (s[i]=='-' && (i==0 || s[i-1]=='(' || isOp(s[i-1]))) {
                // unary minus: parse the number after it
                int j = i+1;
                bool negativeNumber = false;
                if (s[j] == '(') { // treat as 0 - ( ... )
                    values.push(0);
                } else {
                    while (j < n && (isdigit(s[j]) || s[j]=='.')) ++j;
                }
                // fall through to normal operator processing
            }
            while (!ops.empty() && prec(ops.top()) >= prec(s[i])) {
                char op = ops.top(); ops.pop();
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                values.push(applyOp(a,b,op));
            }
            ops.push(s[i]);
            ++i;
        } else {
            throw runtime_error(string("Invalid character: ") + s[i]);
        }
    }
    while (!ops.empty()) {
        char op = ops.top(); ops.pop();
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        values.push(applyOp(a,b,op));
    }
    if (values.empty()) throw runtime_error("Empty expression");
    return values.top();
}

int main() {
    vector<string> tests = {
        "3 + (2 * 2) - 5",
        "2^3 + 4*5",
        "10 / (2 + 3)",
        "-3 + 4 * 2",
        "3.5 + 2.25 * (1.2 + 0.8)"
    };
    for (auto &t : tests) {
        try {
            cout << t << " = " << evaluate(t) << "\n";
        } catch (exception &e) {
            cout << "Error evaluating: " << t << " : " << e.what() << "\n";
        }
    }
    return 0;
}
