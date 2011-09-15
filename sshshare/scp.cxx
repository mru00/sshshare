#include <pty.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <string>
#include <vector>
#include <iostream>


#include "process.hxx"
#include "scp.hxx"
#include "config.hxx"


using namespace std;

// http://cwshep.blogspot.com/2009/06/showing-scp-progress-using-zenity.html
// http://rmathew.blogspot.com/2006/09/terminal-sickness.html


