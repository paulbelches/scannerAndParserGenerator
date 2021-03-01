#include <iostream>
#include <string>
#include <stack>
#include <queue>

using namespace std;

class Node{
  public:
  char data;
  Node* left;
  Node* right;
  Node(char d, Node* left, Node* right){
    data=d;
    this->left=left;
    this->right=right;
  }
};

//expand string to include concat as a symbol
string expand(string expr){
    string r = "";
    int cont = 0;
    for (int i = 0; i < expr.length(); i = i + 1){
        if (expr[i] == '|'){
            cont = 0;
        } 
        else if (expr[i] == '('){
            if (cont == 1){
                r = r + '.';
                cont = 0;
            }
        } else if (expr[i] == ')' || expr[i] == '*'){
            ;
        }
        else {
            cont = cont + 1;
        }
        if (cont == 2){
            r = r + '.' + expr[i];
            cont = 1;
        } else {
            r = r + expr[i];
        }
    }
    //r = r + "." + "#"
    return r;
}

bool isOperand(char v){
    string ops = "()|.*";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return 1;
    }
    return 0;
}

int precedence(char v){
    string ops = "()|.*";
    for (int i = 0; i < ops.length(); i = i + 1){
        if (ops[i] == v)
            return i;
    }
    return -1;
}

void printTree(Node* root, int counter){
    string left = "";
    string right = "";
    if (root->left != NULL){
        Node* leftNode = root->left;
        left = leftNode->data;
    } 
    if (root->right != NULL){
        Node* rightNode = root->right;
        right = rightNode->data;
    } 
    cout << "(" << counter << ", value: " << root->data << ", left: " << left << ", right: " << right << ")" << "\n";
    if (root->left != NULL){
        printTree(root->left, counter + 1);
    }
    if (root->right != NULL){
        printTree(root->right, counter + 1);
    }
}

int main(int argc, char **argv) {
    //stack<Node*> tree;
    queue<Node*> tree;
    stack<char> opStack;  
    string expr = expand(argv[1]); //asign the regex expresion
    string result = "";
    //cout << expr << " " << expand(expr);
    
    //Read the expression
    for (int i = 0; i < expr.length(); i = i + 1){
        if (isOperand(expr[i])){
            if (opStack.size() == 0){
                opStack.push(expr[i]);
            } else {
                if (precedence(expr[i]) > precedence(opStack.top())) {
                    opStack.push(expr[i]);
                } else if (expr[i] == '(') {
                    opStack.push(expr[i]);
                } else if (expr[i] == ')') {
                    while (opStack.top() != '('){
                        if (opStack.top() == '*'){
                            Node* temp = new Node(opStack.top(), tree.front(), NULL);
                            tree.front() = temp;
                            opStack.pop();
                        } else if (opStack.top() == '.' || opStack.top() == '|'){
                            Node* firstValue = tree.front();
                            tree.pop();
                            Node* temp = new Node(opStack.top(), firstValue, tree.front());
                            tree.front() = temp;
                            opStack.pop();
                        }
                    }
                    opStack.pop();
                } else {
                    while (precedence(expr[i]) < precedence(opStack.top())){
                        if (opStack.top() == '*'){
                            Node* temp = new Node(opStack.top(), tree.front(), NULL);
                            tree.front() = temp;
                            opStack.pop();
                        } else if (opStack.top() == '.' || opStack.top() == '|'){
                            Node* firstValue = tree.front();
                            tree.pop();
                            Node* temp = new Node(opStack.top(), firstValue, tree.front());
                            tree.front() = temp;
                            opStack.pop();
                        }
                        if (opStack.empty()) {
                            break;
                        } 
                    }
                    opStack.push(expr[i]);
                }
            }
        } else {
            Node* temp = new Node(expr[i], NULL, NULL);
            tree.push(temp);
        }
    }

    while (!opStack.empty()){
        if (opStack.top() == '*'){
            Node* temp = new Node(opStack.top(), tree.front(), NULL);
            tree.front() = temp;
            opStack.pop();
        } else if (opStack.top() == '.' || opStack.top() == '|'){
            Node* firstValue = tree.front();
            tree.pop();
            Node* temp = new Node(opStack.top(), firstValue, tree.front());
            tree.front() = temp;
            opStack.pop();
        }
    }
    
    Node* root = tree.front();

    printTree(root, 0);
    
    
    while (!tree.empty()){
        cout << tree.front()->data << '\n';    
        tree.pop();
    }
    //cout << tree.size();
    return 0;

}