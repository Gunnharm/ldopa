"""
Complex experiments (experiment schemes) for learning on the reduction problem.
"""

import sys

from pnsynth_experimenter import Experimenter
from pnsynth_experimenter import ExperimentStepper
from pnsynth_experimenter import build_prefixtree_fixwnd as build_pt

# test log file name (user-specific)
TEST_LOGS_DIR = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/tests/gtest/work_files/logs/"
LOG_FILE_RTS_04 = TEST_LOGS_DIR + "log04.sq3"
LOG_FILE_RTS_05 = TEST_LOGS_DIR +"log05.sq3"
LOG_FILE_R1 = TEST_LOGS_DIR + "r-all-shorts.sq3";
LOG_FILE_GOODRU_R2 = TEST_LOGS_DIR + "good.ru/good.ru_full_reduced2.sq3";
LOG_FILE_BPIC15_1_2 = TEST_LOGS_DIR + "BPIC15_1_2/BPIC15_1_2.sq3";
LOG_FILE_BPIC15_1132 = TEST_LOGS_DIR + "bpic15-1132/bpic15-1132.sq3";

CARMONA_LOGS_DIR = "f:/temp/20181004/carmona-trs/Tkde2010PaperLogs/"

PN_RES_GV_05 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/log05-pn.gv"

CSV_SER_1 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/cexp/ser01.csv.txt"
CSV_OUT_DIR = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/cexp/"



#------------------------------------------------------------------------------
def evlog_ts_reduce_n_metrics_test1():
    ## create a log
    ##log = SQLiteLog(LOG_FILE_RTS_04)
    #log = ld.SQLiteLog(LOG_FILE_RTS_05)
    #log.autoload_conf_qry = "SELECT * FROM DefConfig"
    #log.open()

    ## TS fBuilder
    ## pool, state function and builder
    #pool = ld.AttrListStateIDsPool()
    #fnc = ld.PrefixStateFunc(log, pool, ld.PrefixStateFunc.UNLIM_WND_SIZE)
    #bldr = ld.TsBuilder(log, fnc, pool)
    #ts1 = bldr.build(False)

    ## condenser
    #cond = ld.CondensedTsBuilder(ts1)
    #ts2 = cond.build(0.33)

    xp = Experimenter(LOG_FILE_RTS_05)
    xp.mine_prefix_tree(ld.PrefixStateFunc.UNLIM_WND_SIZE)

    
    # condenser
    cond = ld.CondensedTsBuilder(xp.ts1)
    ts2 = cond.build(0.33)

    # restater
    rest = ld.VarWsTsBuilder(xp.log, ts2, xp.fnc)
    
    # zsa = ZeroSizeWndAction.zsaSpecState    
    #ts3 = rest.build(1, VarWsTsBuilder.ZeroSizeWndAction.zsaSpecState)
    ts3 = rest.build(1, ld.VarWsTsBuilder.Zsa.spec_state)

    #metrics = ld.TsMetricsCalc(log, ts1)
    #ts3_ms = metrics.calc_simplicity(ts3)
    #ts3_mp = metrics.calc_precision(ts3)
    #ts3_mg = metrics.calc_general(ts3)

    answer = 42


##------------------------------------------------------------------------------


def exp1():
    print(Experimenter.__doc__)
    xp = Experimenter(LOG_FILE_RTS_05)
    xp.mine_prefix_tree(ld.PrefixStateFunc.UNLIM_WND_SIZE)
    xp.reduce_ts(0.33, 1)

    (ms1, mp1, mg1) = xp.calc_metrics(Experimenter.TsNum.TS3)
    xp.make_sas(Experimenter.TsNum.TS3)
    # здесь сильно все просядет! поэтому метрики считаем _до_, а не _после_
    (ms2, mp2, mg2) = xp.calc_metrics(Experimenter.TsNum.TS3)   
    
    
    xp.synth_pn(Experimenter.TsNum.TS3, True, ld.EvLogPNSynth.SelfLoopPolicy.slProcess)


    #ts1 = xp.ts1
    
    1 # просто один (чтобы стопануть отладку по шагам)
    

#------------------------------------------------------------------------------

def exp2():
    xp = ExperimentStepper()
    params = { #"csv_fn" : CSV_SER_1 ,
               "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_RTS_05,
               "thresh_set" : [ 0.33, 0.66, 0.75 ],
               "vwsc_set" : [ 1, 0.75, 0.5, 0.25 ]
             }
    xp.series1(params)

    1 #
               

#------------------------------------------------------------------------------


def exp3():
    xp = ExperimentStepper()
    params = { #"csv_fn" : CSV_SER_1 ,
               "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_GOODRU_R2,
               "thresh_set" : [ 0.33 ],
               #"vwsc_set" : [ 0.1, 0.15, 0.2, 0.25 ]
               "vwsc_set" : [ 0.3, 0.35, 0.4, 0.45, 0.5 ]
             }
    xp.series1(params)


               
#------------------------------------------------------------------------------

def exp4():
    xp = ExperimentStepper()
    params = { #"csv_fn" : CSV_SER_1 ,
               "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_R1,
               "thresh_set" : [ 0.33 ],
               #"vwsc_set" : [ 0.1, 0.15, 0.2, 0.25 ]
               "vwsc_set" : [ 0.05, 0.1, 0.15, 0.2 ]
             }
    xp.series1(params)



#------------------------------------------------------------------------------

def exp5():
    """Data for r-all-shorts.sq3"""
    xp = ExperimentStepper()
    params = { #"csv_fn" : CSV_SER_1 ,
               "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_R1,
               #"thresh_set" : [ 0.33 ],
               "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.65, 0.7, 0.8, 0.9, 0.95 ],
               #"vwsc_set" : [ 0.1, 0.15, 0.2, 0.25 ]
               #"vwsc_set" : [ 0.05, 0.1, 0.15, 0.2 ]
               #"vwsc_set" : [ 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95 ]
               "vwsc_set" : [ 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1 ]
             }
    xp.series2(params)


#------------------------------------------------------------------------------

def exp6():
    """Data for good.ru_full_reduced2.sq3"""
    xp = ExperimentStepper()
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_GOODRU_R2,

               "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.65, 0.7, 0.8, 0.9, 0.95 ],
               "vwsc_set" : [ 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1 ]
             }
    xp.series2(params)




#------------------------------------------------------------------------------

def exp7():
    """r-all-shorts.sq3 fix wndsize = 1"""

    #build_pt(LOG_FILE_GOODRU_R2, 1)  # контрольный
    
    xp = ExperimentStepper()
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_GOODRU_R2, #LOG_FILE_R1,

               "wnd_sz" : 1
             }
    xp.prefixtree_fixwnd(params)


#------------------------------------------------------------------------------

def exp8():
    """Data for LOG_FILE_BPIC15....."""


    xp = ExperimentStepper()
    
    # фиксированное окно
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : LOG_FILE_BPIC15_1132,

               "wnd_sz" : 1
             }
    xp.prefixtree_fixwnd(params)    
    
    ## редукция
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : LOG_FILE_BPIC15_1132, #LOG_FILE_BPIC15_1_2,

    #           "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95 ],
    #           "vwsc_set" : [ 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95, 1 ]
    #         }
    #xp.series2(params)


#------------------------------------------------------------------------------

def exp9():
    """Data for Carmona's logs....."""

    print("Carmona's Logs")

    xp = ExperimentStepper()
    
    fn = CARMONA_LOGS_DIR + "a12f0n00_5.tr.sq3"

    ## фиксированное окно
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : fn,

    #           "wnd_sz" : 1
    #         }
    #xp.prefixtree_fixwnd(params)    
    
    # редукция (здесь интерены еще меньшие, чем 0.05, параметры vwsc)
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : fn, 

               "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95 ],
               "vwsc_set" : [ 0.005, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, #0.7, 
                                0.8, 0.9, 1 ]
             }
    xp.series2(params)


#------------------------------------------------------------------------------

def exp9_1():
    """Carmona's log a12f0n00_5.tr.sq3: special stepping"""

    print("Carmona's Logs")

    xp = ExperimentStepper()
    
    fn = CARMONA_LOGS_DIR + "a12f0n00_5.tr.sq3"

    ## фиксированное окно
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : fn,

    #           "wnd_sz" : 1
    #         }
    #xp.prefixtree_fixwnd(params)    
    
    # редукция (здесь интерены еще меньшие, чем 0.05, параметры vwsc)
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : fn, 

               "thresh_set" : [ 0.2, 0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.3 ],
               "vwsc_set" : [ 0.005, 0.006, 0.007, 0.008, 0.009, 0.01, 0.011, 0.012, 0.013, 0.014, 0.015 ]
             }
    xp.series2(params)



#------------------------------------------------------------------------------

def exp10():
    """Data for Carmona's logs (2)....."""

    print("Carmona's Logs")

    xp = ExperimentStepper()
    
    fn = CARMONA_LOGS_DIR + "a22f0n00_1.tr.sq3"

    # фиксированное окно
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : fn,

               "wnd_sz" : 1
             }
    xp.prefixtree_fixwnd(params)    
    
    ## редукция (здесь интерены еще меньшие, чем 0.05, параметры vwsc)
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : fn, 

    #           "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95 ],
    #           "vwsc_set" : [ 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95, 1 ]
    #         }
    #xp.series2(params)




#------------------------------------------------------------------------------

def exp11():
    """Mine PN on prefix tree"""

    print("Mine PN on prefix tree. Carmona's Logs")

    xp = ExperimentStepper()
    
    #fn = CARMONA_LOGS_DIR + "a12f0n00_5.tr.sq3"
    fn = CARMONA_LOGS_DIR + "a22f0n00_1.tr.sq3"


    # сеть Петри по префиксному дереву
    params = { "csv_out_dir": CSV_OUT_DIR,
               "log_fn" : fn,
               "viz_pn" : True,                 # visualize PN
             }
    xp.synthpn_by_prefixtree(params)

    ## фиксированное окно
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : fn,

    #           "wnd_sz" : 1
    #         }
    #xp.prefixtree_fixwnd(params)    
    
    ## редукция (здесь интерены еще меньшие, чем 0.05, параметры vwsc)
    #params = { "csv_out_dir": CSV_OUT_DIR,
    #           "log_fn" : fn, 

    #           "thresh_set" : [ 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95 ],
    #           "vwsc_set" : [ 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.95, 1 ]
    #         }
    #xp.series2(params)


#------------------------------------------------------------------------------


def main():
    print("Hello world!")
    

    exp9_1() #special stepping for Carmona's log a12f0n00_5.tr.sq3
    #exp11() # Synth pn on prefix tree for Carmona's logs (2)
    #exp10() # Carmona's logs (2)
    #exp9() # Carmona's logs

    #exp8() # bpic15-1132
    #exp7() # r wndsize = 1

    #exp6()  # goodru    
    #exp5() # r
    #evlog_ts_reduce_n_metrics_test1()
    #exp1()
    #exp2()
    #exp3()
    #exp4()

#------------------------------------------------------------------------------

if __name__ == "__main__":
    try:
        main()
    except:
        print("Unexpected error:", sys.exc_info()[0])
        raise
