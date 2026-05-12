#include "text.h"
#include<iostream>
using namespace std;
textfile :: textfile(string name , node* parent) : file(name,parent,".txt"){lines.push_back("");}
textfile :: textfile(string name, node* parent, string extract): file(name,parent,extract){lines.push_back("");}
void textfile::open(){
    cout<<"opened "<<name<<extension<<endl;
    edit();
}
void textfile:: edit(){
    int c = 0;
    string cmd;
    while(true){
        cout<<endl;
        for (int i = 0; i < (int)lines.size(); i++)
        {
            if (i == c)
            {
              cout<<"  "<<(i+1)<<" "<<lines[i]<< endl;
            }
            else
            cout<<"up | down | addline | edit | close";
            cin>> cmd;
            if (cmd == "up")
            {
                if (c>0)
                {
                    c--;
                }
                else cout<<"already at first line"<<endl;
            }
            else if (cmd == "down")
            { if (c < (int)lines.size() - 1) c++;
            else cout << "  Already at last line"<<endl;
        }
        else if (cmd == "addline") {
            lines.push_back("");
            c = lines.size() - 1;
            cout << "  New line added.\n";
        }
        else if (cmd == "edit") {
            cin.ignore();
            cout << "  Enter text for line " << (c+1) << ": ";
            getline(cin, lines[c]);
        }
        else if (cmd == "close") {
            cout << "  File saved and closed.\n";
            break;
        }
        else {
            cout << "  Unknown command.\n";
        }
    }
}

void textfile ::display{
    cout << "textfile  " << name << extension
         << "  (" << getsize() << " bytes)\n";
}

int textfile::getSize() {
    int total = 0;
    for (const string& l : line) total += l.size();
    return total;
}
            
            
}