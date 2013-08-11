#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_random(int file_cnt, int min_file_size, int max_file_size, const char* dir)
{
    FILE* fp = NULL;
    char filename[256];
    int i = 0;

    for ( ; i < file_cnt; i++)
    {
        bzero(filename, sizeof(filename)); 
        sprintf(filename, "%s/%d", dir, i);
        fp = fopen(filename, "wb");
        if (fp == NULL)
        {
            perror("file open failed");
            exit(1);
        }
        int cnt = rand() % max_file_size + min_file_size;
        char *p = (char*)malloc(cnt * sizeof(char));
        if (p == NULL)
        {
            perror("file open failed");
            exit(1);
        }
        fwrite(p, sizeof(char), cnt, fp);
        fclose(fp);
        free(p);
    }
    return 0;
}

int print_filepath(int file_cnt, const char* dir)
{
    int i = 0;
    for (; i < file_cnt; i++)
    {
        printf("%s/%d\n", dir, i);
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s file_cnt min_file_size(bytes) max_file_size(bytes) dir\n", argv[0]);
        exit(1);
    }
    int file_cnt = atoi(argv[1]);
    int min_file_size = atoi(argv[2]);
    int max_file_size = atoi(argv[3]);
    const char* dir = argv[4];
    write_random(file_cnt, min_file_size, max_file_size, dir);
    print_filepath(file_cnt, dir);


	return 0;
}

