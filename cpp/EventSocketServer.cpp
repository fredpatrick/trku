/*
 * ============================================================================
 *                   The XyloComp Software License, Version 1.1
 * ============================================================================
 * 
 *    Copyright (C) 2016 XyloComp Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of  source code must  retain the above copyright  notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The end-user documentation included with the redistribution, if any, must
 *    include  the following  acknowledgment:  "This product includes  software
 *    developed  by  XyloComp Inc.  (http://www.xylocomp.com/)." Alternately, 
 *    this  acknowledgment may  appear in the software itself,  if
 *    and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The name "XyloComp" must not be used to endorse  or promote  products 
 *    derived  from this  software without  prior written permission. 
 *    For written permission, please contact fred.patrick@xylocomp.com.
 * 
 * 5. Products  derived from this software may not  be called "XyloComp", 
 *    nor may "XyloComp" appear  in their name,  without prior written 
 *    permission  of Fred Patrick
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL
 * XYLOCOMP INC. OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
 * DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
 * ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
 * (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "EventSocketServer.h"
#include "EventBuffer.h"
#include "event_device_error.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

trk::EventSocketServer::
EventSocketServer(int portno)
{
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if ( listen_fd_ == -1) {
        ::perror("socket");
        // need to throw exception here
    }
    std::cout << "EventSocketServer.ctor, listen_fd = " << listen_fd_ << std::endl;
    struct sockaddr_in      serv_addr;
    ::memset(&serv_addr, 0, sizeof(serv_addr) );
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);
    serv_addr.sin_port          = htons(portno);
    int ierr = ::bind(listen_fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr) );
    if ( ierr == -1 ) {
        ::perror( "bind");
        throw event_device_error("EventSocketServer-ctor socket");
    }
    listen(listen_fd_, 10);
    socket_fd_ = accept(listen_fd_, (struct sockaddr*)NULL, NULL);
    if ( socket_fd_ == -1 ) {
        ::perror("accept");
        throw event_device_error("EventSocketServer-ctor socket");
    }
}

trk::EventSocketServer::
~EventSocketServer()
{
    close ( listen_fd_);
}

int
trk::EventSocketServer::
write(EventBuffer* ebfr)
{
    int bfrlen = ebfr->bfrlen();
    char* bfr  = ebfr->bfr();
    char ctag[4];
    ::memcpy(ctag, bfr, 4);
    std::string tag = ctag;
    int nl = ::write( socket_fd_, &bfrlen, sizeof(int) );
    if ( nl != sizeof(int) ) {
        std::ostringstream ost;
        ost << "EventSocketServer:write, error writing bfrlen, ns = " << nl;
        throw event_device_error(ost.str() );
    }
    int ns = ::write( socket_fd_, bfr, bfrlen);
    if ( ns != bfrlen) {
        std::ostringstream ost;
        ost << "EventSocketServer:write, error writing bfr, ns = " << ns;
        throw event_device_error(ost.str() );
    }
    return ns;
}

trk::EventBuffer* 
trk::EventSocketServer::
read()
{
    int bfrlen;
    int nl = ::read(socket_fd_, &bfrlen, sizeof(int) );
    if ( nl != sizeof(int) ) {
        std::ostringstream ost;
        ost << "EventSocketServer:read, error reading bfrlen, nl = " << nl;
        throw event_device_error(ost.str() );
    }
    char* bfr = new char[bfrlen];
    int ns = ::read( socket_fd_, bfr, bfrlen);
    if ( ns != sizeof(int) ) {
        std::ostringstream ost;
        ost << "EventSocketServer:read, error reading bfrlen, ns = " << ns;
        throw event_device_error(ost.str() );
    }

    char ctag[4];
    ::memcpy(ctag, bfr, 4);
    std::string tag = ctag;
    EventBuffer* ebfr = new EventBuffer(bfrlen, bfr); 
    delete[] bfr;
    return ebfr;
}

