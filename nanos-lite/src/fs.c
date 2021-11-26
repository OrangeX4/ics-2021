#include <fs.h>

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t get_fb_size();
size_t fb_write(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENT, FD_DISPINFO};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here: Invalid Read");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here: Invalid Write");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // initialize the size of /dev/fb
  file_table[FD_FB].size = get_fb_size();
}

char *fs_getname(int fd) {
  assert(fd < LENGTH(file_table));
  return file_table[fd].name;
}

int fs_open(const char *pathname, int flags, int mode) {
  int fd = 0;
  for (; fd < LENGTH(file_table) && strcmp(pathname, file_table[fd].name); ++fd);
  assert(fd != LENGTH(file_table));
  file_table[fd].open_offset = 0;
  return fd;
}


size_t fs_read(int fd, void *buf, size_t len) {
  assert(buf != 0);
  assert(fd < LENGTH(file_table));
  Finfo *finfo = &file_table[fd];
  if (finfo->read) {
    // Virtual File
    finfo->open_offset += len;
    return finfo->read(buf, finfo->open_offset - len, len);
    // finfo->open_offset += len;
  } else {
    // Normal File
    // assert(finfo->open_offset + len <= finfo->size);
    if (finfo->open_offset + len <= finfo->size) {
      ramdisk_read(buf, finfo->disk_offset + finfo->open_offset, len);
      finfo->open_offset += len;
      return len;
    } else {
      len = ramdisk_read(buf, finfo->disk_offset + finfo->open_offset, finfo->size - finfo->open_offset);
      finfo->open_offset += len;
      return len;
    }
  }
}


size_t fs_write(int fd, const void *buf, size_t len) {
  assert(buf != 0);
  assert(fd < LENGTH(file_table));
  Finfo *finfo = &file_table[fd];
  if (finfo->write) {
    // Virtual File
    finfo->open_offset += len;
    return finfo->write(buf, finfo->open_offset - len, len);
    // finfo->open_offset += len;
  } else {
    // Normal File
    assert(finfo->open_offset + len <= finfo->size);
    ramdisk_write(buf, finfo->disk_offset + finfo->open_offset, len);
    finfo->open_offset += len;
    return len;
  }
}


size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd == FD_FB) {
    assert(0);
  }
  assert(fd < LENGTH(file_table));
  switch (whence) {
  case SEEK_SET:
    assert(offset <= file_table[fd].size);
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    assert(file_table[fd].open_offset + offset <= file_table[fd].size);
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    assert(file_table[fd].size + offset <= file_table[fd].size);
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  
  default:
    panic("Invalid whence");
    break;
  }
  return file_table[fd].open_offset;
}


int fs_close(int fd) {
  // Always success
  return 0;
}
