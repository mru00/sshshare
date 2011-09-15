SSHSHARE
========

mru, 2011-09-15


sshshare allows simple management for sharing file via ~/public_html.

If you maintain a webserver with `~/public_html` and would like to make sharing files easier, this is the right thing for you.

The software maintains the `.htaccess` and `htpasswd` and creates the necessary directories.

Information about the shares is stored in an XML file which resides on the server.
The file is downloaded on application startup, and uploaded when the application exits.
With that file it is easy to manage the shares from different computers.

The current implementation is very simple; error checking is almost not available. Everything is hardcoded.


For ssh communications, it is assumed that you use public key authentication, because no passwords can be entered.


Screenshots
-----------

![screenshot](https://github.com/mru00/sshshare/raw/master/doc/screenshot1.png)


File organisation
-----------------

    ~/public_html/shares/$SHARENAME
    ~/public_html/shares/$SHARENAME/.htaccess
    ~/shares/sharedata.xml
    ~/shares/htpasswd.$SHARENAME


Development
-----------

 * Ubuntu
 * C++ / Code::Blocks

 * gtk
 * libutil

 * libxerces

 * xsdcxx http://www.codesynthesis.com/projects/xsd/documentation/cxx/tree/guide/
