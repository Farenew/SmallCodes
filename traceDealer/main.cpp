#include <iostream>
#include <algorithm>

#include "trace.h"

using namespace trace;
using namespace std;



namespace traceTiny {
    int main() {

        traceType T = traceType::FIU;
        lineType L = lineType::BASIC;

        const int MAX = 4000000;
        const string readDir = "/home/astl/hyx/FIU_home/";
        const string readFile = "homes-110108-112108.1.blkparse";

        const string writeFile = "homes_1_tiny.blkparse";
        const string writeDir = "/home/astl/hyx/task6/";

        traceFile file(readFile, readDir, writeFile, writeDir);
        while (true) {
            traceLineBasic *line = (traceLineBasic *) file.readLine_nokeep(T, L);

            if (file.totalLines == 4000000 || line == nullptr)
                break;

            //file.printLine(line, L);
            file.writeLine(line, L);


            if (file.totalLines % 100000 == 0)
                cout << "now done " << file.totalLines << " traces\n";

        }

        cout << "we have read " << file.totalLines << " traces" << '\n';
        cout << "including " << file.writeLines << " write traces" << '\n';
        cout << "including " << file.readLines << " read traces" << '\n';
        cout << "write " << file.fileWrite << " traces" << '\n';
    }
}

namespace addressCompress {

    bool BlkSmaller(traceLineBasic *line1, traceLineBasic *line2) {
        return line1->blkno < line2->blkno;
    }

    bool TimeSmaller(traceLineBasic *line1, traceLineBasic *line2) {
        return line1->time < line2->time;
    }

    int main() {

        traceType T = traceType::BASE;
        lineType L = lineType::BASIC;

        const int BCOUNT = 8;

        const string readDir = "/home/astl/hyx/task6/";
        const string readFile = "homes_1_tiny.blkparse";

        const string writeDir = "/home/astl/hyx/task6/";
        const string writeFile = "homes_1_tinyCompress.blkparse";
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

            if (traces[j]->blkno - 2 * BCOUNT <= traces[i]->blkno) {
                continue;
            }

            traces[j]->blkno = traces[i]->blkno + 2 * BCOUNT;
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

    int homesCompress(){
        traceType T = traceType::BASE;
        lineType L = lineType::BASIC;

        const int BCOUNT = 8;

        const string readDir = "/home/astl/hyx/task6/";
        const string readFile = "homes_1_tiny.blkparse";

        const string writeDir = "/home/astl/hyx/task6/";
        const string writeFile = "homes_1_tinyCompress.blkparse";
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

}


namespace addressDiscard {
    int main() {
        traceType T = traceType::BASE;
        lineType L = lineType::BASIC;

        const int BCOUNT = 8;

        const int MAX_BLK = 100000;

        const string readDir = "/home/astl/hyx/task/";
        const string readFile = "mails_1_400W_compress.blkparse";

        const string writeDir = "/home/astl/hyx/task/";
        const string writeFile = "mails_1_400W_compress_discard.blkparse";
        traceFile file(readFile, readDir, writeFile, writeDir);

        while (true) {
            traceLineBasic *line = (traceLineBasic *) file.readLine_nokeep(T, L);

            if (file.totalLines == 4000000 || line == nullptr)
                break;

            if (line->blkno < MAX_BLK)
                file.writeLine(line, L);


            if (file.totalLines == 10)
                break;

            if (file.totalLines % 100000 == 0)
                cout << "now done " << file.totalLines << " traces\n";

        }

        cout << "we have read " << file.totalLines << " traces" << '\n';
        cout << "including " << file.writeLines << " write traces" << '\n';
        cout << "including " << file.readLines << " read traces" << '\n';
        cout << "write " << file.fileWrite << " traces" << '\n';
    }

}


int main(){
    traceTiny::main();
    addressCompress::homesCompress();
}
