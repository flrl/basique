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

#include "variant.h"

#define MAX_IDENTIFIER_LENGTH (31)

enum Token {
    /* special stuff */
    TkINVALID = 0,  Tk_MIN = 0,
    TkEOF, TkEOL, TkCOLON, TkCOMMA, TkSEMICOLON,
    TkIDENTIFIER, TkLITERAL, 
    
    // FIXME collapse TkINTEGER, TkREAL and TkString into a single (Variant) TkLITERAL, and then re-add "integer", "real", 
    // and "string" keywords for use in type statements?  (eg   `dim foo as string')
    
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
    TkDO, TkLOOP, TkWHILE, TkUNTIL, TkWEND,
    TkFOR, TkTO, TkSTEP, TkNEXT,
    TkFUNCTION, TkAS, TkRETURN, TkSUB, TkCALL,
    TkINTEGER, TkREAL, TkSTRING,
    
    /* that's all, folks */
    Tk_MAX,
};

struct StrComparator;
typedef std::map<const char*,Token,StrComparator> TokenMap;

class Tokeniser {
public:
    Tokeniser(const char *);
    Tokeniser(int);
    ~Tokeniser(void) { fclose(source); }
    
    Token getToken(void);
    int getLine(void) const { return token_line; }
    int getColumn(void) const { return token_column; }
    
    Variant getValue(void) const { return value; }
    
    static const char *tokenDescriptions[Tk_MAX];

private:
    static TokenMap keywords;
    static void setupKeywords(void);
    
    FILE *source;
    int token_line;
    int token_column;
    int cursor_line;
    int cursor_column;
    Variant value;
    
    void skipWhitespace(void);
    void updateTokenPosition(void) { token_line = cursor_line; token_column = cursor_column; }
    Token readNumeric(char);
    Token readAlphanumeric(char);
    Token readQuoted(char);
    
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