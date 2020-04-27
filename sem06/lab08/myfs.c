#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("my filesystem");

#define MYFS_MAGIC_NUMBER 0xDEADBEAF

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
	struct inode *ret = new_inode(sb);
	if (ret) {
		inode_init_owner(ret, NULL, mode);
		ret->i_size = PAGE_SIZE;
		ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
	}

	return ret;
}

static void myfs_put_super(struct super_block *sb)
{
	printk(KERN_DEBUG "myfs: super block destroyed\n");
}

static const struct super_operations myfs_super_ops = {
	.put_super  = myfs_put_super,
	.statfs     = simple_statfs,
	.drop_inode = generic_delete_inode
};

static int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	struct inode *root = NULL;

	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic = MYFS_MAGIC_NUMBER;
	sb->s_op = &myfs_super_ops;

	if (!(root = myfs_make_inode(sb, S_IFDIR | 0755))) {
		printk(KERN_ERR "myfs: inode allocation failed\n");
		return -ENOMEM;
	}
	root->i_op = &simple_dir_inode_operations;
	root->i_fop = &simple_dir_operations;

	if (!(sb->s_root = d_make_root(root))) {
		iput(root);
		printk(KERN_ERR "myfs: root creation failed\n");
		return -ENOMEM;
	}

	return 0;
}

static struct dentry *myfs_mount(struct file_system_type *type, int flags, const char *dev, void *data)
{
	struct dentry *const entry = mount_bdev(type, flags, dev, data, myfs_fill_sb);
	if (IS_ERR(entry)) {
		printk(KERN_ERR "myfs: mounting failed\n");
	} else {
		printk(KERN_DEBUG "myfs: mounted\n");
	}
	return entry;
}

static struct file_system_type myfs_type = {
	.owner   = THIS_MODULE,
	.name    = "myfs",
	.mount   = myfs_mount,
	.kill_sb = kill_block_super,
};

static int __init myfs_init(void)
{
	const int ret = register_filesystem(&myfs_type);
	if (ret) {
		printk(KERN_ERR "myfs: cannot register filesystem\n");
		return ret;
	}
	printk(KERN_DEBUG "myfs: module loaded\n");
	return 0;
}

static void __exit myfs_exit(void)
{
	const int ret = unregister_filesystem(&myfs_type);
	if (ret) {
		printk(KERN_ERR "myfs: cannot unregister filesystem\n");
	}
	printk(KERN_DEBUG "myfs: module unloaded\n");
}

module_init(myfs_init)
module_exit(myfs_exit)
