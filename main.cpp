//Proyecto 2 Daniela Brenes y Miguel Estrada 
#include <iostream>
#include <string>
#include <stack>
#include <map> //se importan para utilizarlas luego en el programa

class ExpressionNode {
public:
    virtual bool evaluate(const std::map<char, bool>& variables) const = 0;
    virtual std::string toString() const = 0;
    virtual ~ExpressionNode() {}
};

class BinaryOperatorNode : public ExpressionNode { //clase para los operadores 
public:
    BinaryOperatorNode(char op, ExpressionNode* left, ExpressionNode* right) : op(op), left(left), right(right) {}

    bool evaluate(const std::map<char, bool>& variables) const override {
        bool leftValue = left->evaluate(variables);
        bool rightValue = right->evaluate(variables);

        if (op == '+') return leftValue || rightValue; //operador or
        if (op == '*') return leftValue && rightValue; //operador and
        if (op == '^') return leftValue != rightValue; //operador xor

        return false; // Operador no válido
    }

    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }

    ~BinaryOperatorNode() {
        delete left;
        delete right;
    }

private:
    char op;
    ExpressionNode* left;
    ExpressionNode* right;
};

class UnaryOperatorNode : public ExpressionNode { //para defirnir la expresion booleana
public:
    UnaryOperatorNode(char op, ExpressionNode* operand) : op(op), operand(operand) {}

    bool evaluate(const std::map<char, bool>& variables) const override {
        bool operandValue = operand->evaluate(variables);

        if (op == '-') return !operandValue;

        return false; // Operador no válido
    }

    std::string toString() const override {
        return "(" + op + operand->toString() + ")";
    }

    ~UnaryOperatorNode() {
        delete operand;
    }

private:
    char op;
    ExpressionNode* operand;
};

class VariableNode : public ExpressionNode {    
public:
    VariableNode(char name) : name(name) {}

    bool evaluate(const std::map<char, bool>& variables) const override {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return false; // Variable no encontrada
    }

    std::string toString() const override {
        return std::string(1, name);
    }

private:
    char name;
};

class ExpressionParser {
public:

    ExpressionParser(const std::string& expression) : expression(expression) {}

    // Estructura de datos para almacenar un nodo de árbol binario
    struct TreeNode
    {
        char data;
        TreeNode* left, * right;

        TreeNode()
        {
            this->data;
            this->left = this->right = nullptr;
        };

        TreeNode(char data)
        {
            this->data = data;
            this->left = this->right = nullptr;
        };

        TreeNode(char data, TreeNode* left, TreeNode* right)
        {
            this->data = data;
            this->left = left;
            this->right = right;
        };
    };

    std::string postfix_expression;
    TreeNode* tree_root; 

    /**
     * Internal method to make subtree empty.
     */
    void EmptyTree(TreeNode*& t)
    {
        if (t != nullptr)
        {
            EmptyTree(t->left);
            EmptyTree(t->right);
            delete t;
        }
        t = nullptr;
    }

    // Función para verificar si un token dado es un operador
    bool isOperator(char c) {
        return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
    }

    /**
     * Internal method to print a subtree rooted at t in hierarchy.
     */
    void hierarchy_print(TreeNode* t, int nivel)
    {
        if (t != nullptr)
        {
            // Print the node in the level you find
            for (int i = 0; i < nivel; ++i)
            {
                if (i != nivel - 1) {
                    std::cout << "    ";
                }
                else {
                    std::cout << "|-- ";
                }
            }
            std::cout << t->data << std::endl;

            // Print son nodes from left to right
            if (t->left != nullptr)
            {
                hierarchy_print(t->left, nivel + 1);
            }
            if (t->right != nullptr)
            {
                hierarchy_print(t->right, nivel + 1);
            }
        }
    }

    // Función para construir un árbol de expresión a partir de la expresión sufijo dada
    void construct_tree()
    {
        // caso base
        if (this->postfix_expression.length() == 0) {
            return;
        }

        // crea una stack vacía para almacenar punteros de árbol
        std::stack<TreeNode*> s;

        // recorrer la expresión de sufijo
        for (char c : this->postfix_expression)
        {
            // si el token actual es un operador
            if (isOperator(c))
            {
                // extrae dos nodos `x` e `y` de la stack
                TreeNode* x = s.top();
                s.pop();

                TreeNode* y = s.top();
                s.pop();

                // construye un nuevo árbol binario cuya raíz es el operador y cuyo
                // los niños izquierdo y derecho apuntan a `y` y `x`, respectivamente
                TreeNode* node = new TreeNode(c, y, x);

                // inserta el nodo actual en la stack
                s.push(node);
            }

            // si el token actual es un operando, crea un nuevo nodo de árbol binario
            // cuya raíz es el operando y lo empuja a la stack
            else {
                s.push(new TreeNode(c));
            }
        }

        // un puntero a la raíz del árbol de expresión permanece en la stack
        this->tree_root = s.top();
        hierarchy_print(this->tree_root, 0);
    }

    std::string convertToPostfix() { //convertir en expresion posfija
        std::string postfixExpression;
        std::stack<char> operatorStack;

        for (char c : expression) {
            if (isalpha(c)) {  
                postfixExpression += c;
            }
            else if (c == '+' || c == '*' || c == '^' || c == '-') {
                while (!operatorStack.empty() && precedence(c) <= precedence(operatorStack.top())) {
                    postfixExpression += operatorStack.top();
                    operatorStack.pop();
                }
                operatorStack.push(c);
            }
            else if (c == '(') {
                operatorStack.push(c);
            }
            else if (c == ')') {
                while (!operatorStack.empty() && operatorStack.top() != '(') {
                    postfixExpression += operatorStack.top();
                    operatorStack.pop();
                }
                if (!operatorStack.empty() && operatorStack.top() == '(') {
                    operatorStack.pop();
                }
            }
        }

        while (!operatorStack.empty()) {
            postfixExpression += operatorStack.top();
            operatorStack.pop();
        }
        this->postfix_expression = postfixExpression;
        return postfixExpression;
    }

    ExpressionNode* parseToExpressionTree() {
        std::string postfixExpression = convertToPostfix();
        std::stack<ExpressionNode*> nodeStack;

        for (char c : postfixExpression) {
            if (isalpha(c)) {
                nodeStack.push(new VariableNode(c));
            }
            else if (c == '+' || c == '*' || c == '^' || c == '-') {
                ExpressionNode* right = nodeStack.top();
                nodeStack.pop();
                if (c != '-') {
                    ExpressionNode* left = nodeStack.top();
                    nodeStack.pop();
                    nodeStack.push(new BinaryOperatorNode(c, left, right));
                }
                else {
                    nodeStack.push(new UnaryOperatorNode(c, right));
                }
            }
        }

        if (!nodeStack.empty()) {
            return nodeStack.top();
        }
        else {
            return new VariableNode('F'); // Expresión vacía
        }
    }

private:
    std::string expression;

    int precedence(char c) {
        if (c == '+' || c == '-') return 1;
        if (c == '*') return 2;
        if (c == '^') return 3;
        return 0;
    }
};

int main() {
    int accion;
    //variables
    std::map<char, bool> variableValues = {
        {'T', true},
        {'F', false},
        {'t', true},
        {'f', false}
    };
    do {
        //Menu de opciones
        std::cout << "--------------- MENU --------------------" << std::endl;
        std::cout << "1. Evaluar expresion." << std::endl;
        std::cout << "2. Salir" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Numero de opcion que desea realizar: ";
        std::cin >> accion;
        if (std::cin.fail()) {
            std::cout << "No ha ingresado un numero. Esto genera un error. \nSaliendo del programa." << std::endl;
            std::cin.clear();
            break;
        }
        switch (accion) {
            //Arbol de expresion
        case 1: {
            std::string expression;
            std::cout << "ingrese una expresion booleana, donde (F) es falso y (T) es verdadero:  ";
            std::cin >> expression;

            ExpressionParser parser(expression);
            ExpressionNode* root = parser.parseToExpressionTree();

            std::cout << "Expresion en Postfijo: " << parser.postfix_expression << std::endl;
            std::cout << "Expresion: " << root->toString() << std::endl;

            //Imprime arbol
            std::cout << "Arbol de expresion: " << std::endl;
            parser.construct_tree();

            bool result = root->evaluate(variableValues);
            std::cout << "Resultado: " << (result ? "Verdadero" : "Falso") << std::endl;

            delete root; // Liberar la memoria del árbol de expresión
            parser.EmptyTree(parser.tree_root);

            std::cout << "\n" << std::endl;
            break;
        }
              //Salir del programa
        case 2: {
            std::cout << "Saliendo del programa." << std::endl;

            std::cout << "\n" << std::endl;
            break;
        }
              //Opcion ingresada no valida
        default: {
            std::cout << "Por favor, ingrese un numero que se encuentre en el rango de opciones." << std::endl;

            std::cout << "\n" << std::endl;
            break;
        }
        }
    } while (accion != 2);
    return 0;
}
