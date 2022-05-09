#ifndef GLOBAL
#define GLOBAL

#ifdef MAIN
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN int mainWndID;
EXTERN int subWndID;
EXTERN int AnimateWndID;

#endif // GLOBAL