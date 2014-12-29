#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>        /* for put_user */

#define AUTOR           "Victor Rodriguez"
#define DESCRIPCION     "Driver Hola mundo"
#define DEVICE_NAME     "holamundodriver"
#define MAXIMO          1024

static int num_mayor;
static unsigned long procfs_buffer_size = 0;
static char buffer_data[MAXIMO];


static char msg[MAXIMO];        /* The msg the device will give when asked */
static char *msg_Ptr;



static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);



static struct file_operations fops= {
        .open = device_open,
        .release = device_release,
        .read = device_read,
        .write = device_write,
};



static int __init hola_inicio(void)
{

        printk(KERN_INFO "HOLA DESDE LA HAWKBOARD \n");

        /*Pide un numero mayor al kernel */

        num_mayor=register_chrdev(0, DEVICE_NAME, &fops);
        printk(KERN_INFO "EL numero mayor es %d \n",num_mayor);
        printk(KERN_INFO "EL nombre es %s \n",DEVICE_NAME);
        printk(KERN_INFO "Genera el device fiel con mknod /dev/%s c %d 0
\n",DEVICE_NAME,num_mayor);

        return 0 ;

}

static void __exit hola_fin(void)
{

        unregister_chrdev(num_mayor, DEVICE_NAME);
        printk(KERN_INFO "Adios mundo, modulo desintalado \n");
}

module_init(hola_inicio);
module_exit(hola_fin);


/* Methods */

static int device_open(struct inode *inode, struct file *file)
{
        sprintf(msg,buffer_data);
        msg_Ptr = msg;
        return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
        return 0;
}

static ssize_t device_read(struct file *filp,   /* see include/linux/fs.h   */
                           char *buf,   /* buffer to fill with data */
                           size_t len,  /* length of the buffer     */
                           loff_t * off)
{
        int bytes_read = 0;

        if (*msg_Ptr == 0)
                return 0;


        while (len && *msg_Ptr) {

                /*
                 * The buffer is in the user data segment, not the kernel
                 * segment so "*" assignment won't work.  We have to use
                 * put_user which copies data from the kernel data segment to
                 * the user data segment.
                 */
                put_user(*(msg_Ptr++), buf++);

                len--;
                bytes_read++;
        }

         /*
         * Most read functions return the number of bytes put into the buffer
         */
        return bytes_read;


}

static ssize_t device_write(struct file *filp,
                            const char *buf,
                            size_t len,
                            loff_t * off)
{

procfs_buffer_size = len;

        if ( copy_from_user(buffer_data, buf, procfs_buffer_size) ) {
                return -EFAULT;
        }
         *off += len;
        return procfs_buffer_size;
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTOR);   /* Who wrote this module? */
MODULE_DESCRIPTION(DESCRIPCION);        /* What does this module do */
MODULE_SUPPORTED_DEVICE("holamundohawkboard");