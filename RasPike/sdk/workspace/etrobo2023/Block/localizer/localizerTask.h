/*
 * localizerTask.h
 *
 *  Created on: 2011/09/25
 *      Author: tdohshi
 */

#ifndef LOCALIZERTASK_H_
#define LOCALIZERTASK_H_

#include "ev3api.h"

#define TASK_CYCLE_LOCALIZER	(2)

#ifndef _MACRO_ONLY

void localizerTask(intptr_t exinf);
void localizerCyclicActivator(intptr_t exinf);
void startLocalizerTask(void);

#endif /* _MACRO_ONLY */

#endif /* LOCALIZERTASK_H_ */