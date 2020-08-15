#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

static size_t dir_count = 0;
static char *dirs[100];
static const char *empty = "empty\n";

static void *my_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	(void)conn;
	cfg->umask = 0;
	return NULL;
}

static int my_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    (void)fi;
    size_t i, sl_count = 0;
    
    for (i = 0; i < strlen(path); i++)
        sl_count += (path[i] == '/');
    
    memset(stbuf, 0, sizeof(struct stat));
    for (i = 0; i < dir_count; i++)
    {
        if (strcmp(path + 1, dirs[i]) == 0)
        {
            stbuf->st_mode = __S_IFDIR | 0777;
            stbuf->st_nlink = 2;
            return 0;    
        }
    }

    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = __S_IFDIR | 0777;
        stbuf->st_nlink = 2;
    }
    else if (sl_count > 1)
    {
        stbuf->st_mode = __S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(empty);
    }
    else
        return -ENOENT;
    
    return 0;
}

static int my_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags)
{
    (void)offset;
    (void)fi;
    (void)flags;
    size_t i = 0;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    if (strcmp(path, "/") == 0)
    {
        for (i = 0; i < dir_count; i++)
            filler(buf, dirs[i], NULL, 0, 0);
    }
    else
    {
        //fill ips
        FILE *fips;
        char raw[1000], request[150], ip[30];
        int last_char;
        size_t last_ip = 0;

        snprintf(request, sizeof(request),
                "traceroute '%s'"
                "| egrep -o \"\\(([0-9]{1,3}[\\.]){3}[0-9]{1,3}\\)\""
                "| tr '\n' ' '"
                "| sed 's/[()]//g'",
                path + 1);

        fips = popen(request, "r");
        last_char = fread(raw, 1, sizeof(raw), fips);
        raw[last_char] = '\0';
        fclose(fips);

        for (i = 0; i < strlen(raw); i++)
        {
            if (raw[i] == ' ')
            {
                strncpy(ip, raw + last_ip, i - last_ip);
                ip[i - last_ip] = '\0';
                filler(buf, ip, NULL, 0, 0);
                last_ip = i + 1;
            }
        }
    }

    return 0;
}

static int my_open(const char *path, struct fuse_file_info *fi)
{
    (void)path;

	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;
	return 0;
}

static int my_read(const char *path, char *buf, size_t size, off_t offset,
				   struct fuse_file_info *fi)
{
	(void)fi;
    (void)path;
	size_t len;

	len = strlen(empty);
	if ((size_t)offset < len)
	{
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, empty + offset, size);
	}
	else
		size = 0;
	
    return size;
}

static int my_mkdir(const char *path, mode_t mode)
{
    (void)mode;
    size_t i;
    int res = 0;

    for (i = 0; i < dir_count; i++)
    {
        if (strcmp(path + 1, dirs[i]) == 0)
        {
            res = -1;
            break;
        }
    }

    if (res == 0)
    {
        dirs[dir_count] = (char *)malloc(strlen(path + 1));
        strcpy(dirs[dir_count], path + 1);
        dir_count++;
    }

    return res;
}

static int my_rmdir(const char *path)
{
    size_t i;
    int res = -1;

    for (i = 0; i < dir_count; i++)
    {
        if (strcmp(path + 1, dirs[i]) == 0)
        {
            memset(dirs[i], '\0', strlen(dirs[i]));
            memmove(dirs[i], dirs[dir_count - 1], strlen(dirs[dir_count - 1]));    
            free(dirs[dir_count - 1]);
            dir_count--;    
            res = 0;
            break;
        }
    }

    return res;
}

static const struct fuse_operations my_oper = {
	.init 		= 	my_init,
    .getattr    =   my_getattr,
    .readdir    =   my_readdir,
    .open       =   my_open,
    .read       =   my_read,
    .mkdir      =   my_mkdir,
    .rmdir      =   my_rmdir,
};

int main(int argc, char *argv[])
{
    umask(0);
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    int rc = fuse_main(args.argc, args.argv, &my_oper, NULL);
    fuse_opt_free_args(&args);
    return rc;
}