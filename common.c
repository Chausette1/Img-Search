#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int read_safe(int fd, char * buffer, size_t size)
{
    int bytes = 0;

    while (bytes < size)
    {
        int res = read(fd, (&buffer)[bytes], size - bytes);

        if (bytes < 0 && errno != EINTR)
        {
            perror("read");
            return 1;
        }
        else 
        {
            bytes += res;
        }
    }

    return 0;
}

int write_safe(int fd, char * buffer, size_t size)
{
    int bytes = 0;

    while (bytes < size)
    {
        int res = write(fd, (&buffer)[bytes], size - bytes);

        if (bytes < 0 && errno != EINTR)
        {
            perror("write");
            return 1;
        }
        else 
        {
            bytes += res;
        }
    }

    return 0;
}
