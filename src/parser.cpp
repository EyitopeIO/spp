#include "parser.h"
#include "hash.h"
#include "sppdebug.h"
#include <vector>
#include <string>
#include <cctype>
#include <stack>
#include <queue>

/*

    The grammar for the parser is as follows:

    E ---> (E)
    E ---> E && E
    E ---> E || E
    E ---> DEF
    DEF ---> t | f


    DEF: definition
    t | f: true or false
    E: expression
*/


/**
 * @brief Function to convert input string to an array of tokens
 * 
 * @param input Annotated line read from a file but with annotation stripped.
 * @param tokens A vector to store the tokens. Definitions found in the @ref global_hash_table are marked as 't' and 'f' otherwise.
 *               After the input has been converted to tokens, the vector may look like this: [ t, &, t, |, f, &, t, (, t, |, f, ) ]
 *
 */
static void fetch_tokens(std::vector<char>& tokens, const std::string& input)
{
    std::string token = "";
    int i = 0;
    int l = input.length();
    char c;

    while (i < l)
    {
        c = input[i];

        /* We have potentially found a valid alphanumeric token, so we read character by character to know when it stops being
           a valid alphanumeric. */
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

        /* Skip all spaces until we find a valid character */
        if (c == ' ')
        {
            while (i < l && ((c = input[i]) == ' '))
                i++;
        }
        
        /* For the logical operators, they are always in pairs, so we can check for them together */
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

/**
 * @brief Function to evaluate the reverse polish notation
 * 
 * @param os An empty stack to store the operands in the Shunting-yard algorithm
 * @param oq A queue containing the reverse polish notation for the algorithm
 * 
 * @return true if the expression evaluates to true; false otherwise
 */
static bool evaluate_rpn(std::stack<char>& os, std::queue<char>& oq)
{
    bool a, b;
    char c;

    while (oq.size())
    {
        c = oq.front();
        oq.pop();

        if (isalpha(c)) // 't' or 'f'
            os.push(c);

        else if (c == '&' || c == '|')
        {
            a = (os.top() == 't') ? true : false;
            os.pop();

            b = (os.top() == 'f') ? false : true;
            os.pop();

            if (c == '&')
                os.push((a && b) == true ? 't' : 'f');

            else if (c == '|')
                os.push((a || b) == true ? 't' : 'f');
        }
    }
    return os.top() == 't' ? true : false;
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
    std::queue<char> tq(oq);
    std::cout << "\n=== [reverse polish] ===" << std::endl;
    while (tq.size())
    {
        std::cout << tq.front() << " ";
        tq.pop();
    }
    std::cout << std::endl;
    std::cout << "===                  ===\n\n";
#endif
    
    return evaluate_rpn(os, oq);
}
