#include "parser.h"
#include "hash.h"
#include "sppdebug.h"
#include <vector>
#include <string>
#include <cctype>
#include <stack>
#include <queue>
#include <regex>

/* 
 * Global variables
*/
static char nextToken;


enum nodeType
{
    p_DEF,
    p_AND,
    p_OR,
    p_LPAREN,
    p_RPAREN
};

class ASTNode
{
    public:
        ASTNode();
        ASTNode *left;
        ASTNode *right;
        nodeType ntype;
};

class definitionNode : public ASTNode
{
    public:
        definitionNode(bool result);
        bool result; 
};

class LogicalNode : public ASTNode
{
    public:
        LogicalNode(nodeType op);
};

ASTNode::ASTNode()
{
    left = nullptr;
    right = nullptr;
    ntype = p_DEF;
}

definitionNode::definitionNode(bool result) : result(result)
{
    ntype = p_DEF;
}

LogicalNode::LogicalNode(nodeType op)
{
    ntype = op;
}

static LogicalNode* createSimpleTree(std::string& left, std::string& right, nodeType op)
{
    LogicalNode* root = new LogicalNode(op);
    definitionNode *l = nullptr;
    definitionNode *r = nullptr;

    if (is_string_in_hash_table(left))
        l = new definitionNode(true);
    else
        l = new definitionNode(false);
    root->left = l;

    if (is_string_in_hash_table(right))
        r = new definitionNode(true);
    else
        r = new definitionNode(false);
    root->right = r;
}




static void fetch_tokens(std::vector<char>& tokens, const std::string& input)
{
    std::string token = "";
    int i = 0;
    int l = input.length();
    char c;

    while (i < l)
    {
        c = input[i];

        if (isalnum(c) || c == '_')
        {
            token += c;
            i++;
            while (i < l && (isalnum(c = input[i]) || c == '_'))
            {
                token += c;
                i++;
            }

            cerr_debug_print("[token]: " << token);

            if (is_string_in_hash_table(token))
                tokens.push_back('t');
            else
                tokens.push_back('f');
            token.clear();
            continue;
        }

        // cerr_debug_print("[char]: " << c);

        if (c == ' ')
        {
            while (i < l && ((c = input[i]) == ' '))
                i++;
        }
        
        else if ((i+1 < l) && (input[i] == '&') && (input[i+1] == '&'))
        {
            tokens.push_back('&');
            i += 2;
        }

        else if ((i+1 < l) && (input[i] == '|') && (input[i+1] == '|'))
        {
            tokens.push_back('|');
            i += 2;
        }

        else if (c == '(' || c == ')')
        {
            tokens.push_back(c);
            i++;
        }

        else
        {
            std::cerr << "Invalid char: " << c << std::endl;
            std::exit(EXIT_FAILURE);
        }

    }

#if defined(DEBUG) || defined(_DEBUG)
    std::cout << "\n=== [vector tokens] ===" << std::endl;
    for (auto t: tokens)
        std::cout << t << " ";
    std::cout << std::endl;
    std::cout << "===                 ===\n\n";
#endif

}

bool parse(const std::string& input)
{
    /* 
     * We are implementing  the shunting-yard algorithm in this function. Visit
     * this link for help: https://brilliant.org/wiki/shunting-yard-algorithm/
     */

    cerr_debug_print("[parsing]: " << input);

    std::vector<char>tk;
    std::stack<char> os;
    std::queue<char> oq;
    char tmp;

    fetch_tokens(tk, input);

    for (auto t: tk)
    {
        if (t == 'f' || t == 't')
            oq.push(t);

        else if (t == '|')
        {
            if (os.empty())
                os.push(t);
            else
            {
                while (os.size())
                {
                    if ((tmp = os.top()) == '&')
                    {
                        os.pop();
                        oq.push(tmp);
                    }
                    else
                        break;
                }
                os.push(t);
            }
        }

        else if (t == '&')  // No operator with higher precedence than '&' supported
            os.push(t);

        else if (t == '(')
            os.push(t);

        else if (t == ')')
        {
            while (os.size())
            {
                if ((tmp = os.top()) != '(')
                {
                    os.pop();
                    oq.push(tmp);
                }
                else
                    break;
            }
            os.pop();   // discard the left bracket
        }
    }

    while (os.size())
    {
        tmp = os.top();
        oq.push(tmp);
        os.pop();
    }

#if defined(DEBUG) || defined(_DEBUG)
    std::cout << "\n=== [reverse polish] ===" << std::endl;
    while (oq.size())
    {
        std::cout << oq.front() << " ";
        oq.pop();
    }
    std::cout << std::endl;
    std::cout << "===                  ===\n\n";
#endif
    
    return false;
}
