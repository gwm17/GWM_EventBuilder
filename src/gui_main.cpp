#include "EventBuilder.h"
#include <TApplication.h>
#include "EVBMainFrame.h"

int main(int argc, char** argv) {
	TApplication app("app", &argc, argv);
	UInt_t h = 700;
	UInt_t w = 700;
	EVBMainFrame* myEVB = new EVBMainFrame(gClient->GetRoot(), w, h);
	app.Run();
	return 0;
}