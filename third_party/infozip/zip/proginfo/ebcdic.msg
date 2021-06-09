From dima@mitrah.ru  Mon Nov 10 02:25:38 2003
Return-Path: <dima@mitrah.ru>
Received: from b.mx.sonic.net (eth0.b.mx.sonic.net [209.204.159.4])
	by eth0.a.lds.sonic.net (8.12.10/8.12.9) with ESMTP id hAAAPccT025257
	for <roelofs@lds.sonic.net>; Mon, 10 Nov 2003 02:25:38 -0800
Received: from icicle.pobox.com (icicle.pobox.com [207.8.214.2])
	by b.mx.sonic.net (8.12.10/8.12.7) with ESMTP id hAAAPar9007141
	for <roelofs@sonic.net>; Mon, 10 Nov 2003 02:25:37 -0800
Received: from icicle.pobox.com (localhost[127.0.0.1])
	by icicle.pobox.com (Postfix) with ESMTP id 9BA347A96B
	for <roelofs@sonic.net>; Sat,  8 Nov 2003 06:15:13 -0500 (EST)
Delivered-To: newt@pobox.com
Received: from mail.ropnet.ru (mail.ropnet.ru[212.42.37.90])
	by icicle.pobox.com (Postfix) with ESMTP id A96817A8F7
	for <newt@pobox.com>; Sat,  8 Nov 2003 06:15:04 -0500 (EST)
Received: from d34-67.ropnet.ru (d34-67.ropnet.ru [212.42.34.67])
	by mail.ropnet.ru (8.11.7/8.11.7) with ESMTP id hA8BEjF76200
	for <newt@pobox.com>; Sat, 8 Nov 2003 14:14:46 +0300 (MSK)
Resent-Date: Sat, 8 Nov 2003 14:14:46 +0300 (MSK)
Resent-Message-Id: <200311081114.hA8BEjF76200@mail.ropnet.ru>
Date: Sat, 8 Nov 2003 14:18:18 +0300
From: Dmitri Koulikov <dima@mitrah.ru>
X-Mailer: The Bat! (v1.62r) Personal
Reply-To: Dmitri Koulikov <dima@mitrah.ru>
X-Priority: 3 (Normal)
Message-ID: <815640011.20031108141818@mitrah.ru>
To: newt@pobox.com
Subject: unzip and zip lack NLS - 2
Resent-From: Dmitri Koulikov <dima@mitrah.ru>
MIME-Version: 1.0
Content-Type: multipart/mixed; boundary="----------EB1581C42AB86662"
Status: R

------------EB1581C42AB86662
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 7bit

Hello Greg Roelofs,

   By mistake I sent you wrong version of ebcdic.h. Now it is as it
have to.
   Additionally I found that zip works with Russian filenames good.
But fails to process -D switch. So I have to chahge zipfile.c. Most
probably this is not good but it works.

-- 
Best regards,
 Dmitri 

mailto:dima@mitrah.ru
------------EB1581C42AB86662
Content-Type: application/octet-stream; name="ebcdic.h"
Content-Transfer-Encoding: base64
Content-Disposition: attachment; filename="ebcdic.h"

------------EB1581C42AB86662
Content-Type: application/octet-stream; name="zipfile.c"
Content-Transfer-Encoding: base64
Content-Disposition: attachment; filename="zipfile.c"

------------EB1581C42AB86662--


