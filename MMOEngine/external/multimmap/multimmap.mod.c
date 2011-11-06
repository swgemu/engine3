#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8da11c0f, "struct_module" },
	{ 0xedd14538, "param_get_uint" },
	{ 0x126970ed, "param_set_uint" },
	{ 0xdf4cc0ef, "device_create_drvdata" },
	{ 0x2740b361, "class_create" },
	{ 0x42400774, "cdev_add" },
	{ 0x803cddff, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x932da67e, "kill_proc" },
	{ 0x1eca1260, "find_vma" },
	{ 0x3302b500, "copy_from_user" },
	{ 0x93afa72e, "vm_get_page_prot" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xc16fe12d, "__memcpy" },
	{ 0x3477a375, "down_killable" },
	{ 0x381da1, "up" },
	{ 0xdb8bd46f, "down" },
	{ 0xea410fb4, "put_page" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x9eddef07, "class_destroy" },
	{ 0x9aaffbfa, "class_unregister" },
	{ 0xad0a27ad, "device_destroy" },
	{ 0xc1a97e30, "cdev_del" },
	{ 0x999e8297, "vfree" },
	{ 0xf627746, "alloc_pages_current" },
	{ 0x509579f5, "up_write" },
	{ 0xece9d314, "do_munmap" },
	{ 0x8b2b22d2, "do_mmap_pgoff" },
	{ 0x6f8fa3cd, "down_write" },
	{ 0xdd132261, "printk" },
	{ 0xac75d3f4, "get_unmapped_area" },
	{ 0xe85fe7dd, "pv_mmu_ops" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

