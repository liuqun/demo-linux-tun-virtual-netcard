#include "platform.h"
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
#include <stdarg.h>
#include <errno.h>
#include <openconnect.h>

/////////////
//static int write_new_config(void *_vpninfo,
//                const char *buf, int buflen);
static void __attribute__ ((format(printf, 3, 4)))
    write_progress(void *_vpninfo, int level, const char *fmt, ...);
static int validate_peer_cert(void *_vpninfo, const char *reason);
static int process_auth_form_cb(void *_vpninfo,
                struct oc_auth_form *form);
//static void init_token(struct openconnect_info *vpninfo,
//            oc_token_mode_t token_mode, const char *token_str);
//////////////

// int create_tun_interface(int flags)
// {

//     struct ifreq ifr;
//     int fd, err;
//     char *clonedev = "/dev/net/tun";

//     if ((fd = open(clonedev, O_RDWR)) < 0) {
//         perror(clonedev);
//         return fd;
//     }

//     memset(&ifr, 0, sizeof(ifr));
//     ifr.ifr_flags = flags;

//     if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
//         perror("Faild to create tunX interface");
//         close(fd);
//         return err;
//     }

//     printf("Open tun/tap device: %s for reading...\n", ifr.ifr_name);

//     return fd;
// }

// static struct {
//     int tun_fd;
// } global = {
//     .tun_fd = -1,
// };

// static void auto_cleanup_tun_fd()
// {/* Clean up callback function */
//     int fd = global.tun_fd;

//     if (fd < 0) {
//         return;
//     }
//     close(fd);
//     fprintf(stderr, "Closed tun_fd=%d before program exit\n", fd);
// }

int main()
{

    int tun_fd, nread;
    char buffer[1500];
    int i;
    struct openconnect_info *vpninfo;
    const char *DEMO_AGENT_STR = "Open AnyConnect VPN Agent";

    openconnect_init_ssl();
    vpninfo = openconnect_vpninfo_new(DEMO_AGENT_STR,
        validate_peer_cert, NULL, process_auth_form_cb, write_progress, NULL);
    if (!vpninfo) {
        fprintf(stderr, "Failed to allocate vpninfo structure\n");
        exit(1);
    }
    openconnect_vpninfo_free(vpninfo);
    return 0;

    // /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
    //  *        IFF_TAP   - TAP device
    //  *        IFF_NO_PI - Do not provide packet information
    //  */
    // tun_fd = create_tun_interface(IFF_TUN | IFF_NO_PI);
    // printf("Debug: tun_fd=%d\n", tun_fd);

    // if (tun_fd < 0) {
    //     exit(1);
    // } else {
    //     global.tun_fd = tun_fd;
    //     (void) atexit(auto_cleanup_tun_fd);
    // }

    // for (i=0; i<20; i++) {
    //     nread = read(tun_fd, buffer, sizeof(buffer));
    //     if (nread < 0) {
    //         perror("Failed to read from interface");
    //         exit(1);
    //     }

    //     printf("i=%d, nread = %d bytes\n", i, nread);
    // }
    // return 0;
}

static int validate_peer_cert(void *_vpninfo, const char *reason)
{
    struct openconnect_info *vpninfo = (struct openconnect_info *)_vpninfo;
// const char *fingerprint;
// struct accepted_cert *this;
//
// if (server_cert) {
//     int err = openconnect_check_peer_cert_hash(vpninfo, server_cert);
//
//     if (!err)
//         return 0;
//
//     if (err < 0)
//         vpn_progress(vpninfo, PRG_ERR,
//                     _("Could not calculate hash of server's certificate\n"));
//     else
//         vpn_progress(vpninfo, PRG_ERR,
//                     _("Server SSL certificate didn't match: %s\n"),
//                     openconnect_get_peer_cert_hash(vpninfo));
//
//     return -EINVAL;
// }
//
// fingerprint = openconnect_get_peer_cert_hash(vpninfo);
//
// for (this = accepted_certs; this; this = this->next) {
//     if (!strcasecmp(this->host, vpninfo->hostname) &&
//         this->port == vpninfo->port &&
//         !openconnect_check_peer_cert_hash(vpninfo, this->fingerprint))
//         return 0;
// }
//
// while (1) {
//     char *details;
//     char *response = NULL;
//
//     fprintf(stderr, _("\nCertificate from VPN server \"%s\" failed verification.\n"
//             "Reason: %s\n"), vpninfo->hostname, reason);
//
//     fprintf(stderr, _("To trust this server in future, perhaps add this to your command line:\n"));
//     fprintf(stderr, _("    --servercert %s\n"), fingerprint);
//
//     if (non_inter)
//         return -EINVAL;
//
//     fprintf(stderr, _("Enter '%s' to accept, '%s' to abort; anything else to view: "),
//             _("yes"), _("no"));
//
//     read_stdin(&response, 0, 0);
//     if (!response)
//         return -EINVAL;
//
//     if (!strcasecmp(response, _("yes"))) {
//         struct accepted_cert *newcert = malloc(sizeof(*newcert));
//         if (newcert) {
//             newcert->next = accepted_certs;
//             accepted_certs = newcert;
//             newcert->fingerprint = strdup(fingerprint);
//             newcert->host = strdup(vpninfo->hostname);
//             newcert->port = vpninfo->port;
//         }
//         free(response);
//         return 0;
//     }
//     if (!strcasecmp(response, _("no"))) {
//         free(response);
//         return -EINVAL;
//     }
//     free(response);
//
//     details = openconnect_get_peer_cert_details(vpninfo);
//     fputs(details, stderr);
//     openconnect_free_cert_info(vpninfo, details);
//     fprintf(stderr, _("Server key hash: %s\n"), fingerprint);
// }
    return -EINVAL;
}

/* Return value:
 *  < 0, on error
 *  = 0, when form was parsed and POST required
 *  = 1, when response was cancelled by user
 */
static int process_auth_form_cb(void *_vpninfo, struct oc_auth_form *form)
{
	struct openconnect_info *vpninfo = (struct openconnect_info *)_vpninfo;
// struct oc_form_opt *opt;
// int empty = 1;

// if (form->banner && verbose > PRG_ERR)
// 	fprintf(stderr, "%s\n", form->banner);

// if (form->error)
// 	fprintf(stderr, "%s\n", form->error);

// if (form->message && verbose > PRG_ERR)
// 	fprintf(stderr, "%s\n", form->message);

// /* Special handling for GROUP: field if present, as different group
//    selections can make other fields disappear/reappear */
// if (form->authgroup_opt) {
// 	if (!authgroup)
// 		authgroup = saved_form_field(vpninfo, form->auth_id, form->authgroup_opt->form.name);
// 	if (!authgroup ||
// 	    match_choice_label(vpninfo, form->authgroup_opt, authgroup) != 0) {
// 		if (prompt_opt_select(vpninfo, form->authgroup_opt, &authgroup) < 0)
// 			goto err;
// 	}
// 	if (!authgroup_set) {
// 		authgroup_set = 1;
// 		return OC_FORM_RESULT_NEWGROUP;
// 	}
// }

// for (opt = form->opts; opt; opt = opt->next) {

// 	if (opt->flags & OC_FORM_OPT_IGNORE)
// 		continue;

// 	/* I haven't actually seen a non-authgroup dropdown in the wild, but
// 	   the Cisco clients do support them */
// 	if (opt->type == OC_FORM_OPT_SELECT) {
// 		struct oc_form_opt_select *select_opt = (void *)opt;
// 		char *opt_response;

// 		if (select_opt == form->authgroup_opt)
// 			continue;

// 		opt_response = saved_form_field(vpninfo, form->auth_id, select_opt->form.name);
// 		if (opt_response &&
// 		    match_choice_label(vpninfo, select_opt, opt_response) == 0) {
// 			free(opt_response);
// 			continue;
// 		}
// 		free(opt_response);
// 		if (prompt_opt_select(vpninfo, select_opt, NULL) < 0)
// 			goto err;
// 		empty = 0;

// 	} else if (opt->type == OC_FORM_OPT_TEXT) {
// 		if (username &&
// 		    !strncmp(opt->name, "user", 4)) {
// 			opt->_value = username;
// 			username = NULL;
// 		} else {
// 			opt->_value = saved_form_field(vpninfo, form->auth_id, opt->name);
// 			if (!opt->_value)
// 				opt->_value = prompt_for_input(opt->label, vpninfo, 0);
// 		}

// 		if (!opt->_value)
// 			goto err;
// 		empty = 0;

// 	} else if (opt->type == OC_FORM_OPT_PASSWORD) {
// 		if (password) {
// 			opt->_value = password;
// 			password = NULL;
// 		} else {
// 			opt->_value = saved_form_field(vpninfo, form->auth_id, opt->name);
// 			if (!opt->_value)
// 				opt->_value = prompt_for_input(opt->label, vpninfo, 1);
// 		}

// 		if (!opt->_value)
// 			goto err;
// 		empty = 0;
// 	} else if (opt->type == OC_FORM_OPT_TOKEN) {
// 		/* Nothing to do here, but if the tokencode is being
// 		 * automatically generated then don't treat it as an
// 		 * empty form for the purpose of loop avoidance. */
// 		empty = 0;
// 	}
// }

// /* prevent infinite loops if the authgroup requires certificate auth only */
// if (last_form_empty && empty)
// 	return OC_FORM_RESULT_CANCELLED;
// last_form_empty = empty;

	return OC_FORM_RESULT_OK;

 err:
	return OC_FORM_RESULT_ERR;
}

static void __attribute__ ((format(printf, 3, 4)))
write_progress(void *_vpninfo, int level, const char *fmt, ...)
{
// FILE *outf = level ? stdout : stderr;
// va_list args;
//
// if (cookieonly)
//     outf = stderr;
//
// if (verbose >= level) {
//     if (timestamp) {
//         char ts[64];
//         time_t t = time(NULL);
//         struct tm *tm = localtime(&t);
//
//         strftime(ts, 64, "[%Y-%m-%d %H:%M:%S] ", tm);
//         fprintf(outf, "%s", ts);
//     }
//     va_start(args, fmt);
//     vfprintf(outf, fmt, args);
//     va_end(args);
//     fflush(outf);
// }
}