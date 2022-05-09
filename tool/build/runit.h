#ifndef COSMOPOLITAN_TOOL_BUILD_RUNIT_H_
#define COSMOPOLITAN_TOOL_BUILD_RUNIT_H_

#define RUNITD_PORT       31337
#define RUNITD_MAGIC      0xFEEDABEEu
#define RUNITD_TIMEOUT_MS (1000 * 60 * 60)

enum RunitCommand {
  kRunitExecute,
  kRunitStdout,
  kRunitStderr,
  kRunitExit,
};

#endif /* COSMOPOLITAN_TOOL_BUILD_RUNIT_H_ */
