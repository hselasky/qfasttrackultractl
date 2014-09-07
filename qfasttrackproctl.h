/*-
 * Copyright (c) 2014 Hans Petter Selasky. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _QFASTTRACKPROCTL_H_
#define	_QFASTTRACKPROCTL_H_

#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QWidget>
#include <QScrollArea>
#include <QPainter>
#include <QMouseEvent>
#include <QIcon>

#include <sys/queue.h>
#include <err.h>
#include <sysexits.h>

enum {
	FTP_MIXER_DIGITAL,	/* playback */
	FTP_MIXER_ANALOG,	/* record */
	FTP_EFFECT_SEND,
	FTP_EFFECT_RET,
	FTP_EFFECT_FB,
	FTP_EFFECT_DUR,
	FTP_EFFECT_VOL,
	FTP_EFFECT_SW,
	FTP_EFFECT_SEND_DIGITAL,
	FTP_EFFECT_SEND_ANALOG,
	FTP_MAX
};

enum {
	FTP_SUB_VAL,
	FTP_SUB_MIN,
	FTP_SUB_MAX,
	FTP_SUB_DESC,
};

struct FTPEntry {
	TAILQ_ENTRY(FTPEntry) entry;
	char *path;
	char *value;
	int unit;
	int type;
	int subtype;
	int x_coord;
	int y_coord;
};

typedef TAILQ_HEAD(,FTPEntry) FTPHead_T;

class FTPMainWindow : public QScrollArea
{
	Q_OBJECT;
public:
	FTPMainWindow();
	~FTPMainWindow();

	void parse(char *);

	FTPHead_T head;
	FTPEntry **pp_entry;

public slots:

};

#endif		/* _QFASTTRACKPROCTL_H_ */
