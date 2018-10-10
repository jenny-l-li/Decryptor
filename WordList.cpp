//
//  WordList.cpp
//  proj4
//
//  Created by Jenny Li on 3/9/18.
//  Copyright © 2018 Jenny Li. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <functional>
#include "provided.h"
#include <string>
#include <vector>
#include "MyHash.h"
using namespace std;

class WordListImpl
{
public:
    WordListImpl();
    bool loadWordList(string filename);
    bool contains(string word) const;
    vector<string> findCandidates(string cipherWord, string currTranslation) const;
private:
    MyHash<string, string> m_wList; //all words in text file
    MyHash<string, vector<string>> m_patternList; //maps pattern to words
    string getPattern(string word) const;
};

//Private:
string WordListImpl::getPattern(string word) const //O(L); L is length of word
{
    int pattBool[26];
    char pattChar[26];
    char patternChar = 'A';
    for (int i = 0; i < word.size(); i++)
    {
        const char c = toupper(word[i]);
        pattBool[c-'A'] = true;
    }
    for (int i = 0; i < word.size(); i++)
    {
        const char c = toupper(word[i]);
        if (pattBool[c-'A']) {
            pattChar[c-'A'] = patternChar; //record char
            pattBool[c-'A'] = false;
            patternChar++;
        }
    }
    string pattern = "";
    for (int i = 0; i < word.size(); i++)
    {
        const char c = toupper(word[i]);
        pattern += pattChar[c-'A'];
    }
    return pattern;
}

//Public:
WordListImpl::WordListImpl() {}

bool WordListImpl::loadWordList(string filename) //O(W) (treat length of words as constant)
{
    ifstream infile(filename);   // infile is a name of our choosing
    if ( !infile )    // Did the creation fail?
        return false;
    
    string s;
    m_wList.reset(); 
    m_patternList.reset();
    
    while (getline(infile, s))
    {
        bool validChar = true;
        for (int i = 0; i < s.size(); i++) //O(L) = O(1) for our purposes
        {
            if (!isalpha(s[i]) && s[i] != '\'')
            {
                validChar = false;
                break;
            }
            s[i] = tolower(s[i]);
        }
        if (!validChar)
            continue;
        
        m_wList.associate(s, s);
        string myPattern = getPattern(s); //O(L) = O(1) for our purposes
        vector<string> *v = m_patternList.find(myPattern);

        if (v != nullptr) //pattern exists in map
        {
            v->push_back(s);
        }
        else
        {
            vector<string> newVec;
            newVec.push_back(s);
            m_patternList.associate(myPattern, newVec);
        }
    }
    return true;
}

bool WordListImpl::contains(string word) const //O(L) = O(1) for our purposes
{
    for (int i = 0; i < word.size(); i++)
        word[i] = tolower(word[i]);
    if (m_wList.find(word) == nullptr)
        return false;
    return true;
}

vector<string> WordListImpl::findCandidates(string cipherWord, string currTranslation) const //O(Q) where Q is words that match pattern
{
    string pattern = getPattern(cipherWord);
    vector<string> const *v = m_patternList.find(pattern); //create new vector
    
    if (v != nullptr)
    {
        vector<string> const w = *v; //copy of v
        vector<string> candidates;
        for (int i = 0; i < w.size(); i++) //all words with the pattern: O(Q)
        {
            bool badCand = false;
            for (int j = 0; j < currTranslation.size(); j++) //treat currTranslation's size as constant
            {
                if (isalpha(currTranslation[j]))
                {
                    if (!isalpha(cipherWord[j]) || tolower(currTranslation[j]) != tolower(w[i][j])) {
                        badCand = true;
                        break;
                    }
                }
                else if (currTranslation[j] == '?')
                {
                    if (!isalpha(cipherWord[j]) || !isalpha(w[i][j])) {
                        badCand = true;
                        break;
                    }
                }
                else if (currTranslation[j] == '\'')
                {
                    if (cipherWord[j] != '\'' || w[i][j] != '\'') {
                        badCand = true;
                        break;
                    }
                }
            }
            if (!badCand)
                candidates.push_back(w[i]);
        }
        return candidates;
    }
    return vector<string>();
}

//***** hash functions for string, int, and char *****

unsigned int hash(const std::string& s)
{
    return std::hash<std::string>()(s);
}

unsigned int hash(const int& i)
{
    return std::hash<int>()(i);
}

unsigned int hash(const char& c)
{
    return std::hash<char>()(c);
}

//******************** WordList functions ************************************

// These functions simply delegate to WordListImpl's functions.
// You probably don't want to change any of this code.

WordList::WordList()
{
    m_impl = new WordListImpl;
}

WordList::~WordList()
{
    delete m_impl;
}

bool WordList::loadWordList(string filename)
{
    return m_impl->loadWordList(filename);
}

bool WordList::contains(string word) const
{
    return m_impl->contains(word);
}

vector<string> WordList::findCandidates(string cipherWord, string currTranslation) const
{
    return m_impl->findCandidates(cipherWord, currTranslation);
}
