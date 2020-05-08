#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("my filesystem");

#define MYFS_MAGIC_NUMBER 0xDEADBEAF
#define SLABNAME "myfs_inode_cache"

struct myfs_inode {
	umode_t       i_mode;
	unsigned long i_ino;
};

static struct kmem_cache *myfs_inode_cachep = NULL;
static struct myfs_inode **myfs_inodes = NULL;
static int cache_inode_count = 128;
module_param(cache_inode_count, int, 0);
static int busy = 0;

static struct myfs_inode *cache_get_inode(void)
{
	if (busy == cache_inode_count) {
		return NULL;
	}

	return myfs_inodes[busy++] = kmem_cache_alloc(myfs_inode_cachep, GFP_KERNEL);
}

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
	struct myfs_inode *myfs_inodep = NULL;
	struct inode *ret = new_inode(sb);
	if (ret) {
		inode_init_owner(ret, NULL, mode);
		ret->i_size = PAGE_SIZE;
		ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
		if ((myfs_inodep = cache_get_inode())) {
			myfs_inodep->i_mode = ret->i_mode;
			myfs_inodep->i_ino = ret->i_ino;
		}
		ret->i_private = myfs_inodep;
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
	.drop_inode = generic_delete_inode,
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

static struct dentry *myfs_mount(struct file_system_type *type, int flags, __attribute__((unused)) const char *dev, void *data)
{
	struct dentry *entry = mount_nodev(type, flags, data, myfs_fill_sb);
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
	.kill_sb = kill_anon_super,
};

static int __init myfs_init(void)
{
	int ret = register_filesystem(&myfs_type);
	if (ret) {
		printk(KERN_ERR "myfs: cannot register filesystem\n");
		return ret;
	}

	if (!(myfs_inodes = kmalloc(sizeof(struct myfs_inode *) * cache_inode_count, GFP_KERNEL))) {
		printk(KERN_ERR "myfs: kmalloc error\n");
		return -ENOMEM;
	}

	if (!(myfs_inode_cachep = kmem_cache_create(SLABNAME, sizeof(struct myfs_inode), 0, SLAB_POISON, NULL))) {
		kfree(myfs_inodes);
		printk(KERN_ERR "myfs: kmem_cache_create error\n");
		return -ENOMEM;
	}

	printk(KERN_DEBUG "myfs: module loaded\n");
	return 0;
}

static void __exit myfs_exit(void)
{
	int i;
	for (i = 0; i < busy; ++i) {
		kmem_cache_free(myfs_inode_cachep, myfs_inodes[i]);
	}
	kmem_cache_destroy(myfs_inode_cachep);
	kfree(myfs_inodes);

	if (unregister_filesystem(&myfs_type)) {
		printk(KERN_ERR "myfs: cannot unregister filesystem\n");
	}
	printk(KERN_DEBUG "myfs: module unloaded\n");
}

module_init(myfs_init)
module_exit(myfs_exit)
