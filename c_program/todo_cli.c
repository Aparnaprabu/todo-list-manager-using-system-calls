#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME "tasks.txt"

// Add http://127.0.0.1:5000/a task
void add_task(char *task) {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) { perror("open"); return; }

    write(fd, task, strlen(task));
    write(fd, " [PENDING]\n", 11);
    close(fd);
    printf("OK\n");
}

// View all tasks (numbered)
void view_tasks() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) { printf(""); return; }

    char buffer[4096];
    int n = read(fd, buffer, sizeof(buffer)-1);
    if (n <= 0) { close(fd); return; }
    buffer[n] = '\0';

    char *line = strtok(buffer, "\n");
    int i = 1;
    while (line) {
        printf("%d|%s\n", i++, line);
        line = strtok(NULL, "\n");
    }
    close(fd);
}

// Mark a task as done
void mark_done(int task_no) {
    int fd = open(FILENAME, O_RDWR);
    if (fd < 0) { perror("open"); return; }

    char buffer[4096];
    int n = read(fd, buffer, sizeof(buffer)-1);
    buffer[n] = '\0';

    char *lines[100];
    int count = 0;
    char *line = strtok(buffer, "\n");
    while (line) {
        lines[count++] = line;
        line = strtok(NULL, "\n");
    }

    if (task_no > 0 && task_no <= count) {
        char *p = strstr(lines[task_no - 1], "[PENDING]");
        if (p) strcpy(p, "[DONE]");
    }

    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);

    for (int i = 0; i < count; i++) {
        write(fd, lines[i], strlen(lines[i]));
        write(fd, "\n", 1);
    }
    close(fd);
    printf("OK\n");
}

// Delete a task
void delete_task(int task_no) {
    int fd = open(FILENAME, O_RDWR);
    if (fd < 0) { perror("open"); return; }

    char buffer[4096];
    int n = read(fd, buffer, sizeof(buffer)-1);
    buffer[n] = '\0';

    char *lines[100];
    int count = 0;
    char *line = strtok(buffer, "\n");
    while (line) {
        lines[count++] = line;
        line = strtok(NULL, "\n");
    }

    lseek(fd, 0, SEEK_SET);
    ftruncate(fd, 0);

    for (int i = 0; i < count; i++) {
        if (i != task_no - 1) {
            write(fd, lines[i], strlen(lines[i]));
            write(fd, "\n", 1);
        }
    }
    close(fd);
    printf("OK\n");
}

// Backup tasks
void backup_tasks() {
    int fd1 = open(FILENAME, O_RDONLY);
    int fd2 = open("backup.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd1 < 0 || fd2 < 0) { perror("open"); return; }

    char buffer[1024];
    int n;
    while ((n = read(fd1, buffer, sizeof(buffer))) > 0) {
        write(fd2, buffer, n);
    }

    close(fd1);
    close(fd2);
    printf("Backup created in backup.txt\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("%s add \"task\"\n", argv[0]);
        printf("%s view\n", argv[0]);
        printf("%s mark <task_no>\n", argv[0]);
        printf("%s delete <task_no>\n", argv[0]);
        printf("%s backup\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") == 0 && argc >= 3) {
        add_task(argv[2]);
    } else if (strcmp(argv[1], "view") == 0) {
        view_tasks();
    } else if (strcmp(argv[1], "mark") == 0 && argc >= 3) {
        mark_done(atoi(argv[2]));
    } else if (strcmp(argv[1], "delete") == 0 && argc >= 3) {
        delete_task(atoi(argv[2]));
    } else if (strcmp(argv[1], "backup") == 0) {
        backup_tasks();
    } else {
        printf("Invalid command.\n");
        return 1;
    }
    return 0;
}
