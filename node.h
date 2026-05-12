#pragma once
#include <iostream>
using namespace std;
class node{
    protected: 
    string name;
    node* parent;
    public:
    node(string name, node* parent) : name (name), parent(parent){}
    virtual ~node(){}
    string getter_name() const {
        return name;
    }
    void set_name(string n){
        name = n;
    }
    node* getparent()const{
        return parent;
    }
    virtual void open() = 0;
    virtual void display()= 0;
    virtual string gettype()= 0;
    virtual int getsize()= 0; 
};
