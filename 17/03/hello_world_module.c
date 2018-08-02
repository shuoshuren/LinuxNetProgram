#include <linux/init.h>
#include <linux/module.h>

/*版权申明*/
MODULE_LICENSE("Dual BSD/GPL");

/*初始化模块*/
static int __init helloworld_init(void){
	printk(KERN_ALERT "Hello world module init\n");
	return 0;
}

/*清理模块*/
static void __exit helloworld_exit(void){
	printk(KERN_ALERT "Hello world module exit\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

/*作者，软件描述，版本等声明信息*/
MODULE_AUTHOR("xc");
MODULE_DESCRIPTION("Hello world demo");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("Chapter 17,example 1");
