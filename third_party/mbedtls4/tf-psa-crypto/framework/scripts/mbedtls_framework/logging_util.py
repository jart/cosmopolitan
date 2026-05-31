"""Auxiliary functions used for logging module.
"""

# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
#

import logging
import sys

def configure_logger(
        logger: logging.Logger,
        log_format="[%(levelname)s]: %(message)s",
        split_level=logging.WARNING
    ) -> None:
    """
    Configure the logging.Logger instance so that:
        - Format is set to any log_format.
            Default: "[%(levelname)s]: %(message)s"
        - loglevel >= split_level are printed to stderr.
        - loglevel <  split_level are printed to stdout.
            Default: logging.WARNING
    """
    class MaxLevelFilter(logging.Filter):
        # pylint: disable=too-few-public-methods
        def __init__(self, max_level, name=''):
            super().__init__(name)
            self.max_level = max_level

        def filter(self, record: logging.LogRecord) -> bool:
            return record.levelno <= self.max_level

    log_formatter = logging.Formatter(log_format)

    # set loglevel >= split_level to be printed to stderr
    stderr_hdlr = logging.StreamHandler(sys.stderr)
    stderr_hdlr.setLevel(split_level)
    stderr_hdlr.setFormatter(log_formatter)

    # set loglevel < split_level to be printed to stdout
    stdout_hdlr = logging.StreamHandler(sys.stdout)
    stdout_hdlr.addFilter(MaxLevelFilter(split_level - 1))
    stdout_hdlr.setFormatter(log_formatter)

    logger.addHandler(stderr_hdlr)
    logger.addHandler(stdout_hdlr)
