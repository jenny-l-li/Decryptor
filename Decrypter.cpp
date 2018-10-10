//
//  Decrypter.cpp
//  proj4
//
//  Created by Jenny Li on 3/9/18.
//  Copyright © 2018 Jenny Li. All rights reserved.
//


// must not use the STL map or unordered_map classes
// use stl sort pls

#include "provided.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

class DecrypterImpl
{
public:
    DecrypterImpl();
    bool load(string filename);
    vector<string> crack(const string& ciphertext);
private:
    WordList m_words;
    Translator m_trans;
    Tokenizer m_token;
    bool fullyTranslated(string word) const;
    string getUntranslated(string word) const;
    void crackHelper(const string& ciphertext, string& partial, vector<string>& deciphered);
};

//Public:
DecrypterImpl::DecrypterImpl() : m_token(" 1234567890,;:.!()[]{}-\"#$%^&")
{}

bool DecrypterImpl::load(string filename)
{
    if ( !m_words.loadWordList(filename))
        return false;
    return true;
}

vector<string> DecrypterImpl::crack(const string& ciphertext) //need helper function help
{
    vector<string> result;
    string transString = getUntranslated(ciphertext);
    crackHelper(ciphertext, transString, result);
    return result;
}

//Private:
void DecrypterImpl::crackHelper(const string& ciphertext, string& partial, vector<string>& deciphered)
{
    vector<string> ciphervec = m_token.tokenize(ciphertext); //vector of ciphertext words
    vector<string> transvec = m_token.tokenize(partial);
    
    //pick word with most untranslated letters
    string cipherWord;
    if (!transvec.empty())
        cipherWord = ciphervec[0];
    int maxCount = 0;
    for (int i = 0; i < transvec.size(); i++) //has not yet been chosen
    {
        int count = 0;
        for (int j = 0; j < transvec[i].size(); j++) //every character in word
            if (transvec[i][j] == '?') //count untranslated letters
                count++;
        if (count > maxCount) {
            maxCount = count;
            cipherWord = ciphervec[i];
        }
    }
    //translate chosen word
    string transWord = m_trans.getTranslation(cipherWord);
    
    //locate matching English words
    vector<string> candidates = m_words.findCandidates(cipherWord, transWord);
    
    if (candidates.empty()) //no compatible words
    {
        m_trans.popMapping();
        return;
    }
    
    //update mapping table
    for (int i = 0; i < candidates.size(); i++) //for every candidate word
    {
        bool validpush = m_trans.pushMapping(cipherWord, candidates[i]);
        if (!validpush)
            continue;
        string transString = m_trans.getTranslation(ciphertext); //decode entire message
        
        transvec = m_token.tokenize(transString);
        bool mismatch = false;
        int countTrans = 0;
        
        for (int j = 0; j < transvec.size(); j++) //check all the fully translated cipher words
        {
            if (fullyTranslated(transvec[j]))
            {
                if (!m_words.contains(transvec[j])) //one mismatch
                {
                    mismatch = true;
                    m_trans.popMapping();
                    break;
                }
                countTrans++;
            }
        }
        
        if (!mismatch) {
            if (countTrans == ciphervec.size()) {
                deciphered.push_back(transString);
                m_trans.popMapping();
            }
            else  //else, continue with next candidate word
                crackHelper(ciphertext, transString, deciphered);
        }
    }
    m_trans.popMapping();
    sort(deciphered.begin(), deciphered.end());
} //return

bool DecrypterImpl::fullyTranslated(string word) const
{
    for (int i = 0; i < word.size(); i++)
        if (word[i] == '?')
            return false;
    return true;
}
string DecrypterImpl::getUntranslated(string word) const
{
    string res = "";
    for (int i = 0; i < word.size(); i++) {
        if (isalpha(word[i]))
            res += '?';
        else
            res += word[i];
    }
    return res;
}

//******************** Decrypter functions ************************************

// These functions simply delegate to DecrypterImpl's functions.
// You probably don't want to change any of this code.

Decrypter::Decrypter()
{
    m_impl = new DecrypterImpl;
}

Decrypter::~Decrypter()
{
    delete m_impl;
}

bool Decrypter::load(string filename)
{
    return m_impl->load(filename);
}

vector<string> Decrypter::crack(const string& ciphertext)
{
    return m_impl->crack(ciphertext);
}

