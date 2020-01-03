#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>

#define KEY 24601

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                             (Linux-specific) */
// };

void creating(){
  union semun s;
  int semd, shmd;
  semd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  shmd = shmget(KEY, sizeof(int), 0644 | IPC_CREAT);
  if (semd < 0) {
    printf("error %d: %s\n", errno, strerror(errno));
    semd = semget(KEY, 1, 0);
    int v = semctl(semd, 0, GETVAL, 0);
    printf("semctl returned: %d\n", v);
  }
  else{
    printf("semaphore created\n");
    s.val = 1;
    semctl(semd, 0, SETVAL, s);
  }
  if (shmd < 0) {
    printf("error %d: %s\n", errno, strerror(errno));
    semd = semget(KEY, 1, 0);
    int v = semctl(semd, 0, GETVAL, 0);
    printf("semctl returned: %d\n", v);
  }
  else{
    printf("shared memory created\n");
  }
  int fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
  if (fd < 0){
    printf("error %d: %s\n", errno, strerror(errno));
  }
  else{
    printf("file created\n\n");
  }
}

void viewing(){
  int fd = open("story.txt", O_RDONLY);
  char *text = malloc(10000);
  read(fd, text, 10000);
  printf("The story so far:\n%s", text);
  free(text);
  close(fd);
}

void removing(){
  int semd, shmd;
  semd = semget(KEY, 1, 0644);
  if (semd < 0) {
    printf("error: %s\n", strerror(errno));
  }
  shmd = shmget(KEY, 0, 0644);
  if (shmd < 0) {
    printf("error: %s\n", strerror(errno));
  }
  struct sembuf *sbuffer = malloc(sizeof(struct sembuf));
  sbuffer->sem_num = 0;
  sbuffer->sem_op = -1;
  sbuffer->sem_flg = SEM_UNDO;
  semop(semd, sbuffer, 1);

  printf("Trying to get in\n");
  sbuffer->sem_num = 0;
  sbuffer->sem_op = -1;
  int i = semop(semd, sbuffer, 1);
  if (i < 0){
    printf("error: %s\n", strerror(errno));
  }
  else{
    printf("got the semaphore\n");
  }

  printf("trying to get in\n");
  viewing();

  i = semctl(semd, IPC_RMID, 0);
  if (i < 0){
    printf("error: %s\n", strerror(errno));
  }
  else{
    printf("semaphore removed\n");
  }
  i = shmctl(shmd, IPC_RMID, 0);
  if (i < 0) {
    printf("error: %s\n", strerror(errno));
  }
  else{
    printf("shared memory removed\n");
  }

  remove("story.txt");
  printf("file removed");
}

int main(int argc, char *argv[]){
  char *line = malloc(256);
  if (argc == 2){
    if (strcmp(argv[1], "-c") == 0){
      creating();
    }
    else if (strcmp(argv[1], "-r") == 0){
      removing();
    }
    else if (strcmp(argv[1], "-v") == 0){
      viewing();
    }
    else{
      printf("please enter a valid command\n");
    }
    return 0;
  }
}
