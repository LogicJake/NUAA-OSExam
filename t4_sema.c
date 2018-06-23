/*
  通过信号量控制线程执行顺序
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
	int value;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}sema_t;

sema_t t1_2_ready;
sema_t t1_3_ready;
sema_t t2_ready;
sema_t t3_ready;


void sema_init(sema_t *sema, int value)
{
	sema->value = value;
	pthread_mutex_init(&sema->mutex, NULL);
	pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	sema->value--;
	while (sema->value < 0)
		pthread_cond_wait(&sema->cond, &sema->mutex);
	pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
	pthread_mutex_lock(&sema->mutex);
	++sema->value;
	pthread_cond_signal(&sema->cond);
	pthread_mutex_unlock(&sema->mutex);
}

void *T1_entry(void *arg)
{
	sleep(2);  // 睡眠2秒，不准删除此条语句，否则答题无效
	puts("T1");
	sema_signal(&t1_2_ready);
	sema_signal(&t1_3_ready);

}

void *T2_entry(void *arg)
{
	sema_wait(&t1_2_ready);
	sleep(1);  // 睡眠1秒，不准删除此条语句，否则答题无效
	puts("T2");
	sema_signal(&t2_ready);
}

void *T3_entry(void *arg)
{
	sema_wait(&t1_3_ready);
	sleep(1);  // 睡眠1秒，不准删除此条语句，否则答题无效
	puts("T3");
	sema_signal(&t3_ready);

}

void *T4_entry(void *arg)
{
	sema_wait(&t2_ready);
	sema_wait(&t3_ready);
	puts("T4");
}

int main()
{
	sema_init(&t1_2_ready,0);
	sema_init(&t1_3_ready,0);
	sema_init(&t2_ready,0);
	sema_init(&t3_ready,0);

	pthread_t T1,T2,T3,T4;
	pthread_create(&T1, NULL, T1_entry, NULL);
	pthread_create(&T2, NULL, T2_entry, NULL);
	pthread_create(&T3, NULL, T3_entry, NULL);
	pthread_create(&T4, NULL, T4_entry, NULL);
	pthread_join(T1,NULL);
	pthread_join(T2,NULL);
	pthread_join(T3,NULL);
	pthread_join(T4,NULL);
	return 0;
}
