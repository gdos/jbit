/*** File generated by inc2h.pl - DO NOT EDIT ***/

/*                                                                         */
/*  xv65.inc                                                               */
/*                                                                         */

/*                                                                         */
/*  Copyright (C) 2012-2013  Emanuele Fornara                              */
/*  All rights reserved.                                                   */
/*                                                                         */
/*  Redistribution and use in source and binary forms, with or without     */
/*  modification, are permitted provided that the following conditions     */
/*  are met:                                                               */
/*   * Redistributions of source code must retain the above copyright      */
/*     notice, this list of conditions and the following disclaimer.       */
/*   * Redistributions in binary form must reproduce the above copyright   */
/*     notice, this list of conditions and the following disclaimer in     */
/*     the documentation and/or other materials provided with the          */
/*     distribution.                                                       */
/*                                                                         */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    */
/*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS      */
/*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE         */
/*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,    */
/*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,   */
/*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS  */
/*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED     */
/*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, */
/*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT     */
/*  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF     */
/*  SUCH DAMAGE.                                                           */
/*                                                                         */

#define PUTCHAR 0x0200

#define REQPUT 0x0201
#define REQEND 0x0202
#define REQRES 0x0203

#define REQPTRLO 0x0204
#define REQPTRHI 0x0205

#define REQ_DEBUG 0

#define REQ_SYS_fork 1
#define REQ_SYS_exit 2
#define REQ_SYS_wait 3
#define REQ_SYS_pipe 4
#define REQ_SYS_read 5
#define REQ_SYS_kill 6
#define REQ_SYS_exec 7
#define REQ_SYS_fstat 8
#define REQ_SYS_chdir 9
#define REQ_SYS_dup 10
#define REQ_SYS_getpid 11
#define REQ_SYS_sbrk 12
#define REQ_SYS_sleep 13
#define REQ_SYS_uptime 14
#define REQ_SYS_open 15
#define REQ_SYS_write 16
#define REQ_SYS_mknod 17
#define REQ_SYS_unlink 18
#define REQ_SYS_link 19
#define REQ_SYS_mkdir 20
#define REQ_SYS_close 21
