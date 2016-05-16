#ifndef OCVTHREAD_H_
#define OCVTHREAD_H_

#include <wx/wxprec.h>
#include <wx/thread.h>

#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif //precompiled headers

class ocvFrame;
class ocvCamera;

class ocvThread : public wxThread {
public:
	ocvThread(ocvFrame* frame);
	virtual ~ocvThread();

	virtual void *Entry();
	virtual void OnExit();
	double GetTime(void);

public:
	ocvFrame *frame;
	ocvCamera *camera;

	// timers
	double m_nCpuFreq;
	unsigned char m_bLife;

	// private data
private:
	// process syncronizer/locker
	wxMutex *m_pMutex;
};

#endif /*OCVTHREAD_H_*/
