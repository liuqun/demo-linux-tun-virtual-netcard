#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "platform.h"

const char StandardTunDeviceNode[] = "/dev/net/tun";  // 标准TUN设备节点

typedef struct {
	int fd;
	enum {
		DEV_TYPE_TUN,
		DEV_TYPE_TAP,
	} type;
	const char *device_node;
} demo_tuntap_ctx;

void tuntap_read_incoming_pkts(const demo_tuntap_ctx *ctx, int cnt)
{
	int i;
	unsigned char buf[1500];
	const size_t buf_size = 1500;

	for (i=0; i<cnt; i++) {
		//TODO: read data
		//int n;
		//n = read(ctx->fd, buf, buf_size);
		//printf("n=%d\n", n);
		//buf_print(buf, n);
		sleep(1);
	}
	return;
}

void tuntap_add_ip_address(const demo_tuntap_ctx *ctx, const char *str)  // TODO: set IP address
{
	return;
}

int main(int argc, char *argv[])
{
	demo_tuntap_ctx ctx;
	struct ifreq ifr;
	int ret;

	ctx.type = DEV_TYPE_TUN;
	ctx.device_node = StandardTunDeviceNode;
	if ((ctx.fd = open(ctx.device_node, O_RDWR)) < 0) {
		ret = 127;
		goto ERR_CHECK_WHY_FAILED_TO_OPEN_DEVICE_NODE;
	}
	ifr.ifr_name[0] = '\0';
	if (ctx.type == DEV_TYPE_TUN) {
		ifr.ifr_flags = IFF_TUN;
	} else {
		ifr.ifr_flags = IFF_TAP;
	}
	ifr.ifr_flags |= IFF_NO_PI;
	ifr.ifr_flags |= IFF_ONE_QUEUE;
	if ((ret = ioctl(ctx.fd, TUNSETIFF, &ifr)) < 0) {
		fprintf(stderr, "ERROR: ioctl() returns %d: %s\n", ret, strerror(errno));
		goto NORMAL_EXIT;
	} else {
		char str[IFNAMSIZ+1];

		memcpy(str, ifr.ifr_name, IFNAMSIZ);
		str[IFNAMSIZ] = '\0';
		printf("Successfully created TUN interface=%s\n", str);
	}
	tuntap_add_ip_address(&ctx, "10.1.2.3");
	tuntap_read_incoming_pkts(&ctx, 5);

NORMAL_EXIT:
	close(ctx.fd);
	return (0);

ERR_CHECK_WHY_FAILED_TO_OPEN_DEVICE_NODE:
	fprintf(stderr, "ERROR: ctx.fd=%d: %s\n", ctx.fd, strerror(errno));
	return (ret);
}
