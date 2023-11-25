
#include <iostream>
#include <catch2/catch_all.hpp>
#include "AinFile.hpp"
#include "BadFileFormatException.hpp"
#include "FileNotFoundException.hpp"

SCENARIO("Test AinFile.cpp","[AinFileTest.cpp]"){
    GIVEN("A file"){
        WHEN("format is *.ain, file exists"){
            /*
            You should run tests from the directory of unit_tests test as it is in build/bin
            */
            std::string path="../../test/ain_file/testPass.ain";
            AinFile file(path);
            std::vector<std::wstring> LINES={L"##",L"This for testing AinFile.cpp",L"##"};
            THEN("read lines in file"){
                auto lines=file.getLines();
                REQUIRE(lines.size()==LINES.size());
                for(int i=0;i<lines.size();i++){
                    REQUIRE(lines[i]==LINES[i]);
                }
            };
        };
        
        WHEN("format is *.ain, file doesn't exist"){
            THEN("Throw FileNotFoundException with file name"){
                std::string path="../test/ain_file/testFail.ain";

                auto matcher=Catch::Matchers::Predicate<FileNotFoundException>(
                    [&](const FileNotFoundException e){
                        std::string what=e.what();
                        return
                            what.find("testFail.ain")!=std::string::npos
                                &&
                            what.find("ain_file/")==std::string::npos
                        ;
                    }
                );

                REQUIRE_THROWS_MATCHES(
                    AinFile(path),
                    FileNotFoundException,
                    matcher
                );
            };
        };

        WHEN("file in path and format is not *.ain"){
            THEN("Throw BadFileFormatException with file name"){
                std::string path="app/testFail.notain";

                auto matcher=Catch::Matchers::Predicate<BadFileFormatException>(
                    [&](const BadFileFormatException e){
                        std::string what=e.what();
                        return
                            what.find("testFail.notain")!=std::string::npos
                                &&
                            what.find("app/")==std::string::npos
                        ;
                    }
                );

                REQUIRE_THROWS_MATCHES(
                    AinFile(path),
                    BadFileFormatException,
                    matcher
                );

            };
        };
    };

}