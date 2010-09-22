/*
 *  tokeniser.h
 *  Untitled
 *
 *  Created by Ellie on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef BASIQUE_TOKENISER_H
#define BASIQUE_TOKENISER_H

#include <cstdio>
#include <map>

#include "string.h"
#include "variant.h"

#define MAX_IDENTIFIER_LENGTH (31)

namespace basic {
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

class basic::Tokeniser {
public:
    Tokeniser(const char *);
    Tokeniser(FILE *);
    ~Tokeniser(void) { fclose(m_source); }
    
    Token getToken(void);
    int getLine(void) const { return m_token_line; }
    int getColumn(void) const { return m_token_column; }
    
    basic::Variant getValue(void) const { return m_value; }
    
    static const char *tokenDescriptions[Tk_MAX];

private:
    static std::map<const String, Token> keywords;
    static void setupKeywords(void);
    
    FILE *m_source;
    int m_token_line;
    int m_token_column;
    int m_cursor_line;
    int m_cursor_column;
    basic::Variant m_value;
    
    void skipWhitespace(void);
    void updateTokenPosition(void) { m_token_line = m_cursor_line; m_token_column = m_cursor_column; }
    Token readNumeric(char);
    Token readAlphanumeric(char);
    Token readQuoted(char);
    int readHexByte(void);
    int readOctalByte(void);
    
    int getChar(void) {
        if (feof(m_source))  return EOF;
        int ch = fgetc(m_source);
        switch (ch) {
            case '\n':  m_cursor_column = 1;  m_cursor_line++;  break;
            case '\t':  m_cursor_column += 1 + 8 - (m_cursor_column % 8);  break;
            default:    m_cursor_column++;  break;
        }
        return ch;  // FIXME handle EOF/error correctly?
    }
    
    int peekChar(void) {
        if (feof(m_source))  return EOF;
        int ch = fgetc(m_source);
        ungetc(ch, m_source);
        return ch;
    }
};

#endif
