#include "AudioFile.h"
#include <filesystem>

void AudioFile::Open()
{
    string filePath = filesystem::absolute(calculateParentPath() + name + ".mpg").string();

    string cmd = "cmd /c \"\"C:\\ffmpeg\\bin\\ffplay.exe\" -loglevel quiet"
        " -autoexit -nodisp"
        " \"" + filePath + "\"\"";

    cout << "Playing: " << name << ".mpg" << endl;
    system(cmd.c_str());
    cout << "Playback finished." << endl;
}

AudioFile::AudioFile(const string& name, Node* parent)
    : Node(name, parent)
{
    string filePath = filesystem::absolute(calculateParentPath() + name + ".mpg").string();

    string cmd = "cmd /c \"\"C:\\ffmpeg\\bin\\ffmpeg.exe\" -loglevel quiet"
        " -f dshow -i audio=\"Microphone (3- High Definition Audio Device)\""
        " -t 5 -filter:a \"volume=10.0\""
        " \"" + filePath + "\"\"";

    cout << "Recording for 5 seconds, please speak into the microphone." << endl;
    system(cmd.c_str());
    cout << "Recording complete. Audio saved as " << name << ".mpg" << endl;
    calculateSize();
}

AudioFile::AudioFile(const string& name, Node* parent, bool)
    : Node(name, parent)
{
    calculateSize();
}

void AudioFile::Delete()
{
    string filePath = filesystem::absolute(calculateParentPath() + name + ".mpg").string();
    remove(filePath.c_str());
}

string AudioFile::getType() const { return "AudioFile"; }
string AudioFile::getExtension() const { return ".mpg"; }

void AudioFile::calculateSize()
{
    string filePath = calculateParentPath() + name + ".mpg";
    ifstream file(filePath, ios::binary | ios::ate);
    size = file.is_open() ? static_cast<int>(file.tellg()) : 0;
    file.close();
}