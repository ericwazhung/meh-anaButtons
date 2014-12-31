/* mehPL:
 *    This is Open Source, but NOT GPL. I call it mehPL.
 *    I'm not too fond of long licenses at the top of the file.
 *    Please see the bottom.
 *    Enjoy!
 */


//tcnter 0.20-10
// Similar functionality to dmsTimer
//   Doesn't require interrupts
//   MUCH higher resolution-potential (e.g. for bit-rate generation)
//      limited only by the tcnter_update() call rate(?)
// TODO: Make some, if not all, functions inline.
//       CONSIDER: if timing is highly critical, it may make sense
//                 to call update() with every get()
//                 THOUGH: this would add a lot of calculations...
//       CONSIDER2: could myTcnter_t be smaller, and use wraparound math?
//                 e.g.  if(get() >= next) NOGO ? next=3, get=254
//                 e.g.2 if(get() - next >= 0) ... same
//                  wasn't there a way to do this?
//                       if(get() - last >= next - last) ?
//                             next=3, get=254, last=253
//                          get()-last = 254-253 = 1
//                          next-last = 3-253 = 6
//                  Promotion issues may occur, watch casts.
//                  See cTools/unsignedSubtraction.c
//                             next=5, get=7, last=6
//             Create "wrappableTimeCompare()" macro?
//             Also usable in dms, etc...
//0.20-10 Replacing tests' makefiles with those that refer to tcnter
//        properly... a/o/ala anaButtons 0.50
//0.20-9 adding this TODO: Apparently math doesn't work with
//       tcnter_source_t = uint16_t???
//       And looking into it...
//0.20-8 a/o polled_uar/0.40/avrTest...
//       adding some Usage notes...
//0.20-7 adding isItTime (non 8-bit)
//0.20-6 adding to COM_HEADERS when INLINED
//0.20-5 Renaming Globals to tcnter_*... nastiness
//       see puar notes...
//0.20-4 tcnter8_t -> tcnter_isItTime8(startTime, delta)
//       HANDY: When inlined, if(tcnter_isItTime()) results in
//       an if statement from inside tcnter_isItTime, which is used
//       *in place of* the if surrounding tcnter_isItTime()
//       Inlining optimization is great!
//0.20-3 myTcnter_t is stupid -> tcnter_t
//0.20-2 adding _SPECIALHEADER_FOR_TCNTER_
//       e.g. TCNTER OVERFLOW VAL = _DMS_OCR_VAL_ requires dmsTimer.h
//       so CFLAGS+=-D'_SPECIALHEADER_FOR_TCNTER_=_DMSTIMER_HEADER_'
//0.20-1 inlining...
//          SEE NOTES in .c before using inlining
//          and experiment and stuff
//0.20 TCNTER_SOURCE_MAX renamed to TCNTER_SOURCE_OVERFLOW_VAL
//0.10-3 fixing names in test (to match 0.10-1 changes)
//       experimenting with test
//0.10-2 Need <avr/io.h> when TCNT, etc. is used...
//0.10-1 (forgot to backup first version?)
//       renaming all tcnt and whatnot to tcnter
//0.10 First version, stolen and modified from polled_uar 0.10-6
//
//---------
//polled_uar 0.10-6:
//   Doesn't use interrupts (no lag-times, etc.)
//0.10-4 
//       Looking into running-tcnt
//          myTcnter and nextTcnter now implemented
//          Fixes potential issues with multi-TCNTs per update
//            Aiding cumulative-error fixing
//            (Next time was dependent on the time of the current update)
//0.10-3 test app using makefile...
//       cleanup
//0.10-1 More mods, test app
//0.10 First Version stolen and modified heavily from usi_uart 0.22-3

#ifndef __TCNTER_H__
#define __TCNTER_H__

//This shouldn't be necessary... (or its necessity removed soon)
//#include <avr/io.h>
#include <stdint.h>

#ifdef __AVR_ARCH__
 //This is only necessary if TCNT0 (etc.) is used
 // according to the project makefile
 #include <avr/io.h>
#endif

//This is for, e.g. where TCNTER_SOURCE_OVERFLOW_VAL = _DMS_OCR_VAL_
// Then tcnter needs to be aware of the definition of _DMS_OCR_VAL_
#ifdef _SPECIALHEADER_FOR_TCNTER_
 #include _SPECIALHEADER_FOR_TCNTER_
#endif

/*
// This'd be nice to change...
#if (!defined(PU_PC_DEBUG) || !PU_PC_DEBUG)
// #include "../../bithandling/0.94/bithandling.h"
#else
 #include <stdio.h>
#endif
*/

//DON'T CHANGE THIS WITHOUT CHANGING both!
#if ( (!defined(tcnter_source_t) && defined(tcnter_compare_t)) \
    ||(!defined(tcnter_compare_t) && defined(tcnter_source_t)) )
 #error "tcnter_source_t and tcnter_compare_t must *both* be overridden"
#endif

#ifndef tcnter_source_t
#define tcnter_source_t   uint8_t
#endif

#ifndef tcnter_compare_t
#define tcnter_compare_t  int16_t
#endif

//This is a stupid name...
#define myTcnter_t  uint32_t
//This makes more sense...
// But I think I did it that way because I might later want to make this
// an internal structure, in case I want multiple tcnters...
// (e.g. like xyt, or hfm, etc...)
// seems less likely in this case...
#define tcnter_t uint32_t
#define tcnter8_t uint8_t


//********** TCNTER Prototypes **********//

//These are the functions that are used in main code...
// General initialization...
void tcnter_init(void);
// To be called in the main loop...
// (Handles reading the TCNT deltas and adding to myTcnter, etc)
void tcnter_update(void);
// Get the current value of myTcnter
tcnter_t tcnter_get(void);

//Of course, this is only safe if it's called often enough...
// and deltaTime is small enough...
// and tcnterUpdate is called often enough, as well (?)
uint8_t tcnter_isItTime8(tcnter8_t *startTime, tcnter8_t deltaTime);
uint8_t tcnter_isItTime(tcnter_t *startTime, tcnter_t deltaTime);

#if (defined(_TCNTER_INLINE_) && _TCNTER_INLINE_)
   #define TCNTER_INLINEABLE extern __inline__
   #include "tcnter.c"
#else
   #define TCNTER_INLINEABLE //Nothing Here
#endif


//Basic Usage:
// (Not particularly accurate...)
// (See also polled_uar/0.40/avrTest/)
// makefile:
// CFLAGS+=-D'TCNTER_SOURCE_VAR=TCNT0'
// CFLAGS+=-D'TCNTER_SOURCE_OVERFLOW_VAL=255' #should this be 255 or 256?
// #OVERFLOW_VAL Should be the value at which it overflows back to 0
// so for a uint8_t that wraps-around on its own, the overflowVal should be
// ___256___

// # Or if the DMS timer is running (heartBeat included?)
// CFLAGS += -D'TCNTER_SOURCE_VAR=(TCNT0)'
// CFLAGS += -D'TCNTER_SOURCE_OVERFLOW_VAL=(_DMS_OCR_VAL_)'
// CFLAGS += -D'_SPECIALHEADER_FOR_TCNTER_=_DMSTIMER_HEADER_'

// init();
// while(1)
// {
//    update();
//    ...
//    if(tcnter_get() >= nextTime)
//       nextTime += delay;
//       ...
// }
//
//  This should probably be changed to be #defined application-specifically

#endif

/* mehPL:
 *    I would love to believe in a world where licensing shouldn't be
 *    necessary; where people would respect others' work and wishes, 
 *    and give credit where it's due. 
 *    A world where those who find people's work useful would at least 
 *    send positive vibes--if not an email.
 *    A world where we wouldn't have to think about the potential
 *    legal-loopholes that others may take advantage of.
 *
 *    Until that world exists:
 *
 *    This software and associated hardware design is free to use,
 *    modify, and even redistribute, etc. with only a few exceptions
 *    I've thought-up as-yet (this list may be appended-to, hopefully it
 *    doesn't have to be):
 * 
 *    1) Please do not change/remove this licensing info.
 *    2) Please do not change/remove others' credit/licensing/copyright 
 *         info, where noted. 
 *    3) If you find yourself profiting from my work, please send me a
 *         beer, a trinket, or cash is always handy as well.
 *         (Please be considerate. E.G. if you've reposted my work on a
 *          revenue-making (ad-based) website, please think of the
 *          years and years of hard work that went into this!)
 *    4) If you *intend* to profit from my work, you must get my
 *         permission, first. 
 *    5) No permission is given for my work to be used in Military, NSA,
 *         or other creepy-ass purposes. No exceptions. And if there's 
 *         any question in your mind as to whether your project qualifies
 *         under this category, you must get my explicit permission.
 *
 *    The open-sourced project this originated from is ~98% the work of
 *    the original author, except where otherwise noted.
 *    That includes the "commonCode" and makefiles.
 *    Thanks, of course, should be given to those who worked on the tools
 *    I've used: avr-dude, avr-gcc, gnu-make, vim, usb-tiny, and 
 *    I'm certain many others. 
 *    And, as well, to the countless coders who've taken time to post
 *    solutions to issues I couldn't solve, all over the internets.
 *
 *
 *    I'd love to hear of how this is being used, suggestions for
 *    improvements, etc!
 *         
 *    The creator of the original code and original hardware can be
 *    contacted at:
 *
 *        EricWazHung At Gmail Dotcom
 *
 *    This code's origin (and latest versions) can be found at:
 *
 *        https://code.google.com/u/ericwazhung/
 *
 *    The site associated with the original open-sourced project is at:
 *
 *        https://sites.google.com/site/geekattempts/
 *
 *    If any of that ever changes, I will be sure to note it here, 
 *    and add a link at the pages above.
 *
 * This license added to the original file located at:
 * /home/meh/_commonCode/anaButtons/0.50-gitHubbing/testMega328p+NonBlocking/_commonCode_localized/tcnter/0.20/tcnter.h
 *
 *    (Wow, that's a lot longer than I'd hoped).
 *
 *    Enjoy!
 */
