#ifndef VM_WRITER_H
#define VM_WRITER_H

typedef enum {
  S_Const,
  S_Arg,
  S_Local,
  S_Static,
  S_This,
  S_That,
  S_Pointer,
  S_Temp
} Segment;
typedef enum { Add, Sub, Neg, Eq, Gt, Lt, And, Or, Not } Arithmetic;

void init_writer(char *filename);
void write_push(Segment segment);
void write_pop(Segment segment);
void write_arithmetic(Arithmetic command);
void write_label(char *label);
void write_goto(char *label);
void write_if(char *label);
void write_call(char *name, int nargs);
void write_function(char *name, int nlocals);
void write_return(void);
void close_writer(void);

#endif
