#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int x;
  int y;
  int z;
} Point;

// general assumption is that second point always has bigger or equal coordinate
// values
typedef struct {
  Point p1;
  Point p2;
} Brick;

void print_brick(Brick b);

bool is_support_for(Brick support, Brick supported) {
  if (supported.p1.z == 1 || supported.p2.z == 1) {
    return false;
  }

  if (support.p1.z != supported.p1.z - 1 && support.p2.z != supported.p1.z - 1) {
    return false;
  }

  if (supported.p1.z != supported.p2.z) {
    if (supported.p1.x >= support.p1.x && supported.p1.x <= support.p2.x &&
        supported.p1.y >= support.p1.y && supported.p1.y <= support.p2.y) {

      return support.p2.z == supported.p1.z - 1;
    }
    return false;
  }

  if (supported.p1.x != supported.p2.x) {
    if (((supported.p1.x >= support.p1.x && supported.p1.x <= support.p2.x) ||
         (supported.p2.x >= support.p1.x && supported.p2.x <= support.p2.x) ||
         (support.p1.x >= supported.p1.x && support.p1.x <= supported.p2.x) ||
         (support.p2.x >= supported.p1.x && support.p2.x <= supported.p2.x)

             ) &&
        supported.p1.y >= support.p1.y && supported.p1.y <= support.p2.y) {

      return support.p2.z == supported.p1.z - 1;
    }
    return false;
  }

  if (supported.p1.y != supported.p2.y) {
    if (((supported.p1.y >= support.p1.y && supported.p1.y <= support.p2.y) ||
         (supported.p2.y >= support.p1.y && supported.p2.y <= support.p2.y) ||
         (support.p1.y >= supported.p1.y && support.p1.y <= supported.p2.y) ||
         (support.p2.y >= supported.p1.y && support.p2.y <= supported.p2.y)) &&
        supported.p1.x >= support.p1.x && supported.p1.x <= support.p2.x) {

      return support.p2.z == supported.p1.z - 1;
    }
    return false;
  }

  if (supported.p1.x >= support.p1.x && supported.p2.x <= support.p2.x &&
      supported.p1.y >= support.p1.y && supported.p2.y <= support.p2.y) {

    return support.p2.z == supported.p1.z - 1;
  }

  return false;
}

int compare_z_desc(const void *a, const void *b) {
  int z1 = ((Brick *)a)->p1.z;
  int z2 = ((Brick *)b)->p1.z;
  return z1 - z2;
}

Point parse_point(char **line) {
  Point p;
  char c;
  size_t buf_i = 0;
  char buf[10];

  while ((c = *(*line)++) != ',' && c != '\n' && c != '\0' && c != '~') {
    buf[buf_i++] = c;
  }
  buf[buf_i] = 0;
  p.x = atoi(buf);
  buf_i = 0;

  while ((c = *(*line)++) != ',' && c != '\n' && c != '\0' && c != '~') {
    buf[buf_i++] = c;
  }
  buf[buf_i] = 0;
  p.y = atoi(buf);
  buf_i = 0;

  while ((c = *(*line)++) != ',' && c != '\n' && c != '\0' && c != '~') {
    buf[buf_i++] = c;
  }
  buf[buf_i] = 0;
  p.z = atoi(buf);
  buf_i = 0;

  return p;
}

void print_point(Point p) {
  printf("point: x: %d, y: %d, z: %d\n", p.x, p.y, p.z);
}

void print_brick(Brick b) {
  printf("brick\n");
  print_point(b.p1);
  print_point(b.p2);
}

int main() {
  FILE *f;
  if ((f = fopen("input.txt", "r")) == NULL) {
    perror("can't open input.txt");
    exit(1);
  }

  unsigned long long res = 0;
  char *l = NULL;
  char *line = NULL;
  size_t n = 0;

  size_t bs_len = 0;
  size_t bs_size = 16;
  Brick *bs = malloc(sizeof(Brick) * bs_size);

  while (getline(&l, &n, f) != -1) {
    line = l;
    Point p1 = parse_point(&line);
    Point p2 = parse_point(&line);
    bs[bs_len++] = (Brick){p1, p2};
    if (bs_len == bs_size) {
      bs_size *= 2;
      bs = realloc(bs, sizeof(Brick) * bs_size);
    }
  }

  // sort from lowest to highest brick
  qsort(bs, bs_len, sizeof(Brick), compare_z_desc);

  // move bricks downward until supported
  for (int i = 0; i < bs_len; i++) {
    bool landed = bs[i].p1.z == 1;
    while (!landed) {
      for (int j = 0; j < bs_len; j++) {
        if (is_support_for(bs[j], bs[i])) {
          landed = true;
          break;
        }
      }
      if (landed) {
        break;
      }
      bs[i].p1.z--;
      bs[i].p2.z--;
      if (bs[i].p1.z == 1) {
        landed = true;
      }
    }
  }

  size_t cant_remove_len = 0;
  Brick **cant_remove = malloc(sizeof(Brick *) * bs_size);
  for (int i = 0; i < bs_len; i++) {
    print_brick(bs[i]);
    int no_supports = 0;
    Brick *last_support = NULL;
    for (int j = 0; j < bs_len; j++) {
      if (i == j) {
        continue;
      }
      if (is_support_for(bs[j], bs[i])) {
        printf("---------------------------------------\n");
        print_brick(bs[j]);
        printf("is support for:\n");
        print_brick(bs[i]);
        printf("---------------------------------------\n");
        last_support = &bs[j];
        no_supports++;
      }
    }
    if (no_supports == 1) {
      bool to_add = true;
      for (int j = 0; j < cant_remove_len; j++) {
        if (cant_remove[j] == last_support) {
          to_add = false;
          break;
        }
      }
      if (to_add) {
        cant_remove[cant_remove_len++] = last_support;
      }
    }
  }

  printf("cant_remove_len: %zu\n", cant_remove_len);

  res = bs_len - cant_remove_len;

  printf("\n");
  printf("\n");
  printf("%lld\n", res);
  printf("\n");
  fclose(f);
  if (l) {
    free(l);
  }
}
