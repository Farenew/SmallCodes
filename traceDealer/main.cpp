#include <iostream>

#include "trace.h"

using namespace trace;
using namespace std;


traceType T = traceType::FIU;
lineType L = lineType::BASIC;

int main() {
    const string readDir = "/home/astl/hyx/FIU_mail/";
    const string readFile = "cheetah.cs.fiu.edu-110108-113008.01.blkparse";
    traceFile file(readFile, readDir);
    while(true){
        traceLineBasic* line = (traceLineBasic*)file.readLine_nokeep(T, L);
        file.printLine(line, L);

        if(file.totalLines == 10){
            break;
        }
    }
}