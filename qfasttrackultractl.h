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
#include <QGroupBox>
#include <QProcess>
#include <QMessageBox>

#include <sys/queue.h>
#include <err.h>
#include <sysexits.h>

enum {
	FTU_MIXER_DIGITAL,	/* playback */
	FTU_MIXER_ANALOG,	/* record */
	FTU_EFFECT_SEND,
	FTU_EFFECT_RET,
	FTU_EFFECT_FB,
	FTU_EFFECT_DUR,
	FTU_EFFECT_VOL,
	FTU_EFFECT_SW
};

enum {
	FTU_SUB_VAL,
	FTU_SUB_MIN,
	FTU_SUB_MAX,
	FTU_SUB_DESC,
};

#define	FTU_MIN_COORD 1
#define	FTU_MAX_COORD 64

struct FTUEntry;

class FTUMainWindow;
class FTUGroupBox;

typedef TAILQ_HEAD(,FTUEntry) FTUHead_T;

struct FTUEntry {
	TAILQ_ENTRY(FTUEntry) entry;
	FTUMainWindow *parent;
	const char *path;
	const char *value;
	int unit;
	int type;
	int subtype;
	int x_coord;
	int y_coord;
};

class FTUGridLayout : public QWidget, public QGridLayout
{
public:
        FTUGridLayout();
        ~FTUGridLayout();
};

class FTUGroupBox : public QGroupBox, public QGridLayout
{
public:
	FTUGroupBox(const QString &);
	~FTUGroupBox();
};

class FTUMainWindow : public QScrollArea
{
	Q_OBJECT;
public:
	FTUMainWindow(char *);
	~FTUMainWindow();

	FTUHead_T head;
	int	pp_entries;
	struct FTUEntry **pp_entry;
	FTUGridLayout gl_main;

	void parse(char *);
	FTUEntry *find(int, int, int, const char *);
	void getRange(const struct FTUEntry *, int *, int *, int *);

public slots:
	void handle_value_changed(int,void *);
};

#endif		/* _QFASTTRACKPROCTL_H_ */
