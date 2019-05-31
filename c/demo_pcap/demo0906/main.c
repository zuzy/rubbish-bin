#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <alloca.h>
#include <pcap.h>


#if 0
int main(int argc, char *argv[])
{
    pcap_t *handle;		/* Session handle */
    // char dev[] = "rl0";		/* Device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    struct bpf_program fp;		/* The compiled filter expression */
    char filter_exp[] = "port 23";	/* The filter expression */
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */

    // char *dev, *errbuf;
    // errbuf = (char *)alloca(PCAP_ERRBUF_SIZE);
    char *dev;
    
    if((dev = pcap_lookupdev(errbuf)) == NULL) {
        fprintf(stderr, "Error cannot find default dev: %s\n", errbuf);
        return 2;
    }
    printf("find device %s\n", dev);

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", dev);
        net = 0;
        mask = 0;
    }
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return(2);
    }
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }


    pcap_t *pcap_handle;

    return 0;
}

#else

int main(int argc, char *argv[])
{
    pcap_t *handle;                /* Session handle */
    char *dev;                     /* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE]; /* Error string */
    struct bpf_program fp;         /* The compiled filter */
    // char filter_exp[] = "port 23"; /* The filter expression */
    char filter_exp[] = "port 445"; /* The filter expression */
    bpf_u_int32 mask;              /* Our netmask */
    bpf_u_int32 net;               /* Our IP */
    struct pcap_pkthdr header;     /* The header that pcap gives us */
    const u_char *packet;          /* The actual packet */

    /* Define the device */
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL)
    {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return (2);
    }
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 3000, errbuf);
    if (handle == NULL)
    {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return (2);
    }
    /* Compile and apply the filter */
    // if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
    // {
    //     fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    //     return (2);
    // }
    // if (pcap_setfilter(handle, &fp) == -1)
    // {
    //     fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    //     return (2);
    // }
    /* Grab a packet */
    packet = pcap_next(handle, &header);
    // int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
    /* Print its length */
    printf("Jacked a packet with length of [%d]\n", header.len);
    /* And close the session */
    pcap_close(handle);
    return (0);
}
#endif