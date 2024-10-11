#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stack>
#include <cctype>
#include <vector>
#include <string>

#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 80
#define DISPLAY_HEIGHT 100

// 按钮定义
const char* buttons[6][4] = {
        { "7", "8", "9", "/" },
        { "4", "5", "6", "*" },
        { "1", "2", "3", "-" },
        { "0", ".", "=", "+" },
        { "(", ")", "^2", "!" },
        { "sqrt", "C", "", "" }
};

char input[256] = { 0 };
int input_len = 0;
bool isResultDisplayed = false;

double eval(const char* expression);
double factorial(double n);
int precedence(char op);
double applyOp(double a, double b, char op);
double evalRPN(std::vector<std::string>& tokens);
std::vector<std::string> infixToPostfix(const std::string& expression);

// 计算器界面
void drawCalculator() {
    cleardevice();
    setfillcolor(LIGHTGRAY);
    solidrectangle(0, 0, 320, DISPLAY_HEIGHT);
    settextstyle(40, 0, _T("宋体"));
    setbkcolor(LIGHTGRAY);
    setcolor(BLACK);
    outtextxy(10, 30, input);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            int x = j * BUTTON_WIDTH;
            int y = i * BUTTON_HEIGHT + DISPLAY_HEIGHT;
            setfillcolor(LIGHTBLUE);
            solidrectangle(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT);
            settextstyle(40, 0, _T("宋体"));
            setcolor(WHITE);
            if (strlen(buttons[i][j]) > 0) {
                outtextxy(x + 15, y + 20, buttons[i][j]);
            }
        }
    }
}

// 处理按钮点击
void handleButtonClick(int x, int y) {
    if (y < DISPLAY_HEIGHT) {
        return;
    }

    int row = (y - DISPLAY_HEIGHT) / BUTTON_HEIGHT;
    int col = x / BUTTON_WIDTH;

    const char* button = buttons[row][col];

    if (strcmp(button, "C") == 0) {
        memset(input, 0, sizeof(input));
        input_len = 0;
        isResultDisplayed = false;
    } else if (strcmp(button, "=") == 0) {
        double result = eval(input);
        sprintf(input, "%.2f", result);
        input_len = strlen(input);
        isResultDisplayed = true;
    } else if (strcmp(button, "sqrt") == 0) {
        strcat(input, "sqrt(");
        input_len = strlen(input);
    } else {
        if (isResultDisplayed) {
            memset(input, 0, sizeof(input));
            input_len = 0;
            isResultDisplayed = false;
        }
        strcat(input, button);
        input_len = strlen(input);
    }

    drawCalculator();
}

// 运算符优先级
int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    }
    if (op == '*' || op == '/') {
        return 2;
    }
    if (op == '^' || op == '!') {
        return 3;
    }
    return 0;
}

// 应用运算符操作
double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
        case '^': return pow(a, b);
        default: return 0;
    }
}

// 阶乘函数
double factorial(double n) {
    if (n == 0) return 1;
    double result = 1;
    for (int i = 1; i <= (int)n; i++) {
        result *= i;
    }
    return result;
}

// 处理平方根
double sqrtOp(double n) {
    return sqrt(n);
}

// 逆波兰表达式
std::vector<std::string> infixToPostfix(const std::string& expression) {
    std::vector<std::string> output;
    std::stack<std::string> ops;
    std::string num;

    for (size_t i = 0; i < expression.length(); i++) {
        char c = expression[i];

        if (isdigit(c) || c == '.') {
            num += c;
        } else {
            if (!num.empty()) {
                output.push_back(num);
                num.clear();
            }
            if (c == '(') {
                ops.push("(");
            } else if (c == ')') {
                while (!ops.empty() && ops.top() != "(") {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.pop(); // 弹出左括号
            } else if (c == '!') {
                output.push_back("!");
            } else if (expression.substr(i, 4) == "sqrt") {
                ops.push("sqrt");
                i += 3;  // 跳过 "sqrt" 的剩余部分
            } else if ((c == '-' || c == '+') && (i == 0 || expression[i - 1] == '(')) {
                // 处理一元负号和正号（忽略正号）
                if (c == '-') {
                    num = "-";
                }
                // 一元正号不需要特殊处理，直接跳过
            } else {
                while (!ops.empty() && precedence(ops.top()[0]) >= precedence(c)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(std::string(1, c));
            }
        }
    }
    if (!num.empty()) {
        output.push_back(num);
    }
    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

// 计算逆波兰表达式
double evalRPN(std::vector<std::string>& tokens) {
    std::stack<double> values;

    for (std::string& token : tokens) {
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            values.push(std::stod(token));
        } else if (token == "!") {
            if (values.empty()) {
                printf("^2堆栈下溢\n");
                return 0;
            }
            double val = values.top();
            values.pop();
            values.push(factorial(val));
        } else if (token == "sqrt") {
            if (values.empty()) {
                printf("sqrt堆栈下溢\n");
                return 0;
            }
            double val = values.top();
            values.pop();
            values.push(sqrtOp(val));
        } else {
            if (values.size() < 2) {
                printf("太长啦 %s\n", token.c_str());
                return 0;
            }
            double val2 = values.top();
            values.pop();
            double val1 = values.top();
            values.pop();
            values.push(applyOp(val1, val2, token[0]));
        }
    }

    if (values.size() != 1) {
        printf("表达式无效\n");
        return 0;
    }

    return values.top();  // 返回栈中结果
}

// 解析并计算
double eval(const char* expression) {
    std::string expr(expression);
    std::vector<std::string> postfix = infixToPostfix(expr);
    return evalRPN(postfix);
}

int main() {
    initgraph(480, 580);
    drawCalculator();

    while (true) {
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                handleButtonClick(msg.x, msg.y);
            }
        }
    }

    closegraph();
    return 0;
}