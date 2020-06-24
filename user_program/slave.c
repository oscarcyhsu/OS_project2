#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512
int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int i, dev_fd, file_fd, N = 0;// the fd for the device and the fd for the input file
	size_t ret, file_size = 0, data_size = -1, offset = 0;
	size_t total_file_size = 0;
	char file_name[50];
	char method[20];
	char ip[20];
	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed
	char *kernel_address, *file_address;

	
	N = atoi(argv[1]);

	assert(argc == N + 4);
	

	strncpy(method, argv[3+N-1], 20);
	method[19] = '\0';

	strncpy(ip, argv[4+N-1], 20);
	ip[19] = '\0';

	if( (dev_fd = open("/dev/slave_device", O_RDWR)) < 0)//should be O_RDWR for PROT_WRITE when mmap()
	{
		perror("failed to open /dev/slave_device\n");
		return 1;
	}
	gettimeofday(&start ,NULL);

	for (i = 0; i < N; i++)
	{
		strncpy(file_name, argv[2+i], 50);
		file_name[49] = '\0';
		if( (file_fd = open (file_name, O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)
		{
			perror("failed to open input file\n");
			return 1;
		}
		if(ioctl(dev_fd, 0x12345677, ip) == -1)	//0x12345677 : connect to master in the device
		{char* file_address, kernel_address;
			perror("ioctl create slave socket error\n");
			return 1;
		}
		write(1, "ioctl success\n", 14);

		switch(method[0])
		{
			case 'f'://fcntl : read()/write()
				printf("method: fcntl\n");
				do
				{
					ret = read(dev_fd, buf, sizeof(buf)); // read from the the deviceioclt
					//write(STDOUT_FILENO, buf, ret); //write to stdout
					int tmp;
					tmp = write(file_fd, buf, ret); //write to the input file
					//printf("write ret:%d\n", tmp);
					
					file_size += ret;
				}while(ret > 0);
				break;
			case 'm':
				printf("method: mmap\n");
				offset = 0;
				while(1)
				{
					ret = ioctl(dev_fd, 0x12345678);
					if (ret == 0){
						file_size = offset;
						break;
					}
					//printf("ioctl return: %d\n", ret);
					//printf("calling file mmap\n");
					posix_fallocate(file_fd, offset, ret);
					file_address = mmap(NULL, ret, PROT_WRITE, MAP_SHARED, file_fd, offset);
					//printf("calling dev mmap\n");
					kernel_address = mmap(NULL, ret, PROT_READ, MAP_SHARED, dev_fd, offset);
					//printf("memcpy...\n");
					// for (int i =0; i < 20; i++){
					// 	printf(":%c",kernel_address[i]);
					// }

					memcpy(file_address, kernel_address, ret);
					//printf("calling file munmap\n");			
					munmap(file_address, ret);
					//printf("calling file munmap\n");			
					munmap(kernel_address, ret);
					
					offset += ret;
				}
				break;

		}



		if(ioctl(dev_fd, 0x12345679) == -1)// end receiving data, close the connection
		{
			perror("ioctl client exits error\n");
			return 1;
		}
		close(file_fd);
		total_file_size += file_size;
	}

	
	
	
	

    
	gettimeofday(&end, NULL);
	trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
	printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time, total_file_size);


	
	close(dev_fd);
	return 0;
}


