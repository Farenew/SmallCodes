//
// Created by JF_Work on 2019/5/5.
//

#ifndef TRACEDEALER_TRACE_H
#define TRACEDEALER_TRACE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace trace{

    using std::string;
    using std::cout;
    using std::vector;

    // what trace are we using, different trace align with different reading method
    enum class traceType{FIU, BASE, CAFTL};
    /* * what foramt should we get after reading
     * - basic is mostly used, format as: [time][blkno][bcount][flag][md5]
     * - FIU is trace type for FIU trace
     * - CAFTL is trace type for CAFTL trace
     * */
    enum class lineType{BASIC};

    // align with lineType::BASIC
    class traceLineBasic{
    public:
        long double time;
        int blkno;
        int bcount;
        int flag;
        string md5;
        traceLineBasic(long double t, int blk, int bc, int flg, string s):
                time{t}, blkno{blk}, bcount{bc}, flag{flg}, md5{s}{}
    };

    class traceFile{
    public:

        // construct with no need to write
        traceFile(const string &fileToRead, const string &readDir);
        // construct with the file to write
        traceFile(const string &fileToRead, const string &readDir,
                  const string& fileToWrite, const string& writeDir);


        ~traceFile();


        // total lines in tracefile
        int totalLines;
        // lines indicate a read in tracefile
        int readLines;
        // lines indicate a write in tracefile
        int writeLines;

        // how many lines we've written
        int fileWrite;

        /* * There are two ways to read line from tracefile, since we return a pointer here
         *   readLine_keep would keep the result we've read
         *   readLine_nokeep would automaticlly delete the last read result
         *
         *   if read failed, return result would be nullptr
         * */
        void* readLine_keep(traceType T, lineType L);
        void* readLine_nokeep(traceType T, lineType L);

        // write a line format as L to file
        void writeLine(void* line, lineType L);
        // print a linen format as L
        void printLine(void* line, lineType L);

    private:
        std::ifstream readFile;
        std::ofstream writeFile;
        int writeTag;

        template <typename Out>
        void split(const string &s, char delim, Out result) const;

        void parseLine(vector<string>& tokens);

        // currentline, used for delete last result
        void* currentLine;
    };
}



#endif //TRACEDEALER_TRACE_H
