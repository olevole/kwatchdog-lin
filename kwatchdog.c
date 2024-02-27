#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include <linux/reboot.h>

MODULE_LICENSE("BSD");

#define PROCFS_MAX_SIZE         4

static struct timer_list my_timer;
static int kwatcher_count_critical = 60;
int kwatcher_count = 0;

// The size of the buffer
static unsigned int procfs_buffer_size = 0;

// The buffer used to store character for this module
static char procfs_buffer[PROCFS_MAX_SIZE];

static struct proc_dir_entry *kwatchdog;
//static struct proc_dir_entry *Our_Proc_File;

void my_timer_callback(struct timer_list *timer) {
	pr_crit("Software Watchdog Timer expired: initiating reset\n");
	kernel_restart("kwatchdog");
}

static ssize_t kwatchdog_read(struct file *fp, char *buf, size_t len, loff_t * off)
{
	printk(KERN_ALERT "Software Watchdog Timer: reset from /proc\n");
	del_timer(&my_timer);
	timer_setup(&my_timer, my_timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(kwatcher_count_critical * 1000));

	return 0;
}

static ssize_t kwatchdog_write(struct file *file, const char *buf, size_t len, loff_t *data)
{
	int ret=0;
	unsigned long long res;
	/* get buffer size */
	procfs_buffer_size = len;
	if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
		procfs_buffer_size = PROCFS_MAX_SIZE;
	}

	/* write data to the buffer */
	if ( copy_from_user(procfs_buffer, buf, procfs_buffer_size) ) {
		return -EFAULT;
	}

	ret = kstrtoull_from_user(buf, len, 10, &res);
	if (ret) {
		/* Negative error code. */
		pr_info("ko = %d\n", ret);
		return ret;
	}
	*data= len;
	pr_info("Change Software Timer to: %llu\n", res);
	kwatcher_count_critical = res;

	del_timer(&my_timer);
	timer_setup(&my_timer, my_timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(kwatcher_count_critical * 1000));

	return len;
}

static struct proc_ops kwatchdog_fops = { .proc_read=kwatchdog_read, .proc_write=kwatchdog_write };
//static struct proc_ops kwatchdog_fops = { .proc_read=kwatchdog_read, };

static int init_module_with_timer(void) {
	kwatchdog = proc_create( "kwatchdog", 0444, NULL, &kwatchdog_fops);

	if(kwatchdog==NULL) {      printk(KERN_ALERT "Error: Could not initialize %s\n", "kwatchdog"); }

	printk(KERN_ALERT "Initializing a module with timer: %d secs\n", kwatcher_count_critical);

	timer_setup(&my_timer, my_timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(kwatcher_count_critical * 1000));

	return 0;
}

static void exit_module_with_timer(void) {
	printk(KERN_ALERT "Goodbye, cruel world!\n");
	del_timer(&my_timer);
	remove_proc_entry("kwatchdog",NULL);
}

module_init(init_module_with_timer);
module_exit(exit_module_with_timer);
