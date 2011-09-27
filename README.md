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

The schema definitions (*.xsd) are downloaded from a hardcoded address when the xml files are loaded.


Installation & Setup
--------------------

download & extract the latest `sshshare-x.x.x.x.tar.gz`.
execute 


     ./configure

install missing dependencies, as ./configure requires.
to cut it short:
   
     sudo apt-get install libboost-regex-dev libboost-filesystem-dev libxerces-c-dev xsdcxx

     make

After running it the first time, an example configuration file will be 
created at `$HOME/.sshshare\_config.xml`

Kill, yes, kill the current runnning instance, and edit the configuration
files to your needs (username and server are required)


Screenshots
-----------

![screenshot](https://github.com/mru00/sshshare/raw/master/doc/screenshot1.png)
![screenshot](https://github.com/mru00/sshshare/raw/master/doc/screenshot2.png)
![screenshot](https://github.com/mru00/sshshare/raw/master/doc/screenshot3.png)


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

 * boost::filesystem


The files sshshare.hxx and sshshare.cxx are generated with the xsdcxx tool. The tool can be started with the
`makexsd.sh` command.

    sudo apt-get install xsdcxx libxerces-c-dev


