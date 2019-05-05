//
// Created by JF_Work on 2019/5/5.
//

#include "trace.h"


#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cassert>


namespace trace{

    traceFile::traceFile(const string &fileToRead, const string &readDir):
            totalLines{0}, readLines{0}, writeLines{0}, writeTag{0}, fileWrite{0}, currentLine{nullptr} {

        std::cout << "reading from " << readDir + fileToRead << '\n';
        readFile.open(readDir+fileToRead, std::ios_base::in);
        if(!readFile.is_open()) {
            std::cout << "cannot open trace file to read" << '\n';
            exit(1);
        }
    }

    traceFile::traceFile(const string &fileToRead, const string &readDir,
                             const string &fileToWrite, const string &writeDir):
            totalLines{0}, readLines{0}, writeLines{0}, writeTag{1}, fileWrite{0}, currentLine{nullptr}{

        std::cout << "reading from " << readDir + fileToRead << '\n';
        readFile.open(readDir+fileToRead, std::ios_base::in);
        if(!readFile.is_open()) {
            std::cout << "cannot open trace file to read" << '\n';
            exit(1);
        }

        std::cout << "writing to " << writeDir + fileToWrite << '\n';
        writeFile.open(writeDir+fileToWrite, std::ios_base::out);
        if(!writeFile.is_open()){
            std::cout << "cannot open trace file to write" << '\n';
            exit(1);
        }
    }

    traceFile::~traceFile(){
        if(writeTag == 1){
            writeFile.close();
        }
        readFile.close();

    }

    void* traceFile::readLine_keep(traceType T, lineType L){
        void* line = nullptr;

        vector<string> tokens;
        parseLine(tokens);
        if(tokens.empty())
            return nullptr;

        switch(T){
            case traceType::FIU: {
                long time = stol(tokens[0]);
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
            case traceType::BASE: {
                long time = stol(tokens[0]);
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
        switch(L){
            case lineType::BASIC:
                auto traceline = (traceLineBasic*)line;
                writeFile << traceline->time << ' ' << traceline->blkno
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