#ifndef GPIB_H
#define GPIB_H

int gpib_init(int PrimaryAddress);
int gpib_close(void);
float gpib_temp_inc(void);
float gpib_temp_dec(void);
int   gpib_next_target(void);

#endif 