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

#include "qfasttrackultractl.h"
#include "qfasttrackultractl_volume.h"

static int
compare(const void *_pa, const void *_pb)
{
	const struct FTUEntry *pa = *(struct FTUEntry **)_pa;
	const struct FTUEntry *pb = *(struct FTUEntry **)_pb;
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

	ret = strcmp(pa->path, pb->path);
	return (ret);
}

FTUMainWindow :: FTUMainWindow(char *line)
{
	FTUEntry *ptr;
	FTUVolume *spn;
	FTUGroupBox *gb;
	char *next;
	int num;
	int x;
	int y;

	TAILQ_INIT(&head);
	pp_entries = 0;
	pp_entry = 0;

	while (line != 0) {
		next = strchr(line, '\n');
		if (next != 0)
			*next++ = 0;
		parse(line);
		line = next;
	}

	num = 0;
	TAILQ_FOREACH(ptr, &head, entry) {
		num++;
		if (num <= 0)
			errx(EX_SOFTWARE, "Out of memory");
	}

	if (num == 0) {
		QMessageBox box;

		box.setText(tr("No supported devices found"));
		box.setStandardButtons(QMessageBox::Ok);
		box.setIcon(QMessageBox::Critical);
		box.setWindowIcon(QIcon(QString(":/qfasttrackultractl.png")));
		box.exec();
	}

	pp_entry = (FTUEntry **)malloc(sizeof(void *) * num);
	if (pp_entry == 0)
		errx(EX_SOFTWARE, "Out of memory");
	pp_entries = num;

	num = 0;
	TAILQ_FOREACH(ptr, &head, entry)
		pp_entry[num++] = ptr;

	qsort(pp_entry, num, sizeof(void *), compare);

	num = 0;
	for (x = 0; x != pp_entries; ) {
		for (y = x + 1; y != pp_entries; y++) {
			if (pp_entry[x]->unit != pp_entry[y]->unit ||
			    pp_entry[x]->type != pp_entry[y]->type)
				break;
		}
		int type = pp_entry[x]->type;

		switch (type) {
		case FTU_MIXER_DIGITAL:
			gb = new FTUGroupBox(QString("PCM%1 - Digital Mixer Controls").arg(pp_entry[x]->unit));
			break;
		case FTU_MIXER_ANALOG:
			gb = new FTUGroupBox(QString("PCM%1 - Analog Mixer Controls").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_SEND:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Send").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_RET:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Return").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_FB:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Feedback").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_DUR:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Duration").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_VOL:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Volume").arg(pp_entry[x]->unit));
			break;
		case FTU_EFFECT_SW:
			gb = new FTUGroupBox(QString("PCM%1 - Effect Number").arg(pp_entry[x]->unit));
			break;
		default:
			gb = 0;
			break;
		}

		if (gb == 0) {
			x = y;
			continue;
		}
		int coord_x_max = 0;
		int coord_y_max = 0;
		int min, max, val;

		for (; x != y; x++) {
			if (pp_entry[x]->subtype != FTU_SUB_DESC)
				continue;
			if (coord_x_max < pp_entry[x]->x_coord)
				coord_x_max = pp_entry[x]->x_coord;
			if (coord_y_max < pp_entry[x]->y_coord)
				coord_y_max = pp_entry[x]->y_coord;

			switch (type) {
			case FTU_EFFECT_SW:
				spn = new FTUVolume(0, pp_entry[x]);
				getRange(pp_entry[x], &min, &max, &val);
				spn->setRange(min, max, min + 1);
				spn->setValue(val);
				gb->addWidget(spn, pp_entry[x]->x_coord, pp_entry[x]->y_coord, 1, 1);
				connect(spn, SIGNAL(valueChanged(int,void *)), this, SLOT(handle_value_changed(int,void *)));
				break;
			default:
				spn = new FTUVolume(0, pp_entry[x]);
				getRange(pp_entry[x], &min, &max, &val);
				spn->setRange(min, max, ((max - min) / 2) + min);
				spn->setValue(val);
				gb->addWidget(spn, pp_entry[x]->x_coord, pp_entry[x]->y_coord, 1, 1);
				connect(spn, SIGNAL(valueChanged(int,void *)), this, SLOT(handle_value_changed(int,void *)));
				break;
			}
		}
		for (val = 1; val <= coord_x_max; val++) {
			switch (type) {
			case FTU_MIXER_DIGITAL:
				gb->addWidget(new QLabel(QString("DIn%1").arg(val)), val, 0, 1, 1);
				break;
			case FTU_MIXER_ANALOG:
				gb->addWidget(new QLabel(QString("AIn%1").arg(val)), val, 0, 1, 1);
				break;
			case FTU_EFFECT_SEND:
				gb->addWidget(new QLabel(QString("XIn%1").arg(val)), val, 0, 1, 1);
				break;
			default:
				break;
			}
		}
		for (val = 1; val <= coord_y_max; val++) {
			switch (type) {
			case FTU_MIXER_DIGITAL:
			case FTU_MIXER_ANALOG:
				gb->addWidget(new QLabel(QString("Out%1").arg(val)), 0, val, 1, 1);
				break;
			case FTU_EFFECT_RET:
				gb->addWidget(new QLabel(QString("Ret%1").arg(val)), 0, val, 1, 1);
				break;
			case FTU_EFFECT_SEND:
				gb->addWidget(new QLabel((val == 2) ? QString("Analog") : QString("Digital")), 0, val, 1, 1);
				break;
			default:
				break;
			}
		}
		gl_main.addWidget(gb, num++, 0, 1, 1);	
	}
        setWindowTitle(QString("FastTrackUltra Control Panel"));
        setWindowIcon(QIcon(QString(":/qfasttrackultractl.png")));
	setWidget(&gl_main);
}

struct FTUEntry *
FTUMainWindow :: find(int unit, int type, int subtype, const char *path)
{
	struct FTUEntry temp;
	struct FTUEntry *ptemp = &temp;
	struct FTUEntry **pretval;

	memset(&temp, 0, sizeof(temp));

	temp.unit = unit;
	temp.type = type;
	temp.subtype = subtype;
	temp.path = path;

	pretval = (struct FTUEntry **)bsearch(&ptemp, pp_entry, pp_entries, sizeof(void *), &compare);
	if (pretval == 0)
		return (0);

	return (*pretval);
}

void
FTUMainWindow :: getRange(const struct FTUEntry *pentry, int *pmin,
    int *pmax, int *pval)
{
	struct FTUEntry *entry_min;
	struct FTUEntry *entry_max;
	struct FTUEntry *entry_val;

	entry_min = find(pentry->unit, pentry->type, FTU_SUB_MIN, pentry->path);
	entry_max = find(pentry->unit, pentry->type, FTU_SUB_MAX, pentry->path);
	entry_val = find(pentry->unit, pentry->type, FTU_SUB_VAL, pentry->path);

	if (entry_min == 0 || entry_max == 0 || entry_val == 0) {
		*pmin = *pmax = *pval = 0;
	} else {
		*pmin = atoi(entry_min->value);
		*pmax = atoi(entry_max->value);
		*pval = atoi(entry_val->value);

		if (*pmin > *pmax) {
			int temp = *pmin;
			*pmin = *pmax;
			*pmax = temp;
		}

		if (*pval > *pmax)
			*pval = *pmax;
		else if (*pval < *pmin)
			*pval = *pmin;
	}
}

FTUMainWindow :: ~FTUMainWindow()
{
}

void
FTUMainWindow :: parse(char *line)
{
	struct FTUEntry *pentry;
	char *ptr;
	char sub[strlen(line) + 1];
	int unit = -1;
	int type;
	int subtype;
	int num;

	sub[0] = 0;
	unit = -1;
	type = -1;
	subtype = -1;
	num = -1;

	if (sscanf(line, "dev.pcm.%d.mixer.effect_send_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_SEND;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_ret_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_RET;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_fb_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_FB;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_dur_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_DUR;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_vol_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_VOL;
	} else if (sscanf(line, "dev.pcm.%d.mixer.effect_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_EFFECT_SW;
	} else if (sscanf(line, "dev.pcm.%d.mixer.mix_rec_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_MIXER_ANALOG;
	} else if (sscanf(line, "dev.pcm.%d.mixer.mix_play_%d.%s",
		   &unit, &num, sub) == 3) {
		type = FTU_MIXER_DIGITAL;
	} else {
		return;
	}

	if (strcmp(sub, "desc:") == 0)
		subtype = FTU_SUB_DESC;
	else if (strcmp(sub, "val:") == 0)
		subtype = FTU_SUB_VAL;
	else if (strcmp(sub, "min:") == 0)
		subtype = FTU_SUB_MIN;
	else if (strcmp(sub, "max:") == 0)
		subtype = FTU_SUB_MAX;
	else
		return;

	ptr = strstr(line, sub);
	if (ptr == 0)
		return;

	*ptr = 0;

	ptr += strlen(sub);

	while (*ptr == ' ')
		ptr++;

	pentry = (struct FTUEntry *)malloc(sizeof(*pentry));
	if (pentry == 0)
		errx(EX_SOFTWARE, "Out of memory");

	memset(pentry, 0, sizeof(*pentry));

	pentry->path = line;
	pentry->value = ptr;
	pentry->unit = unit;
	pentry->type = type;
	pentry->subtype = subtype;
	pentry->parent = this;

	if (subtype == FTU_SUB_DESC) {
	  switch (type) {
	  case FTU_MIXER_ANALOG:
		if (sscanf(ptr, "AIn%d - Out%d Record Volume",
		     &pentry->x_coord, &pentry->y_coord) != 2) {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	  case FTU_MIXER_DIGITAL:
		if (sscanf(ptr, "DIn%d - Out%d Playback Volume",
		     &pentry->x_coord, &pentry->y_coord) != 2) {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	  case FTU_EFFECT_SEND:
		if (sscanf(ptr, "Effect Send DIn%d Volume",
		     &pentry->x_coord) == 1) {
			pentry->y_coord = 1;
		} else if (sscanf(ptr, "Effect Send AIn%d Volume",
		     &pentry->x_coord) == 1) {
			pentry->y_coord = 2;
		} else {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	  case FTU_EFFECT_RET:
		pentry->x_coord = 1;
		if (sscanf(ptr, "Effect Return %d Volume",
		    &pentry->y_coord) != 1) {
			pentry->x_coord = 0;
			pentry->y_coord = 0;
		}
		break;
	  default:
		break;
	  }
	}
	TAILQ_INSERT_TAIL(&head, pentry, entry);
}

void
FTUMainWindow :: handle_value_changed(int value, void *arg)
{
	const struct FTUEntry *entry = (const struct FTUEntry *)arg;
	QProcess p;
	QString cmd = QString("sysctl %1val=%2").arg(entry->path).arg(value);

	p.start(cmd);
	p.waitForFinished(-1);

	if (p.exitCode()) {
		QMessageBox box;

		box.setText(tr("Error executing ") + cmd +
		    tr("\n" "This program might need to be run as super user."));
		box.setStandardButtons(QMessageBox::Ok);
		box.setIcon(QMessageBox::Critical);
		box.setWindowIcon(QIcon(QString(":/qfasttrackultractl.png")));
		box.exec();
	}
}

FTUGroupBox :: FTUGroupBox(const QString &title)
  : QGroupBox(title), QGridLayout(this)
{

}

FTUGroupBox :: ~FTUGroupBox()
{

}

FTUGridLayout :: FTUGridLayout() : QGridLayout(this)
{
}

FTUGridLayout :: ~FTUGridLayout()
{
}

int
main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QProcess p;
	QString cmd = "sysctl -a dev.pcm";

	p.start(cmd);
	p.waitForFinished(-1);

	if (p.exitCode()) {
		QMessageBox box;

		box.setText(QObject::tr("Error executing ") + cmd);
		box.setStandardButtons(QMessageBox::Ok);
		box.setIcon(QMessageBox::Critical);
		box.setWindowIcon(QIcon(QString(":/qfasttrackultractl.png")));
		box.exec();
	}

	QByteArray data = p.readAllStandardOutput();

	FTUMainWindow mw((char *)data.data());

	mw.show();

	return (app.exec());
}
