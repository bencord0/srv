#include <stdio.h>     /* printf    */
#include <stdlib.h>    /* exit      */
#include <string.h>    /* memcpy    */
#include <resolv.h>    /* res_query */
#include <arpa/inet.h> /* inet_ntop */

#define NS_RECORD_MAXSIZE 4096

typedef struct __srv_rdata_t {
        short priority;
        short weight;
        short port;
        short extra;
} srv_rdata_t;

const char *rr_srv_ip(ns_rr, char*);
int rr_srv_port(ns_rr);

int main(int argc, char *argv[])
{
    char lookup[NS_RECORD_MAXSIZE];
    int l;
    ns_msg message;
    ns_rr rr_ans, rr_add;

    char *ipaddr;
    short port;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage:\n\t%s <service>\n", argv[0]);
        exit(1);
    }

    l = res_query(argv[1], ns_c_in, ns_t_srv, lookup, sizeof(lookup));
    if (l < 0) {
        fprintf(stderr, "SRV lookup failed. No services found.\n");
        printf("[]\n");
        exit(1);
    }

    ns_initparse(lookup, l, &message);
    l = ns_msg_count(message, ns_s_an);
    fprintf(stderr, "%d records found for %s.\n", l, argv[1]);
    
    printf("[\n");
    for(i = 0; i < l; i++) {
        printf(" {\n");
        ns_parserr(&message, ns_s_an, i, &rr_ans);
        ns_parserr(&message, ns_s_ar, i, &rr_add);

        rr_srv_ip(rr_add, ipaddr);
        port = rr_srv_port(rr_ans);

        printf("  \"address\": \"%s\",\n", ipaddr);
        printf("  \"port\": %d\n", port);

        if (i != l - 1)
            printf(" },\n");
        else
            printf(" }\n");
    }
    printf("]\n");

    return 0;
}

const char *rr_srv_ip(ns_rr rr, char* ipaddr)
{
    if (ns_rr_type(rr) == ns_t_a) {
        /* IPv4 */
        return inet_ntop(AF_INET, ns_rr_rdata(rr), ipaddr, INET_ADDRSTRLEN);
    } else if (ns_rr_type(rr) == ns_t_aaaa) {
        /* IPv6 */
        return inet_ntop(AF_INET6, ns_rr_rdata(rr), ipaddr, INET6_ADDRSTRLEN);
    }
    return 0;
}

int rr_srv_port(ns_rr rr)
{
    srv_rdata_t srv_rdata;
    memcpy(&srv_rdata, ns_rr_rdata(rr), sizeof(srv_rdata));
    return ntohs(srv_rdata.port);
}
