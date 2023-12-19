#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint len;
  uint dir;
  uint color;
} Inst;

typedef struct {
  long y;
  long x;
} Point;

typedef struct {
  long *xs;
  size_t len;
  size_t size;
} Row;

bool find(Row r, long x) {
  for (int i = 0; i < r.len; i++) {
    if (r.xs[i] == x) {
      return true;
    }
  }
  return false;
}

int cmpfunc(const void *a, const void *b) { return (*(long *)a - *(long *)b); }

int main() {
  FILE *f;
  if ((f = fopen("input.txt", "r")) == NULL) {
    perror("can't open input.txt");
    exit(1);
  }

  long long res = 0;
  char *l = NULL;
  size_t n = 0;
  char *line = NULL;

  size_t insts_len = 0;
  size_t insts_size = 8;
  Inst *insts = malloc(sizeof(Inst) * insts_size);

  while (getline(&l, &n, f) != -1) {
    line = l;

    Inst inst;
    char c;
    char buf[7];
    size_t buf_i = 0;

    c = *line++;
    inst.dir = c == 'R' ? 0 : c == 'D' ? 1 : c == 'L' ? 2 : 3;

    line++;
    while (isdigit((c = *line++))) {
      buf[buf_i++] = c;
    }
    buf[buf_i] = 0;
    inst.len = atoi(buf);

    line += 2;
    buf_i = 0;
    while ((c = *line++) != ')') {
      buf[buf_i++] = c;
    }
    inst.dir = buf[buf_i - 1] == '0'   ? 0
               : buf[buf_i - 1] == '1' ? 1
               : buf[buf_i - 1] == '2' ? 2
                                       : 3;
    buf[buf_i - 1] = 0;
    inst.len = strtol(buf, NULL, 16);

    insts[insts_len++] = inst;
    if (insts_len == insts_size) {
      insts_size *= 2;
      insts = realloc(insts, sizeof(Inst) * insts_size);
    }
  }

  size_t p_len = 0;
  size_t p_size = 16;
  Point *p = malloc(sizeof(Point) * p_size);
  Point start = (Point){.x = 0, .y = 0};
  p[p_len++] = start;
  long min_x = 0;
  long min_y = 0;
  long max_x = 0;
  long max_y = 0;

  for (int i = 0; i < insts_len; i++) {
    for (int j = 0; j < insts[i].len; j++) {
      if (insts[i].dir == 0) {
        p[p_len] = (Point){.x = p[p_len - 1].x + 1, .y = p[p_len - 1].y};
      } else if (insts[i].dir == 1) {
        p[p_len] = (Point){.x = p[p_len - 1].x, .y = p[p_len - 1].y + 1};
      } else if (insts[i].dir == 2) {
        p[p_len] = (Point){.x = p[p_len - 1].x - 1, .y = p[p_len - 1].y};
      } else if (insts[i].dir == 3) {
        p[p_len] = (Point){.x = p[p_len - 1].x, .y = p[p_len - 1].y - 1};
      }
      p_len++;
      if (p_len == p_size) {
        p_size *= 2;
        p = realloc(p, sizeof(Point) * p_size);
      }
    }
    min_x = p[p_len - 1].x < min_x ? p[p_len - 1].x : min_x;
    min_y = p[p_len - 1].y < min_y ? p[p_len - 1].y : min_y;
    max_x = p[p_len - 1].x > max_x ? p[p_len - 1].x : max_x;
    max_y = p[p_len - 1].y > max_y ? p[p_len - 1].y : max_y;
  }

  long x_size = max_x - min_x + 1;
  long y_size = max_y - min_y + 1;
  Row *rows = malloc(sizeof(Row) * y_size);

  for (int i = 0; i < y_size; i++) {
    rows[i].len = 0;
    rows[i].size = 8;
    rows[i].xs = malloc(sizeof(long) * rows[i].size);
  }

  for (int i = 0; i < p_len; i++) {
    size_t row_i = p[i].y - min_y;
    rows[row_i].xs[rows[row_i].len++] = p[i].x - min_x;
    if (rows[row_i].len == rows[row_i].size) {
      rows[row_i].size *= 2;
      rows[row_i].xs = malloc(sizeof(long) * rows[row_i].size);
    }
  }

  for (int i = 0; i < y_size; i++) {
    qsort(rows[i].xs, rows[i].len, sizeof(long), cmpfunc);
  }

  for (int i = 0; i < rows[1].len; i++) {
      printf("%ld, ", rows[1].xs[i]);
  }
  

  for (int y = 1; y < y_size - 1; y++) {
    long counting = false;
    bool from_top;
    for (int i = rows[y].len; i >= 0; i--) {
      long x = rows[y].xs[i];
      if (counting) {
        if (find(rows[y - 1], x) && find(rows[y + 1], x)) {
          res += rows[y].xs[i + 1] - x - 1;
          counting = false;
        } else if (find(rows[y-1], x) && !from_top){
          res += rows[y].xs[i + 1] - x - 1;
          counting = false;
        } else if (find(rows[y+1], x) && from_top){
          res += rows[y].xs[i + 1] - x - 1;
          counting = false;
        }
      } else {
        if (find(rows[y - 1], x) && find(rows[y + 1], x)) {
          counting = true;
        } else if (find(rows[y-1], x)) {
          counting = true;
            from_top = true;
        } else if (find(rows[y+1], x)) {
          counting = true;
            from_top = false;
        }
      }
    }
  }

  res += p_len;

  printf("\n");
  printf("\n");
  printf("%lld", res);
  printf("\n");
  fclose(f);
  if (l) {
    free(l);
  }
}
