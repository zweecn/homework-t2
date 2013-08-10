#include "w_shm_conf.h"

const char* __shmid_conf = "conf/shmid.conf";

const char* __shm_mem_table_conf = "conf/shm_mem_table.conf";
const char* __sem_mem_table_conf = "conf/sem_mem_table.conf";

const char* __shm_pid_table_conf = "conf/shm_pid_table.conf";
const char* __sem_pid_table_conf = "conf/sem_pid_table.conf";

const char* __fifo_prefix = "conf/fifo_";

const int __shm_block_size = 10; 
const int __process_size = 2;
const int __mem_size = 8*1024*__shm_block_size;


const char* __mem_file = "conf/mem_file.conf";
const char* __proc_file = "conf/proc_file.conf";


const char* __sem_table_conf = "conf/sem_table.conf";
