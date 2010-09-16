/*
 *  tokeniser.cpp
 *  Untitled
 *
 *  Created by Ellie on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cctype>
#include <cstdio>
#include <cstring>
#include <map>

#include "tokeniser.h"

std::map<const String, basic::Token> basic::Tokeniser::keywords = std::map<const String, basic::Token>();

void basic::Tokeniser::setupKeywords(void) {
    if (keywords.empty()) {
        keywords["and"] = TkAND;
        keywords["or"] = TkOR;
        keywords["not"] = TkNOT;
        keywords["print"] = TkPRINT;
        keywords["input"] = TkINPUT;
        keywords["let"] = TkLET;
        keywords["mod"] = TkMOD;
        keywords["if"] = TkIF;
        keywords["then"] = TkTHEN;
        keywords["elseif"] = TkELSEIF;
        keywords["else"] = TkELSE;
        keywords["end"] = TkEND;
        keywords["do"] = TkDO;
        keywords["loop"] = TkLOOP;
        keywords["while"] = TkWHILE;
        keywords["until"] = TkUNTIL;
        keywords["done"] = TkDONE;
        keywords["for"] = TkFOR;
        keywords["to"] = TkTO;
        keywords["step"] = TkSTEP;
        keywords["next"] = TkNEXT;
        keywords["function"] = TkFUNCTION;
        keywords["as"] = TkAS;
        keywords["return"] = TkRETURN;
        keywords["sub"] = TkSUB;
        keywords["call"] = TkCALL;
        keywords["exit"] = TkEXIT;
        keywords["dim"] = TkDIM;
        keywords["integer"] = TkINTEGER;
        keywords["real"] = TkREAL;
        keywords["string"] = TkSTRING;
        keywords["variant"] = TkVARIANT;
    }
}

const char* basic::Tokeniser::tokenDescriptions[] = {
    "invalid token",
    "end of file", "end of line", ":", ",", ";",
    "identifier", "literal",
    "+", "-", "*", "/",
    "(", ")", "[", "]",
    "=", "<>", "<", ">", "<=", ">=",
    "and", "or", "not",
    "print", "input",
    "let",
    "mod",
    "if", "then", "elseif", "else", "end",
    "do", "loop", "while", "until", "done",
    "for", "to", "step", "next",
    "function", "as", "return", "sub", "call", "exit",
    "dim", "integer", "real", "string", "variant",
};

basic::Tokeniser::Tokeniser(const char * filename) {
    this->source = fopen(filename, "r");
    this->token_line = this->cursor_line = 0;
    this->token_column = this->cursor_column = 0;
    this->value = Variant();
    setupKeywords();
}

basic::Tokeniser::Tokeniser(int fd) {
    this->source = fdopen(fd, "r");
    this->token_line = this->cursor_line = 0;
    this->token_column = this->cursor_column = 0;
    this->value = Variant();
    setupKeywords();
}

basic::Token basic::Tokeniser::getToken(void) {
    Token token;
    int ch;
    
    skipWhitespace();
    updateTokenPosition();
    switch ((ch = getChar())) {
        case EOF:   token = TkEOF;          break;  // FIXME think about this
        case '+':   token = TkPLUS;         break;
        case '-':   token = TkMINUS;        break;
        case '*':   token = TkMULTIPLY;     break;
        case '/':   token = TkDIVIDE;       break;
        case '(':   token = TkLPAREN;       break;
        case ')':   token = TkRPAREN;       break;
        case '[':   token = TkLBRACKET;     break;
        case ']':   token = TkRBRACKET;     break;
        case ':':   token = TkCOLON;        break;
        case '\n':  
            token = TkEOL;
            while (peekChar() == '\n')  {
                getChar();  // consume \n+ as if it were \n   
            }
            break;
        case '=':   token = TkEQUALS;       break;
        case '>':
            if (peekChar() == '=') {
                getChar();  // consume the '='
                token = TkGTEQUALS;
            }
            else {
                token = TkGT;
            }
            break;
        case '<':
            switch (peekChar()) {
                case '>':
                    getChar();  // consume the '>'
                    token = TkNOTEQUALS;
                    break;
                case '=':
                    getChar();  // consume the '='
                    token = TkLTEQUALS;
                    break;
                default:
                    token = TkLT;
                    break;
            }
            break;
        case '"':
            token = readQuoted(ch);
            break;            
        default:
            if (isnumber(ch)) {
                token = readNumeric(ch);
            }
            else if (isalpha(ch)) {
                token = readAlphanumeric(ch);
            }
            else {
                token = TkINVALID;
                fprintf(stderr, "Unrecognised token at line %i, column %i: \"%c...\"\n",
                        token_line, token_column, ch);
            }
            break;
    }
    
    return token;
}

// Tries to parse an integer or double from the source.  The first digit is provided in 
// 'first' as a character (i.e. this is not an int value!).
// Sets 'value' to the value found.
// Hint: to start without supplying an initial digit, pass in a leading '0'.
basic::Token basic::Tokeniser::readNumeric(char first) {
    int ch = first, i;
    double d, div;
    
    i = digittoint(ch);
    while (isnumber(peekChar())) {
        ch = getChar();  // consume it
        i = 10 * i + digittoint(ch);
    }
    if (peekChar() == '.') {  
        // found a decimal place
        getChar();  // consume the '.'
        d = i;
        div = 10.0;
        while (isnumber(peekChar())) {
            ch = getChar();
            d += digittoint(ch) / div;
            div *= 10.0;
        }
        // FIXME if (tolower(peekChar()) == 'e')  parse an exponent as well
        value.setDoubleValue(d);
    }
    else {
        // it's just an integer
        value.setIntValue(i);
    }
    return TkLITERAL;
}

// Tries to parse an alphanumeric string.  The first character is provided in 'first'.
// If the string is not a recognised keyword, sets 'value' to the string found and returns 
// TkIDENTIFIER; otherwise returns an appropriate Token value for the keyword matched.
// This function treats the underscore character as an alphanumeric character.
basic::Token basic::Tokeniser::readAlphanumeric(char first) {
    Token token;
    int ch = first, i = 0, len = MAX_IDENTIFIER_LENGTH;
    char *buffer = new char[len];
    
    buffer[i++] = ch;
    while ((ch = peekChar()) and (ch == '_' or isalnum(ch))) {
        if (len - i <= 2) {
            char *tmp = buffer;
            len *= 2;
            buffer = new char[len];
            strcpy (buffer, tmp);
            delete[] tmp;
        }
        ch = getChar();  // consume the char
        buffer[i++] = ch;
    }
    buffer[i] = '\0';
    
    if (keywords.count(buffer) != 0) {
        // it's in the keywords table
        token = keywords[buffer];
    }
    else {
        // if not, assume it's an identifier
        // FIXME possibly look up a symbol table to identify valid identifiers? -- but when defining one, it won't be there yet
        token = TkIDENTIFIER;
        value.setStringValue(buffer);
    }
    
    delete[] buffer;
    return token;
}

// Tries to parse a quoted string constant.  The 'first' argument is there for consistency;
// its value is assumed to be a double-quote character and is ignored.
// On success, stores the string parsed in 'value' and returns TkSTRING.  On failure
// returns TkINVALID;
basic::Token basic::Tokeniser::readQuoted(char first) {
    int ch = first, i = 0, len = 32;
    char *buffer = new char[len];
    bool found_closing_double_quote = false;
    
    while (!found_closing_double_quote) {
        if (len - i <= 2) {
            char *tmp = buffer;
            len *= 2;
            buffer = new char[len];
            strcpy(buffer, tmp);
            delete[] tmp;
        }
        switch ((ch = getChar())) {
            case '"':
                if (peekChar() == '"') {  // "" to escape a "
                    getChar();  // consume the second "
                    buffer[i++] = ch;
                }
                else {
                    found_closing_double_quote = true;
                }
                break;
            case '\\':
                switch (peekChar()) {
                    case '\\':  getChar(); buffer[i++] = '\\'; break;
                    case '\'':  getChar(); buffer[i++] = '\''; break;
                    case '"':   getChar(); buffer[i++] = '"';  break;
                    case 'a':   getChar(); buffer[i++] = '\a'; break;
                    case 'b':   getChar(); buffer[i++] = '\b'; break;
                    case 'f':   getChar(); buffer[i++] = '\f'; break;
                    case 'n':   getChar(); buffer[i++] = '\n'; break;
                    case 'r':   getChar(); buffer[i++] = '\r'; break;
                    case 't':   getChar(); buffer[i++] = '\t'; break;
                    case 'v':   getChar(); buffer[i++] = '\v'; break;
                        
                    case 'x':
                        buffer[i++] = readHexByte();
                        break;
                        
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                        buffer[i++] = readOctalByte();
                        break;
                    
                    default:  // unrecognised escape sequence, keep the backslash and carry on as usual
                        buffer[i++] = ch;
                }
                
            case EOF:  // FIXME handle this differently from \n?
            case '\n':
                buffer[i] = '\0';
                fprintf(stderr, "Unterminated string literal at line %i, column %i: \"%s\"\n",
                        token_line, token_column, buffer);
                delete[] buffer;
                return TkINVALID;
            
            default:
                buffer[i++] = ch;
        }
    }
    buffer[i] = '\0';
    value.setStringValue(buffer);
    delete[] buffer;
    return TkLITERAL;
}

int basic::Tokeniser::readHexByte(void) {
    int val;
    char digits[3], *endptr;
    int i = 0, line = cursor_line, column = cursor_column;
    
    getChar(); // consume the leading 'x'
    memset(digits, 0, sizeof(digits));
    while (i < 2 and isxdigit(peekChar())) {
        digits[i] = (char) getChar();
    }
    
    val = strtol(digits, &endptr, 16);
    if (endptr != NULL) {
        fprintf(stderr, "warning: invalid characters in hex byte at line %i, column %i: `\\x%s'\n", line, column, digits);
    }
    return (char) val;
}

int basic::Tokeniser::readOctalByte(void) {
    int val;
    char digits[4], *endptr;
    int i = 0, line = cursor_line, column = cursor_column;
    
    memset(digits, 0, sizeof(digits));
    while (i < 3 and isdigit(peekChar())) {
        digits[i] = (char) getChar();
    }
    
    val = strtol(digits, &endptr, 8);
    if (endptr != NULL) {
        fprintf(stderr, "warning: invalid characters in octal byte at line %i, column %i: `\\%s'\n", line, column, digits);
    }
    if (val > 0xFF) {
        fprintf(stderr, "warning: octal byte value out of range at line %i, column %i: `\\%s'\n", line, column, digits);
    }
    return (char) val;
}

void basic::Tokeniser::skipWhitespace(void) {
    int ch=0, saved=0;
    while ((ch = peekChar()) != EOF) {
        // look ahead to see what's next
        switch (ch) {
            case '\t':  
            case ' ':
                // consume ordinary whitespace
                ch = getChar();
                break;
                
            case '\'':
            case '#':
                // consume comments until (but not including) end of line
                getChar();
                while ((ch = peekChar()) != EOF && ch != '\n') {
                    getChar();
                }
                break;
                
            case '_':
                // join up continued lines
                saved = fgetc(source);  // read the underscore
                ch = fgetc(source);     // read the next character
                ungetc(ch, source);     // put them both back
                ungetc(saved, source);
                if (isspace(ch)) {      // if continuation char is followed by whitespace...
                    ch = getChar();     // consume the continuation properly
                    ch = getChar();     // consume the space properly
                }
                else {                  // but if it's not followed by whitespace, it's not whitespace either
                    return;
                }
                break;
                
            case '\n':  // eol is significant, so treat it as non-whitespace
            default:    // break out of the loop if there's no (more) whitespace ahead
                return;
        }
    }
}
