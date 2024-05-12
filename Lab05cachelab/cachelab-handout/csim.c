#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "cachelab.h"

int v = 0, s, E, b, t, timestamp = 0;

unsigned hit = 0, miss = 0, eviction = 0;

void printUsage()
{
    puts("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>");
    puts("Options:");
    puts("  -h         Print this help message.");
    puts("  -v         Optional verbose flag.");
    puts("  -s <num>   Number of set index bits.");
    puts("  -E <num>   Number of lines per set.");
    puts("  -b <num>   Number of block offset bits.");
    puts("  -t <file>  Trace file.");
    puts("");
    puts("Examples:");
    puts("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace");
    puts("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace");
}

typedef struct
{
    int valid;
    int tag;
    int last_used_time;
} cache_line;

// 定义组，每组E行
typedef cache_line *set;

// 缓存，共S个组(S == 2**s)
set *cache;

void useCache(size_t address, int is_modify)
{
    int set_pos = address >> b & ((1 << s) - 1);
    int tag = address >> (s + b);

    set cur_set = cache[set_pos];
    int lru_pos = 0, lru_time = cur_set[0].last_used_time;

    for (int i = 0; i < E; ++i)
    {
        if (cur_set[i].tag == tag)
        {
            ++hit;
            hit += is_modify; // 修改操作先读后写，因此有两次hit
            cur_set[i].last_used_time = timestamp;
            if (v)
            {
                printf("hit\n");
            }
            return;
        }

        if (cur_set[i].last_used_time < lru_time)
        {
            lru_time = cur_set[i].last_used_time;
            lru_pos = i;
        }
    }

    // 当前组未找到
    ++miss;

    // load or eviction
    cur_set[lru_pos].tag = tag;
    cur_set[lru_pos].last_used_time = timestamp;

    eviction += (lru_time != -1);
    hit += is_modify;

    if (v)
    {
        if (is_modify)
        {
            if (lru_time != -1)
            {
                printf("miss eviction hit\n");
            }
            else
            {
                printf("miss hit\n");
            }
        }
        else
        {
            if (lru_time != -1)
                printf("miss eviction\n");
            else
                printf("miss\n");
        }
    }

    return;
}

int main(int argc, char *argv[])
{
    int opt;

    FILE *trace_file;

    if (argc == 1)
    {
        printUsage();
        exit(0);
    }
    // 获取命令行参数
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printUsage();
            exit(0);
        case 'v':
            v = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = fopen(optarg, "r");
            break;
        default:
            printUsage();
            exit(0);
        }
    }
    // 校验参数
    if (s <= 0 || E <= 0 || b <= 0 || s + b > 64 || trace_file == NULL)
    {
        printUsage();
        exit(1);
    }

    // 初始化缓存
    int set_num = 1 << s;
    cache = (set *)malloc(sizeof(set) * set_num);
    for (int i = 0; i < set_num; ++i)
    {
        cache[i] = (set)malloc(sizeof(cache_line) * E);
        for (int j = 0; j < E; ++j)
        {
            cache[i][j].valid = -1;
            cache[i][j].tag = -1;
            cache[i][j].last_used_time = -1;
        }
    }

    char operation;
    size_t address; // unsigned long
    int size;

    while (fscanf(trace_file, "%s %lx,%d\n", &operation, &address, &size) == 3)
    {
        ++timestamp;
        if (v)
        {
            printf("%c %lx,%d ", operation, address, size);
        }
        switch (operation)
        {
        case 'I':
            continue;
        case 'M': // Modify = Load + Store
            useCache(address, 1);
            break;
        case 'L': // Load
        case 'S': // Store
            useCache(address, 0);
        }
    }

    // 释放动态分配的内存
    for (int i = 0; i < set_num; ++i)
    {
        free(cache[i]);
    }
    free(cache);

    printSummary(hit, miss, eviction);
    return 0;
}
