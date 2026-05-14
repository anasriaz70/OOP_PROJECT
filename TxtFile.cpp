#include "TxtFile.h"
TxtFile::TxtFile(const string& name, Node* parent) : Node(name, parent) ,totalLines(0),activeLine(0),text(nullptr){
    fstream file(format("{}{}.txt",calculateParentPath(), name), ios::out | ios::trunc);
    file.close();
}

TxtFile::TxtFile(const string& name, Node* parent, bool skipCreate) : Node(name, parent), totalLines(0), activeLine(0), text(nullptr) {
    fstream file(format("{}{}.txt", calculateParentPath(), name), ios::in);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            string* newText = new string[totalLines + 1];
            for (int i = 0; i < totalLines; i++)
                newText[i] = text[i];
            newText[totalLines] = line;
            delete[] text;
            text = newText;
            totalLines++;
        }
        file.close();
        activeLine = totalLines > 0 ? 1 : 0;
    }
}

void TxtFile :: Read() {
    fstream file(format("{}{}.txt",calculateParentPath(), name), ios::in);
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
}

void TxtFile::moveUp() {
    if (activeLine <= 1)
        cout << "Cannot move up";
    else
        activeLine--;
}
void TxtFile::moveDown() {
    if (activeLine == totalLines)
        cout << "Cannot move down";
    else
        activeLine++;
}

void TxtFile::addNewLine() {
    string* newText;
    string line;
    newText = new string[totalLines + 1];
    for (int i = 0; i < activeLine; i++)
        newText[i] = text[i];
    cout << "Enter a new line:";
    getline(cin, line);
    newText[activeLine] = line;
    for (int i = activeLine; i < totalLines; i++)
        newText[i+1] = text[i];
    delete[] text;
    text = newText;
    totalLines++;
    activeLine++;
}
void TxtFile::editExistingLine() {
    if (activeLine == 0)
        cout << "No line to edit." << endl;
    else
    {
        string line;
        cout << "Enter a line:";
        getline(cin, line);
        text[activeLine - 1] = line;
    }
}
void TxtFile::savefile() {
    fstream file(format("{}{}.txt",calculateParentPath(), name), ios::out);
    for (int i = 0; i < totalLines; i++)
        file << text[i] << endl;
    file.close();
    calculateSize();
}
void TxtFile::displayContents() {
    for (int i = 0; i < totalLines; i++)
    {
        if (i + 1 == activeLine)
            cout << "~";
        cout << i + 1 << ". " << text[i] << endl;

    }
}
void TxtFile::Delete() {
    remove(format("{}{}.txt",calculateParentPath(), name).c_str());
}
void TxtFile::Open() {
    cout << "File: " << format("{}.txt", name) << endl;
    while (true) {
        displayContents();
        cout << "\n1.Move up\n2.Move down\n3.Add new line\n4.Edit existing Line\n";
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 5)
        {
            savefile();
            break;
        }
        switch (choice) {
        case 1:
            moveUp();
            break;
        case 2:
            moveDown();
            break;
        case 3:
            addNewLine();
            break;
        case 4:
            editExistingLine();
            break;
        }
    }
}
string TxtFile::getType() const {
    return "TxtFile";
}
void TxtFile::calculateSize() {
    ifstream file(format("{}{}.txt",calculateParentPath(), name), std::ios::binary | std::ios::ate);
    size = static_cast<int>(file.tellg());
    file.close();
}

string TxtFile::getExtension() const {
    return ".txt";
}

