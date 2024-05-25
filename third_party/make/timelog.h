#ifndef MAKE_TIMELOG_H_
#define MAKE_TIMELOG_H_

struct timelog;

extern int timelog_fd;
extern char *timelog_path;

void timelog_init (void);
struct timelog *timelog_begin (char **);
void timelog_end (struct timelog *);

#endif /* MAKE_TIMELOG_H_ */
