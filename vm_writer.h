#ifndef VM_WRITER_H
#define VM_WRITER_H

typedef enum { Add, Sub, Neg, Eq, Gt, Lt, And, Or, Not } Arithmetic;

void init_writer(char *filename);
void write_push(const char *segment, int index);
void write_pop(const char *segment, int index);
void write_arithmetic(char *command);
void write_label(int label_num);
void write_goto(int label_num);
void write_if(int label_num);
void write_call(char *name, int nargs);
void write_function(char *name, int nlocals);
void write_return(void);
void close_writer(void);
char *get_class_name(void);

#endif
