#ifndef _SCP_H
#define _SCP_H


extern int get_file(const char* filename, void (*on_progress)(int), void (*on_status_change)(int, const char*));

#endif
