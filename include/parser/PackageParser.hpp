#pragma once
#include "BaseParser.hpp"
#include "SharedPtrTypes.hpp"
class PackageParser:public BaseParser<SharedPackageScope>{
public:
    using BaseParser<SharedPackageScope>::BaseParser;
    SharedPackageScope parse();
private:
    SharedPackageScope addPackageTo(SharedPackageScope package);
};