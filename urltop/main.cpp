#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include <unordered_set>
#include "MinHeapHelper.h"

using std::string;
using std::unordered_map;
using std::unordered_set;

const int kSortPartitionNum = 7;
const int kSeed = 131313;
const int kHashSize = 1 << 28;

unsigned int BKDRHash(const char* str, int num)
{
    unsigned int hash = 0;
    for(int i = 0; i < num; ++i)
        hash = hash * kSeed + (*str++);
    return hash % kHashSize;
}


int LoadBuffer(char* buffer, int fd, off_t& offset)
{
    memset(buffer, 0, 4096); 
    int ret = pread(fd, buffer, 4096, offset);
    if(ret == -1)
        return -1;
    if(ret == 0)
        return 0;
    while(buffer[--ret] != '\n') ;
    offset = offset + ret + 1;
    return ret + 1;
}

void HandleBuffer(char* buffer, int length, unsigned int *hash_arr)
{
    int start = 0;
    for(int i = 0; i < length; ++i)
    {
        if(buffer[i] == '\n')
        {
            hash_arr[BKDRHash(buffer+start, i-start+1)] += 1;
            //write(1, buffer+start, i-start+1);
            start = i + 1;
        }
    }
}


void HandleBuffer2(char* buffer, int length, const unordered_set<unsigned int>& hash_set, unordered_map<string, unsigned int> &url_map)
{
    int start = 0;
    for(int i = 0; i < length; ++i)
    {
        if(buffer[i] == '\n')
        {
            if(hash_set.find(BKDRHash(buffer+start, i-start+1)) != hash_set.end())
            {
                string tmp = string(buffer+start, i-start);
                if(url_map.find(tmp) != url_map.end())
                    url_map[tmp] += 1;
                else
                    url_map[tmp] = 1;
            }
            start = i + 1;
        }
    }
}


int main(int argc, char** argv)
{
    char buffer[4096];
    if(argc != 2)
        fprintf(stderr, "invalid param\nargv[1]: file_name\n");
    int fd = open(argv[1], O_RDONLY, 0666);
    if(-1 == fd)
    {
        fprintf(stderr, "open file failed, file:%s\n", argv[1]);
        return 0;
    }
    unsigned int* hash_arr = (unsigned int*)calloc(kHashSize, sizeof(unsigned int));
    if(hash_arr == NULL)
    {
        fprintf(stderr, "malloc memory failed\n");
        return 0;
    }
    memset((void*)hash_arr, 0, kHashSize);
    off_t offset = 0;
    int read_length = 0;
    while((read_length = LoadBuffer(buffer, fd, offset)) > 0)
        HandleBuffer(buffer, read_length, hash_arr);

    MinHeapHelper hp(100);
    auto top100_hash = hp.GetResult(hash_arr, kHashSize);
    //for(int i = 0; i < top100_hash.size(); ++i)
    //{
    //    printf("hash:%u, count:%u\n", top100_hash[i].first, top100_hash[i].second);
    //}
    //
    free((void*)hash_arr);

    unordered_set<unsigned int> hash_set;
    for(int i = 0; i < top100_hash.size(); ++i)
        hash_set.insert(top100_hash[i].first);

    offset = 0;
    unordered_map<string, unsigned int> url_map;
    while((read_length = LoadBuffer(buffer, fd, offset)) > 0)
        HandleBuffer2(buffer, read_length, hash_set, url_map);

    auto top100_url = hp.GetResult2(url_map);
    for(int i = 0; i < top100_url.size(); ++i)
    {
        printf("url:%s, count:%u\n", top100_url[i].first.c_str(), top100_url[i].second);
    }


    return 0;

}
