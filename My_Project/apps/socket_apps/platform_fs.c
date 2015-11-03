#include "mfs_config.h"

int platform_init_fs()
{
	/* initialize the memory file system (MFS) image pre-loaded into memory */
	mfs_init_fs(MFS_NUMBYTES, (char *)(MFS_BASE_ADDRESS+4), MFS_INIT_TYPE);

	/* check if we can access index.html */
	if (mfs_exists_file("index.html") == 0) {
		xil_printf("%s: ERROR: unable to locate index.html in MFS\r\n", __FUNCTION__);
		xil_printf("One of your applications requires a Memory File System to be loaded.\r\n");
		xil_printf("Please check if Memory File System has been loaded,\r\n"
				"and it has index.html file in root directory\r\n");
		return -1;
	}
	return 0;
}
