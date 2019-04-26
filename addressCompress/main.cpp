/* used to compress trace file
 *
 * written by forenewhan, 2019.4.23
 *
 * ---------------------------------------------
 *
 * trace file should have format like [time(long)][blkno(int)][bcount(int)][flag(int)][hash(char*)]
 *
 *
 * change const variables to align with md5 value size, file location, etc.
 * this script is written in mixed C and C++, just because I'm such a newbie.
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using std::vector;

typedef struct traceLine{
    long time;
    long blkno;
    int bcount;
    int flag;
    char* hash;
}traceLine;

const char* dir = "/home/astl/hyx/task2/";
const char* fileName = "mails_1.blkparse";
const char* writeFile = "mails_1_addCompress.blkparse";

const int HASH_SIZE = 32;
const int BCOUNT = 8;

const int PRINT_MARK = 500000;


const int SIZE = 200;
bool traceLineBLKCompare(traceLine* t1, traceLine* t2){
    return t1->blkno < t2->blkno;
}

bool traceLineTIMECompare(traceLine* t1, traceLine* t2){
    return t1->time < t2->time;
}

int main() {
    char file[SIZE];
    char line[SIZE];
    int times = 0;
    int discard = 0;

    vector<traceLine*> traces;

    snprintf(file, SIZE, "%s%s", dir, fileName);
    FILE* traceFile;
    traceFile = fopen(file, "r");

    // # read file and parse it into vector
    while(true){
        if(fgets(line, SIZE-1, traceFile) == NULL){
            printf("now file is over\n");
            break;
        }
        else{
            traceLine* oneline = new(traceLine);
            oneline->hash = (char*)malloc(sizeof(char) * HASH_SIZE);

            sscanf(line, "%ld %ld %d %d %s\n", &oneline->time, &oneline->blkno, &oneline->bcount, &oneline->flag, oneline->hash);

            if(oneline->bcount == BCOUNT){
                traces.push_back(oneline);
            }
            else{
                discard ++ ;
            }

            if(times == PRINT_MARK){
                printf("now have read %d traces\n", times);
            }

            times ++ ;
        }
    }
    fclose(traceFile);
    printf("file have %d traces, discard %d traces\n", times, discard);


    // sort by blkno
    printf("now doing first sort\n");
    std::sort(traces.begin(), traces.end(), traceLineBLKCompare);


    printf("the largest blk requested is %ld\n", traces[traces.size()-1]->blkno);


    // # compress by its blkno, make the biggest slot no more than 2 * BCOUNT
    for(std::vector<traceLine*>::size_type i = 0; i<traces.size() - 1; i++){
        auto j = i + 1;

        if(traces[j]->blkno - 2 * BCOUNT <= traces[i]->blkno){
            continue;
        }

        traces[j]->blkno = traces[i]->blkno + 2 * BCOUNT;

    }

    printf("after compression, the largest blk requested now is %ld\n", traces[traces.size()-1]->blkno);

    // resort, make the tracefile align with time
    printf("now doing second sort\n");
    std::sort(traces.begin(), traces.end(), traceLineTIMECompare);


    snprintf(file, SIZE, "%s%s", dir, writeFile);
    traceFile = fopen(file, "w");
    times = 0;

    printf("now start to write to file\n");
    for(std::vector<traceLine*>::size_type i = 0; i<traces.size(); i++) {
        times ++;
        fprintf(traceFile, "%ld %ld %d %d %s\n", traces[i]->time, traces[i]->blkno, traces[i]->bcount, traces[i]->flag, traces[i]->hash);
        free(traces[i]->hash);
        delete(traces[i]);
        if(times == PRINT_MARK){
            printf("now have write %d traces\n", times);
        }
    }


    printf("successfull write to file\n");
    fclose(traceFile);


    return 0;
}