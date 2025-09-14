#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "file_operations.h"

int create_and_write_file(const char *filename, const char *content) {
    // Declare variables
    int fd;
    ssize_t bytes_written;

    // Print intent
    printf("[create] Creating file: \"%s\"\n", filename);
    printf("[create] Content to write (%zu bytes): \"%s\"\n",
           content ? strlen(content) : 0UL,
           content ? content : "(null)");

    // Open or create the file for writing (0644 permissions)
    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("[create] open");
        return -1;
    }

    printf("[create] File descriptor: %d\n", fd);

    
    const char *data = content ? content : "";
    size_t remaining = strlen(data);
    const char *p = data;
    size_t total_written = 0;

    // Write loop 
    while (remaining > 0) {
        bytes_written = write(fd, p, remaining);
        if (bytes_written == -1) {
            perror("[create] write");
            if (close(fd) == -1) {
                perror("[create] close (after write error)");
            }
            return -1;
        }
        total_written += (size_t)bytes_written;
        p += bytes_written;
        remaining -= (size_t)bytes_written;
    }

    printf("[create] Wrote %zu bytes to \"%s\"\n", total_written, filename);

    // Close the file
    if (close(fd) == -1) {
        perror("[create] close");
        return -1;
    }

    printf("[create] File \"%s\" closed successfully.\n", filename);
    return 0;
}

int read_file_contents(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        ssize_t off = 0;
        while (off < bytes_read) {
            ssize_t w = write(STDOUT_FILENO, buffer + off, (size_t)(bytes_read - off));
            if (w == -1) {
                perror("write");
                close(fd);
                return -1;
            }
            off += w;
        }
    }

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return -1;
    }

    //add one trailing newline for clean terminal output
    if (write(STDOUT_FILENO, "\n", 1) == -1) {
        perror("write");
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror("close");
        return -1;
    }

    return 0;
}

