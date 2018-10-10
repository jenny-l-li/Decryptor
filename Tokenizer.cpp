//
//  Tokenizer.cpp
//  proj4
//
//  Created by Jenny Li on 3/9/18.
//  Copyright © 2018 Jenny Li. All rights reserved.
//
#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class TokenizerImpl
{
public:
    TokenizerImpl(string separators);
    vector<string> tokenize(const std::string& s) const;
private:
    std::string m_sep;
    bool isSeparator(const char c) const;
};

//Public:
TokenizerImpl::TokenizerImpl(string separators) : m_sep("") //O(num of separators), or O(P)
{
    for (int i = 0; i < separators.size(); i++)
        m_sep += separators[i];
}

vector<string> TokenizerImpl::tokenize(const std::string& s) const //O(num of char in input string * P); O(SP)
{
    vector<string> v;
    string curr = "";
    bool token = false;
    for (int i = 0; i < s.size(); i++)
    {
        if(isSeparator(s[i]))
        {
            if (token)
            {
                v.push_back(curr);
                token = false;
                curr = "";
            }
        }
        else //not separator
        {
            token = true;
            curr += s[i];
        }
    }
    if (token) v.push_back(curr);
    return v;
}

//Private:
bool TokenizerImpl::isSeparator(const char c) const //O(P)
{
    for (int i = 0; i < m_sep.size(); i++)
        if (m_sep[i] == c) return true;
    return false;
}

//******************** Tokenizer functions ************************************

// These functions simply delegate to TokenizerImpl's functions.
// You probably don't want to change any of this code.

Tokenizer::Tokenizer(string separators)
{
    m_impl = new TokenizerImpl(separators);
}

Tokenizer::~Tokenizer()
{
    delete m_impl;
}

vector<string> Tokenizer::tokenize(const std::string& s) const
{
    return m_impl->tokenize(s);
}
