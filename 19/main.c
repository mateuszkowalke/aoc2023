#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int cat;
  int cmp;
  int val;
  char dst[8];
  size_t dst_len;
} Rule;

typedef struct {
  char name[8];
  size_t name_len;
  Rule rules[8];
  size_t rules_len;
} Workflow;

typedef struct {
  int x_min;
  int x_max;
  int m_min;
  int m_max;
  int a_min;
  int a_max;
  int s_min;
  int s_max;
} Range;

Workflow find_next(char *name, Workflow *ws, size_t w_len) {
  for (int i = 0; i < w_len; i++) {
    if (strcmp(ws[i].name, name) == 0) {
      return ws[i];
    }
  }
  printf("Can not find workflow: %s\n", name);
  exit(1);
}

long long accept(Range r, Workflow *ws, size_t w_len, char *next) {
  printf("next dst: %s\n", next);
  long long tmp = 0;
  if (strcmp("A", next) == 0) {
    for (int i = r.x_min; i <= r.x_max; i++) {
      tmp += i * (r.m_max - r.m_min + 1) * (r.a_max - r.a_min + 1) *
             (r.s_max - r.s_min + 1);
    }
    for (int i = r.m_min; i <= r.m_max; i++) {
      tmp += i * (r.a_max - r.a_min + 1) * (r.s_max - r.s_min + 1) *
             (r.x_max - r.x_min + 1);
    }
    for (int i = r.a_min; i <= r.a_max; i++) {
      tmp += i * (r.s_max - r.s_min + 1) * (r.x_max - r.x_min + 1) *
             (r.m_max - r.m_min + 1);
    }
    for (int i = r.s_min; i <= r.s_max; i++) {
      tmp += i * (r.a_max - r.a_min + 1) * (r.x_max - r.x_min + 1) *
             (r.m_max - r.m_min + 1);
    }
    return tmp;
  }

  if (strcmp("R", next) == 0) {
    return 0;
  }

  Workflow w = find_next(next, ws, w_len);
  /* printf("next: %s\n", next); */
  int i = 0;
  while (i < w.rules_len) {
    if (w.rules[i].cat == 4) {
      break;
    }
    if (w.rules[i].cat == 0) {
      if (w.rules[i].cmp == -1) {
        if (r.x_min > w.rules[i].val - 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = w.rules[i].val - 1,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.x_max < w.rules[i].val) {
          i++;
          continue;
        }
        r.x_min = w.rules[i].val;
      } else {
        if (r.x_max < w.rules[i].val + 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = w.rules[i].val + 1,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        r.x_min = w.rules[i].val + 1;
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.x_min > w.rules[i].val) {
          i++;
          continue;
        }
        r.x_max = w.rules[i].val;
      }
    } else if (w.rules[i].cat == 1) {
      if (w.rules[i].cmp == -1) {
        if (r.m_min > w.rules[i].val - 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = w.rules[i].val - 1,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.m_max < w.rules[i].val) {
          i++;
          continue;
        }
        r.m_min = w.rules[i].val;
      } else {
        if (r.m_max < w.rules[i].val + 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = w.rules[i].val + 1,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.m_min > w.rules[i].val) {
          i++;
          continue;
        }
        r.m_max = w.rules[i].val;
      }
    } else if (w.rules[i].cat == 2) {
      if (w.rules[i].cmp == -1) {
        if (r.a_min > w.rules[i].val - 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = w.rules[i].val - 1,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.a_max < w.rules[i].val) {
          i++;
          continue;
        }
        r.a_min = w.rules[i].val;
      } else {
        if (r.a_max < w.rules[i].val + 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = w.rules[i].val + 1,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.a_min > w.rules[i].val) {
          i++;
          continue;
        }
        r.a_max = w.rules[i].val;
      }
    } else if (w.rules[i].cat == 3) {
      if (w.rules[i].cmp == -1) {
        if (r.s_min > w.rules[i].val - 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = r.s_min,
                              .s_max = w.rules[i].val - 1};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.s_max < w.rules[i].val) {
          i++;
          continue;
        }
        r.s_min = w.rules[i].val;
      } else {
        if (r.s_max < w.rules[i].val + 1) {
          i++;
          continue;
        }
        Range r_tmp = (Range){.x_min = r.x_min,
                              .x_max = r.x_max,
                              .m_min = r.m_min,
                              .m_max = r.m_max,
                              .a_min = r.a_min,
                              .a_max = r.a_max,
                              .s_min = w.rules[i].val + 1,
                              .s_max = r.s_max};
        tmp += accept(r_tmp, ws, w_len, w.rules[i].dst);
        if (r.s_min > w.rules[i].val) {
          i++;
          continue;
        }
        r.s_max = w.rules[i].val;
      }
    }
    i++;
  }

  if (strcmp(w.rules[w.rules_len - 1].dst, "A") == 0) {
    for (int i = r.x_min; i <= r.x_max; i++) {
      tmp += i * (r.m_max - r.m_min + 1) * (r.a_max - r.a_min + 1) *
             (r.s_max - r.s_min + 1);
    }
    for (int i = r.m_min; i <= r.m_max; i++) {
      tmp += i * (r.a_max - r.a_min + 1) * (r.s_max - r.s_min + 1) *
             (r.x_max - r.x_min + 1);
    }
    for (int i = r.a_min; i <= r.a_max; i++) {
      tmp += i * (r.s_max - r.s_min + 1) * (r.x_max - r.x_min + 1) *
             (r.m_max - r.m_min + 1);
    }
    for (int i = r.s_min; i <= r.s_max; i++) {
      tmp += i * (r.a_max - r.a_min + 1) * (r.x_max - r.x_min + 1) *
             (r.m_max - r.m_min + 1);
    }
    return tmp;
  }

  if (strcmp(w.rules[w.rules_len - 1].dst, "R") == 0) {
    return tmp;
  }

  /* 4196961739762 */
  /* 167409079868000 */

  return tmp + accept(r, ws, w_len, w.rules[w.rules_len - 1].dst);
}

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

  size_t w_len = 0;
  size_t w_size = 8;
  Workflow *ws = malloc(sizeof(Workflow) * w_size);

  size_t p_len = 0;
  size_t p_size = 8;
  int *ps = malloc(sizeof(int) * p_size * 4);

  bool in_workflows = true;

  while (getline(&l, &n, f) != -1) {
    line = l;

    if (strlen(line) < 3) {
      in_workflows = false;
      continue;
    }

    if (in_workflows) {
      Workflow w;
      char c;
      char buf[8];
      size_t buf_len = 0;

      while ((c = *line++) != '{') {
        buf[buf_len++] = c;
      }
      buf[buf_len] = 0;
      w.name_len = buf_len;
      strcpy(w.name, buf);

      w.rules_len = 0;
      buf_len = 0;
      while ((c = *line++) != '}' && c != '\n') {
        Rule r;
        if (c == 'A' || c == 'R') {
          r.cat = 4;
          r.dst_len = 1;
          r.dst[0] = c;
          r.dst[1] = 0;
          w.rules[w.rules_len++] = r;
          break;
        }

        if (*line != '<' && *line != '>') {
          r.cat = 4;
          buf_len = 0;
          line--;
          while ((c = *line++) != ',' && c != '}') {
            buf[buf_len++] = c;
          }
          buf[buf_len] = 0;
          r.dst_len = buf_len;
          strcpy(r.dst, buf);
          w.rules[w.rules_len++] = r;
          break;
        }

        r.cat = c == 'x' ? 0 : c == 'm' ? 1 : c == 'a' ? 2 : 3;

        c = *line++;
        r.cmp = c == '<' ? -1 : 1;

        buf_len = 0;
        while ((c = *line++) != ':') {
          buf[buf_len++] = c;
        }
        buf[buf_len] = 0;
        r.val = atoi(buf);

        buf_len = 0;
        while ((c = *line++) != ',' && c != '}') {
          buf[buf_len++] = c;
        }
        buf[buf_len] = 0;
        r.dst_len = buf_len;
        strcpy(r.dst, buf);

        /* printf("rule: cat: %d, cmp: %d, val: %d, dst: %s\n", r.cat, r.cmp, */
        /*        r.val, r.dst); */

        w.rules[w.rules_len++] = r;
      }
      ws[w_len++] = w;
      if (w_len == w_size) {
        w_size *= 2;
        ws = realloc(ws, sizeof(Workflow) * w_size);
      }
    }
  }

  for (int i = 0; i < w_len; i++) {
    printf("workflow name: %s\n", ws[i].name);
    for (int j = 0; j < ws[i].rules_len; j++) {
      Rule r = ws[i].rules[j];
      printf("rule: cat: %d, cmp: %d, val: %d, dst: %s\n", r.cat, r.cmp, r.val,
             r.dst);
    }
  }

  Range r = (Range){.x_min = 1,
                    .x_max = 4000,
                    .m_min = 1,
                    .m_max = 4000,
                    .a_min = 1,
                    .a_max = 4000,
                    .s_min = 1,
                    .s_max = 4000};
  res = accept(r, ws, w_len, "in");

  printf("\n");
  printf("\n");
  printf("%lld", res);
  printf("\n");
  fclose(f);
  if (l) {
    free(l);
  }
}
