#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using std::unordered_map;
using std::string;

const double kPI = 3.141592654;
const char* kHTTP = "http://";
const char* kLETTER = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&?=%";
const int kMU = 50;
const double kSIGMA = 10.0;

char gBuffer[8192];

int Gaussrand(int mu, double sigma) // generate gauss distribution
{
    static double u1, u2;
    static int phase = 0;
    double z;
    u1 = rand()/(RAND_MAX + 1.0);
    u2 = rand()/(RAND_MAX + 1.0);
    if(phase)
        z = sqrt((-2.0) * log(u1)) * sin(2.0 * kPI * u2);
    else
        z = sqrt((-2.0) * log(u1)) * cos(2.0 * kPI * u2);
    return int(z * sigma + mu);
}


void GenerateSingleUrl(char* buffer)
{
    strcpy(buffer, kHTTP);
    int letter_len = strlen(kLETTER);
    int http_len = strlen(kHTTP);
    int length = Gaussrand(kMU, kSIGMA);
    int j = http_len;
    for(; j < length && j < 128 - http_len - 2; ++j)
        buffer[j] = kLETTER[rand() % letter_len];
    buffer[j] = '\n';
    buffer[j+1] = '\0';
    return;
}

void GenerateTop(int num, unordered_map<string, int>& top_map, int fd)
{
    char buffer[128];
    for(int i = 0; i < num; i++)
    {
        GenerateSingleUrl(buffer);
        top_map[string(buffer)] = 0;
        write(fd, buffer, strlen(buffer));
    }
    return;
}


uint64_t GetFileSize(int fd)
{
    uint64_t file_size = -1;    
    struct stat statbuff;
    if(fstat(fd, &statbuff) < 0){
        return file_size;
    }else{
        file_size = statbuff.st_size;
    }
    return file_size;
}

void GenerateTarget(unordered_map<string, int>& top, uint64_t file_size, int fd)
{
    uint64_t size = file_size << 30;
    uint64_t line_count = 0;
    unordered_map<string, int>::iterator iter = top.begin();
    char tmp_buffer[128]; 
    uint64_t ret;
    while((ret = GetFileSize(fd)) != -1)
    {
        if(ret < size)
        {
            int buffer_size = 0;
            while(buffer_size < 8192 - 128)
            {
                strcpy(gBuffer + buffer_size, iter->first.c_str());
                buffer_size = buffer_size + iter->first.size();
                ++(iter->second);
                ++iter;
                if(iter == top.end())
                    iter = top.begin();
                ++line_count;
                GenerateSingleUrl(tmp_buffer);
                strcpy(gBuffer + buffer_size, tmp_buffer);
                buffer_size = buffer_size + strlen(tmp_buffer);
                ++line_count;
            }
            write(fd, gBuffer, buffer_size);
        }
        else
            break;
    }
    printf("file size:%ld, line_count:%ld\n", GetFileSize(fd), line_count);
    for(iter = top.begin(); iter != top.end(); ++iter)
    {
        printf("url: %s", iter->first.c_str());
        printf("num:%ld\n", iter->second);
    }
    
}


int main(int argc, char** argv) 
{
    if(argc != 3)
    {
        fprintf(stderr, "invalid param\nargv[1]: total_size(GB)\nargv[2]:file_name\nexit\n");
        return 0;
    }
    uint64_t file_size = atoi(argv[1]); // GB
    int fd = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(-1 == fd)
    {
        fprintf(stderr, "open file failed, file:%s\n", argv[2]);
        return 0;
    }
    int fd_top100 = open("./top100.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(-1 == fd_top100)
    {
        fprintf(stderr, "open file failed, file:./top100.txt\n");
        return 0;
    }

    srand((unsigned)time(NULL));

    unordered_map<string, int> top100;
    GenerateTop(100, top100, fd_top100);
    close(fd_top100);
    GenerateTarget(top100, file_size, fd);
    close(fd);
    return 0;

}
