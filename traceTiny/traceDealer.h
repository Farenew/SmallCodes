//
// Created by JF_Work on 2019/4/28.
//

#ifndef TRACETINY_traceTinyWrite_H
#define TRACETINY_traceTinyWrite_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

class traceLine{
public:
    long int time;
    int blkno;
    int bcount;
    int flag;
    string md5;

    traceLine(long int t, int blk, int bc, int flg, string& s):
            time{t}, blkno{blk}, bcount{bc}, flag{flg}, md5{s} {}

    void prt(){
        cout << time << ' ' << blkno << ' ' << bcount << ' ' << flag << ' ' << md5 << '\n';
    }
};


// base class

class traceDealer{
public:
    int totalLines;
    int totalReads;
    int totalWrites;

    virtual traceLine readLine();
    traceDealer(){};
    traceDealer(const string& fileToRead, const string& readDir = "");
    virtual ~traceDealer(){     traceFile.close();   };

protected:
    ifstream traceFile;

    template <typename Out>
    void split(const string&s, char delim, Out result) const;
};

// this is the constructor we should use, since we have to indicate which trace we are working with
traceDealer::traceDealer(const string &fileToRead, const string &readDir):
        totalLines{0}, totalReads{0}, totalWrites{0}  {

    traceFile.open(readDir+fileToRead, ios_base::in);
    if(!traceFile.is_open()){
        cout << "cannot open trace file to read" << '\n';
        exit(1);
    }
}

// read line, this function may be redefined. base on using split() function to parse
traceLine traceDealer::readLine() {
    string line;
    getline(traceFile, line);
    if(line.empty()){
        cout << "read empty from trace file" << '\n';
        exit(1);
    }

    vector<string> tokens;
    split(line, ' ', back_inserter(tokens));

    long int time = stol(tokens[0]);
    int blkno = stoi(tokens[3]);
    int flag;
    if(tokens[5] == "R"){
        flag = 1;
    }
    if(tokens[5] == "W"){
        flag = 0;
    }
    int bcount = stoi(tokens[4]);

    traceLine trace(time, blkno, bcount, flag, tokens[8]);

    totalLines++;
    if(flag == 1)
        totalReads++;
    if(flag == 0)
        totalWrites++;

    return trace;
}

template <typename Out>
void traceDealer::split(const string &s, char delim, Out result) const {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)){
        *(result++) = item;
    }
}


// derived class, this is used for tiny the trace and write, which not only specifiy the read trace, write trace as well

class traceTinyWrite: public traceDealer{
public:
    traceTinyWrite(const string& fileToRead, const string& fileToWrite,
                   const string& readDir = "", const string& writedir = "");
    ~traceTinyWrite();

    // write trace struct into file
    void writeLine(traceLine& trace);

private:
    ofstream writeTrace;

};


traceTinyWrite::traceTinyWrite(const string& fileToRead, const string& fileToWrite,
                               const string& readDir, const string& writedir){

    totalLines = 0;
    totalReads = 0;
    totalWrites = 0;
    cout << "reading from " << readDir + fileToRead << '\n';
    traceFile.open(readDir+fileToRead, ios_base::in);
    if(!traceFile.is_open()){
        cout << "cannot open trace file to read" << '\n';
        exit(1);
    }

    cout << "writing to " << writedir + fileToWrite << '\n';
    writeTrace.open(writedir+fileToWrite, ios_base::out);
    if(!writeTrace.is_open()){
        cout << "cannot open trace file to write" << '\n';
        exit(1);
    }
}


traceTinyWrite::~traceTinyWrite() {
    traceFile.close();
    writeTrace.close();
}


void traceTinyWrite::writeLine(traceLine& trace) {
    if(writeTrace.is_open()){
        writeTrace << trace.time << ' ' << trace.blkno
                   << ' ' << trace.bcount << ' ' << trace.flag << ' ' << trace.md5 << '\n';
    }
    else{
        cout << "this write file is not open!" << '\n';
        exit(1);
    }
}

#endif //TRACETINY_traceTinyWrite_H
