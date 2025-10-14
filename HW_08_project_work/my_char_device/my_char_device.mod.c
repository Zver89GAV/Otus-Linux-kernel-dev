#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x92997ed8, "_printk" },
	{ 0x9f3c769a, "single_open" },
	{ 0x705efdcd, "seq_printf" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xbb9ec552, "__asan_report_load8_noabort" },
	{ 0xe93de306, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x595fe262, "cdev_init" },
	{ 0x793b5b39, "cdev_add" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xf08072dd, "device_create" },
	{ 0xe558e862, "cdev_del" },
	{ 0xbaa48e34, "class_destroy" },
	{ 0x4efe1f1d, "proc_create" },
	{ 0x10b4502e, "sysfs_create_group" },
	{ 0x24383f, "kmalloc_caches" },
	{ 0x5947560a, "kmalloc_trace" },
	{ 0x77c49907, "sysfs_remove_group" },
	{ 0x95950f06, "proc_remove" },
	{ 0xf646093a, "device_destroy" },
	{ 0x37a0cba, "kfree" },
	{ 0xa19b956, "__stack_chk_fail" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x62274ff1, "seq_read" },
	{ 0x80d7fc8, "seq_lseek" },
	{ 0x5a909eb3, "seq_release" },
	{ 0xe894f8f4, "__asan_unregister_globals" },
	{ 0xdc3c46c1, "__asan_register_globals" },
	{ 0xdb26e399, "module_layout" },
};

MODULE_INFO(depends, "");

