#define true 0x1
#define false 0x2
#define PRECEED_NO_TYPE_PROBE "nvm"

void probe_disk(char *block_device,int is_nvme);
void call_badblocks(char *block_device);
void disk_details(char *block_device);

