#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include <memory>

PackageScope::PackageScope(std::wstring name,SharedPackageScope parentScope)
:BaseScope(0,name,parentScope){}

SharedPackageScope PackageScope::findPackageByName(std::wstring name){
    auto package=packages.find(name);
    if(package==packages.end())
        return nullptr;
    return package->second;
}

SharedFileScope PackageScope::findFileByPath(std::wstring path) {
    auto file=files.find(path);
    if(file==files.end())
        return nullptr;
    return file->second;
}

void PackageScope::addPackage(SharedPackageScope package) {
    packages[package->getName()]=package;
}

void PackageScope::addFile(SharedFileScope file){
    files[file->getName()]=file;
}

SharedClassScope PackageScope::getClassByType(SharedType type){
    for(auto iterator:files){
        auto publicClasses=iterator.second->getPublicClasses();
        auto classScopeIterator=publicClasses->find(*type->getName());
        if(classScopeIterator!=publicClasses->end())
            return classScopeIterator->second;
    }
    return nullptr;
}

std::unordered_map<std::wstring, SharedPackageScope> PackageScope::getPackages()const{
    return this->packages;
}

std::unordered_map<std::wstring, SharedFileScope> PackageScope::getFiles()const{
    return this->files;
}

SharedPackageScope PackageScope::AIN_PACKAGE=std::make_shared<PackageScope>(L"عين");