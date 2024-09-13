/*
 * etrobo_inc.h
 *
 *  Created on: 2011/07/23
 *      Author: tdohshi
 */

#ifndef ETROBO_INC_H_
#define ETROBO_INC_H_

/* ITRON(TOPPERS/JSP) API */
#include "kernel.h"
//#include "sil.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* Mindstorms NXT API */
#include "Sapporo.h"
//#include "ecrobot_interface.h"
//#include "ecrobot_base.h"

#endif /* ETROBO_INC_H_ */