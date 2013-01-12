#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

struct clientdata {
  int sock;
  struct sockaddr_in saddr;
};

void *threadfunc(void *data)
{
  int sock;
  struct clientdata *cdata = data;
  char buf[1024];
  int n;

  if (data == NULL) {
    return (void *)-1;
  }

  /* (5) */
  sock = cdata->sock;

  /* (6) */
  for(;;){
	n = read(sock, buf, sizeof(buf));
  if (n < 0) {
    perror("read");
    goto err;
  }

  n = write(sock, buf, n);
  if (n < 0) {
    perror("write");
    goto err;
  }
	}
  /* (7) */
  /* TCPǷǿǳ�����U�פF */
  if (close(sock) != 0) {
    perror("close");
    goto err;
  }

  free(data);

  return NULL;

err:
  free(data);
  return (void *)-1;
}

int
main()
{
  int sock0;
  struct sockaddr_in addr;
  socklen_t len;
  pthread_t th;
  struct clientdata *cdata;

  /* (1) */
  /* ǹǭǿ���U�@�� */
  sock0 = socket(AF_INET, SOCK_STREAM, 0);

  /* ǹǭǿ���U�]�w */
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

  /* TCPǫ���~�|�������p�U���d�n�D�y���M�r�]�A�R�@�r */
  listen(sock0, 5);
  /* (1) ���v�q */

  /* (2) */
  for (;;) {
    cdata = malloc(sizeof(struct clientdata));
    if (cdata == NULL) {
      perror("malloc");
      return 1;
    }

    /* TCPǫ���~�|�������p�U���d�n�D�y�����I���r */
    len = sizeof(cdata->saddr);
    cdata->sock = accept(sock0, (struct sockaddr *)&cdata->saddr, &len);

    /* (3) */
    if (pthread_create(&th, NULL, threadfunc, cdata) != 0) {
      perror("pthread_create");
      return 1;
    }

    /* (4) */
    if (pthread_detach(th) != 0) {
      perror("pthread_detach");
      return 1;
    }
  }

  /* (8) */
  /* listen �@�rsocket�U�פF */
  if (close(sock0) != 0) {
    perror("close");
    return 1;
  }

  return 0;
}

