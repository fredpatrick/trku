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

#include "switchevent.h"
#include "packetbuffer.h"
#include "eventdevice.h"

#include <iostream>
#include <unistd.h>

trk::SwitchEvent::
SwitchEvent(PacketBuffer* ebfr)
{
    tag_ = "SW ";
    ebfr_ =ebfr;
    tm_event_       = ebfr_->dbldat();
    event_seq_n_    = ebfr_->intdat();
    switch_name_    = ebfr_->strdat();
    sw_num_         = ebfr_->intdat();
    state_          = swstate( ebfr_->intdat() );
}

trk::SwitchEvent::
SwitchEvent(double             tm_event,
            const std::string& switch_name,
            int                sw_num,
            SW_DIRECTION       state)
{
    tag_          = "SW ";
    tm_event_     = tm_event;
    switch_name_  = switch_name;
    sw_num_       = sw_num;
    state_        = state;
    event_seq_n_++;
    ebfr_ = new PacketBuffer(tag_);
    ebfr_->dbldat(tm_event_);
    ebfr_->intdat(event_seq_n_);
    ebfr_->strdat(switch_name_);
    ebfr_->intdat(sw_num_);
    ebfr_->intdat(state_);
}

trk::SwitchEvent::
~SwitchEvent()
{
    delete ebfr_;
}

int
trk::SwitchEvent::
write_event(EventDevice* efd)
{
    int ns = efd->write(ebfr_);
    return ns;
}

void
trk::SwitchEvent::
print(int ntab)
{
    std::cout.width(ntab);
    std::cout << "| ";
    std::cout << "SwitchEvent::" << 
                                 switch_name_ << " - " << 
                                 sw_num_ << " - " << 
                                 state_ <<  " - " << 
                                 event_seq_n_ << " - " << tm_event_ << std::endl;
}

std::string
trk::SwitchEvent::
switch_name()
{
    return switch_name_;
}

int
trk::SwitchEvent::
sw_num()
{
    return sw_num_;
}

trk::SW_DIRECTION
trk::SwitchEvent::
state()
{
    return state_;
}
