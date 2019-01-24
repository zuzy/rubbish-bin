#ifndef __HALO_HTTP_MLIST_H__
#define __HALO_HTTP_MLIST_H__

/** 
 * @brief  call media navi to search the local music
 * @note   the same as local mode of music player
 * @retval 
 */
int get_items();

/** 
 * @brief  collect the info of the music searched, and post to the hope server
 * @note   create a thread to collect the total music id3 info restore and post
 * @param  *mlist: target mlist open
 * @retval 
 */
int mlist_update(void *mlist);

#endif