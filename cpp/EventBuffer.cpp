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

#include "EventBuffer.h"
#include "enum_out_of_range.h"
#include <string.h>

trk::EventBuffer::
EventBuffer(const std::string& tag)
{
    tag_    = tag;
    ::memcpy( bfr_, tag_.c_str(), tag_.length() +1 );
    bfrndx_ = tag_.length() + 1;;
}

trk::EventBuffer::
EventBuffer(int                bfrlen,
            const char*        bfr)
{
    bfrlen_ = bfrlen;
    ::memcpy( bfr_, bfr, bfrlen_);
    bfrndx_ = 0;
    ::memcpy( ctag_, &bfr_[bfrndx_], sizeof(ctag_) );
    tag_    = ctag_;
    bfrndx_ += sizeof(ctag_);
}

trk::EventBuffer::
~EventBuffer()
{
}

void
trk::EventBuffer::
reset()
{
    bfrndx_ = sizeof(ctag_);;
}

std::string
trk::EventBuffer::
tag()
{
    return tag_;;
}

int
trk::EventBuffer::
bfrlen()
{
    return bfrlen_;
}

char*
trk::EventBuffer::
bfr()
{
    return bfr_;
}

void
trk::EventBuffer::
strdat(const std::string& sdat)
{
    int ls = sdat.length() + 1;
    ::memcpy(&bfr_[bfrndx_], sdat.c_str(), ls);
    bfrndx_ +=  ls;
    bfrlen_ = bfrndx_;
}

std::string
trk::EventBuffer::
strdat()
{
    int ls;
    std::string s = &bfr_[bfrndx_];
    bfrndx_ += (s.length() + 1);
    return s;
}

void 
trk::EventBuffer::
intdat(int i)
{
    ::memcpy(&bfr_[bfrndx_], &i, sizeof(int) );
    bfrndx_ += sizeof(int);
    bfrlen_ = bfrndx_;
}

int
trk::EventBuffer::
intdat()
{
    int i;
    ::memcpy(&i, &bfr_[bfrndx_], sizeof(int) );
    bfrndx_ += sizeof(double);
    return i;
}
    
void
trk::EventBuffer::
dbldat(double d)
{
    ::memcpy(&bfr_[bfrndx_], &d, sizeof(double) );
    bfrndx_ += sizeof(double);
    bfrlen_ = bfrndx_;
}

double
trk::EventBuffer::
dbldat()
{
    double d;
    ::memcpy(&d, &bfr_[bfrndx_], sizeof(double) );
    bfrndx_ += sizeof(double);
    return d;
}

trk::BLK_STATE
trk::EventBuffer::
blkstate()
{
    int b;
    ::memcpy(&b, &bfr_[bfrndx_], sizeof(int) );
    bfrndx_+= sizeof(int);
    if (      b == 0 ) return GO;
    else if ( b == 1 ) return STOP;
    else {
        throw enum_out_of_range ("blkstate");
    }
}

trk::TRK_STATE
trk::EventBuffer::
trkstate()
{
    int b;
    ::memcpy(&b, &bfr_[bfrndx_], sizeof(int) );
    bfrndx_+= sizeof(int);
    if (      b == 0 ) return BUSY;
    else if ( b == 1 ) return IDLE;
    else {
        throw enum_out_of_range ("trkstate");
    }
}

trk::SW_DIRECTION
trk::EventBuffer::
swdirec()
{
    int b;
    ::memcpy(&b, &bfr_[bfrndx_], sizeof(int) );
    bfrndx_+= sizeof(int);
    if (      b == 0 ) return THRU;
    else if ( b == 1 ) return OUT;
    else if ( b == 2 ) return NOVAL;
    else {
        throw enum_out_of_range ("swdirec");
    }
}

