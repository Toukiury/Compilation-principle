#include "token.hpp"
#include <string>

// Default constructor implementation
Token::Token()
{
    // Empty constructor
}

/**
 * @brief Populate token with all required data
 * @param token_type Type identifier for the token
 * @param token_value String representation of the token
 * @param lineno Source line number
 * @param colno Source column number
 */
void Token::fill(int token_type, std::string token_value, int lineno, int colno)
{
    this->token_type = token_type;
    this->token_value = token_value;
    this->lineno = lineno;
    this->colno = colno;
}

// Virtual destructor implementation
Token::~Token()
{
    // Empty destructor
}

/**
 * @brief Retrieves the token type identifier
 * @return Integer representing token type
 */
int Token::get_token_type()
{
    return token_type;
}

/**
 * @brief Retrieves the line number in source
 * @return Source line number
 */
int Token::get_lineno()
{
    return lineno;
}

/**
 * @brief Retrieves the column number in source
 * @return Source column number
 */
int Token::get_colno()
{
    return colno;
}

/**
 * @brief Get raw token string value
 * @return Token as string
 */
std::string Token::get_string()
{
    return token_value;
}

/**
 * @brief Convert token value to integer
 * @return Integer value of the token
 */
long long Token::get_int()
{
    return std::stoll(token_value);
}

/**
 * @brief Convert token value to real number
 * @return Real (floating point) value
 */
double Token::get_real()
{
    return std::stod(token_value);
}

/**
 * @brief Convert token value to boolean
 * @return Boolean value (true/false)
 */
bool Token::get_bool()
{
    return (token_value == "true");
}

/**
 * @brief Extract character value from token
 * @return Character value
 */
char Token::get_char()
{
    return token_value[1];
}

/**
 * @brief Convert token type to human-readable string
 * @return String representation of token type
 */
std::string Token::get_type()
{
    switch (token_type)
    {
        case ERROR:     return "ERROR";
        case TOKENEOF:  return "TOKENEOF";
        case ID:        return "ID";
        case KW:        return "KW";
        case INT:       return "INT";
        case REAL:      return "REAL";
        case CHAR:      return "CHAR";
        case STRING:    return "STRING";
        case BOOL:      return "BOOL";
        case OPERATOR:  return "OPERATOR";
        case SEPARATOR: return "SEPARATOR";
        case ANNO:      return "ANNO";
        default:        return "ERROR";
    }
}

/**
 * @brief Format token as display string with metadata
 * @return Formatted string representation with position info
 */
std::string Token::to_string()
{
    std::string position = "[" + std::to_string(lineno) + "," + std::to_string(colno) + "]";
    std::string type_str = get_type();
    std::string result = "[Token] " + type_str + " ";
    
    switch (token_type)
    {
        case BOOL:
            result += (get_bool() ? "true" : "false");
            break;
            
        case CHAR:
            result += get_char();
            break;
            
        case STRING:
            // Handle string formatting - remove surrounding quotes
            if (token_value.length() >= 2) {
                result += token_value.substr(1, token_value.length() - 2);
            } else {
                result += token_value;
            }
            break;
            
        case INT:
            result += std::to_string(get_int());
            break;
            
        case REAL:
            result += std::to_string(get_real());
            break;
            
        case ID:
        case KW:
        case OPERATOR:
        case SEPARATOR:
        case ANNO:
            result += get_string();
            break;
            
        case TOKENEOF:
        case ERROR:
            // No additional text for these types
            break;
            
        default:
            return "[Token] DEFAULT TOKENTYPE line : >>> " + 
                   std::to_string(lineno) + " col : >>> " + 
                   std::to_string(colno);
    }
    
    result += " " + position;
    return result;
}
