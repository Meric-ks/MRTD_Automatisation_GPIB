#ifdef _WIN64
    #include <ni488.h>
#else
    #include <gpib/ib.h>
#endif

static float current_temp   = 20.0f;
static int   current_target = 1;

float gpib_temp_inc(void)
{
    current_temp += 1.0f;
    return current_temp;
}

float gpib_temp_dec(void)
{
    current_temp -= 1.0f;
    return current_temp;
}

int gpib_next_target(void)
{
    current_target++;
    return current_target;
}
