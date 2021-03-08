#include <iostream>
#include <bits/stdc++.h> 
#include <utility> 
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include<set>

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
////////Tree external methods
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
    //r = r + "." + "#";
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

set<string> generateTree(Node* root, set<string> nodes){
    string s(1, root->data);
    if (root->left != NULL){
        Node* leftNode = root->left;
        string t(1, leftNode->data);
        nodes.insert(s+" "+t);
        nodes = generateTree(leftNode, nodes);
    } 
    if (root->right != NULL){
        Node* rightNode = root->right;
        string t(1, rightNode->data);
        nodes.insert(s+" "+t);
        nodes = generateTree(rightNode, nodes);
    } 
    return nodes;
}

void writeTree(Node* root){
    set<string> nodes;
    nodes = generateTree(root, nodes);
    fstream my_file;
	my_file.open("tree.txt", ios::out);
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        for (auto const &e: nodes) {
            my_file << e << "\n";
        }
		my_file.close();
	}
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

class Tree{
  public:
  Node* root;
  Tree(string expr){
    //queue<Node*> tree;
    stack<char> opStack; 
    //Read the expression
    string result;
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
                        result = result + opStack.top();
                        opStack.pop();
                    }
                    opStack.pop();
                } else {
                    while (precedence(expr[i]) < precedence(opStack.top())){
                        result = result + opStack.top();
                        opStack.pop();
                        if (opStack.empty()) {
                            break;
                        } 
                    }
                    opStack.push(expr[i]);
                }
            }
        } else {
            result = result + expr[i];
        }
    }
    
    while (!opStack.empty()){
        result = result + opStack.top();
        opStack.pop();
    }

    stack<Node*> tree;
    for (int i=0; i < result.size(); i=i+1){
        if (isOperand(result[i])){
            if (result[i] == '*'){
                Node* temp = new Node('*', tree.top(), NULL);
                tree.top() = temp;
            } else if (result[i] == '.' || result[i] == '|'){
                Node* firstValue = tree.top();
                tree.pop();
                Node* temp = new Node(result[i], tree.top(), firstValue);
                tree.top() = temp;
            }
            //cout << result[i] << "\n";
        } else {
            tree.push(new Node(result[i], NULL, NULL));
        }

       //opStack.push(new Node(opStack.top(), NULL, NULL);); 
    }
    //cout << "Resultado: " << result << "\n";
    //cout << "stack: " << tree.size() << "\n";
    root = tree.top();
  }
};

class AFNode{
    public:
    int id;
    vector <pair<char, AFNode*>> edges;
    void addEdge(char values, AFNode* node);
    AFNode(){
        id = rand() % 2000 + 1;
    }
};

void AFNode::addEdge(char value, AFNode* node){
    edges.push_back(make_pair(value, node)); 
}

class AFN{
    public:
    AFNode* start;
    AFNode* end;
    AFN(Node* root){
        string empty = "e";
        AFN* operand1;
        AFN* operand2;
        switch(root->data){
            case '*':
                //cout << "Estoy en * \n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();
                //crear transicion de inicial a final
                start->addEdge(empty[0], end);
                //llamar a automata de hijos
                operand1 = new AFN(root->left);

                //crear transicion de final de hijo a inicio de hijo
                //crear transicion de inicial a hijo
                //crear transicion de hijo a final
                start->addEdge(empty[0], operand1->start);
                operand1->end->addEdge(empty[0], end);
                operand1->end->addEdge(empty[0], operand1->start);
                //cout << "sali de * \n";
            break;
            case '|':
                //cout << "Estoy en | \n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();

                //llamar a automata de hijos
                operand1 = new AFN(root->left);
                //llamar a automata de hijos
                operand2 = new AFN(root->right);

                //crear transici[on] de inicial a hijo con e
                //crear transici[on] de hijo a final
                start->addEdge(empty[0], operand1->start);
                start->addEdge(empty[0], operand2->start);
                //crear transici[on] de inicial a hijo con e
                //crear transici[on] de hijo a final
                operand1->end->addEdge(empty[0], end);
                operand2->end->addEdge(empty[0], end);
                //cout << "Sali de |\n";
            break;
            case '.':
                //cout << "Estoy en . \n";
                //llamar a automata de hijos
                operand1 = new AFN(root->left);
                //llamar a automata de hijos
                operand2 = new AFN(root->right);
                //crear transici[on] de hijo a hijo
                //primer hijo se vuelve el inicio
                //vector <pair<char, AFNode*>> Temporaledges;
                //cout << "Creo que es aqui ... valor: " << operand2->start->edges.size() <<"\n";
                //cout << "Creo que es aqui ... valor: " << operand1->end->id << "\n";
                operand1->end->edges = operand2->start->edges;
                //cout << "No no lo es \n";
                operand2->start = operand1->end;
                start = operand1->start;
                end = operand2->end;
                //cout << "Sali de . \n";
            break; //add terminal character
            case '#':
                start = new AFNode();
                end = new AFNode();
                end->id = 9999;
                start->addEdge('#', end);
                //cout << "Sali de " << root->data << "\n";
            break; //add terminal character
            default:
                //cout << "Estoy en " << root->data << "\n";
                //crear nodo inicial
                start = new AFNode();
                //crear nodo final
                end = new AFNode();
                //crear transici[on]
                start->addEdge(root->data, end);
                //cout << "Sali de " << root->data << "\n";
        }
    }
        //Simular cadena, moverse
};

set<string> generateSet(AFNode* start){
    set<string> result;
    stack<AFNode*> nodes;
    set<int> checkedNodes;
    nodes.push(start);
    checkedNodes.insert(start->id);
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            int tnum = temporalNode->edges[i].second->id;
            if (checkedNodes.find(tnum) == checkedNodes.end()){
                nodes.push(temporalNode->edges[i].second);
                checkedNodes.insert(tnum);
            }
            string s = to_string(temporalNode->id)+" "+to_string(tnum) +" "+temporalNode->edges[i].first;
            result.insert(s);
        }
    }
    return result;
}

void writeAFN(AFNode* start){
    set<string> nodes;
    nodes = generateSet(start);
    fstream my_file;
	my_file.open("thompson.txt", ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        for (auto const &e: nodes) {
            my_file << e << "\n";
        }
		my_file.close();
	}
}

set<string> getAlphabet(string s){
    //string nonAlphabet = '.*|()';
    set<string> result;
    for (int i=0; i<s.length();i++){
        if (s[i] != '.' && s[i] != '*' && s[i] != '|' && s[i] != '(' && s[i] != ')'){
            string t(1,s[i]);
            result.insert(t);
        }
    }
    return result;
}

set<AFNode*> lock(set<AFNode*> initialNodes){
    set<AFNode*> result;
    stack<AFNode*> nodes;
    for (auto const &e: initialNodes) {
        nodes.push(e);
        result.insert(e);
    }
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            if (temporalNode->edges[i].first == 'e'){
                result.insert(temporalNode->edges[i].second);
                nodes.push(temporalNode->edges[i].second);
            }
        }
    }
    return result;
}

set<AFNode*> move(set<AFNode*> initialNodes, char character){
    set<AFNode*> result;
    stack<AFNode*> nodes;
    for (auto const &e: initialNodes) {
        nodes.push(e);
    }
    while (!nodes.empty()){
        AFNode* temporalNode =  nodes.top();
        nodes.pop();
        for (int i = 0; i < temporalNode->edges.size(); i = i + 1){
            if (temporalNode->edges[i].first == character){
                result.insert(temporalNode->edges[i].second);
                nodes.push(temporalNode->edges[i].second);
            }
        }
    }
    return result;
}

void printSet(set<AFNode*> nodes){
    cout << "-----------------inicio------------------ \n";
    for (auto const &e: nodes) {
        cout << e->id << "\n";
    }
    cout << "-----------------FIn------------------ \n";
}

class AFD{
    public:
    //AFDode* start;
    vector <set<AFNode*>> states;
    vector <vector <int>> transitions;
    set<string> alphabet;
    AFD(set<AFNode*> start, set<string> alphabet){
        this->alphabet = alphabet;
        states.push_back(start);
        queue<set<AFNode*>> pendingStates;
        pendingStates.push(start);
        while (!pendingStates.empty()){
            vector<int> temporalTransitions;
            for (auto const &e: alphabet) {
                set<AFNode*> temporalSet;
                temporalSet = lock(move(pendingStates.front(), e[0]));
                int pos = -1;
                for (int i = 0; i < states.size(); i = i + 1){
                    if (equal(states[i].begin(), states[i].end(), temporalSet.begin(),temporalSet.end())){
                        pos = i;
                    }
                }
                if (pos == -1){
                    states.push_back(temporalSet);
                    pos = states.size() - 1;
                    pendingStates.push(temporalSet);
                }
                temporalTransitions.push_back(pos);
            }
            transitions.push_back(temporalTransitions);
            pendingStates.pop();
        }
    }
};

string setToString(set<string> s){
    string result = "";
    for (auto const &e: s) {
        result = result + e;
    }
    return result;
}


void writeAFD(AFD* afd){
    fstream my_file;
	my_file.open("afd.txt", ios::out);
    //cout << nodes.size() << "\n";
	if (!my_file) {
		cout << "File not created!";
	}
	else {
		cout << "File created successfully!" << "\n";
        string s = setToString(afd->alphabet);
        for (int i = 0; i < afd->transitions.size(); i = i + 1){
            for (int j = 0; j < afd->transitions[i].size(); j = j + 1){
                my_file << i << " "<< afd->transitions[i][j] << " " << s[j] << "\n";
            }
        }
		my_file.close();
	}
}

int main(int argc, char **argv) {
    //stack<Node*> tree; 
    string expr = expand(argv[1]); //asign the regex expresion
    cout << expr << "\n";
    Tree* tree = new Tree(expr);
    printTree(tree->root, 0);
    //cout << "TreeBuild \n";
    AFN* afn = new AFN(tree->root);
    afn->end->id = 9999;
    writeAFN(afn->start);

    set<string> alphabet = getAlphabet(expr);

    set<AFNode*> initialState;
    initialState.insert(afn->start);
    initialState = lock(initialState);
    //printSet(initialState);

    AFD* afd = new AFD(initialState, alphabet);
    writeAFD(afd);
    //cout << afd->alphabet.size() << "\n";
    //cout << setToString(afd->alphabet) << "\n";

    return 0;

}