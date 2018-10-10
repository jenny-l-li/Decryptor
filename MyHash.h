//
//  main.cpp
//  proj4
//
//  Created by Jenny Li on 3/8/18.
//  Copyright © 2018 Jenny Li. All rights reserved.
//
#include <iostream>

#ifndef MYHASH_H
#define MYHASH_H

template<typename KeyType, typename ValueType>
class MyHash
{
public:
    MyHash(double maxLoadFactor = 0.5) //O(B)
    : m_maxLoad(maxLoadFactor), m_nBuckets(100), m_nItems(0)
    {
        if (maxLoadFactor <= 0) m_maxLoad = 0.5;
        else if (maxLoadFactor > 2.0) m_maxLoad = 2.0;
        m_buckets = new Node*[100];
        for (int i = 0; i < m_nBuckets; i++)
            m_buckets[i] = nullptr;
    }
    
    ~MyHash()
    {
        for (int i = 0; i < m_nBuckets; i++) //O(B)
            if (m_buckets[i] != nullptr)
                deleteItems(m_buckets[i]);
        delete [] m_buckets;
    }
    
    void reset() //O(B) because X total items<B
    {
        for (int i = 0; i < m_nBuckets; i++)
            if (m_buckets[i] != nullptr)
                deleteItems(m_buckets[i]);
        delete [] m_buckets;
        m_nBuckets = 100;
        m_nItems = 0;
        m_buckets = new Node*[100];
        for (int i = 0; i < m_nBuckets; i++)
            m_buckets[i] = nullptr;
    }
    
    void associate(const KeyType& key, const ValueType& value) //O(1); O(X) pathological
    {
        ValueType* v = find(key);
        if (v == nullptr) {
            addItem(key, value, m_buckets[getBucketNumber(key)]);
            m_nItems++;
            if (getLoadFactor() > m_maxLoad) //move items, delete old array, allocate new array; O(B)
                reallocate();
        }
        else
            *v = value;
    }
    
    int getNumItems() const { return m_nItems; }
    //maxLoadFactor = max # of values to insert/total buckets in array
    double getLoadFactor() const { return (double)m_nItems/m_nBuckets; }
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const //O(1), pathological O(X)
    {
        Node* p = m_buckets[getBucketNumber(key)];
        while (p != nullptr)
        {
            if (p->k == key)
                return &(p->v);
            p = p->next;
        }
        return nullptr;
    }
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyHash*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyHash(const MyHash&) = delete;
    MyHash& operator=(const MyHash&) = delete;
    
private:
    struct Node
    {
        KeyType   k;
        ValueType v;
        Node*     next;
    };
    
    Node** m_buckets;
    int   m_nBuckets;
    int   m_nItems;
    double m_maxLoad;
    void addItem(const KeyType& key, const ValueType& value, Node*& ptr)
    {
        Node* n = new Node;
        n->k = key;
        n->v = value;
        n->next = ptr;
        ptr = n;
    }
    
    unsigned int getBucketNumber(const KeyType& key)    const
    {
        unsigned int hash(const KeyType& k);  //prototype
        unsigned int h = hash(key);
        return h % m_nBuckets;
    }
    
    void deleteItems(Node*& p)
    {
        while (p != nullptr)
        {
            Node *temp = p->next;
            delete p;
            p = temp;
        }
    }
    
    void reallocate()
    {
        m_nItems = 0;
        int oldBuckets = m_nBuckets;
        m_nBuckets *= 2; //now 200 buckets
        Node** temp = new Node*[m_nBuckets];
        for (int i = 0; i < m_nBuckets; i++) //new hash table
            temp[i] = nullptr;
        for (int i = 0; i < oldBuckets; i++)
        {
            Node* p = m_buckets[i];
            while (p != nullptr)
            {
                addItem(p->k, p->v, temp[getBucketNumber(p->k)]);
                m_nItems++;
                Node *nextNode = p->next;
                delete p;
                p = nextNode;
            }
        }
        delete [] m_buckets;
        m_buckets = temp;
        temp = nullptr;
    }
};

#endif
