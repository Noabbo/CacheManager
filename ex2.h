//
// Created by noa on 03/12/2019.
//

#ifndef EX2_EX2_H
#define EX2_EX2_H

//class ex2 {};
#include <iostream>
#include <list>
#include <iterator>
#include <unordered_map>
#include <string>
#include <fstream>
using namespace std;
template <class T> class CacheManager {
    string capacity;
    list <T> lru_list;
    unordered_map <string, typename list<T>::iterator> cacheMap;
public:
    // constructor
    explicit CacheManager(int c) {
        this->capacity = to_string(c);
        //this->lru_list = new list<T>(stoi(this->capacity));
        //this->cacheMap = new unordered_map<string, typename list<T>::iterator>(stoi(this->capacity));
    }
    void insert(string key, T obj) {
        string s = T::class_name + "_" + key;
        // open file for new item
        fstream file;
        file.open(s, ios::out | ios::binary);
        if (!file) {
            throw std::invalid_argument("error - file didn't open");
        }
        typename unordered_map <string, typename list<T>::iterator>::iterator find = this->cacheMap.find(key);
        // key doesn't exist in cache
        if (find == this->cacheMap.end()) {
            // adding object to file
            file.write((char*)&obj, sizeof(T));
        } else {
            // key exists in cache
            // modifying data in file
            int pos = file.tellg();
            file.seekg(pos);
            file.write((char*)&obj, sizeof(T));
        }
        // move object to front of the line
        moveMRU(key, obj);
        // close file
        file.close();
    }
    T get(string key) {
        T ob;
        string s = T::class_name + "_" + key;
        // open file for new item
        fstream file;
        file.open(s, ios::in | ios::binary);
        typename unordered_map <string, typename list<T>::iterator>::iterator find = this->cacheMap.find(key);
        // object was not found in cache or in files
        if ((!file) && (find == this->cacheMap.end())) {
            throw std::invalid_argument("error - key doesn't exist");
        } else if (find != this->cacheMap.end()) {
            // object was found in cache
            ob = (*this->cacheMap[key]);
        } else if (file) {
            // object not in cache but has a file
            file.read((char*)&ob, sizeof(ob));
        }
        // move object to front of the line
        moveMRU(key, ob);
        file.close();
        return ob;
    }
    void foreach(void(func(T&))) {
        for (T obj : this->lru_list) {
            func(obj);
        }
    }

    void moveMRU(string key, T obj) {
        // object doesn't exist in cache - add new object as mru
        if (this->cacheMap.find(key) == this->cacheMap.end()) {
            // cache has reached its limit
            if ((stoi(this->capacity)) <= this->lru_list.size()) {
                // removing lru object from map and list
                auto itrRemove = prev(this->lru_list.cend());
                for(typename unordered_map<string, typename list<T>::iterator>::iterator iter = this->cacheMap.begin();
                    iter != this->cacheMap.end(); ++iter)
                {
                    auto value =  iter->second;
                    if (value == itrRemove) {
                        this->cacheMap.erase(iter->first);
                        break;
                    }
                }
                this->lru_list.pop_back();
            }
            // adding mru new object to map and list
            this->lru_list.push_front(obj);
            auto itr = this->lru_list.begin();
            this->cacheMap.insert({key, itr});
        } else {
            // object exists in cache
            this->lru_list.erase(this->cacheMap[key]);
            // moving object in map and list as mru
            this->lru_list.push_front(obj);
            auto itr = this->lru_list.begin();
            this->cacheMap[key] = itr;
        }
    }
    ~CacheManager()= default;
};

#endif //EX2_EX2_H
