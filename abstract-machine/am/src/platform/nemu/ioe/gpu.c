#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
//   uint32_t hw = inl(VGACTL_ADDR);
//   int i;
//   int w = (hw >> 16);
//   int h = ((hw << 16) >> 16);
//   uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
//   for (i = 0; i < w * h; i ++) fb[i] = i;
//   outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t hw = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = (hw >> 16), .height = ((hw << 16) >> 16),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t hw = inl(VGACTL_ADDR);
  int i, j;
  int w = (hw >> 16);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < ctl->h; ++i) {
    for (j = 0; j < ctl->w; ++j) {
      fb[(ctl->y + i) * w + (ctl->x + j)] = ((uint32_t *)ctl->pixels)[i * w + j];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
