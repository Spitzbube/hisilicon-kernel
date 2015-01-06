
struct file *kfile_open(const char *filename, int flags, int mode);

void kfile_close(struct file *filp);

int kfile_read(char *buf, unsigned int len, struct file *filp);

int kfile_write(char *buf, int len, struct file *filp);

int kfile_seek(loff_t offset, int origin, struct file *filp);

void dump(unsigned char data[], int len);

