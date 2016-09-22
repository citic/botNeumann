#ifndef GDBRESPONSE_H
#define GDBRESPONSE_H


enum GdbResult
{
	GDB_DONE = 0,
	GDB_RUNNING,
	GDB_CONNECTED,
	GDB_ERROR,
	GDB_EXIT,
	GDB_UNKNOWN
};

class GdbResponse
{
  protected:
	GdbResult result = GDB_UNKNOWN;

  public:
	GdbResponse();
	inline operator GdbResult() const { return result; }
};

#endif // GDBRESPONSE_H
