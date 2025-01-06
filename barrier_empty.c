#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

static int nthread = 1;
static int round = 0;
static int timeout_ms = 1; // 超时时间，单位毫秒

// 获取当前时间
struct timespec timeout_time;

struct barrier {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  int nthread;       // 已到达屏障的线程数量
  int round;         // 当前屏障的轮次
  int waiting;       // 当前屏障的等待线程数
  struct timespec start_time; // 每轮开始时间
  long* sync_times;  // 每轮同步耗时（微秒）
  bool outputed;
} bstate;

static void barrier_init(int num_threads) {
  assert(pthread_mutex_init(&bstate.barrier_mutex, NULL) == 0);
  assert(pthread_cond_init(&bstate.barrier_cond, NULL) == 0);
  bstate.nthread = num_threads;
  bstate.round = -1;
  bstate.waiting = 0;
  bstate.sync_times = calloc(20000, sizeof(long)); // 假设最多 20000 轮
  clock_gettime(CLOCK_REALTIME, &bstate.start_time);
  // 计算超时时间
  timeout_time.tv_sec = bstate.start_time.tv_sec + timeout_ms / 1000;
  timeout_time.tv_nsec = bstate.start_time.tv_nsec + (timeout_ms % 1000) * 1000000;
  if (timeout_time.tv_nsec >= 1000000000) {
    timeout_time.tv_sec++;
    timeout_time.tv_nsec -= 1000000000;
  }
    
}

static void barrier_destroy() {
  free(bstate.sync_times);
  pthread_mutex_destroy(&bstate.barrier_mutex);
  pthread_cond_destroy(&bstate.barrier_cond);
}

// 超时屏障函数
static void barrier_with_timeout(int roundInThread, long id) {
  if(/* TODO: 需要忽略该屏障的情况 */){
    return;
  }
  pthread_mutex_lock(&bstate.barrier_mutex);

  struct timespec arrive_time;
  clock_gettime(CLOCK_REALTIME, &arrive_time);
  if(roundInThread == bstate.round + 1){
    // TODO: 计算超时时间，更新bstate状态
  }
  bstate.waiting++;
  if (bstate.waiting < bstate.nthread) {
    // TODO: 如果不是最后一个线程，等待其他线程或超时后唤醒所有线程
    // 输出: 到达信息、若超时，则输出屏障等待时间以及超时提示
  }
  else {
    // TODO: 最后一个线程到达，唤醒所有线程
    // 输出: 到达信息和屏障等待时间
  }

  pthread_mutex_unlock(&bstate.barrier_mutex);
}

// 测试线程函数
static void* thread(void* xa) {
  long id = (long)xa;
  for (int i = 0; i < 20; i++) { // 测试20轮
    barrier_with_timeout(i, id);
    usleep(random() % 100 * 100); // 模拟随机延迟
  }
  return NULL;
}

// 主函数
int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s nthread timeout_ms\n", argv[0]);
    exit(-1);
  }

  nthread = atoi(argv[1]);
  timeout_ms = atoi(argv[2]);
  pthread_t* tha = malloc(sizeof(pthread_t) * nthread);

  barrier_init(nthread);
  srandom(0);

  // 创建线程
  for (long i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void*)i) == 0);
  }

  // 等待线程完成
  for (int i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], NULL) == 0);
  }

  printf("All threads finished.\n");

  // 打印统计信息
  printf("\n=== Barrier Synchronization Statistics ===\n");
  for (int i = 0; i < 20; i++) {
    printf("Round %d: %ld us\n", i, bstate.sync_times[i]);
  }

  barrier_destroy();
  free(tha);
  return 0;
}
