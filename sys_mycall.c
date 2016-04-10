/*
<Add new system call--ptree>
copyright @ MihawkHu
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/unistd.h>

#include <linux/syscalls.h>
#include <linux/slab.h> //alloction function
#include <linux/uaccess.h>	//kernel--user
#include <linux/list.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_mycall 223

static int (*oldcall)(void);

//==========================================================================================

struct prinfo {
	pid_t parent_pid;		        /* process id of parent */
	pid_t pid;			            /* process id */
	pid_t first_child_pid;		  /* pid of youngest child */
	pid_t next_sibling_pid;		  /* pid of older sibling */

	long state;			            /* current state of process */
	long uid;			              /* user id of process owner */
	char comm[64];			        /* name of program executed */
};

// transform task_struct to prinfo struct
void taskToPrinfo(struct task_struct *task, struct prinfo *prin) {
	prin->state = task->state;	//state
	prin->uid =	task->cred->uid;	//uid
	get_task_comm(prin->comm, task);	//comm[64]

	prin->parent_pid = task->parent->pid;	//parent_pid
	prin->pid = task->pid;	//process id

	//first child pid
	if (list_empty(&task->children)) {
		prin->first_child_pid = 0;
	} else {
		prin->first_child_pid = list_entry((&task->children)->next, struct task_struct, sibling)->pid;
	}

	// next sibling pid
	if (list_empty(&task->sibling)) {
		prin->next_sibling_pid = 0;
	} else {
		struct list_head *list;
		struct task_struct *task_tmp;
		list_for_each(list, &(task->parent->children)) {	//traverse siblings of it
			task_tmp = list_entry(list, struct task_struct, sibling);
			if (task_tmp->pid == task->pid) {
				prin->next_sibling_pid = list_entry(list->next, struct task_struct, sibling)->pid;
				break;
			}
		}
	}
}

void dfs(struct task_struct *task, struct prinfo *buf, int *nr) {
	struct task_struct *task_tmp;
	struct list_head *list;

	// operation
	taskToPrinfo(task, &buf[*nr]);
	*nr = *nr + 1;

	list_for_each(list, &task->children) {
		task_tmp = list_entry(list, struct task_struct, sibling);
		dfs(task_tmp, buf, nr);
	}
}

int sys_mycall(struct prinfo *buf, int *nr) {
  //initialize dynamic space
	struct prinfo *buf_tmp;
	int *nr_tmp;
  buf_tmp = kmalloc_array(1000, sizeof(*buf), GFP_KERNEL);  //suppose processes less than 200
  nr_tmp = kmalloc(sizeof(int), GFP_KERNEL);

  if (buf_tmp == NULL || nr_tmp == NULL) {
    printk("Allocation initialize error!\n");
    return -EFAULT;
  }

	//initializition
	*nr_tmp = 0;

  //critical segment
  read_lock(&tasklist_lock);  //use tasklist_lock to avoid sleep
	dfs(&init_task, buf_tmp, nr_tmp);
  read_unlock(&tasklist_lock);

	// transform to user
	if (copy_to_user(buf, buf_tmp, 1000 * sizeof(*buf_tmp))) {
		printk("Copy_to_user error!\n");
		return -EFAULT;
	}
	if (copy_to_user(nr, nr_tmp, sizeof(int))) {
		printk("Copy_to_user error!\n");
		return -EFAULT;
	}

	kfree(buf_tmp);  kfree(nr_tmp);

  return *nr;
}
//==========================================================================================

static int addsyscall_init(void)
{
  long *syscall = (long*)0xc000d8c4;
  oldcall = (int(*)(void))(syscall[__NR_mycall]);
  syscall[__NR_mycall] = (unsigned long)sys_mycall;
  printk(KERN_INFO "module load!\n");
  return 0;
}

static void addsyscall_exit(void)
{
  long *syscall = (long*)0xc000d8c4;
  syscall[__NR_mycall] = (unsigned long)oldcall;
  printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);


//
