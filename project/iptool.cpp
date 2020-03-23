#include "../iptools/core.h"
#include <strings.h>
#include "roi.hpp"
#include <string.h>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

#define MAXLEN 256

int main (int argc, char** argv)
{
    image src, tgt;
    FILE *fp;
    char str[MAXLEN];
    char outfile[MAXLEN];
    char *pch;
    if ((fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Can't open file: %s\n", argv[1]);
		exit(1);        
    }

    while (fgets(str, MAXLEN, fp) != NULL) {
        pch = strtok(str, " ");
        cout << pch << endl;
        src.read(pch); // creating the source image
        string src_name = pch;

        pch = strtok(NULL, " ");
        cout << pch << endl;
        strcpy(outfile, pch);

        pch = strtok(NULL, " ");
        cout << pch << endl;
        string func_name = pch;

        pch = strtok(NULL, " ");
        cout << pch << endl;
        int num_regions = atoi(pch);

        if (num_regions > 3) {
            cout << "Only 3 ROI allowed" << endl;
            exit(1);
        }

        vector<roi> regions;
        for (int i = 0; i < num_regions; i++) {
            if (fgets(str, MAXLEN, fp) != NULL) {
                pch = strtok(str, " ");
                int x = atoi(pch);
                cout << pch << endl;
                pch = strtok(NULL, " ");
                int y = atoi(pch);
                cout << pch << endl;
                pch = strtok(NULL, " ");
                int sx = atoi(pch);
                cout << pch << endl;
                pch = strtok(NULL, " ");
                int sy = atoi(pch);
                cout << pch << endl;

                if (func_name == "hist") {
                    pch = strtok(NULL, " ");
                    int arg1 = atoi(pch);
                    cout << pch << endl;
                    pch = strtok(NULL, " ");
                    int arg2 = atoi(pch);
                    cout << pch << endl;
                }
                else {
                    cout << "ERROR: Function DNE" << endl;
                    exit(1);
                }
            }
        }
    }

    // while (fgets(str, MAXLEN, fp) != NULL) {
    //     pch = strtok(str, " ");
    //     src.read(pch); // creating the source image
    //     string src_name = pch;
~
    //     pch = strtok(NULL, " ");
    //     strcpy(outfile, pch);

    //     pch = strtok(NULL, " ");
    //     pch[strlen(pch) - 1] = '\0';
    //     if (strncasecmp(pch, "hist_stretch", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::histStretch(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "histStretch time for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }   
    //     else if (strncasecmp(pch, "opt_thresh", MAXLEN) == 0) {
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::optimalThresholding(src, tgt, regions);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "optimalThresholding for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else if (strncasecmp(pch, "comb_hist_thresh", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::combHistOptThresh(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "combHistOptThresh for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else if (strncasecmp(pch, "hist_red", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::histRed(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "histRed for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else if (strncasecmp(pch, "hist_green", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::histGreen(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "histGreen for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else if (strncasecmp(pch, "hist_blue", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::histBlue(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "histBlue for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else if (strncasecmp(pch, "hist_rgb", MAXLEN) == 0) {
    //         string file_name = outfile;
    //         auto start = chrono::high_resolution_clock::now();
    //         utility::histRGB(src, tgt, regions, file_name);
    //         auto end = chrono::high_resolution_clock::now();
    //         cout << "histRGB for " << src_name << " = " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    //     }
    //     else {
    //         cout << "No function: " << pch << endl;
    //         continue;
    //     }

    //     tgt.save(outfile);
    // }

    fclose(fp);
    return 0;
}