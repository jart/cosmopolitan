/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/stdio/stdio.h"
#include "third_party/python/Include/node.h"
#include "third_party/python/Include/pgenheaders.h"
#include "third_party/python/Include/token.h"

static void list1node(FILE *, node *);
static void listnode(FILE *, node *);

void
PyNode_ListTree(node *n)
{
    listnode(stdout, n);
}

static int level, atbol;

static void
listnode(FILE *fp, node *n)
{
    level = 0;
    atbol = 1;
    list1node(fp, n);
}

static void
list1node(FILE *fp, node *n)
{
    if (n == 0)
        return;
    if (ISNONTERMINAL(TYPE(n))) {
        int i;
        for (i = 0; i < NCH(n); i++)
            list1node(fp, CHILD(n, i));
    }
    else if (ISTERMINAL(TYPE(n))) {
        switch (TYPE(n)) {
        case INDENT:
            ++level;
            break;
        case DEDENT:
            --level;
            break;
        default:
            if (atbol) {
                int i;
                for (i = 0; i < level; ++i)
                    fprintf(fp, "\t");
                atbol = 0;
            }
            if (TYPE(n) == NEWLINE) {
                if (STR(n) != NULL)
                    fprintf(fp, "%s", STR(n));
                fprintf(fp, "\n");
                atbol = 1;
            }
            else
                fprintf(fp, "%s ", STR(n));
            break;
        }
    }
    else
        fprintf(fp, "? ");
}
