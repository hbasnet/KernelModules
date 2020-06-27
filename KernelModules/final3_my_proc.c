#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAX_LEN 4096
static struct proc_dir_entry *proc_entry;
static char *info;
static unsigned long procfs_buffer_size = 0;

ssize_t read_proc(struct file *f, char *user_buf, size_t count, loff_t *off)
{
	//copy data from kernel space to user space
	static int finished = 0;
	//we return 0 for eof otherwise it will go in endless loop
	
	if (finished){
		printk(KERN_INFO "procfs_read: END\n");
		finished = 0;
		return 0;
	}

	finished = 1;
		
	//if(count > MAX_LEN)
	//	count = MAX_LEN; //trim data
	if(copy_to_user(user_buf, info, procfs_buffer_size))
		return -EFAULT;
	printk(KERN_INFO "procfs_read: read %lu bytes\n", procfs_buffer_size);
	return procfs_buffer_size;
}

ssize_t write_proc(struct file *f, const char *user_buf, size_t count, loff_t *off)
{
	//copy data from user space and save it in the my proc entry
	
	if(count > MAX_LEN)
	{
		procfs_buffer_size = MAX_LEN;
	}else{
		procfs_buffer_size = count;
	}
	if(copy_from_user(info, user_buf, procfs_buffer_size))
		return -EFAULT;

	printk(KERN_INFO "procfs_write: write %lu bytes\n", procfs_buffer_size);
	return procfs_buffer_size;
}

struct file_operations proc_fops = {
	read: read_proc,
	write: write_proc
};

int init_module(void)
{
	int ret = 0;
	//allocated memory space for the proc entry
	info = (char *)vmalloc(MAX_LEN);
	memset(info, 0, MAX_LEN);

	//create entry
	proc_entry = proc_create("final3proc", 0, NULL, &proc_fops);
	
	printk(KERN_INFO "final3proc created.\n");

	return ret;
}

void cleanup_module(void)
{
	remove_proc_entry("final3proc",NULL);
	
	printk(KERN_INFO "final3proc deleted.\n");
	kfree(info);
}
