/*
Use system call 223.
Copyright @ MihawkHu
*/
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __NR_mycall 223

struct prinfo {
	pid_t parent_pid;		        /* process id of parent */
	pid_t pid;			            /* process id */
	pid_t first_child_pid;		  /* pid of youngest child */
	pid_t next_sibling_pid;		  /* pid of older sibling */

	long state;			            /* current state of process */
	long uid;			              /* user id of process owner */
	char comm[64];			        /* name of program executed */
};

//system call
int ptree(struct prinfo *buf, int *nr) {
  return syscall(__NR_mycall, buf, nr);
}

// print the tree
void ptree_display(struct prinfo *buf, int *nr) {
	int pid_pos[1000] = { 0 };	//record the each pid position
	int tab_cnt = 0;	//record the number of tab

	printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[0].comm,
		buf[0].pid, buf[0].state, buf[0].parent_pid,
			buf[0].first_child_pid, buf[0].next_sibling_pid, buf[0].uid);

	// print
	int i = 1;
	while (i < *nr) {
		if (buf[i].parent_pid == buf[i - 1].pid) {	//forwark tab
			tab_cnt++;
		}
		else if (buf[i].parent_pid != buf[i - 1].parent_pid) {	//back tab
			int lvl_tmp = buf[i - 1].parent_pid;
			tab_cnt--;
			while (buf[i].parent_pid != buf[pid_pos[lvl_tmp]].parent_pid) {
				tab_cnt--;
				lvl_tmp = buf[pid_pos[lvl_tmp]].parent_pid;
			}
		}

		pid_pos[buf[i].pid] = i;	//record position

		// print tab
		int j = 1;
		while (j <= tab_cnt) {
			printf("\t");
			++j;
		}

		printf("%s,%d,%ld,%d,%d,%d,%ld\n", buf[i].comm,
			buf[i].pid, buf[i].state, buf[i].parent_pid,
				buf[i].first_child_pid, buf[i].next_sibling_pid, buf[i].uid);

		++i;
	}
}

int main(int argc, char **argv)
{
	//check input
	if (argc != 1) {
		printf("Input Error!\n");
		exit(EXIT_FAILURE);
	}

	// allocate dynamic space
	struct prinfo *buf = malloc(1000 * sizeof(struct prinfo));
	int *nr = malloc(sizeof(int));
	if (buf == NULL || nr == NULL) {
		printf("Allocation error!\n");
		exit(EXIT_FAILURE);
	}

	ptree(buf, nr);
	printf("There are %d processes!\n", *nr);
	ptree_display(buf, nr);

	free(buf);  free(nr);

  return 0;
}
