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

#define KEY 24602

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                             (Linux-specific) */
// };

int main(int argc, char const *argv[]){
  struct sembuf *sbuffer;
  int semd, shmd;

  semd = semget(KEY, 1, 0);
  shmd = shmget(KEY, sizeof(int), 0644);

  sbuffer = malloc(sizeof(struct sembuf));
  sbuffer->sem_num = 0;
  sbuffer->sem_op = -1;
  sbuffer->sem_flg = SEM_UNDO;
  semop(semd, sbuffer, 1);

  int fd = open("story.txt", O_RDWR | O_APPEND);
  char * storyNow = malloc(10000);
  read(fd, storyNow, 10000);

  int *lastAddition = shmat(shmd, 0, 0);
  printf("Last Addition: %s\n", lastAddition);

  printf("Your Addition: ");
  char * addText = malloc(1000);
  fgets(addText, 1000, stdin);
  addText[strlen(addText) - 1] = '\0';
  write(fd, addText, strlen(addText));
  strcpy(storyNow, addText);

  close(fd);
  sbuffer->sem_op = 1;
  semop(semd, sbuffer, 1);
  free(storyNow);
  free(addText);
}
