#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  FILE *f;
  if ((f = fopen("input.txt", "r")) == NULL) {
    perror("can't open input.txt");
    exit(1);
  }
  char *l = NULL;
  char *line = NULL;
  size_t n = 0;
  long long res = 0;
  // TODO
  // work with seed ranges
  long long *seeds = malloc(sizeof(long long) * 64);
  size_t seeds_size = 64;
  size_t seeds_len = 0;
  size_t seed_i = 0;
  bool in_map = false;
  long long **maps = malloc(sizeof(uint *) * 64);
  size_t maps_i = 0;
  while (getline(&l, &n, f) != -1) {
    line = l;
    if (strstr(l, "seeds:") != NULL) {
      while (*line++ != ':')
        ;
      bool is_range = false;
      long long base;
      long long range;
      char buf[32];
      size_t buf_len = 0;
      while (*line++ != '\0') {
        if (isdigit(*line)) {
          buf[buf_len++] = *line;
        } else if (buf_len > 0) {
          buf[buf_len] = '\0';
          buf_len = 0;
          if (!is_range) {
            base = atoll(buf);
          }
          if (is_range) {
            range = atoll(buf);
            for (long long i = 0; i < range; i++) {
              seeds[seed_i++] = base++;
              seeds_len++;
              if (seeds_len == seeds_size) {
                seeds_size *= 2;
                seeds = realloc(seeds, sizeof(long long) * seeds_size);
              }
            }
            base = 0;
          }
          is_range = !is_range;
        }
      }
      seed_i = 0;
      continue;
    }
    if (strlen(l) < 2) {
      in_map = false;
      for (int i = 0; i < seeds_len; i++) {
        for (int j = 0; j < maps_i; j++) {
          long long low = maps[j][1];
          long long high = maps[j][1] + maps[j][2];
          if (seeds[i] >= low && seeds[i] < high) {
            seeds[i] = seeds[i] - maps[j][1] + maps[j][0];
            break;
          }
        }
      }
      maps_i = 0;
      seed_i = 0;
    }
    if (in_map) {
      char buf[32];
      size_t buf_len = 0;
      long long *map = malloc(sizeof(uint) * 3);
      size_t map_i = 0;
      char c;
      while ((c = *line++) != '\0') {
        if (isdigit(c)) {
          buf[buf_len++] = c;
        } else if (buf_len > 0) {
          buf[buf_len] = '\0';
          buf_len = 0;
          map[map_i++] = atoll(buf);
        }
      }
      maps[maps_i++] = map;
    }
    if (strstr(l, "map") != NULL) {
      in_map = true;
    }
  }
  for (int i = 0; i < seeds_len; i++) {
    if (res == 0) {
      res = seeds[i];
    } else if (res > seeds[i]) {
      res = seeds[i];
    }
  }
  printf("\n");
  printf("%lld", res);
  printf("\n");
  fclose(f);
  if (l) {
    free(l);
  }
}
