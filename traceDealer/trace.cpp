//
// Created by JF_Work on 2019/5/5.
//

#include "trace.h"


#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>


namespace trace{

    traceFile::traceFile(const string &file, const string &dir, char type):
            totalLines{0}, readLines{0}, writeLines{0}, fileWrite{0}, currentLine{nullptr} {

        if(type == 'R' || type == 'r'){
            std::cout << "reading from " << dir + file << '\n';
            readFile.open(dir+file, std::ios_base::in);
            if(!readFile.is_open()) {
                std::cout << "cannot open trace file to read" << '\n';
                exit(1);
            }
            readOrwrite |= 1u << 0u;
        }
        else if(type == 'W' || type == 'w'){
            std::cout << "writing to " << dir + file << '\n';
            writeFile.open(dir+file, std::ios_base::out);
            if(!writeFile.is_open()) {
                std::cout << "cannot open trace file to write" << '\n';
                exit(1);
            }
            readOrwrite |= 1u << 1u;
        }
        else{
            std::cout << "cannot tell what type you are doing!" << std::endl;
            exit(1);
        }
    }

    traceFile::traceFile(const string &fileToRead, const string &readDir,
                             const string &fileToWrite, const string &writeDir):
            totalLines{0}, readLines{0}, writeLines{0}, fileWrite{0}, currentLine{nullptr}{

        std::cout << "reading from " << readDir + fileToRead << '\n';
        readFile.open(readDir+fileToRead, std::ios_base::in);
        if(!readFile.is_open()) {
            std::cout << "cannot open trace file to read" << '\n';
            exit(1);
        }
        readOrwrite |= 1u << 0u;

        std::cout << "writing to " << writeDir + fileToWrite << '\n';
        writeFile.open(writeDir+fileToWrite, std::ios_base::out);
        if(!writeFile.is_open()){
            std::cout << "cannot open trace file to write" << '\n';
            exit(1);
        }
        readOrwrite |= 1u << 1u;
    }

    traceFile::~traceFile(){
        if(readOrwrite & 1u << 0u){
            readFile.close();
        }
        if(readOrwrite & 1u << 1u){
            writeFile.close();
        }
    }

    void* traceFile::readLine_keep(traceType T, lineType L){
        assert(readFile.is_open());
        assert(readOrwrite & 1u << 0u);

        void* line = nullptr;

        vector<string> tokens;
        parseLine(tokens);

        if(tokens.empty())
            return line;

        switch(T){
            case traceType::FIU: {
                long double time = stold(tokens[0]);
                int blkno = stoi(tokens[3]);
                int flag;
                if (tokens[5] == "R") {
                    flag = 1;
                    readLines++;
                }
                if (tokens[5] == "W") {
                    flag = 0;
                    writeLines++;
                }
                int bcount = stoi(tokens[4]);

                string md5 = tokens[8];

                switch (L) {
                    case lineType::BASIC:
                        traceLineBasic *tl = new traceLineBasic(time, blkno, bcount, flag, md5);
                        line = (void *) tl;
                        currentLine = (void *) tl;
                        break;
                }
                break;
            }
            case traceType::CAFTL: {
                long double time = stold(tokens[0]);
                int blkno = stoi(tokens[5]);
                int bcount = 8;
                int flag;
                if (tokens[9] == "write") {
                    flag = 1;
                    readLines++;
                }
                if (tokens[9] == "read") {
                    flag = 0;
                    writeLines++;
                }
                string md5 = tokens[10];
                switch (L) {
                    case lineType::BASIC:
                        traceLineBasic *tl = new traceLineBasic(time, blkno, bcount, flag, md5);
                        line = (void *) tl;
                        currentLine = (void *) tl;
                        break;
                }
                break;
            }
            case traceType::BASE: {
                long double time = stold(tokens[0]);
                int blkno = stoi(tokens[1]);
                int bcount = stoi(tokens[2]);
                int flag = stoi(tokens[3]);
                if(flag == 0)
                    writeLines++;
                if(flag == 1)
                    readLines++;
                string md5 = tokens[4];


                switch (L) {
                    case lineType::BASIC:
                        traceLineBasic *tl = new traceLineBasic(time, blkno, bcount, flag, md5);
                        line = (void *) tl;
                        currentLine = (void *) tl;
                        break;
                }
                break;
            }
        }

        totalLines++;
        return line;
    }

    void* traceFile::readLine_nokeep(traceType T, lineType L) {
        assert(readFile.is_open());
        assert(readOrwrite & 1u << 0u);
        switch(L){
            case lineType::BASIC:
                if(currentLine != nullptr){
                    delete (traceLineBasic*)currentLine;
                }
                break;
        }

        void* line = nullptr;
        line = readLine_keep(T, L);
        assert(currentLine != nullptr);
        return line;
    }

    void traceFile::writeLine(void *line, lineType L) {
        assert(writeFile.is_open());
        assert(readOrwrite & 1u << 1u);
        switch(L){
            case lineType::BASIC:
                auto traceline = (traceLineBasic*)line;
                writeFile << std::setprecision(10) <<traceline->time << std::setprecision(0) <<' ' << traceline->blkno
                          << ' ' << traceline->bcount << ' ' << traceline->flag
                          << ' ' << traceline->md5 << '\n';
        }

        fileWrite++;
    }

    void traceFile::printLine(void *line, lineType L) {
        switch(L){
            case lineType::BASIC:
                auto traceline = (traceLineBasic*)line;
                cout << "time: " << traceline->time << "\tblkno: " << traceline->blkno
                          << "\tbcount: " << traceline->bcount << "\tflag: " << traceline->flag
                          << "\tmd5: " << traceline->md5 << '\n';
                break;
        }
    }


    void traceFile::parseLine(vector<string>& tokens) {
        string line;
        getline(readFile, line);

        if(tokens.empty()) {
            split(line, ' ', back_inserter(tokens));
        }
    }

    template <typename Out>
    void traceFile::split(const string &s, char delim, Out result) const {
        using namespace std;
        stringstream ss(s);
        string item;
        while(getline(ss, item, delim)){
            *(result++) = item;
        }
    }

}