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

#include "qfasttrackproctl.h"

static int
compare(const void *_pa, const void *_pb)
{
	const struct FTPEntry *pa = *(struct FTPEntry **)_pa;
	const struct FTPEntry *pb = *(struct FTPEntry **)_pb;
	int ret;

	ret = pa->unit - pb->unit;
	if (ret != 0)
		return (ret);

	ret = pa->type - pb->type;
	if (ret != 0)
		return (ret);

	ret = pa->subtype - pb->subtype;
	if (ret != 0)
		return (ret);

	ret = pa->x_coord - pb->x_coord;
	if (ret != 0)
		return (ret);

	ret = pa->y_coord - pb->y_coord;
	if (ret != 0)
		return (ret);

	return (0);
}

FTPMainWindow :: FTPMainWindow()
{
	FTPEntry *ptr;
	int num;

	TAILQ_INIT(&head);

#if 0
	foreach_line()
		parse(line);
#endif

	num = 0;
	TAILQ_FOREACH(ptr, &head, entry) {
		num++;
		if (num <= 0)
			errx(EX_SOFTWARE, "Out of memory");
	}

	pp_entry = (FTPEntry **)malloc(sizeof(void *) * num);
	if (pp_entry == 0)
		errx(EX_SOFTWARE, "Out of memory");

	num = 0;
	TAILQ_FOREACH(ptr, &head, entry)
		pp_entry[num++] = ptr;

	qsort(pp_entry, num, sizeof(void *), compare);

#if 0
	layout graphics();
#endif

        setWindowTitle(QString("FastTrack Pro Control Panel"));
        setWindowIcon(QIcon(QString(":/qfasttrackproctl.png")));
}

FTPMainWindow :: ~FTPMainWindow()
{
}

void
FTPMainWindow :: parse(char *line)
{
	struct FTPEntry *pentry;
	char *ptr;
	char sub[strlen(line) + 1];
	int unit = -1;
	int num = -1;
	int type;
	int subtype;

	sub[0] = 0;
	unit = -1;
	type = -1;
	subtype = -1;
	num = -1;

	if (sscanf(line, "dev.pcm.%d.mixer.effect_send_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_SEND;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_ret_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_RET;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_fb_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_FB;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_dur_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_DUR;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_vol_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_VOL;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_SW;
	} else if (sscanf(line, "dev.pcm.%d.mixer.mix_rec_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_SEND_ANALOG;
	} else if (sscanf(line, "dev.pcm.%d.mixer.mix_play_%d.%s:",
		   &unit, &num, sub) == 3) {
		type = FTP_EFFECT_SEND_DIGITAL;
	}

	if (strcmp(sub, "desc") == 0)
		subtype = FTP_SUB_DESC;
	else if (strcmp(sub, "val") == 0)
		subtype = FTP_SUB_VAL;
	else if (strcmp(sub, "min") == 0)
		subtype = FTP_SUB_MIN;
	else if (strcmp(sub, "max") == 0)
		subtype = FTP_SUB_MAX;

	ptr = strchr(line, ':');
	if (ptr == 0)
		return;
	*ptr = 0;
	ptr++;
	while (*ptr == ' ')
		ptr++;

	pentry = (struct FTPEntry *)malloc(sizeof(*pentry));
	pentry->path = line;
	pentry->value = ptr;
	pentry->unit = unit;
	pentry->type = type;
	pentry->subtype = subtype;

	switch (type) {
	case FTP_MIXER_ANALOG:
		if (sscanf(ptr, "AIn%d - Out%d Record Volume",
		     &pentry->x_coord, &pentry->y_coord) != 2) {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	case FTP_MIXER_DIGITAL:
		if (sscanf(ptr, "DIn%d - Out%d Playback Volume",
		     &pentry->x_coord, &pentry->y_coord) != 2) {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	default:
		pentry->x_coord = 0;
		pentry->y_coord = 0;
		break;
	}

	TAILQ_INSERT_TAIL(&head, pentry, entry);
}

int
main(int argc, char **argv)
{
	QApplication app(argc, argv);

	FTPMainWindow mw;

	mw.show();

	return (app.exec());
}
