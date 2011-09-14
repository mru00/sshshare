#include <string>

#include "shares.hxx"


using namespace std;

extern void create_share(string name) {

    string script = "mkdir -p ~/public_html/shares/" + name;



}
