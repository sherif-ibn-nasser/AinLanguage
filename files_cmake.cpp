#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#define cout std::cout
#define endl std::endl
#define string std::string
#define vector std::vector
namespace fs=std::filesystem;


const string C_SRC_FILES="add_library(ainsrc";
string SRC_FILES=C_SRC_FILES;
const string C_INCLUDE="set(INCLUDE";
string INCLUDE=C_INCLUDE;
const string C_TEST="set(TEST";
string TEST_SRC_FILES=C_TEST;
vector<string> cmake_lines;
vector<string> new_cmake_lines;

bool startsWith(string str, string prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

void read_from_cmake_file(){
    std::ifstream file("CMakeLists.txt");
    string line;
    while(std::getline(file,line)){
        cmake_lines.push_back(line);
    }
    file.close();
}

void write_to_cmake_file(){

    bool isEditing=false;

    for(auto &line:cmake_lines){
        string start="### start files_cmake.cpp ###";
        string end="### end files_cmake.cpp ###";

        if(startsWith(line,end)){
            isEditing=false;
            continue;
        }

        if(isEditing)
            continue;

        if(!startsWith(line,start)){
            new_cmake_lines.push_back(line);
            continue;
        }

        isEditing=true;

        new_cmake_lines.push_back(start+"\n");
        new_cmake_lines.push_back(SRC_FILES+"\n");
        new_cmake_lines.push_back(INCLUDE+"\n");
        new_cmake_lines.push_back(TEST_SRC_FILES+"\n");
        new_cmake_lines.push_back(end);
    }
    
    std::ofstream file("CMakeLists.txt");

    auto lastLineIndex=new_cmake_lines.size()-1;

    for(int i=0;i<lastLineIndex;i++){
        auto &line=new_cmake_lines[i];
        file<<line<<endl;
    }
    file<<new_cmake_lines[lastLineIndex];

    file.close();
}

void readinclude(string path){
    for(const auto & entry: fs::directory_iterator(path)){
        auto p=entry.path();
        if(fs::is_directory(p)){
            INCLUDE.append("\n    ").append(p).append("/");
            readinclude(p);
        }
    }
}

void readsrc(string path,string &files){
    for(const auto & entry: fs::directory_iterator(path)){
        auto p=entry.path();
        if(fs::is_directory(p)){
            readsrc(p,files);
        }
        else if(p.extension()==".cpp"&&p.filename()!="main.cpp"){
            files.append("\n    ").append(p);
        }
    }
}

void settestsrc(){
    readsrc("test",TEST_SRC_FILES);
    TEST_SRC_FILES.append("\n)");
}

void setsrc(){
    readsrc("src",SRC_FILES);
    SRC_FILES.append("\n)");
}

void setinclude(){
    readinclude("include");
    INCLUDE.append("\n)");
}

int main(){
    SRC_FILES.append("\n    src/main.cpp");
    INCLUDE.append("\n    include/");
    setsrc();
    setinclude();
    settestsrc();
    read_from_cmake_file();
    write_to_cmake_file();
    return 0;
}