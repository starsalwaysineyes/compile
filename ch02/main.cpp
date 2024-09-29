#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <functional>
using namespace std;

/*
// 语法树节点类型
typelist:
CHAR,       // 单个字符
CONCAT,     // 连接
UNION,      // 或
STAR        // 闭包 *
*/
enum class RegexType {
    CHAR,       // 单个字符
    CONCAT,     // 连接
    UNION,      // 或
    STAR        // 闭包 *
};


/*
// 语法树节点
typelist:
CHAR,       // 单个字符
CONCAT,     // 连接
UNION,      // 或
STAR        // 闭包 *
Node* left
Node* right
*/
struct RegexNode {
    RegexType type;
    char value; // 仅在type == CHAR时使用
    shared_ptr<RegexNode> left;
    shared_ptr<RegexNode> right;

    // 构造函数（char）
    RegexNode(RegexType t, char val = '\0') : type(t), value(val), left(nullptr), right(nullptr) {}
    
    // 构造函数用于二元操作符（连接、或）
    RegexNode(RegexType t, shared_ptr<RegexNode> l, shared_ptr<RegexNode> r)
        : type(t), value('\0'), left(l), right(r) {}
    
    // 新增构造函数用于单元操作符（闭包 *)
    RegexNode(RegexType t, shared_ptr<RegexNode> l)
        : type(t), value('\0'), left(l), right(nullptr) {}
};


/*
插入显式的连接符：
在（字符，闭包，右括号）与（字符）
ab-> a.b
(ab)* -> (a.b)*
当遍历到regex中的每一个字符c时，
如果prev（前一个字符）不是初始值\0，
并且prev不是(（左括号）也不是|（或运算符），
同时当前字符c不是)（右括号）、|（或运算符）或*（闭包运算符），
那么就在结果字符串中插入一个.来表示前一个字符与当前字符之间的连接。
*/
string insertConcat(const string& regex) {
    string result;
    char prev = '\0';
    for (char c : regex) {
        if (prev != '\0') {
            // 如果前一个字符是字符、闭包、或右括号
            if ((prev != '(' && prev != '|') &&
                (c != ')' && c != '|' && c != '*' )) {
                result += '.'; // 使用.表示连接
            }
        }
        result += c;
        prev = c;
    }
    return result;
}

// 获取运算符优先级
int getPrecedence(char op) {
    switch(op) {
        case '*':
            return 3;
        case '.':
            return 2;
        case '|':
            return 1;
        default:
            return 0;
    }
}

// 转换为后缀表达式
string toPostfix(const string& regex) {
    string postfix;
    stack<char> opStack;
    for (char c : regex) {
        if (c == '(') {
            opStack.push(c);
        }
        else if (c == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                postfix += opStack.top();
                opStack.pop();
            }
            if (!opStack.empty()) opStack.pop(); // 弹出 '('
        }
        else if (c == '*' || c == '.' || c == '|') {
            while (!opStack.empty() && getPrecedence(opStack.top()) >= getPrecedence(c)) {
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.push(c);
        }
        else {
            postfix += c; // 操作数直接添加到后缀表达式
        }
    }
    while (!opStack.empty()) {
        postfix += opStack.top();
        opStack.pop();
    }
    return postfix;
}
/*
// 构建语法树
根据后缀表达式构造语法树
parameter: string::postfix(后缀表达式)
return value: shared_ptr<RegexNode>
*/
shared_ptr<RegexNode> buildSyntaxTree(const string& postfix) {
    stack<shared_ptr<RegexNode>> stack;
    for (char c : postfix) {
        if (c == '*') {
            auto child = stack.top(); stack.pop();
            auto node = make_shared<RegexNode>(RegexType::STAR, child);
            stack.push(node);
        }
        else if (c == '.') {
            auto right = stack.top(); stack.pop();
            auto left = stack.top(); stack.pop();
            auto node = make_shared<RegexNode>(RegexType::CONCAT, left, right);
            stack.push(node);
        }
        else if (c == '|') {
            auto right = stack.top(); stack.pop();
            auto left = stack.top(); stack.pop();
            auto node = make_shared<RegexNode>(RegexType::UNION, left, right);
            stack.push(node);
        }
        else {
            auto node = make_shared<RegexNode>(RegexType::CHAR, c);
            stack.push(node);
        }
    }
    return stack.empty() ? nullptr : stack.top();
}
/*
// 完整的解析函数
parameter: string::regex(正则表达式)
return value: shared_ptr<RegexNode>
传入正则表达式，进行插入连接符操作后，转换为后缀表达式，再根据后缀表达式构建语法树
*/
shared_ptr<RegexNode> parseRegex(const string& regex) {
    string withConcat = insertConcat(regex);
    string postfix = toPostfix(withConcat);
    cout << "Postfix: " << postfix << "\n"; // 调试用
    return buildSyntaxTree(postfix);
}
/*
// NFA状态
int id
unordered_map<char, vector<NFAState*>> transitions
bool isFinal
*/
struct NFAState {
    int id;
    unordered_map<char, vector<NFAState*>> transitions;
    bool isFinal;

    NFAState(int id_) : id(id_), isFinal(false) {}
};
/*
// NFA
NFAState* start
NFAState* end
int stateCount
*/
struct NFA {
    NFAState* start;
    NFAState* end;
    int stateCount;

    NFA() : start(nullptr), end(nullptr), stateCount(0) {}

    NFAState* newState() {
        return new NFAState(stateCount++);
    }
};

// 构建NFA
NFA buildNFA(const shared_ptr<RegexNode>& root) {
    NFA nfa;

    // 递归构建NFA
    function<pair<NFAState*, NFAState*>(shared_ptr<RegexNode>)> build = [&](shared_ptr<RegexNode> node) -> pair<NFAState*, NFAState*> {
        if (node->type == RegexType::CHAR) {
            NFAState* start = nfa.newState();
            NFAState* end = nfa.newState();
            start->transitions[node->value].push_back(end);
            return {start, end};
        }
        else if (node->type == RegexType::CONCAT) {
            auto left = build(node->left);
            auto right = build(node->right);
            left.second->transitions['e'].push_back(right.first);
            return {left.first, right.second};
        }
        else if (node->type == RegexType::UNION) {
            NFAState* start = nfa.newState();
            NFAState* end = nfa.newState();
            auto left = build(node->left);
            auto right = build(node->right);
            start->transitions['e'].push_back(left.first);
            start->transitions['e'].push_back(right.first);
            left.second->transitions['e'].push_back(end);
            right.second->transitions['e'].push_back(end);
            return {start, end};
        }
        else if (node->type == RegexType::STAR) {
            NFAState* start = nfa.newState();
            NFAState* end = nfa.newState();
            auto sub = build(node->left);
            start->transitions['e'].push_back(sub.first);
            start->transitions['e'].push_back(end);
            sub.second->transitions['e'].push_back(sub.first);
            sub.second->transitions['e'].push_back(end);
            return {start, end};
        }
        return {nullptr, nullptr};
    };

    auto result = build(root);
    nfa.start = result.first;
    nfa.end = result.second;
    nfa.end->isFinal = true;
    return nfa;
}
/*
// DFA状态
int id
unordered_map<char, DFAState*> transitions
bool isFinal
set<NFAState*> nfaStates
*/
struct DFAState {
    int id;
    unordered_map<char, DFAState*> transitions;
    bool isFinal;
    set<NFAState*> nfaStates; // 对应的NFA状态集合

    DFAState(int id_) : id(id_), isFinal(false) {}
};

// DFA
struct DFA {
    DFAState* start;
    vector<DFAState*> states;
    int stateCount;

    DFA() : start(nullptr), stateCount(0) {}

    DFAState* newState(const set<NFAState*>& nfaStates_) {
        DFAState* state = new DFAState(stateCount++);
        state->nfaStates = nfaStates_;
        // 如果NFA状态集合中有任何一个是接受状态，则DFA状态也是接受状态
        for (auto s : nfaStates_) {
            if (s->isFinal) {
                state->isFinal = true;
                break;
            }
        }
        states.push_back(state);
        return state;
    }
};

// 计算e-闭包
set<NFAState*> epsilonClosure(const set<NFAState*>& states) {
    set<NFAState*> closure = states;
    stack<NFAState*> stackStates;
    for (auto s : states) stackStates.push(s);

    while (!stackStates.empty()) {
        NFAState* state = stackStates.top(); stackStates.pop();
        if (state->transitions.find('e') != state->transitions.end()) {
            for (auto next : state->transitions.at('e')) {
                if (closure.find(next) == closure.end()) {
                    closure.insert(next);
                    stackStates.push(next);
                }
            }
        }
    }
    return closure;
}

// 移动函数
set<NFAState*> move(const set<NFAState*>& states, char symbol) {
    set<NFAState*> result;
    for (auto s : states) {
        if (s->transitions.find(symbol) != s->transitions.end()) {
            for (auto next : s->transitions.at(symbol)) {
                result.insert(next);
            }
        }
    }
    return result;
}

// 构建DFA
DFA buildDFA(const NFA& nfa) {
    DFA dfa;
    queue<set<NFAState*>> q;

    // 初始状态
    set<NFAState*> startSet = epsilonClosure({nfa.start});
    dfa.start = dfa.newState(startSet);
    q.push(startSet);

    while (!q.empty()) {
        set<NFAState*> current = q.front(); q.pop();

        DFAState* currentDFA = nullptr;
        for (auto state : dfa.states) {
            if (state->nfaStates == current) {
                currentDFA = state;
                break;
            }
        }

        // 获取所有可能的输入符号（排除e）
        unordered_set<char> symbols;
        for (auto s : current) {
            for (auto &[c, _] : s->transitions) {
                if (c != 'e') symbols.insert(c);
            }
        }

        for (char symbol : symbols) {
            set<NFAState*> moveSet = move(current, symbol);
            set<NFAState*> closureSet = epsilonClosure(moveSet);

            if (closureSet.empty()) continue;

            // 检查是否已经存在
            bool found = false;
            DFAState* existingState = nullptr;
            for (auto state : dfa.states) {
                if (state->nfaStates == closureSet) {
                    existingState = state;
                    found = true;
                    break;
                }
            }

            if (!found) {
                DFAState* newDFAState = dfa.newState(closureSet);
                q.push(closureSet);
                existingState = newDFAState;
            }

            currentDFA->transitions[symbol] = existingState;
        }
    }

    return dfa;
}

// 打印DFA
void printDFA(const DFA& dfa) {
    cout << "DFA状态转移表:\n";
    cout << "状态\t是否接受\t转移\n";
    for (auto state : dfa.states) {
        cout << "S" << state->id << "\t" << (state->isFinal ? "是" : "否") << "\t\t";
        for (auto &[c, next] : state->transitions) {
            cout << c << "->S" << next->id << " ";
        }
        cout << "\n";
    }
}


void wk1()
{
    // 示例1: (a|b)*abb
    string regex1 = "(a|b)*abb";
    auto tree1 = parseRegex(regex1);
    NFA nfa1 = buildNFA(tree1);
    DFA dfa1 = buildDFA(nfa1);
    cout << "正则表达式: " << regex1 << "\n";
    printDFA(dfa1);

    cout << "\n-----------------------\n";

    // 示例2: 10|(0|11)0*1
    string regex2 = "10|(0|11)0*1";
    auto tree2 = parseRegex(regex2);
    NFA nfa2 = buildNFA(tree2);
    DFA dfa2 = buildDFA(nfa2);
    cout << "正则表达式: " << regex2 << "\n";
    printDFA(dfa2);
}
/*
<letter>->a|b|c...|A|B...|Z
<digit>->1|2|3...|0
<number>-><digit>+
<identifier>-><letter>(<letter>|<digit>)*
<singleword>->+|-|*|(|)|{|}|:|,|;
<division>->/
<comparison_single>-><|>|!|=
<comparison_double>->  >=|<=|!=|==
<comment_first> ->  /*
<comment_last>-> *\/
*/
void wk2()
{
    cout<<endl;
    cout<<"work in wk2"<<endl;
    string letter="a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";
    string digit="0|1|2|3|4|5|6|7|8|9";
    string number="(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*";
    string identifier="("+letter+")"+"("+letter+"|"+digit+")*";
    string singleword="+|-|*|(|)|{|}|:|,|;";
    string division="/";
    string comparison_single="<|>|!|=";
    string comparison_double=">=|<=|!=|==";
    string comment_first="/*";
    string comment_last="*/";
    


    
    cout<<number<<endl;
    auto tree1 = parseRegex(number);
    NFA nfa1 = buildNFA(tree1);
    DFA dfa1 = buildDFA(nfa1);
    cout << "正则表达式: " << number << "\n";
    printDFA(dfa1);
    
}




int main() {
    
    wk1();
    
    wk2();
    
    return 0;
}
