#include "webserver.h"


int main() {
	Webserver svr(8080);
	svr.dispatch();
	return 0;
}
