/*
Andrew Schmidt  |  UCCS-Spring 2022  |  CS 4200-002 (Computer Architecture 1)
Homework 4: Cache replacement LRU vs PLRU Efficency testing
-----------------------------------------------------------------------------
Using 96 sequences of given data of CPU addresses in hex, use an array/struct
system to simulate the LRU and PLRU cache replacement policies to track
efficecy of each given # hit's/96 * 100 = Efficency %. After each sequency
output the current valid bit, Seq #, Way#, c.tag, c.data*4, c.data.index,
c.tag.index, History Stack, Status(hit/miss).
*/


#include <iostream>
#include <string>
#include <cstdio>
using namespace std;


// Given address data, in order from 0-95
const int addresses [96] = {0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394,
                            0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0,
                            0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398, 0x2d4, 0x280, 0x24c, 0x2bc,
                            0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac, 0x158, 0x28c, 0x2fc, 0x300, 0x314, 0x344, 0x374, 0x398,
                            0x2d4, 0x280, 0x24c, 0x2bc, 0x154, 0x280, 0x2f0, 0x2c0, 0x250, 0x27c, 0x370, 0x394, 0x2dc, 0x284, 0x250, 0x2ac};

struct cacheLine
{
    bool valid [4] = {false, false, false, false};
    int tag [4];
    int data [16];
};


// function Headers
float LRUCache();
float PLRUCache();
void PrintTable(int seq, int way, bool valid, int cTag, int cData, int cDataIndex, int cTagIndex, string historyStack, string status);

int main() 
{
    float LRUEffiency = 0;
    LRUEffiency = LRUCache();
    float PLRUEffiency = 0;
    PLRUEffiency = PLRUCache();

    // Compare efficencyies
    std::cout << "LRU efficency: " << LRUEffiency << "%  | PLRU efficency: " << PLRUEffiency << "%" << endl;
}


float LRUCache()
{
    cacheLine cacheLines [4];
    int history [4][4] = {{3, 2, 1, 0}, {3, 2, 1, 0}, {3, 2, 1, 0}, {3, 2, 1, 0}};
    string historyStack;
    string status;
    int hitLocation;
    float hitCounter = 0;
    int tagLocation = 0;

    for (int i = 0; i < 96; i++) 
    {
        //Set the tag logation
        tagLocation = ((addresses[i]>>4) & 0b11);
        
        //Take the data, check tag with all cache lines
        status = "Miss";
        for (int j = 0; j < 4; j++)
        {
            if ((cacheLines[j].tag[tagLocation] == (addresses[i]>>6)) && (cacheLines[j].valid[tagLocation] == true))
            {
                status = "Hit";
                hitLocation = j;
                hitCounter++;

                // Update the history for hit
                if (hitLocation == history[hitLocation][3])
                {
                    history[hitLocation][3] = history[hitLocation][2];
                    history[hitLocation][2] = history[hitLocation][1];
                    history[hitLocation][1] = history[hitLocation][0];
                    history[hitLocation][0] = hitLocation;
                } 
                else if (hitLocation == history[hitLocation][2])
                {
                    history[hitLocation][2] = history[hitLocation][1];
                    history[hitLocation][1] = history[hitLocation][0];
                    history[hitLocation][0] = hitLocation;
                } 
                else if (hitLocation == history[hitLocation][1])
                {
                    history[hitLocation][1] = history[hitLocation][0];
                    history[hitLocation][0] = hitLocation;
                }
            } 
        }
        

        // If miss, fill LRU cacheline, Update History Stack
        if (status == "Miss")
        {
            cacheLines[history[tagLocation][3]].valid[tagLocation] = true;
            cacheLines[history[tagLocation][3]].tag[tagLocation] = ((addresses[i]>>6));
            hitLocation = history[tagLocation][3];

            // Update history
            history[tagLocation][3] = history[tagLocation][2];
            history[tagLocation][2] = history[tagLocation][1];
            history[tagLocation][1] = history[tagLocation][0];
            history[tagLocation][0] = hitLocation;

        }

        historyStack = to_string(history[tagLocation][0]) + ", " + to_string(history[tagLocation][1]) + ", " + to_string(history[tagLocation][2]) + ", " + to_string(history[tagLocation][3]);
        
        // Parse data and set variables
        //seq = i
        //way = hitLocation
        // What is c.data?
        // c.tag =          ((addresses[i]>>4)<<4)
        // c.data.index =   ((addresses[i]>>2)<<2)
        // c.tag.index =    ((addresses[i]>>4)<<4)
        // historyStack is the historyStack
        // status = Hit/Miss

        std::cout << "Using LRU" << endl;
        PrintTable((i + 1), hitLocation, cacheLines[hitLocation].valid, (addresses[i]), 0000, (addresses[i]), (addresses[i]), historyStack, status);
        
    }

    std::cout << hitCounter << " = num of hits" << endl;
    return ((hitCounter/96.00) * 100.00);

}


float PLRUCache()
{
    cacheLine cacheLines [4];
    int Plru [4][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    string historyStack;
    string status;
    int way = 0;
    float hitCounter = 0;
    int tagLocation = 0;

    for (int i = 0; i < 96; i++) 
    {
        tagLocation = ((addresses[i]>>4) & 0b11);

        status = "Miss";
        for (int j = 0; j < 4; j++)
        {
            if ((cacheLines[j].tag[tagLocation] == (addresses[i]>>6)) && (cacheLines[j].valid[tagLocation] == true))
            {
                // HIT
                status = "Hit";
                way = j;
                hitCounter += 1;
                
                if (way == 0)
                {
                    Plru[way][1] = 1;
                    Plru[way][0] = 1;
                }
                else if (way == 1)
                {
                    Plru[way][1] = 1;
                    Plru[way][0] = 0;
                }
                else if (way == 2)
                {
                    Plru[way][1] = 0;
                    Plru[way][2] = 1;
                }
                else if (way == 3)
                {
                    Plru[way][1] = 0;
                    Plru[way][2] = 0;
                }
            }
        }

        //Miss
        if (status == "Miss")
        {
            if (Plru[tagLocation][1] == 0)
            {
                if (Plru[tagLocation][0] == 0)
                {
                    way = 0;
                    Plru[tagLocation][1] = 1;
                    Plru[tagLocation][0] = 1;
                }
                else if (Plru[tagLocation][0] == 1)
                {
                    way = 1;
                    Plru[tagLocation][1] = 1;
                    Plru[tagLocation][0] = 0;
                }
            }
            else if (Plru[tagLocation][1] == 1)
            {
                if (Plru[tagLocation][2] == 0)
                {
                    way = 2;
                    Plru[tagLocation][1] = 0;
                    Plru[tagLocation][2] = 1;
                }
                else if (Plru[tagLocation][2] == 1)
                {
                    way = 3;
                    Plru[tagLocation][1] = 0;
                    Plru[tagLocation][2] = 0;
                }
            }
            cacheLines[way].tag[tagLocation] = (addresses[i]>>6);
            cacheLines[way].valid[tagLocation] = true;
        }


        historyStack = to_string(Plru[tagLocation][0]) + ", " + to_string(Plru[tagLocation][1]) + ", " + to_string(Plru[tagLocation][2]);

        std::cout << "Using PLRU" << endl;
        PrintTable((i + 1), way, cacheLines[way].valid, (addresses[i]), 0000, (addresses[i]), (addresses[i]), historyStack, status);
    }

    std::cout << hitCounter << " num of hits" << endl;
    return ((hitCounter / 96.0) * 100.0);
}


// Print the table formated to show information for each sequence
void PrintTable(int seq, int way, bool valid, int cTag, int cData, int cDataIndex, int cTagIndex, string historyStack, string status)
{
    cout << "\tSeq#" << seq << "\t\tWay#" << way << endl;
    cout << "v\tc.tag\t\tc.data\tc.data.index    " << (cDataIndex>>2 & 0b1111) << endl;
    cout << ((valid)? 1 : 0) << "\t" << (cTag>>6) << "\t\t\tD1\tc.tag.index     " << (cTagIndex>>4 & 0b11) << endl;
    cout << "\t\t\t\tD2\tHistory Stack   " << historyStack << endl;
    cout << "\t\t\t\tD3\tStatus          " << status << endl;
    cout << "\t\t\t\tD4" << endl << endl;
}

