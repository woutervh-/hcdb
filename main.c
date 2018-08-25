#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

#define FILEPATH "./mmapped.bin"
#define INTERNAL_NODE_TYPE 0
#define LEAF_NODE_TYPE 1
// #define ROOT_INTERNAL_NODE_TYPE 2
// #define ROOT_LEAF_NODE_TYPE 3
#define MAX_FILE_SIZE 1UL << 40

struct Index
{
    int readFd;
    int writeFd;
    uint64_t *map;
    uint64_t root;
    uint64_t branchingFactor;
};

struct Words
{
    uint64_t wordCount;
    uint64_t *words;
};

struct InternalNode
{
    uint64_t type;
    uint64_t count;
    struct Words *keys;
    uint64_t *pointers;
};

struct LeafNode
{
    uint64_t type;
    uint64_t count;
    struct Words *keys;
    struct Words *values;
};

uint64_t search(const struct Index index, const struct Words key, uint64_t node)
{
    uint64_t type = index.map[node];

    switch (type)
    {
    // case ROOT_INTERNAL_NODE_TYPE:
    case INTERNAL_NODE_TYPE:
    // TODO: implementation
    // case ROOT_LEAF_NODE_TYPE:
    case LEAF_NODE_TYPE:
        return node;
    }
}

void insert(const struct Index index, const struct Words key, const struct Words value)
{
    uint64_t leaf = search(index, key, index.root);
    uint64_t type = index.map[leaf];
    uint64_t count = index.map[leaf + 1];

    printf("%d %d %d", leaf, type, count);
}

void openIndex(struct Index *index, const char *filename)
{
    index->readFd = open(FILEPATH, O_RDONLY);
    if (index->readFd == -1)
    {
        perror("Error opening file for reading.");
        exit(EXIT_FAILURE);
    }

    index->writeFd = open(FILEPATH, O_WRONLY | O_APPEND);
    if (index->readFd == -1)
    {
        perror("Error opening file for writing.");
        exit(EXIT_FAILURE);
    }

    index->map = mmap(NULL, MAX_FILE_SIZE, PROT_READ, MAP_PRIVATE, index->readFd, 0);
    if (index->map == MAP_FAILED)
    {
        perror("Error mmapping the file.");
        exit(EXIT_FAILURE);
    }

    index->root = 0;
    index->branchingFactor = 4;
}

void closeIndex(struct Index *index)
{
    close(index->readFd);
    close(index->writeFd);
    munmap(index->map, MAX_FILE_SIZE);
    index->readFd = -1;
    index->writeFd = -1;
    index->map = 0;
}

void initialize(const char *filename)
{
    struct LeafNode root = {
        .type = LEAF_NODE_TYPE,
        .count = 123,
        .keys = 0,
        .values = 0};

    int fd = open(FILEPATH, O_WRONLY | O_APPEND | O_CREAT, (mode_t)0600);
    if (fd == -1)
    {
        perror("Error opening file for writing.");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &root, sizeof(struct LeafNode)) == -1)
    {
        perror("Error writing to the file.");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1)
    {
        perror("Error closing the file.");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    struct Index index;

    remove(FILEPATH);
    initialize(FILEPATH);
    openIndex(&index, FILEPATH);

    while (1)
    {
        int c, key, value;
        if (scanf("%d %d", &key, &value) != 2)
        {
            while ((c = getchar()) != EOF && c != '\n')
                continue;
        }
        else
        {
            struct Words key = {
                .wordCount = 1,
                .words = 0
            };
            insert(index, key, key);
        }
    }

    closeIndex(&index);
}
