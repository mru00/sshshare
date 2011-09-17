#ifndef SHARES_H
#define SHARES_H

#include "sshshare.hxx"

extern void create_share(std::string name, const users_t::user_sequence& users);
extern void delete_share(const share_t& share, bool keep_data);
#endif // SHARES_H
