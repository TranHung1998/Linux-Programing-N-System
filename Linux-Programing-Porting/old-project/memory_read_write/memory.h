#define DATA_TRANSMIT   9
#define REG_MAX_SIZE    4
#define DEV_MAX_SIZE    12
struct _memory_dev
{
    unsigned long *input_addr;
    unsigned long *input_val;
    unsigned long *output_val;
};