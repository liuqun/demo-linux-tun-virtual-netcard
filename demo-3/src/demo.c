#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <linux/if_tun.h>
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>

int create_tun_interface(int flags)
{

    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        perror(clonedev);
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        perror("Faild to create tunX interface");
        close(fd);
        return err;
    }

    printf("Open tun/tap device: %s for reading...\n", ifr.ifr_name);

    return fd;
}

static struct {
    int tun_fd;
} global = {
    .tun_fd = -1,
};

static void auto_cleanup_tun_fd()
{/* Clean up callback function */
    int fd = global.tun_fd;

    if (fd < 0) {
        return;
    }
    close(fd);
    fprintf(stderr, "Closed tun_fd=%d before program exit\n", fd);
}

int main()
{

    int tun_fd, nread;
    char buffer[1500];
    int i;

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *        IFF_NO_PI - Do not provide packet information
     */
    tun_fd = create_tun_interface(IFF_TUN | IFF_NO_PI);
    printf("Debug: tun_fd=%d\n", tun_fd);

    if (tun_fd < 0) {
        exit(1);
    } else {
        global.tun_fd = tun_fd;
        (void) atexit(auto_cleanup_tun_fd);
    }

    for (i=0; i<20; i++) {
        nread = read(tun_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            perror("Failed to read from interface");
            exit(1);
        }

        printf("i=%d, nread = %d bytes\n", i, nread);
    }
    return 0;
}
