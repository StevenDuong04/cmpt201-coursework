#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 128

struct header {
  uint64_t size;
  struct header *next;
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data
                                                       : *(void **)data);

  write(STDOUT_FILENO, buf, len);
}

int main(void) {

  void *start_address = sbrk(256);
  if (start_address == (void *)-1) {
    perror("sbrk failed");
    return 1;
  }

  struct header *first_block = (struct header *)start_address;
  struct header *second_block = (struct header *)(start_address + 128);

  first_block->size = 128;
  first_block->next = NULL;

  second_block->size = 128;
  second_block->next = first_block;

  print_out("first block: %p\n", &first_block, sizeof(first_block));
  print_out("second block: %p\n", &second_block, sizeof(second_block));

  print_out("first block size: %lu\n", &first_block->size, sizeof(uint64_t));
  print_out("first block next: %p\n", &first_block->next, sizeof(void *));
  print_out("second block size: %lu\n", &second_block->size, sizeof(uint64_t));
  print_out("second block next: %p\n", &second_block->next, sizeof(void *));

  void *first_data = (char *)first_block + sizeof(struct header);
  void *second_data = (char *)second_block + sizeof(struct header);

  size_t size = 128 - sizeof(struct header);

  memset(first_data, 0, size);
  memset(second_data, 1, size);

  print_out("first block size after: %lu\n", &first_block->size,
            sizeof(uint64_t));
  print_out("first block next after: %p\n", &first_block->next, sizeof(void *));
  print_out("second block size after: %lu\n", &second_block->size,
            sizeof(uint64_t));
  print_out("second block next after: %p\n", &second_block->next,
            sizeof(void *));

  for (size_t i = 0; i < size; i++) {
    uint64_t n = ((unsigned char *)first_data)[i];
    print_out("%lu\n", &n, sizeof(uint64_t));
  }

  for (size_t i = 0; i < size; i++) {
    uint64_t n = ((unsigned char *)second_data)[i];
    print_out("%lu\n", &n, sizeof(uint64_t));
  }

  return 0;
}
