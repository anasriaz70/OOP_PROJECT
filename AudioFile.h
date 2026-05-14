#pragma once
#include "Node.h"

class AudioFile : public Node {
public:
    AudioFile(const string& name, Node* parent = nullptr);
    AudioFile(const string& name, Node* parent, bool load);

    void Open();         
    void Delete();        
    string getType() const;
    void calculateSize();
    string getExtension() const;
};
