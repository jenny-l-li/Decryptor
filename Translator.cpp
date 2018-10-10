//
//  Translator.cpp
//  proj4
//
//  Created by Jenny Li on 3/9/18.
//  Copyright © 2018 Jenny Li. All rights reserved.
//

#include "provided.h"
#include <string>
#include <vector>
#include <map>
using namespace std;
#include <iostream>

class TranslatorImpl
{
public:
    TranslatorImpl();
    bool pushMapping(string ciphertext, string plaintext);
    bool popMapping();
    string getTranslation(const string& ciphertext) const;
private:
    vector<map<char, char>> m_stack;
    map<char, char> m_currMap;
    char cipherToPlain[26];
    char plainToCipher[26];
    char getPlain(char cipher) const;
    bool findCipher(char c) const;
    void updateArrays();
};

//Private:
char TranslatorImpl::getPlain(char cipher) const
{
    cipher = toupper(cipher);
    return cipherToPlain[cipher - 'A'];
}

bool TranslatorImpl::findCipher(char c) const
{
    c = toupper(c);
    return cipherToPlain[c - 'A'] != 0;
}

void TranslatorImpl::updateArrays()
{
    for (int i = 0; i < 26; i++)
    {
        cipherToPlain[i] = 0;
        plainToCipher[i] = 0;
    }
    for (map<char,char>::const_iterator it = m_currMap.begin(); it != m_currMap.end(); it++)
    {
        const char c = it->first;
        const char p = it->second;
        cipherToPlain[c-'A'] = p;
        plainToCipher[p-'A'] = c;
    }
}

//Public:
TranslatorImpl::TranslatorImpl()
{
    for (int i = 0; i < 26; i++)
    {
        cipherToPlain[i] = 0;
        plainToCipher[i] = 0;
    }
}

bool TranslatorImpl::pushMapping(string ciphertext, string plaintext) //O(N+L) time, where N is the length of the parameter strings and L is the number of letters in the English alphabet, but essentially O(1)
{
    if (ciphertext.size() != plaintext.size())
        return false;

    //Checks
    for (int i = 0; i < ciphertext.size(); i++) //O(N)
    {
        if (!isalpha(ciphertext[i]) || !isalpha(plaintext[i]))
            return false;
        if (cipherToPlain[toupper(ciphertext[i]) - 'A'] != 0 &&
            cipherToPlain[toupper(ciphertext[i]) - 'A'] != toupper(plaintext[i])) //cipher has a different plain mapping
            return false;
        if (plainToCipher[toupper(plaintext[i]) - 'A'] != 0 &&
            plainToCipher[toupper(plaintext[i]) - 'A'] != toupper(ciphertext[i])) //plain has a different cipher mapping
            return false;
    }
    
    map<char, char> copy = m_currMap;

    if (!copy.empty()) //O(1)
        m_stack.push_back(copy);
    
    for (int i = 0; i < ciphertext.size(); i++) //O(N)
    {
        const char cipher = toupper(ciphertext[i]);
        const char plain = toupper(plaintext[i]);
        m_currMap[cipher] = plain;
        cipherToPlain[cipher - 'A'] = plain;
        plainToCipher[plain - 'A'] = cipher;
    }
    return true;
}

bool TranslatorImpl::popMapping() //O(L) = letters in alphabet
{
    if (m_stack.empty() && m_currMap.empty())
        return false;
    if (m_stack.empty() && !m_currMap.empty())
    {
        m_currMap.clear();
        updateArrays(); //O(L)
        return true;
    }
    
    vector<map<char, char>>::iterator it;
    it = m_stack.end();
    it--;
    m_currMap = *it;
    updateArrays(); 
    m_stack.erase(it);
    return true;
}

string TranslatorImpl::getTranslation(const string& ciphertext) const
{
    string plaintext = "";
    for (int i = 0; i < ciphertext.size(); i++)
    {
        if (!isalpha(ciphertext[i]))
            plaintext += ciphertext[i];
        else if (findCipher(ciphertext[i]))
        {
            if (islower(ciphertext[i]))
                plaintext += tolower(getPlain(ciphertext[i]));
            else
                plaintext += getPlain(ciphertext[i]);
        }
        else
            plaintext += '?';
    }
    return plaintext;
}


//******************** Translator functions ************************************

// These functions simply delegate to TranslatorImpl's functions.
// You probably don't want to change any of this code.

Translator::Translator()
{
    m_impl = new TranslatorImpl;
}

Translator::~Translator()
{
    delete m_impl;
}

bool Translator::pushMapping(string ciphertext, string plaintext)
{
    return m_impl->pushMapping(ciphertext, plaintext);
}

bool Translator::popMapping()
{
    return m_impl->popMapping();
}

string Translator::getTranslation(const string& ciphertext) const
{
    return m_impl->getTranslation(ciphertext);
}

