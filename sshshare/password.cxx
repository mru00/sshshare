#include <cstdlib>   // for rand()
#include <cctype>    // for isalnum()
#include <algorithm> // for back_inserter
#include <string>


#include "password.hxx"


// http://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
// http://c-faq.com/lib/randrange.html


static char
rand_alnum()
{
    char c;
    while (!std::isalnum(c = static_cast<char>(std::rand())))
        ;
    return c;
}


static std::string
rand_alnum_str (std::string::size_type sz)
{
    std::string s;
    s.reserve  (sz);
    generate_n (std::back_inserter(s), sz, rand_alnum);
    return s;
}



std::string Password::generate(int length) {
    return rand_alnum_str(length);
}


