/*
 * tts.h
 *
 *  Created on: Mar 15, 2019
 *      Author: michelle
 */

#ifndef TTS_H_
#define TTS_H_

extern void speak(char* string);
extern void init(void);
extern void clockSetup(void);
extern void interruptsConfig(void);

#endif /* TTS_H_ */
