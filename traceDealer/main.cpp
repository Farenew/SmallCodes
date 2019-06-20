#include <iostream>
#include <algorithm>
#include <cassert>
#include <string>

#include "trace.h"

using namespace trace;
using namespace std;


namespace traceDealer {
    traceType T = traceType::CAFTL;
    lineType L = lineType::BASIC;

    const int MAX = 200000000;
    string readDir = "/home/astl/hyx/caftl-traces/";
    string readFile = "";

    string writeFile = "hadoop_all.blkparse";
    string writeDir = "/home/astl/hyx/catest/";

    int timeTag = 0;

    int tiny() {

        traceFile file(readFile, readDir, writeFile, writeDir);

        long double start = 0.0;
        long double end = 0.0;

        while (true) {
            traceLineBasic *line = (traceLineBasic *) file.readLine_nokeep(T, L);
            if(timeTag == 0){
                start = line->time;
                timeTag = 1;
            }

//            if (file.totalLines == MAX || line == nullptr)
//                break;

            if(line == nullptr)
                break;

            end = line->time;

            //file.printLine(line, L);
            file.writeLine(line, L);

            if (file.totalLines % 100000 == 0)
                cout << "now done " << file.totalLines << " traces\n";

        }

        cout << "we have read " << file.totalLines << " traces" << '\n';
        cout << "including " << file.writeLines << " write traces" << '\n';
        cout << "including " << file.readLines << " read traces" << '\n';
        cout << "write " << file.fileWrite << " traces" << '\n';

        cout << "total time for caftl is " << end - start << '\n';
    }

    bool BlkSmaller(traceLineBasic *line1, traceLineBasic *line2) {
        return line1->blkno < line2->blkno;
    }

    bool TimeSmaller(traceLineBasic *line1, traceLineBasic *line2) {
        return line1->time < line2->time;
    }

//    traceType T = traceType::BASE;
//    lineType L = lineType::BASIC;

    // this version is better
    int compress(){

        const int BCOUNT = 8;

        traceFile file(readFile, readDir, writeFile, writeDir);

        vector<traceLineBasic *> traces;

        while (true) {
            traceLineBasic *line = (traceLineBasic *) file.readLine_keep(T, L);

            if (line == nullptr)
                break;

            if (line->bcount != 0)
                traces.push_back(line);

            if (file.totalLines % 100000 == 0)
                cout << "now done reading " << file.totalLines << " traces\n";
        }
        cout << "we have read " << file.totalLines << " traces" << '\n';
        cout << "including " << file.writeLines << " write traces" << '\n';
        cout << "including " << file.readLines << " read traces" << '\n';

        cout << "now doing sort:\n";
        std::sort(traces.begin(), traces.end(), BlkSmaller);
        cout << "sort finished\n";

        cout << "now the biggest address is " << (*(traces.end() - 1))->blkno << '\n';


        cout << "now compress address\n";
        // # compress by its blkno, make the biggest slot no more than 2 * BCOUNT, make initial zero
        traces[0]->blkno = 0;
        for (std::vector<traceLineBasic *>::size_type i = 0; i < traces.size() - 1; i++) {
            auto j = i + 1;

            if(traces[i]->bcount == 0 || traces[j]->bcount == 0)
                continue;

            if (traces[j]->blkno - (traces[i]->blkno + traces[i]->bcount) <=  BCOUNT) {
                continue;
            }

            traces[j]->blkno = traces[i]->blkno + traces[i]->bcount + BCOUNT;
        }
        cout << "now done address compress\n";

        cout << "after compress, now the biggest address is " << (*(traces.end() - 1))->blkno << '\n';

        cout << "now restorting\n";
        std::sort(traces.begin(), traces.end(), TimeSmaller);
        cout << "now restorting finished\n";


        cout << "now writing back:\n";
        for (auto i:traces) {
            file.writeLine(i, L);

            if (file.fileWrite % 100000 == 0)
                cout << "now done writing " << file.fileWrite << " traces\n";
        }

        cout << "now finished!\n";
    }

    double lastTime = 0;

    traceFile* write;

    void initial_global_write(){
        write = new traceFile(writeFile, writeDir, 'W');
    }
    void destruct_global_write(){
        delete write;
    }


    int combine(){
        traceFile read(readFile, readDir, 'R');

        double time = 0;

        while (true) {
            traceLineBasic *line = (traceLineBasic *) read.readLine_nokeep(T, L);

            if(line == nullptr){
                lastTime = time;
                cout << "-----now last time is " << lastTime << '\n';
                break;
            }

            time = line->time;
            assert(time > lastTime);

            //file.printLine(line, L);
            write->writeLine(line, L);

            if (read.totalLines % 100000 == 0)
                cout << "now done " << read.totalLines << " traces\n";
        }

        cout << "we have read " << read.totalLines << " traces" << '\n';
        cout << "including " << read.writeLines << " write traces" << '\n';
        cout << "including " << read.readLines << " read traces" << '\n';

        cout << "write " << write->fileWrite << " traces" << '\n';
    }

//    traceType T = traceType::BASE;
//    lineType L = lineType::BASIC;

    const int MAX_BLK = 18388608;


    int addressDiscard() {
        traceFile write(readFile, readDir, writeFile, writeDir);

        cout << "now iterate and discard traces" << '\n';

        while (true) {
            traceLineBasic *line = (traceLineBasic *)write.readLine_nokeep(T, L);

            if (line == nullptr)
                break;

            if (line->blkno < MAX_BLK)
                write.writeLine(line, L);

            if (write.totalLines % 100000 == 0)
                cout << "now done " << write.totalLines << " traces\n";
        }

        cout << "we have read " << write.totalLines << " traces" << '\n';
        cout << "including " << write.writeLines << " write traces" << '\n';
        cout << "including " << write.readLines << " read traces" << '\n';
        cout << "write " << write.fileWrite << " traces" << '\n';
    }
}


int main(){
    traceDealer::T = traceType::BASE;
    traceDealer::readDir = "/home/astl/hyx/catest/";
    traceDealer::readFile = "hadoop_all_compress.blkparse";

    traceDealer::writeFile = "hadoop_all_200.blkparse";

    traceDealer::addressDiscard();

}


//// combine a lot of hadoop files together
//int main(int argc, char* argv[]){
//    traceDealer::initial_global_write();
//
//    for(auto i=8; i<19; i++){
//        string name = "hivetpch" + to_string(i) + "-ubuntu.trace";
//        traceDealer::readFile = name;
//        cout << "--------------------------------\n";
//        traceDealer::combine();
//    }
//    traceDealer::destruct_global_write();
//}

//// compressed hadoop total file
//int main(){
//    traceDealer::T = traceType::BASE;
//    traceDealer::readDir = "/home/astl/hyx/catest/";
//    traceDealer::readFile = "hadoop_all.blkparse";
//
//    traceDealer::writeFile = "hadoop_all_compress.blkparse";
//    traceDealer::compress();
//}

