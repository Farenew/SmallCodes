#include "traceDealer.h"

namespace readAndTiny{
    const string rd = "/home/astl/hyx/FIU_mail/";
    const string file = "cheetah.cs.fiu.edu-110108-113008.01.blkparse";

    const string writeFile = "mails_1_400W.blkparse";

    const int MAX = 4000000;
    int main(){

        traceTinyWrite td(file, writeFile, rd);

        while(td.totalLines <= MAX){
            traceLine tr = td.readLine();
            td.writeLine(tr);
            if(td.totalLines % 1000000 == 0){
                cout << "now have read " << td.totalLines << " traces" << '\n';
            }
        }

        cout << "we have read " << td.totalLines << " traces" << '\n';
        cout << "including " << td.totalWrites << " write traces" << '\n';
        cout << "including " << td.totalReads << " read traces" << '\n';
    }
}




int main(){
    readAndTiny::main();
}
