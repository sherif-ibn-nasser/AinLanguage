#pragma once
#include "IAinFile.hpp"

class AinFile:public IAinFile{

    private:
        void readAinFile()override;

    public:
        AinFile(std::string &path);
};