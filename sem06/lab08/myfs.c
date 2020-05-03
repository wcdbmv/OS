#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("my filesystem");

#define MYFS_MAGIC_NUMBER 0xDEADBEAF
#define MYFS_CACHE_INODE_COUNT 128
#define SLABNAME "myfs_inode_cache"

struct myfs_inode {
	umode_t       i_mode;
	unsigned long i_ino;
};

static struct kmem_cache *myfs_inode_cachep = NULL;
static struct myfs_inode **line = NULL;
static int busy = 0;
static int sco = 0;

struct myfs_inode *cache_get_inode(void)
{
	if (busy == MYFS_CACHE_INODE_COUNT) {
		return NULL;
	}

	return line[busy++];
}

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
	struct myfs_inode *mi = NULL;
	struct inode *ret = new_inode(sb);
	if (ret) {
		inode_init_owner(ret, NULL, mode);
		ret->i_size = PAGE_SIZE;
		ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
		if ((mi = cache_get_inode()) != NULL) {
			mi->i_mode = ret->i_mode;
			mi->i_ino = ret->i_ino;
		}
		ret->i_private = mi;
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
	struct dentry *entry = mount_bdev(type, flags, dev, data, myfs_fill_sb);
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


void co(void *p)
{
	*(int *) p = (int) p;
	++sco;
}

static int __init myfs_init(void)
{
	int i, ret;
	if (!(line = kmalloc(sizeof (struct myfs_inode *) * MYFS_CACHE_INODE_COUNT, GFP_KERNEL))) {
		printk(KERN_ERR "myfs: kmalloc error\n");
		return -ENOMEM;
	}
	if (!(myfs_inode_cachep = kmem_cache_create(SLABNAME, sizeof (struct myfs_inode), 0, SLAB_HWCACHE_ALIGN, co))) {
		kfree(line);
		printk(KERN_ERR "myfs: kmem_cache_create error\n");
		return -ENOMEM;
	}
	for (i = 0; i < MYFS_CACHE_INODE_COUNT; ++i) {
		line[i] = NULL;
		if (!(line[i] = kmem_cache_alloc(myfs_inode_cachep, GFP_KERNEL))) {
			while (--i >= 0) {
				kmem_cache_free(myfs_inode_cachep, line[i]);
			}
			kmem_cache_destroy(myfs_inode_cachep);
			kfree(line);
			printk(KERN_ERR "myfs: kmem_cache_alloc error\n");
			return -ENOMEM;
		}
	}
	printk(KERN_INFO "myfs: allocate %d objects into slab: %s\n", MYFS_CACHE_INODE_COUNT, SLABNAME);
	printk(KERN_INFO "myfs: object size %lu bytes, full size %lu bytes\n", sizeof (struct myfs_inode), sizeof (struct myfs_inode) * MYFS_CACHE_INODE_COUNT);
	printk(KERN_INFO "myfs: constructor called %d times\n", sco);

	if ((ret = register_filesystem(&myfs_type))) {
		printk(KERN_ERR "myfs: cannot register filesystem\n");
		return ret;
	}
	printk(KERN_DEBUG "myfs: module loaded\n");
	return 0;
}

static void __exit myfs_exit(void)
{
	int i, ret;
	for (i = 0; i < MYFS_CACHE_INODE_COUNT; ++i) {
		kmem_cache_free(myfs_inode_cachep, line[i]);
	}
	kmem_cache_destroy(myfs_inode_cachep);
	kfree(line);

	if ((ret = unregister_filesystem(&myfs_type))) {
		printk(KERN_ERR "myfs: cannot unregister filesystem\n");
	}
	printk(KERN_DEBUG "myfs: module unloaded\n");
}

module_init(myfs_init)
module_exit(myfs_exit)
