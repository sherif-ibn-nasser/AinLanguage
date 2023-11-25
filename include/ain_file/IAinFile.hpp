#pragma once
#include <iostream>
#include <string>
#include <vector>

class IAinFile{

    protected:
        std::string path;
        std::vector<std::wstring> lines;
        virtual void readAinFile()=0;

    public:
        std::vector<std::wstring> getLines(){
            return lines;
        }

        std::string getPath()const{
            return path;
        }
        
};