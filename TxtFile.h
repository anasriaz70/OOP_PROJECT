#pragma once
#include "Node.h"

using namespace std;
class TxtFile :public Node{
protected:
    int totalLines;
    int activeLine;
    string* text;
public:
    TxtFile(const string& name, Node* parent = nullptr);
    TxtFile(const string& name, Node* parent, bool skipCreate);
    void Read();
    void moveUp();
    void moveDown();
    void addNewLine();
    void editExistingLine();
    void displayContents();
    virtual void savefile();
    virtual void Delete();
    virtual void Open();
    virtual string getType() const;
    virtual void calculateSize();
    string getExtension() const;
};