#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>

using namespace std;

struct Coordinate {
    float x, y, z;
    bool operator<(const Coordinate& rhs){
        return x<rhs.x || (x == rhs.x&&y<rhs.y) || (x == rhs.x&&y == rhs.y&&z<rhs.z);
    }
    bool operator==(const Coordinate& rhs){
        return x == rhs.x&&y == rhs.y && z == rhs.z;
    }
};
vector<Coordinate> vecSorted, vecOrigin;
vector<Coordinate>::iterator iter, iterBegin;

int numberOfFacets;
int numberOfPoints;
int index;


char c1[] = "ply\nformat binary_little_endian 1.0\ncomment By ET \nelement vertex ";
char c2[] = "\nproperty float x\nproperty float y\nproperty float z\nelement face ";
char c3[] = "\nproperty list uchar int vertex_indices\nend_header\n";

int main()
{
    clock_t start, finish;
    double totaltime;
    start = clock();

    int length;
    int position = 80;
    fstream fileIn("test.stl", ios::in | ios::binary);
    fileIn.seekg(0, ios::end);
    length = (int)fileIn.tellg();
    fileIn.seekg(0, ios::beg);
    char* buffer = new char[length];
    fileIn.read(buffer, length);
    fileIn.close();




    numberOfFacets = *(int*)&(buffer[position]);
    position += 4;
    cout << "Number of Facets: " << numberOfFacets << endl;
    for (int i = 0; i < numberOfFacets; i++)
    {
        Coordinate tmpC;
        position += 12;
        for (int j = 0; j < 3; j++)
        {
            tmpC.x = *(float*)&(buffer[position]);
            position += 4;
            tmpC.y = *(float*)&(buffer[position]);
            position += 4;
            tmpC.z = *(float*)&(buffer[position]);
            position += 4;
            vecOrigin.push_back(tmpC);
        }
        position += 2;
    }

    free(buffer);



    vecSorted = vecOrigin;
    sort(vecSorted.begin(), vecSorted.end());
    iter = unique(vecSorted.begin(), vecSorted.end());
    vecSorted.erase(iter, vecSorted.end());
    numberOfPoints = vecSorted.size();



    ofstream fileOut("test.ply", ios::binary | ios::out | ios::trunc);

    fileOut.write(c1, sizeof(c1)-1);
    fileOut << numberOfPoints;
    fileOut.write(c2, sizeof(c2)-1);
    fileOut << numberOfFacets;
    fileOut.write(c3, sizeof(c3)-1);


    buffer = new char[numberOfPoints * 3 * 4];
    position = 0;
    for (int i = 0; i < numberOfPoints; i++)
    {
        buffer[position++] = *(char*)(&vecSorted[i].x);
        buffer[position++] = *((char*)(&vecSorted[i].x) + 1);
        buffer[position++] = *((char*)(&vecSorted[i].x) + 2);
        buffer[position++] = *((char*)(&vecSorted[i].x) + 3);
        buffer[position++] = *(char*)(&vecSorted[i].y);
        buffer[position++] = *((char*)(&vecSorted[i].y) + 1);
        buffer[position++] = *((char*)(&vecSorted[i].y) + 2);
        buffer[position++] = *((char*)(&vecSorted[i].y) + 3);
        buffer[position++] = *(char*)(&vecSorted[i].z);
        buffer[position++] = *((char*)(&vecSorted[i].z) + 1);
        buffer[position++] = *((char*)(&vecSorted[i].z) + 2);
        buffer[position++] = *((char*)(&vecSorted[i].z) + 3);
    }


    fileOut.write(buffer, numberOfPoints * 3*4);




    free(buffer);

    buffer = new char[numberOfFacets * 13];

    for (int i = 0; i < numberOfFacets; i++)
    {
        buffer[13*i] = (unsigned char)3;
    }

    iterBegin = vecSorted.begin();
    position = 0;
    for (int i = 0; i < numberOfFacets; i++)
    {
        position++;
        for (int j = 0; j < 3; j++)
        {
            iter = lower_bound(vecSorted.begin(), vecSorted.end(), vecOrigin[3*i+j]);
            index = iter - iterBegin;
            buffer[position++] = *(char*)(&index);
            buffer[position++] = *((char*)(&index) + 1);
            buffer[position++] = *((char*)(&index) + 2);
            buffer[position++] = *((char*)(&index) + 3);

        }
    }


    fileOut.write(buffer, numberOfFacets * 13);



    free(buffer);
    fileOut.close();


    finish = clock();
    totaltime = (double)(finish - start) / CLOCKS_PER_SEC*1000;
    cout << "All Time: " << totaltime << "ms\n";

    return 0;
}
