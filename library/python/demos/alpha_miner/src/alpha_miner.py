# demo/test for SQLIte Event Log class


#from xi.ldopa.eventlog import SQLiteLog
from xi.ldopa.eventlog import *

# test log file name (user-specific)
LOG_FILE = "/home/ganvas/ldopa/library/tests/gtest/work_files/logs/log06.csv"
OUT_FILE = "/home/ganvas/ldopa/library/tests/gtest/work_files/pn/alpha_miner/alpha_miner2.gv"


def open_log_test():
    log = CSVLog()
    log.filename = LOG_FILE
    log.open()
    assert log.is_open
    print("Events:", log.get_events_num())
    log.close()
    assert not log.is_open


def alpha_miner_test():
    log = CSVLog()
    log.filename = LOG_FILE
    log.open()
    assert log.is_open
    miner = AlphaMiner()
    pn = miner.mine(log)
    
    pndw = EventLogPetriNetDotWriter()
    pndw.write(OUT_FILE, pn)


def main():
    open_log_test()
    alpha_miner_test()

#------------------------------------------------------------------------------

if __name__ == "__main__":
    main()