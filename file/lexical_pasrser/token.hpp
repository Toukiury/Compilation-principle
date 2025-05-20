#pragma once

#include <iostream>
#include <string>

/**
 * @brief Token class for lexical analysis results
 * 
 * Represents the output of lexical analysis with token type,
 * value, and source position information.
 */
class Token
{
private:
    // Internal state
    int token_type;          ///< Type of token
    std::string token_value; ///< String value
    int lineno;              ///< Line position
    int colno;               ///< Column position

public:
    //====================================================
    // Token type constants
    //====================================================
    
    /// Error token type
    const static int ERROR = -1;
    /// End of file token
    const static int TOKENEOF = 0;
    /// Identifier token
    const static int ID = 1;
    /// Keyword token
    const static int KW = 2;
    
    // Value token types
    /// Integer value token
    const static int INT = 3;
    /// Real/float value token
    const static int REAL = 4;
    /// Character value token
    const static int CHAR = 5;
    /// String value token
    const static int STRING = 6;
    /// Boolean value token
    const static int BOOL = 7;
    
    // Syntax token types
    /// Operator token
    const static int OPERATOR = 8;
    /// Separator token
    const static int SEPARATOR = 9;
    /// Comment/annotation token
    const static int ANNO = 10;

    //====================================================
    // Constructors & Initialization
    //====================================================
    
    /**
     * @brief Default constructor
     */
    Token();
    
    /**
     * @brief Destructor
     */
    ~Token();
    
    /**
     * @brief Initialize token with data
     * 
     * @param token_type Type identifier for the token
     * @param token_value String representation of the token
     * @param lineno Source line number
     * @param colno Source column number
     */
    void fill(int token_type, std::string token_value, int lineno, int colno);
    
    //====================================================
    // Accessors
    //====================================================
    
    /**
     * @brief Get the token type
     * @return Token type identifier
     */
    int get_token_type();
    
    /**
     * @brief Get the line number
     * @return Source line number
     */
    int get_lineno();
    
    /**
     * @brief Get the column number
     * @return Source column number
     */
    int get_colno();
    
    /**
     * @brief Get token value as string
     * @return String value
     */
    std::string get_string();
    
    /**
     * @brief Get token value as integer
     * @return Integer value
     */
    long long get_int();
    
    /**
     * @brief Get token value as real number
     * @return Floating point value
     */
    double get_real();
    
    /**
     * @brief Get token value as boolean
     * @return Boolean value
     */
    bool get_bool();
    
    /**
     * @brief Get token value as character
     * @return Character value
     */
    char get_char();
    
    //====================================================
    // String conversion
    //====================================================
    
    /**
     * @brief Get type as string
     * @return String representation of token type
     */
    std::string get_type();
    
    /**
     * @brief Format token as string
     * @return Full token string representation
     */
    std::string to_string();
};
