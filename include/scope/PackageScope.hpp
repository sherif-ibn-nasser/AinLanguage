#pragma once
#include "ASTNode.hpp"
#include "BaseScope.hpp"
#include "SharedPtrTypes.hpp"
#include <map>
#include <string>
class PackageScope:public BaseScope{
    private:
        std::unordered_map<std::wstring, SharedPackageScope> packages;
        std::unordered_map<std::wstring, SharedFileScope> files;
    public:
    
        void accept(ASTVisitor *visitor) override;

        PackageScope(std::wstring name,SharedPackageScope parentScope=nullptr);
        /**
        @brief search for an internal package by @param[name]
        @if found it will @return it
        @else @returns nullptr
        */
        SharedPackageScope findPackageByName(std::wstring name);

        /**
        @brief search for an internal file by @param[path]
        @if found it will @return it
        @else @returns nullptr
        */
        SharedFileScope findFileByPath(std::wstring path);

        void addPackage(SharedPackageScope package);

        void addFile(SharedFileScope file);

        SharedClassScope getClassByType(SharedType type) override;

        static SharedPackageScope AIN_PACKAGE;

        std::unordered_map<std::wstring, SharedPackageScope> getPackages() const;

        std::unordered_map<std::wstring, SharedFileScope> getFiles() const;

};