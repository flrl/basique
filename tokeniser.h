/*
 *  tokeniser.h
 *  Untitled
 *
 *  Created by Ellie on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _TOKENISER_H
#define _TOKENISER_H

#include <cstdio>
#include <map>

#include "string.h"
#include "variant.h"

#define MAX_IDENTIFIER_LENGTH (31)

namespace Basic {
    class Tokeniser;
    enum Token {
        /* special stuff */
        TkINVALID = 0,  Tk_MIN = 0,
        TkEOF, TkEOL, TkCOLON, TkCOMMA, TkSEMICOLON,
        TkIDENTIFIER, TkLITERAL, 
        
        /* symbols */
        TkPLUS, TkMINUS, TkMULTIPLY, TkDIVIDE, 
        TkLPAREN, TkRPAREN, TkLBRACKET, TkRBRACKET,
        TkEQUALS, TkNOTEQUALS, TkLT, TkGT, TkLTEQUALS, TkGTEQUALS,

        /* keywords */
        TkAND, TkOR, TkNOT,
        TkPRINT, TkINPUT,
        TkLET,
        TkMOD,
        TkIF, TkTHEN, TkELSEIF, TkELSE, TkEND,
        TkDO, TkLOOP, TkWHILE, TkUNTIL, TkDONE,
        TkFOR, TkTO, TkSTEP, TkNEXT,
        TkFUNCTION, TkAS, TkRETURN, TkSUB, TkCALL, TkEXIT,
        TkDIM, TkINTEGER, TkREAL, TkSTRING, TkVARIANT,
        
        /* that's all, folks */
        Tk_MAX,
    };
}

class Basic::Tokeniser {
public:
    Tokeniser(const char *);
    Tokeniser(int);
    ~Tokeniser(void) { fclose(source); }
    
    Token getToken(void);
    int getLine(void) const { return token_line; }
    int getColumn(void) const { return token_column; }
    
    Basic::Variant getValue(void) const { return value; }
    
    static const char *tokenDescriptions[Tk_MAX];

private:
    static std::map<const String, Token> keywords;
    static void setupKeywords(void);
    
    FILE *source;
    int token_line;
    int token_column;
    int cursor_line;
    int cursor_column;
    Basic::Variant value;
    
    void skipWhitespace(void);
    void updateTokenPosition(void) { token_line = cursor_line; token_column = cursor_column; }
    Token readNumeric(char);
    Token readAlphanumeric(char);
    Token readQuoted(char);
    int readHexByte(void);
    int readOctalByte(void);
    
    int getChar(void) {
        int ch = fgetc(source);
        switch (ch) {
            case '\n':  cursor_column = 0;  cursor_line++;  break;
            case '\t':  cursor_column += cursor_column - cursor_column & 8;  break;
            default:    cursor_column++;  break;
        }
        return ch;  // FIXME handle EOF/error correctly?
    }
    
    int peekChar(void) {
        int ch = fgetc(source);
        ungetc(ch, source);
        return ch;
    }
};

#endif
