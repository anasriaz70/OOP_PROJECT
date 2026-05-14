#pragma once
#include "Node.h"
#include <vector>
#include <cstdint>

class Folder;

class ZipFile : public Node {

    struct Entry {
        string   zipPath;           
        string   diskPath;          
        bool     isDir;
        uint32_t crc32;
        uint32_t compressedSize;   
        uint32_t uncompressedSize;
        uint32_t localHeaderOffset;
    };

    Node* sourceNode;   

    void collectEntries(Node* node, const string& basePath,
                        vector<Entry>& entries) const;

    static uint32_t crc32Buf(const char* data, size_t len);
    static void     writeU16(ostream& os, uint16_t v);
    static void     writeU32(ostream& os, uint32_t v);
    void            buildZip(const string& zipPath,
                             vector<Entry>& entries) const;

public:
    ZipFile(const string& name, Node* sourceNode, Node* parent = nullptr);

    ZipFile(const string& name, Node* parent, bool load);

    void unZip();

    void   Open()          override;
    void   Delete()        override;
    string getType()  const override;
    void   calculateSize() override;
    string getExtension() const override;
};
