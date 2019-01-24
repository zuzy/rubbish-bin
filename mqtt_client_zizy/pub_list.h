#ifndef __PUBLISH_LIST_H__
#define __PUBLISH_LIST_H__

#include "mqttclient.h"

int delete_pub(uint16_t id);
void free_publist();
void free_pub(pub_t *p);
pub_t *parse_pub_blob(struct st_data *data);
int handle_from_list(pub_t *pub);
int handle_publist(int fd);

#endif