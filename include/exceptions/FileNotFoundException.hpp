#pragma once
#include<iostream>
#include<string>

class FileNotFoundException:public std::exception{
    private:
    std::string msgStr;

    public:

    FileNotFoundException(std::string &filePath){
        std::string fileName=filePath.substr(filePath.find_last_of("/")+1);
        std::string c1="\033[1;31m";
        std::string armsg="لم يتم العثور على الملف\n\t"+fileName;
        std::string c2="\033[0m";
        msgStr=c1+armsg+c2;
    }

    const char* what() const noexcept override{
        const char* const msg=msgStr.c_str();
        return msg;
    }
};