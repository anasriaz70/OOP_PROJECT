#include "ZipFile.h"
#include "Folder.h"   
#include <vector>
#include <cstdint>
#include <cstring>
#include <ctime>

namespace fs = filesystem;

void ZipFile::writeU16(ostream& os, uint16_t v)
{
    char buf[2] = { (char)(v & 0xFF), (char)((v >> 8) & 0xFF) };
    os.write(buf, 2);
}

void ZipFile::writeU32(ostream& os, uint32_t v)
{
    char buf[4] = {
        (char)(v        & 0xFF), (char)((v >>  8) & 0xFF),
        (char)((v >> 16) & 0xFF), (char)((v >> 24) & 0xFF)
    };
    os.write(buf, 4);
}

uint32_t ZipFile::crc32Buf(const char* data, size_t len)
{
    static uint32_t table[256];
    static bool     ready = false;
    if (!ready) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int k = 0; k < 8; k++)
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        ready = true;
    }

    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++)
        crc = table[(crc ^ (unsigned char)data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}

void ZipFile::collectEntries(Node* node, const string& basePath,
                              vector<Entry>& entries) const
{
    if (node->getType() == "Folder") {
        Folder* folder = dynamic_cast<Folder*>(node);

        Entry dirEntry;
        dirEntry.zipPath           = basePath + node->getName() + "/";
        dirEntry.diskPath          = "";
        dirEntry.isDir             = true;
        dirEntry.crc32             = 0;
        dirEntry.compressedSize    = 0;
        dirEntry.uncompressedSize  = 0;
        dirEntry.localHeaderOffset = 0;
        entries.push_back(dirEntry);

        string childBase = basePath + node->getName() + "/";
        for (int i = 0; i < folder->getChildCount(); i++)
            collectEntries(folder->getChild(i), childBase, entries);
    }
    else {       
        string diskPath = node->calculateParentPath()
                        + node->getName()
                        + node->getExtension();

        ifstream in(diskPath, ios::binary | ios::ate);
        uint32_t fileSize = 0;
        uint32_t crc      = 0;

        if (in.is_open()) {
            fileSize = static_cast<uint32_t>(in.tellg());
            in.seekg(0);
            if (fileSize > 0) {
                vector<char> buf(fileSize);
                in.read(buf.data(), fileSize);
                crc = crc32Buf(buf.data(), fileSize);
            }
            in.close();
        }

        Entry fileEntry;
        fileEntry.zipPath           = basePath + node->getName() + node->getExtension();
        fileEntry.diskPath          = diskPath;
        fileEntry.isDir             = false;
        fileEntry.crc32             = crc;
        fileEntry.compressedSize    = fileSize;
        fileEntry.uncompressedSize  = fileSize;
        fileEntry.localHeaderOffset = 0;
        entries.push_back(fileEntry);
    }
}
void ZipFile::buildZip(const string& zipPath, vector<Entry>& entries) const
{
    ofstream out(zipPath, ios::binary | ios::trunc);
    if (!out.is_open()) {
        cout << "Error: could not create " << zipPath << endl;
        return;
    }

    time_t  now = time(nullptr);
    tm lt_struct;
    tm* lt = &lt_struct;
    localtime_s(lt, &now);
    uint16_t dosTime = (uint16_t)(((lt->tm_hour) << 11) | ((lt->tm_min) << 5) | (lt->tm_sec / 2));
    uint16_t dosDate = (uint16_t)((((lt->tm_year - 80)) << 9) | ((lt->tm_mon + 1) << 5) | lt->tm_mday);

    for (Entry& e : entries) {
        e.localHeaderOffset = static_cast<uint32_t>(out.tellp());
        uint16_t nameLen    = static_cast<uint16_t>(e.zipPath.size());

        writeU32(out, 0x04034B50u); 
        writeU16(out, 20);          
        writeU16(out, 0);           
        writeU16(out, 0);           
        writeU16(out, dosTime);
        writeU16(out, dosDate);
        writeU32(out, e.crc32);
        writeU32(out, e.compressedSize);
        writeU32(out, e.uncompressedSize);
        writeU16(out, nameLen);
        writeU16(out, 0);           
        out.write(e.zipPath.c_str(), nameLen);

        if (!e.isDir && e.compressedSize > 0) {
            ifstream in(e.diskPath, ios::binary);
            if (in.is_open()) {
                vector<char> buf(e.compressedSize);
                in.read(buf.data(), e.compressedSize);
                out.write(buf.data(), e.compressedSize);
            }
        }
    }

    uint32_t centralStart = static_cast<uint32_t>(out.tellp());

    for (const Entry& e : entries) {
        uint16_t nameLen = static_cast<uint16_t>(e.zipPath.size());

        writeU32(out, 0x02014B50u); 
        writeU16(out, 20);          
        writeU16(out, 20);         
        writeU16(out, 0);           
        writeU16(out, 0);          
        writeU16(out, dosTime);
        writeU16(out, dosDate);
        writeU32(out, e.crc32);
        writeU32(out, e.compressedSize);
        writeU32(out, e.uncompressedSize);
        writeU16(out, nameLen);
        writeU16(out, 0);          
        writeU16(out, 0);           
        writeU16(out, 0);           
        writeU16(out, 0);          
        writeU32(out, e.isDir ? 0x10u : 0u); 
        writeU32(out, e.localHeaderOffset);
        out.write(e.zipPath.c_str(), nameLen);
    }

    uint32_t centralEnd  = static_cast<uint32_t>(out.tellp());
    uint32_t centralSize = centralEnd - centralStart;

    writeU32(out, 0x06054B50u);                         
    writeU16(out, 0);                                   
    writeU16(out, 0);                                   
    writeU16(out, static_cast<uint16_t>(entries.size())); 
    writeU16(out, static_cast<uint16_t>(entries.size())); 
    writeU32(out, centralSize);
    writeU32(out, centralStart);
    writeU16(out, 0);                                   

    out.close();
}

ZipFile::ZipFile(const string& name, Node* sourceNode, Node* parent)
    : Node(name + "-zip", parent), sourceNode(sourceNode)
{
    string zipPath = calculateParentPath() + this->name + ".zip";

    vector<Entry> entries;
    collectEntries(sourceNode, "", entries);
    buildZip(zipPath, entries);

    cout << "Zipped successfully → " << this->name << ".zip" << endl;
    calculateSize();
}

ZipFile::ZipFile(const string& name, Node* parent, bool)
    : Node(name, parent), sourceNode(nullptr)
{
    calculateSize();
}
void ZipFile::unZip()
{
    string zipPath  = calculateParentPath() + name + ".zip";
    string outFolder = calculateParentPath() + name + "-unzipped";
    fs::create_directories(outFolder);

    ifstream in(zipPath, ios::binary);
    if (!in.is_open()) {
        cout << "Error: cannot open " << zipPath << endl;
        return;
    }
    while (true) {
        uint32_t sig = 0;
        in.read(reinterpret_cast<char*>(&sig), 4);
        if (!in || sig != 0x04034B50u)
            break; 

        uint16_t versionNeeded, flags, compression, modTime, modDate;
        uint32_t crc, compSize, uncompSize;
        uint16_t nameLen, extraLen;

        in.read(reinterpret_cast<char*>(&versionNeeded), 2);
        in.read(reinterpret_cast<char*>(&flags),         2);
        in.read(reinterpret_cast<char*>(&compression),   2);
        in.read(reinterpret_cast<char*>(&modTime),       2);
        in.read(reinterpret_cast<char*>(&modDate),       2);
        in.read(reinterpret_cast<char*>(&crc),           4);
        in.read(reinterpret_cast<char*>(&compSize),      4);
        in.read(reinterpret_cast<char*>(&uncompSize),    4);
        in.read(reinterpret_cast<char*>(&nameLen),       2);
        in.read(reinterpret_cast<char*>(&extraLen),      2);

        string entryName(nameLen, '\0');
        in.read(&entryName[0], nameLen);
        in.seekg(extraLen, ios::cur);

        string localPath = entryName;
        for (char& c : localPath)
            if (c == '/') c = '\\';

        string fullPath = outFolder + "\\" + localPath;

        if (!entryName.empty() && entryName.back() == '/') {          
            fs::create_directories(fullPath);
        }
        else {
            fs::path p(fullPath);
            if (p.has_parent_path())
                fs::create_directories(p.parent_path());

            ofstream out(fullPath, ios::binary);
            if (!out.is_open()) {
                cout << "Warning: could not create " << fullPath << endl;
                in.seekg(compSize, ios::cur);
                continue;
            }

            if (compSize > 0) {
                vector<char> buf(compSize);
                in.read(buf.data(), compSize);
                out.write(buf.data(), compSize);
            }
            out.close();
            cout << "  Extracted: " << entryName << endl;
        }
    }

    in.close();
    cout << "Unzipped to: " << outFolder << endl;
}
void ZipFile::Open()
{
    cout << "Zipped files cannot be opened, unzip first." << endl;
}
void ZipFile::Delete()
{
    string zipPath = calculateParentPath() + name + ".zip";
    if (fs::remove(zipPath))
        cout << name << ".zip deleted successfully." << endl;
    else
        cout << "Warning: could not delete " << zipPath << endl;
}
string ZipFile::getType() const    { return "ZipFile"; }
string ZipFile::getExtension() const { return ".zip"; }

void ZipFile::calculateSize()
{
    string zipPath = calculateParentPath() + name + ".zip";
    ifstream file(zipPath, ios::binary | ios::ate);
    size = file.is_open() ? static_cast<int>(file.tellg()) : 0;
    file.close();
}
