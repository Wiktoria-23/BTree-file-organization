cmake_minimum_required(VERSION 3.29)
project(organizacja_pliku_b_drzewo)

set(CMAKE_CXX_STANDARD 20)

add_executable(organizacja_pliku_b_drzewo main.cpp
        BTreePage.cpp
        BTreePage.h
        FileRecord.cpp
        FileRecord.h
        BTree.cpp
        BTree.h
        Program.cpp
        Program.h
        BTreeRecord.cpp
        BTreeRecord.h)
