#include <dfs.h>
#include <dfs_posix.h>
#include <dfs_file.h>



#ifdef RT_USING_FINSH
#include <finsh.h>

static struct dfs_fd fd;
static struct dirent dirent;

void	makediskdir(char *src)
{
		DIR 	*dirp = RT_NULL;
		dirp = opendir(src);
		if(dirp == RT_NULL) 
				mkdir(src, 0);
		else
			closedir(dirp);	
}

void disksize(char *path)
{
    int 						result;
    struct statfs 	buffer;
		long 	long			cap, cap1, len;
		
    if (path == RT_NULL)
        result = dfs_statfs("/", &buffer);
    else
        result = dfs_statfs(path, &buffer);
		
    if (result == 0)
    {		
				if (buffer.f_bsize > 512)	
				{
						len = buffer.f_bsize / 1024;
						cap  = len * buffer.f_bfree ;// / 1024;
						cap1 = len * buffer.f_blocks;// / 1024;			
				}
				else
				{
						len = 1024 / buffer.f_bsize;
						cap  = buffer.f_bfree / len;// / 1024;
						cap1 = buffer.f_blocks / len;// / 1024;			
				}	
				rt_kprintf(" %12lu kbytes free\ndisk total: %27lu kbytes\n", 
									(unsigned long)cap, (unsigned long)cap1);						
    }
}
void ls(const char *pathname)
{
    struct 	stat stat;
    int 		length, files = 0, dirs = 0, filelen = 0;
    char 		*fullpath, *path;

    fullpath = RT_NULL;
    if (pathname == RT_NULL)
    {
#ifdef DFS_USING_WORKDIR
        /* open current working directory */
        path = rt_strdup(working_directory);
#else
        path = rt_strdup("/");
#endif
        if (path == RT_NULL)
            return ; /* out of memory */
    }
    else
    {
        path = (char *)pathname;
    }

    /* list directory */
    if (dfs_file_open(&fd, path, DFS_O_DIRECTORY) == 0)
    {
//        rt_kprintf("Filesys %s:\n", fd.fs->ops->name);				
        rt_kprintf("Directory of %s:\n", path);
        do
        {
            rt_memset(&dirent, 0, sizeof(struct dirent));
            length = dfs_file_getdents(&fd, &dirent, sizeof(struct dirent));
            if (length > 0)
            {
                rt_memset(&stat, 0, sizeof(struct stat));

                /* build full path for each file */
                fullpath = dfs_normalize_path(path, dirent.d_name);
                if (fullpath == RT_NULL) 
                    break;

                if (dfs_file_stat(fullpath, &stat) == 0)
                {
										rt_kprintf("%u/%02u/%02u %02u:%02u   ", ((stat.st_mtime >> 25) + 1980), ((stat.st_mtime >> 21) & 15), 
																														((stat.st_mtime >> 16) & 31),
																														((stat.st_mtime >> 11) & 31), (stat.st_mtime >> 5) & 63);									
                    if ( DFS_S_ISDIR(stat.st_mode))
                    {
                        rt_kprintf("%-20s", "<DIR>");
												dirs ++;
                    }
                    else
                    {
                        rt_kprintf("%20lu", stat.st_size);
												files ++;
												filelen += stat.st_size;
                    }
                    rt_kprintf(" %-35s\n", dirent.d_name);										
                }
                else
                    rt_kprintf("BAD file: %s\n", dirent.d_name);
                rt_free(fullpath);
            }
        }while(length > 0);				
        dfs_file_close(&fd);
				
				rt_kprintf("%16lu   File(s) %12lu bytes\n", files, filelen);
				rt_kprintf("%16lu   Dir (s)", dirs);
				disksize(path);					
    }
    else
    {
        rt_kprintf("No directory\n");
    }
    if (pathname == RT_NULL) 
        rt_free(path);
		
}
//FINSH_FUNCTION_EXPORT(ls, list directory contents)
int	rdall(char *filename)
{
		DIR 					*dirp = RT_NULL;
		struct dirent *d = RT_NULL;
		char					*p = RT_NULL;
	
		p = (char *)rt_malloc(256);	
		if (p == RT_NULL)		return -1;
	
		strcpy(p, filename);	
		dirp = opendir(p);	
				
		while ((d = readdir(dirp)) != RT_NULL) 
		{ 
				if (d->d_type == DFS_DT_REG)
				{
						sprintf(p, "%s/%s", filename, d->d_name);						
						if (unlink(p) < 0)
						{	
								rt_free(p);								
								return -1;
						}
				}
				if (d->d_type == DFS_DT_DIR)
				{
						sprintf(p, "%s/%s", filename, d->d_name);				
						if (rdall(p) < 0)
								break;
				}
		}
		closedir(dirp);	
		rt_free(p);			
		
		if (unlink(filename) < 0)
		{			
				return -1;	
		}	
		return 0;		
}

void rm(const char *filename)
{
		char					name[20], f = 0, i;
		DIR 					*dirp = RT_NULL;
		struct dirent *d = RT_NULL;		
	

		if (strcmp(filename , "*.*") == 0)
		{
				f = 2;		
		}
		else if (*filename == '*')
		{
				f = 1;
				strcpy (name, (filename + 1));
		}		
		if (f > 0)
		{
				dirp = opendir(working_directory);	
				while ((d = readdir(dirp)) != RT_NULL) 
				{ 
						if (d->d_type == DFS_DT_REG)
						{								
								if (f == 1)
								{
										for (i = 0; i < 40; i ++)
										{
												if (*(d->d_name + i) == '.')
														break;
										}
										if (strcmp(name, d->d_name + i) == 0)
										{
												if (unlink(d->d_name) < 0)
														rt_kprintf("Del %s failed\n", d->d_name);	
										}
								}									
								else
								{
										if (unlink(d->d_name) < 0)
												rt_kprintf("Del %s failed\n", d->d_name);
								}
						}
				}	
				closedir(dirp);					
		}
		else
		{
				if (unlink(filename) < 0)
						rt_kprintf("Del %s failed\n", filename);
		}		
}
//FINSH_FUNCTION_EXPORT(rm, remove files or directories)

void cat(const char* filename)
{
    rt_uint32_t length;
    char buffer[81];

    if (dfs_file_open(&fd, filename, DFS_O_RDONLY) < 0)
    {
        rt_kprintf("Open %s failed\n", filename);

        return;
    }
    do
    {
        rt_memset(buffer, 0, sizeof(buffer));
        length = dfs_file_read(&fd, buffer, sizeof(buffer)-1 );
        if (length > 0)
        {
            rt_kprintf("%s", buffer);
        }
    }while (length > 0);
    dfs_file_close(&fd);
}

int	move(const char *srcfile, const char *destfile)
{	
		int fd;
		char *dest = RT_NULL;
       
//		rt_kprintf("%s => %s\n", srcfile, destfile);
		fd = open(destfile, O_DIRECTORY, 0);
		if (fd >= 0)
		{
				char *src;				
				close(fd);

				/* it's a directory */			
				dest = (char*)rt_malloc(DFS_PATH_MAX);
				if (dest == RT_NULL)
				{
					rt_kprintf("out of memory\n");
					return -1;
				}

				src = (char *)srcfile + rt_strlen(srcfile);
				while (src != srcfile) 
				{
					if (*src == '/') break;
					src --;
				}
				rt_snprintf(dest, DFS_PATH_MAX - 1, "%s/%s", destfile, src);
		}
		else
		{
				fd = open(destfile, O_RDONLY, 0);
				if (fd >= 0)
				{
					close(fd);
					
					unlink(destfile);
				}

				dest = (char *)destfile;
		}
		rename(srcfile, dest);
		if (dest != RT_NULL && dest != destfile) 
				rt_free(dest);
		return 0;
}


//FINSH_FUNCTION_EXPORT(cat, print file)
#define BUF_SZ  4096
static void copyfile(const char *src, const char *dst)
{
    struct dfs_fd src_fd;
    rt_uint8_t *block_ptr;
    rt_int32_t read_bytes;

    block_ptr = rt_malloc(BUF_SZ);
    if (block_ptr == RT_NULL)
    {
        rt_kprintf("out of memory\n");

        return;
    }

    if (dfs_file_open(&src_fd, src, DFS_O_RDONLY) < 0)
    {
        rt_free(block_ptr);
        rt_kprintf("Read %s failed\n", src);

        return;
    }
    if (dfs_file_open(&fd, dst, DFS_O_WRONLY | DFS_O_CREAT) < 0)
    {
        rt_free(block_ptr);
        dfs_file_close(&src_fd);

        rt_kprintf("Write %s failed\n", dst);

        return;
    }
    do
    {
        read_bytes = dfs_file_read(&src_fd, block_ptr, BUF_SZ);
        if (read_bytes > 0)
        {
            dfs_file_write(&fd, block_ptr, read_bytes);
        }
    } while (read_bytes > 0);
    rt_kprintf("Copy %s file ok\n", src);	
    dfs_file_close(&src_fd);
    dfs_file_close(&fd);
    rt_free(block_ptr);
}

extern int mkdir(const char *path, mode_t mode);
static void copydir(const char * src, const char * dst)
{
    struct dfs_fd fd;
    struct dirent dirent;
    struct stat stat;
    int length;

    if (dfs_file_open(&fd, src, DFS_O_DIRECTORY) < 0)
    {
        rt_kprintf("open %s failed\n", src);
        return ;
    }

    do
    {
        rt_memset(&dirent, 0, sizeof(struct dirent));
        length = dfs_file_getdents(&fd, &dirent, sizeof(struct dirent));
        if (length > 0)
        {
            char * src_entry_full = RT_NULL;
            char * dst_entry_full = RT_NULL;

            if (strcmp(dirent.d_name, "..") == 0 || strcmp(dirent.d_name, ".") == 0)
                continue;

            /* build full path for each file */
            if ((src_entry_full = dfs_normalize_path(src, dirent.d_name)) == RT_NULL)
            {
                rt_kprintf("out of memory!\n");
                break;
            }
            if ((dst_entry_full = dfs_normalize_path(dst, dirent.d_name)) == RT_NULL)
            {
                rt_kprintf("out of memory!\n");
                rt_free(src_entry_full);
                break;
            }

            rt_memset(&stat, 0, sizeof(struct stat));
            if (dfs_file_stat(src_entry_full, &stat) != 0)
            {
                rt_kprintf("open file: %s failed\n", dirent.d_name);
                continue;
            }

            if (DFS_S_ISDIR(stat.st_mode))
            {
                mkdir(dst_entry_full, 0);
								rt_kprintf("Make dir %s ok\n", dst_entry_full);							
                copydir(src_entry_full, dst_entry_full);
            }
            else
            {
                copyfile(src_entry_full, dst_entry_full);
            }
            rt_free(src_entry_full);
            rt_free(dst_entry_full);
        }
    }while(length > 0);

    dfs_file_close(&fd);
}

static const char *_get_path_lastname(const char *path)
{
    char * ptr;
    if ((ptr = strrchr(path, '/')) == RT_NULL)
        return path;

    /* skip the '/' then return */
    return ++ptr;
}
int copy(const char *src, const char *dst)
{
#define FLAG_SRC_TYPE      0x03
#define FLAG_SRC_IS_DIR    0x01
#define FLAG_SRC_IS_FILE   0x02
#define FLAG_SRC_NON_EXSIT 0x00

#define FLAG_DST_TYPE      0x0C
#define FLAG_DST_IS_DIR    0x04
#define FLAG_DST_IS_FILE   0x08
#define FLAG_DST_NON_EXSIT 0x00

    struct stat stat;
    rt_uint32_t flag = 0;

    /* check the staus of src and dst */
    if (dfs_file_stat(src, &stat) < 0)
    {
        rt_kprintf("copy failed, bad %s\n", src);
        return -1;
    }
    if (DFS_S_ISDIR(stat.st_mode))
        flag |= FLAG_SRC_IS_DIR;
    else
        flag |= FLAG_SRC_IS_FILE;

    if (dfs_file_stat(dst, &stat) < 0)
    {
        flag |= FLAG_DST_NON_EXSIT;
    }
    else
    {
        if (DFS_S_ISDIR(stat.st_mode))
            flag |= FLAG_DST_IS_DIR;
        else
            flag |= FLAG_DST_IS_FILE;
    }

    //2. check status
    if ((flag & FLAG_SRC_IS_DIR) && (flag & FLAG_DST_IS_FILE))
    {
        rt_kprintf("cp faild, cp dir to file is not permitted!\n");
        return  -1;
    }

    //3. do copy
    if (flag & FLAG_SRC_IS_FILE)
    {
        if (flag & FLAG_DST_IS_DIR)
        {
            char * fdst;
            fdst = dfs_normalize_path(dst, _get_path_lastname(src));
            if (fdst == NULL)
            {
                rt_kprintf("out of memory\n");
                return -1;
            }
            copyfile(src, fdst);
 //           rt_kprintf("Copy %s file ok\n", src);							
            rt_free(fdst);
        }
        else
        {
            copyfile(src, dst);
 //           rt_kprintf("Copy %s file ok\n", src);					
        }
    }
    else //flag & FLAG_SRC_IS_DIR
    {
        if (flag & FLAG_DST_IS_DIR)
        {
            char * fdst;
            fdst = dfs_normalize_path(dst, _get_path_lastname(src));
            if (fdst == NULL)
            {
                rt_kprintf("out of memory\n");
                return -1;
            }
            mkdir(fdst, 0);
            copydir(src, fdst);
//            rt_kprintf("Copy %s dir ok\n", src);							
            rt_free(fdst);
        }
        else if ((flag & FLAG_DST_TYPE) == FLAG_DST_NON_EXSIT)
        {
            mkdir(dst, 0);
            copydir(src, dst);				
        }
        else
        {
            copydir(src, dst);
 //           rt_kprintf("Copy %s dir ok\n", src);						
        }
    }
		return 0;
}
void readspeed(const char* filename, int block_size)
{
    int fd;
    char *buff_ptr;
    rt_size_t total_length;
    rt_tick_t tick;

    fd = open(filename, 0, O_RDONLY);
    if (fd < 0)
    {
        rt_kprintf("open file:%s failed\n", filename);
        return;
    }

    buff_ptr = rt_malloc(block_size);
    if (buff_ptr == RT_NULL)
    {
        rt_kprintf("no memory\n");
        close(fd);

        return;
    }

    tick = rt_tick_get();
    total_length = 0;
    while (1)
    {
        int length;
        length = read(fd, buff_ptr, block_size);

        if (length <= 0) break;
        total_length += length;
    }
    tick = rt_tick_get() - tick;

	/* close file and release memory */
    close(fd);
		rt_free(buff_ptr);

    /* calculate read speed */
    rt_kprintf("File read  speed: %d byte/s\n", total_length /tick * RT_TICK_PER_SECOND);
}

void writespeed(const char* filename, int total_length, int block_size)
{
    int fd, index, length;
    char *buff_ptr;
    rt_tick_t tick;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        rt_kprintf("open file:%s failed\n", filename);
        return;
    }

    buff_ptr = rt_malloc(block_size);
    if (buff_ptr == RT_NULL)
    {
        rt_kprintf("no memory\n");
        close(fd);

        return;
    }

		/* prepare write data */
		for (index = 0; index < block_size; index++)
		{
			buff_ptr[index] = index;
		}
		index = 0;

		/* get the beginning tick */
		tick = rt_tick_get();
		while (index < total_length / block_size)
		{
				length = write(fd, buff_ptr, block_size);
				if (length != block_size)
				{
						rt_kprintf("write failed\n");
						break;
				}

				index ++;
		}
		tick = rt_tick_get() - tick;

		/* close file and release memory */
		close(fd);
		rt_free(buff_ptr);

			/* calculate write speed */
		rt_kprintf("File write speed: %d byte/s\n", total_length / tick * RT_TICK_PER_SECOND);
}

void filespeed (const char* filename)
{
		
		writespeed(filename, 1024 * 64, 1);	
		readspeed(filename, 1024 * 64);	

}
#endif
