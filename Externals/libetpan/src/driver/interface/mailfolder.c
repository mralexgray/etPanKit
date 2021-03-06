/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001, 2005 - DINH Viet Hoa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the libEtPan! project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Id: mailfolder.c,v 1.6 2006/05/22 13:39:40 hoa Exp $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "mailfolder.h"

#include <string.h>

#include "maildriver.h"
#include "mailstorage.h"

LIBETPAN_EXPORT
int mailfolder_noop(struct mailfolder * folder)
{
  return mailsession_noop(folder->fld_session);
}

LIBETPAN_EXPORT
int mailfolder_check(struct mailfolder * folder)
{
  return mailsession_check_folder(folder->fld_session);
}

LIBETPAN_EXPORT
int mailfolder_expunge(struct mailfolder * folder)
{
  return mailsession_expunge_folder(folder->fld_session);
}

LIBETPAN_EXPORT
int mailfolder_status(struct mailfolder * folder,
    uint32_t * result_messages, uint32_t * result_recent,
    uint32_t * result_unseen)
{
  return mailsession_status_folder(folder->fld_session,
      folder->fld_pathname, result_messages,
      result_recent, result_unseen);
}

LIBETPAN_EXPORT
int mailfolder_append_message(struct mailfolder * folder,
    char * message, size_t size)
{
  return mailsession_append_message(folder->fld_session, message, size);
}

LIBETPAN_EXPORT
int mailfolder_append_message_flags(struct mailfolder * folder,
    char * message, size_t size, struct mail_flags * flags)
{
  return mailsession_append_message_flags(folder->fld_session, message,
      size, flags);
}

LIBETPAN_EXPORT
int mailfolder_get_messages_list(struct mailfolder * folder,
    struct mailmessage_list ** result)
{
  int r;
  struct mailmessage_list * msg_list;
  unsigned int i;
  struct mailstorage * storage;
  
  /* workaround for POP3 case - begin */
  storage = folder->fld_storage;
  if (strcmp(storage->sto_driver->sto_name, "pop3") == 0) {
    mailstorage_disconnect(storage);
    r = mailstorage_connect(storage);
    if (r != MAIL_NO_ERROR)
      return r;
    r = mailfolder_connect(folder);
    if (r != MAIL_NO_ERROR)
      return r;
  }
  /* workaround for POP3 case - begin */
  
  r = mailsession_get_messages_list(folder->fld_session, &msg_list);
  if (r != MAIL_NO_ERROR)
    return r;
  
  for(i = 0 ; i < carray_count(msg_list->msg_tab) ; i ++) {
    mailmessage * msg;
    
    msg = carray_get(msg_list->msg_tab, i);
    msg->msg_folder = folder;
  }
  
  * result = msg_list;
  
  return MAIL_NO_ERROR;
}

LIBETPAN_EXPORT
int mailfolder_get_envelopes_list(struct mailfolder * folder,
    struct mailmessage_list * result)
{
  return mailsession_get_envelopes_list(folder->fld_session, result);
}

LIBETPAN_EXPORT
int mailfolder_get_message(struct mailfolder * folder,
    uint32_t num, mailmessage ** result)
{
  mailmessage * msg;
  int r;
  
  r = mailsession_get_message(folder->fld_session, num, &msg);
  if (r != MAIL_NO_ERROR)
    return r;
  
  msg->msg_folder = folder;
  
  * result = msg;
  
  return MAIL_NO_ERROR;
}

LIBETPAN_EXPORT
int mailfolder_get_message_by_uid(struct mailfolder * folder,
    const char * uid, mailmessage ** result)
{
  mailmessage * msg;
  int r;
  
  r = mailsession_get_message_by_uid(folder->fld_session, uid, &msg);
  if (r != MAIL_NO_ERROR)
    return r;
  
  msg->msg_folder = folder;
  
  * result = msg;
  
  return MAIL_NO_ERROR;
}
