#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander V. Lvov.");
MODULE_DESCRIPTION("A simple Linux module for getting information about connected PCI devices.");
MODULE_VERSION("0.01");

int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}

static int __init init_my_module(void) {
    printk(KERN_INFO
    "Module is loaded.\n");

    const char *filename = "/home/allvo/pci_devices.txt";
    struct file *output = filp_open(filename, O_CREAT | O_WRONLY, S_IROTH);
    if (output == NULL) {
        printk(KERN_INFO
        "Failed to open the output file.\n");
        return 1;
    }

    struct pci_dev *device = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
    if (device == NULL)
        printk(KERN_INFO
    "Not found any PCI devices.\n");


    loff_t position = 0;
    for (; device; device = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, device)) {
        unsigned devfn = device->devfn;
        unsigned short vendor = device->vendor;
        unsigned short dev = device->device;
        printk(KERN_INFO "Found device. File: %d, vendor: %u, device: %u\n", devfn, vendor, dev);

        char buffer[6] = {0};

        ssize_t result = kernel_write(output, "Device file descriptor: ", 24 * sizeof(char), &position);
        if (result != 24 * sizeof(char)) {
            printk(KERN_INFO
            "Writing to the file failed.\n");
            break;
        }

        int written = snprintf(&buffer[0], 6 * sizeof(char), "%u", devfn);
        result = kernel_write(output, buffer, sizeof(char) * written, &position);
        if (result != sizeof(char) * written) {
            printk(KERN_INFO
            "Writing to the file failed (2).\n");
            break;
        }

        result = kernel_write(output, ", vendor ID: ", 13 * sizeof(char), &position);
        if (result != 13 * sizeof(char)) {
            printk(KERN_INFO
            "Writing to the file failed (3).\n");
            break;
        }

        written = snprintf(&buffer[0], 6 * sizeof(char), "%hu", vendor);
        result = kernel_write(output, buffer, sizeof(char) * written, &position);
        if (result != sizeof(char) * written) {
            printk(KERN_INFO
            "Writing to the file failed (4).\n");
            break;
        }

        result = kernel_write(output, ", device number: ", 17 * sizeof(char), &position);
        if (result != 17 * sizeof(char)) {
            printk(KERN_INFO
            "Writing to the file failed (5).\n");
            break;
        }

        written = snprintf(&buffer[0], 6 * sizeof(char), "%hu", dev);
        result = kernel_write(output, buffer, sizeof(char) * written, &position);
        if (result != sizeof(char) * written) {
            printk(KERN_INFO
            "Writing to the file failed (6).\n");
            break;
        }

        result = kernel_write(output, ".\n", 2 * sizeof(char), &position);
        if (result != 2 * sizeof(char)) {
            printk(KERN_INFO
            "Writing to the file failed (7).\n");
            break;
        }
    };

    filp_close(output, NULL);

    return 0;
}

static void __exit remove_my_module(void) {
    printk(KERN_INFO
    "Goodbye, World!\n");
}

module_init(init_my_module);
module_exit(remove_my_module);