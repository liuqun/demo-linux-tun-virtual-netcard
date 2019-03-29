// 本程序节选自博客: https://segmentfault.com/a/1190000009249039

/* 样例代码使用步骤如下

#--------------------------第一个shell窗口----------------------
#程序保存成tun.c，然后编译
dev@debian:~$ gcc -g -O0 tun.c -o tun

#启动tun程序，程序会创建一个新的tun设备，
#程序会阻塞在这里，等着数据包过来
dev@debian:~$ sudo ./tun
Open tun/tap device tun0 for reading...
Read 84 bytes from tun/tap device
Read 84 bytes from tun/tap device
Read 84 bytes from tun/tap device
Read 84 bytes from tun/tap device

#--------------------------第二个shell窗口----------------------
#启动抓包程序，抓经过tun0的包
# tcpdump -i tun0
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on tun0, link-type RAW (Raw IP), capture size 262144 bytes
19:57:13.473101 IP 192.168.3.11 > 192.168.3.12: ICMP echo request, id 24028, seq 1, length 64
19:57:14.480362 IP 192.168.3.11 > 192.168.3.12: ICMP echo request, id 24028, seq 2, length 64
19:57:15.488246 IP 192.168.3.11 > 192.168.3.12: ICMP echo request, id 24028, seq 3, length 64
19:57:16.496241 IP 192.168.3.11 > 192.168.3.12: ICMP echo request, id 24028, seq 4, length 64

#--------------------------第三个shell窗口----------------------
#./tun启动之后，通过ip link命令就会发现系统多了一个tun设备，
#在我的测试环境中，多出来的设备名称叫tun0，在你的环境中可能叫tun1或tun2等等依次递增
#新的设备没有ip，我们先给tun0配上IP地址
dev@debian:~$ sudo ip addr add 192.168.3.11/24 dev tun0

#默认情况下，tun0没有起来，用下面的命令将tun0启动起来
dev@debian:~$ sudo ip link set tun0 up

#尝试ping一下192.168.3.0/24网段的IP，
#根据默认路由，该数据包会走tun0设备，
#由于我们的程序中收到数据包后，啥都没干，相当于把数据包丢弃了，
#所以这里的ping根本收不到返回包，
#但在前两个窗口中可以看到这里发出去的四个icmp echo请求包，
#说明数据包正确的发送到了应用程序里面，只是应用程序没有处理该包
dev@debian:~$ ping -c 4 192.168.3.12
PING 192.168.3.12 (192.168.3.12) 56(84) bytes of data.

--- 192.168.3.12 ping statistics ---
4 packets transmitted, 0 received, 100% packet loss, time 3023ms
...
*/

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

struct tuntap_option {
    char tuntap_interface_name[IFNAMSIZ+1];
    int disable_pktinfo;
};

#include <ctype.h>
int create_tun_interface(const struct tuntap_option *opt, char out_interface_name[IFNAMSIZ])
{

    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if ((fd = open(clonedev, O_RDWR)) < 0) {
        perror(clonedev);
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN;
    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *        IFF_NO_PI - Do not provide packet information
     */
    if (opt->disable_pktinfo) {
        ifr.ifr_flags |= IFF_NO_PI;
    }
    if (isalnum(opt->tuntap_interface_name[0])) {
        ifr.ifr_name[IFNAMSIZ-1] = '\0';
        memcpy(ifr.ifr_name, opt->tuntap_interface_name, IFNAMSIZ-1);
    }

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        perror("Faild to create tunX interface");
        close(fd);
        return err;
    }

    memcpy(out_interface_name, ifr.ifr_name, IFNAMSIZ);

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

#include <errno.h>
#include <signal.h>

void sigaction_cb_func(int signo, siginfo_t *info, void *context)
{
    /* SIGTERM and SIGINT come from different place, print some notice on the screen */
    switch (signo) {
        case SIGINT: {
            fprintf(stderr, "\n[Received keyboard interrupt (Ctrl+C)]\n");
            break;
        }
        case SIGTERM: {
            fprintf(stderr, "\n[Received signal SIGTERM (from Linux OS)]\n");
            break;
        }
        default: {
            fprintf(stderr, "\n[Ignoring other un-handled signal number=%d]\n", signo);
            return;
        }
    }

    fprintf(stderr, "Debug: Some cleanups would be done before exiting...\n");
    exit(0);
}

int main()
{

    int tun_fd, nread;
    char buffer[1500];
    int i;
    struct tuntap_option opt = {
        .tuntap_interface_name = "tun0",
        .disable_pktinfo = 1,
    };
    char ifnambuf[IFNAMSIZ+1] = {0};

    tun_fd = create_tun_interface(&opt, ifnambuf);
    if (tun_fd >=0 ) {
        ifnambuf[sizeof(ifnambuf)-1] = '\0';
        printf("Successfully opened tun/tap device: %s for reading...\n", ifnambuf);
    }
    printf("Debug: tun_fd=%d\n", tun_fd);

    if (tun_fd < 0) {
        exit(1);
    } else {
        struct sigaction action;

        memset(&action, 0, sizeof(struct sigaction));
        action.sa_flags = SA_SIGINFO;
        action.sa_sigaction = sigaction_cb_func;
        sigemptyset(&action.sa_mask);
        if ((sigaction(SIGINT, &action, NULL) < 0) ||
            (sigaction(SIGTERM, &action, NULL) < 0) ) {
            fprintf(stderr, "Debug: Cannot register callback function for SIGTERM / SIGINT.\n");
        }
        global.tun_fd = tun_fd;
        (void) atexit(auto_cleanup_tun_fd);
    }

    for (i=0; i<20; i++) {
        nread = read(tun_fd, buffer, sizeof(buffer));
        if (nread < 0) {
            int e = errno;

            if (EINTR == e) {
                // Ignore SIGINT(Ctrl_C) keyboard interrupt.
                continue;
            }
            fprintf(stderr, "Failed to read from interface: %s", strerror(e));
            exit(1);
        }

        printf("i=%d, nread = %d bytes\n", i, nread);
    }
    return 0;
}
