#ifndef CRITICAL_H_
#define CRITICAL_H_

#ifdef __cplusplus
	extern "C" {
#endif

extern volatile unsigned char bInISR;

#define inISR() (bInISR)						///< Variable must be put to TRUE in every ISR to indicate execution of an ISR and need to put back to FALSE before leaving ISR.
#define enterISR() { bInISR = 1; }				///< Call this method always at the beginning of an interrupt service routine (ISR)
#define exitISR() { bInISR = 0; }				///< Call this method always at the end of an ISR

extern void enterCritical();					///< Called before code which should be protected
extern void exitCritical();						///< Called after leaving the code to protect

#ifdef __cplusplus
}
#endif

#endif /* CRITICAL_H_ */
