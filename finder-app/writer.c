#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static int write_all(int fd, const char *buffer, size_t length)
{
    while (length > 0) {
        ssize_t written = write(fd, buffer, length);

        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (written == 0) {
            errno = EIO;
            return -1;
        }

        buffer += written;
        length -= (size_t)written;
    }

    return 0;
}

static void report_error(const char *operation, const char *path)
{
    int error_number = errno;

    syslog(LOG_ERR, "%s '%s': %s",
           operation, path, strerror(error_number));

    fprintf(stderr, "%s '%s': %s\n",
            operation, path, strerror(error_number));
}

int main(int argc, char *argv[])
{
    openlog("writer", LOG_PID, LOG_USER);

    if (argc != 3) {
        syslog(LOG_ERR, "Usage: %s <file path> <text>", argv[0]);
        fprintf(stderr, "Usage: %s <file path> <text>\n", argv[0]);

        closelog();
        return EXIT_FAILURE;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    int fd = open(writefile, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd < 0) {
        report_error("Cannot open", writefile);
        closelog();
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    if (write_all(fd, writestr, strlen(writestr)) < 0 ||
        write_all(fd, "\n", 1) < 0) {
        report_error("Cannot write", writefile);
        status = EXIT_FAILURE;
    }

    if (close(fd) < 0) {
        report_error("Cannot close", writefile);
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS) {
        syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
    }

    closelog();
    return status;
}
