# demo/test for SQLIte Event Log class


#from xi.ldopa.eventlog import SQLiteLog
from xi.ldopa.eventlog import *

# test log file name (user-specific)
LOG_FILE_RTS_04 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/tests/gtest/work_files/logs/log04.sq3"
LOG_FILE_RTS_05 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/tests/gtest/work_files/logs/log05.sq3"

PN_RES_GV_05 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/log05-pn.gv"

TS_RES_GV_05_1 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/log05-ts-1.gv"
TS_RES_GV_05_2 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/python/demos/work_files/log05-ts-2.gv"


def sqlite_log_tests1():
    ## test log file name (user-specific)
    #LOG_FILE_RTS_04 = "f:/se/projects/cpp/xi/ldopa/dev/root/0.1/tests/gtest/work_files/logs/log04.sq3"
    
    
    # create a log
    log = SQLiteLog(LOG_FILE_RTS_04)

    # check a filename
    print("File Name: %s" % log.filename)

    # is it already open?
    print("Is Open?: %s" % log.is_open)

    # trying to open it check againg
    log.open()
    print("Is Open?: %s" % log.is_open)

    # now trying to close it check againg
    log.close()
    print("Is Open?: %s" % log.is_open)


    # set configuration string and open again
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()
    
    # extract some knowledge about eventlog
    print("Events num: %s" % log.get_events_num())  # 13
    print("Traces num: %s" % log.get_traces_num())  # 4
    print("Acts num: %s" % log.get_activities_num())  # 5

    # some config params
    print("CP(qryl_events_num): %s" % log.get_config_param_or_empty("qryl_events_num"))
    # log.set_config_param("qryl_events_num", "lol")                                        # this works
    # print("CP(qryl_events_num): %s" % log.get_config_param_or_empty("qryl_events_num"))

    print("Is autoload config?: %s" % log.autoload_conf)
    print("Is Vertical EventAttr Extracting?: %s" % log.vert_event_attr_extr)
    print("Is Vertical TraceAttr Extracting?: %s" % log.vert_trace_attr_extr)

    return log


def sqlite_log_tests2():
    # create a log
    log = SQLiteLog(LOG_FILE_RTS_04)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # 
    # print(log.test_string())

    # log attributes
    la1 = log.get_log_attr("myatr1")


    # demos
    ## la2 = log.get_log_attr_d1("")
    #la0 = log.get_log_attr_d1(0)
    #la1 = log.get_log_attr_d1(1)
    #la2 = log.get_log_attr_d1(2)
    #la3 = log.get_log_attr_d1(3)
    #la4 = log.get_log_attr_d1(4)
    #la5 = log.get_log_attr_d1(5)
    #la7 = log.get_log_attr_d1(7)
    ## la8 = log.get_log_attr_d1(8)

    #la14 = log.get_log_attr_d1(14)  # string


    # trying iterators on log attributes
    iter = log.get_log_attrs()
    try:
        while True:
            y = iter.next()         # get each item
            a = 0
    except StopIteration: 
        pass                        # iterator exhausted

    # traces andtrace attributes
    ta1 = log.get_trace_attr(1, "case")
    iter = log.get_trace_attrs(1)
    try:
        while True:
            y = iter.next()         # get each item
            a = 0
    except StopIteration: 
        pass    

    # trace size
    t1_s = log.get_trace_size(0)
    tr = log.get_trace(0)
    tr_an = tr.attrs_num
    tr_a = tr.get_attr("id")
    tr_a = tr.get_attr("case")
    iter = tr.get_attrs()
    try:
        while True:
            y = iter.next()         # get each item
            a = 0
    except StopIteration: 
        pass    

    # alternative version of tr (separate instance?)
    tr_1 = log.get_trace(0)
    #tr_2 = log.get_trace_alt(0)
    #tr_3 = log.get_trace_alt(0)
    tr is tr_1                      # фигушки!

    tr_log = tr.get_log()           # бормота: надо с этим разбираться...

    # working with events of the trace
    tr_s = tr.size
    ev = tr.get_event(0)
    ev_an = ev.attrs_num
    ev_a = ev.get_attr("id")
    ev_a = ev.get_attr("trace")
    ev_a = ev.get_attr("activity")
    iter = ev.get_attrs()
    try:
        while True:
            y = iter.next()         # get each item
            a = 0
    except StopIteration: 
        pass  


    #f = Foo()
    #b = Bar()

    return log





#------------------------------------------------------------------------------

def list_test1():

    o1 = 42
    o2 = "Abc"
    l = [o1, o2, None]

    #sys.getsizeof

    a = 0
    
#------------------------------------------------------------------------------


def evlog_ts_tests1():
    # pools and stateIDs   
    pool = AttrListStateIDsPool()
    init_st = pool.get_init_stateid()
    pool_sz = pool.get_size()

    # eventlog_ts
    ts1 = BaseEventLogTS(pool)

    a = 42


#------------------------------------------------------------------------------

def evlog_ts_fbuiler_tests1():
    # create a log
    log = SQLiteLog(LOG_FILE_RTS_04)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()


    # pool and state function
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)

    # TS func builder
    bldr = TsBuilder(log, fnc, pool)
    ts = bldr.build(False)
    ts_stn = ts.states_num
    ts_rstn = ts.reg_states_num
    ts_astn = ts.anon_states_num
    ts_trn = ts.trans_num

    # more complex TS properties
    init_st = ts.get_init_state()
    init_st_id = ts.get_init_state_id()
    st_init2 = ts.get_state(init_st_id)

    (init_st, ) = init_st                   # extracts a state itself
    st_isreg = ts.is_state_reg(init_st)
    st_isanon = ts.is_state_anon(init_st)

    a = 42


#------------------------------------------------------------------------------


def evlog_ts_reducer_test1():
    # create a log
    log = SQLiteLog(LOG_FILE_RTS_04)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # TS fBuilder
    # pool, state function and builder
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)
    bldr = TsBuilder(log, fnc, pool)
    ts1 = bldr.build(False)

    # condenser
    cond = CondensedTsBuilder(ts1)
    ts2 = cond.build(0.33)

    # restater
    rest = VarWsTsBuilder(log, ts2, fnc)
    
    # zsa = ZeroSizeWndAction.zsaSpecState    
    #ts3 = rest.build(1, VarWsTsBuilder.ZeroSizeWndAction.zsaSpecState)
    ts3 = rest.build(1, VarWsTsBuilder.Zsa.spec_state)

    answer = 42


#------------------------------------------------------------------------------


def evlog_ts_reduce_n_metrics_test1():
    # create a log
    #log = SQLiteLog(LOG_FILE_RTS_04)
    log = SQLiteLog(LOG_FILE_RTS_05)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # TS fBuilder
    # pool, state function and builder
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)
    bldr = TsBuilder(log, fnc, pool)
    ts1 = bldr.build(False)

    # condenser
    cond = CondensedTsBuilder(ts1)
    ts2 = cond.build(0.33)

    # restater
    rest = VarWsTsBuilder(log, ts2, fnc)
    
    # zsa = ZeroSizeWndAction.zsaSpecState    
    #ts3 = rest.build(1, VarWsTsBuilder.ZeroSizeWndAction.zsaSpecState)
    ts3 = rest.build(1, VarWsTsBuilder.Zsa.spec_state)

    metrics = TsMetricsCalc(log, ts1)
    ts3_ms = metrics.calc_simplicity(ts3)
    ts3_mp = metrics.calc_precision(ts3)
    ts3_mg = metrics.calc_general(ts3)

    answer = 42

#------------------------------------------------------------------------------



# proceeds a step of experiment 1
def reduce_experiment1_step(log, ts1, thresh, fnc, vwsc):
    # condenser
    cond = CondensedTsBuilder(ts1)
    ts2 = cond.build(thresh)

    # restater
    rest = VarWsTsBuilder(log, ts2, fnc)    
    ts3 = rest.build(vwsc, VarWsTsBuilder.Zsa.spec_state)

    metrics = TsMetricsCalc(log, ts1)
    ts3_ms = metrics.calc_simplicity(ts3)
    ts3_mp = metrics.calc_precision(ts3)
    ts3_mg = metrics.calc_general(ts3)

    return (ts3_ms, ts3_mp, ts3_mg)

#------------------------------------------------------------------------------

def print_step_res(ress):
    #print("Thres={0}, VWSC={1}".format((ress)))
    print("Thres=%f, VWSC=%f: simpl=%f, prec=%f, gener=%f" % ress)

#------------------------------------------------------------------------------

# generator for stepping range
def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step

#------------------------------------------------------------------------------

# proceeds a series of experiment stepping involved paramters
def reduce_experiment1(log_name):
    # create a log
    log = SQLiteLog(log_name)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # TS fBuilder
    # pool, state function and builder
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)
    bldr = TsBuilder(log, fnc, pool)
    ts1 = bldr.build(False)

    print("Experiment series 01")
    print("Experiment series 02")

    (ts3_ms, ts3_mp, ts3_mg) = reduce_experiment1_step(log, ts1, 0.33, fnc, 1)
    print_step_res((0.33, 1, ts3_ms, ts3_mp, ts3_mg))

    print("Stepping:")
    thresh0 = [t * 0.1 for t in range(0, 11)]   #drange(0.0, 1.0, 0.1)
    vwsc0 = [t * 0.1 for t in range(0, 11)]     #drange(0.0, 1.0, 0.1)
    for x in thresh0:
        for y in vwsc0:
            #print("%f %f" % (x, y))
            (ts3_ms, ts3_mp, ts3_mg) = reduce_experiment1_step(log, ts1, x, fnc, y)
            print_step_res((x, y, ts3_ms, ts3_mp, ts3_mg))


    #i0=drange(0.0, 1.0, 0.1)
    #print(["%g" % x for x in i0])

    answer = 42





def evlog_pn_test1():
    print("Hello World")

    gen_pn = GenPetriNet()
    base_pn = BasePetriNet()
    
    # эксперименты на этой ставим, т.к. должна унаследовать все
    evlog_pn = EventLogPetriNet()

    vnum = evlog_pn.verts_num

    # синтезатор регионами
    pnsynth = EvLogPNSynth()

    a = 42


def pn_synth1(log_name, pn_gv_name):
    print("PN Regions Synth 1")

    # лог
    log = SQLiteLog(log_name)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # TS fBuilder
    # pool, state function and builder
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)
    bldr = TsBuilder(log, fnc, pool)
    ts1 = bldr.build(False)

    # делаем sas
    ts1.make_sas()

    # синтезатор регионами
    pnsynth = EvLogPNSynth(ts1)
    
    # опции
    pnsynth.make_wf_net = True
    print("Opt Make WF: {}".format(pnsynth.make_wf_net))
    
    #pnsynth.out_place_titles = True
    print("Opt Out Place Titles: {}".format(pnsynth.out_place_titles))

    #print("Opt Self-Loop Policy: {}".format(pnsynth.self_loop_policy))
    pnsynth.self_loop_policy = EvLogPNSynth.SelfLoopPolicy.slProcess
    print("Opt Self-Loop Policy: {}".format(pnsynth.self_loop_policy))

    pnsynth.synthesize()
    #pn = pnsynth.get_pn()
    #init_marking = pnsynth.get_init_marking()
    
    print("The PN is synthesized. Elapsed time: {} ms".format(pnsynth.elapsed))
    
    
    pn = pnsynth.pn
    init_marking = pnsynth.init_marking


    # записывать сетипети в графвиз
    pndw = EventLogPetriNetDotWriter()
    pndw.write_pn(pn_gv_name, pn, init_marking, "")
    #

    a = 42


# Делает для заданного лога (log_name) prefix tree, выводит его картинку 
# в ts_gv_name1, делает ему SAS и выводит картинку еще раз в ts_gv_name2
def make_ts_and_sas(log_name, ts_gv_name1, ts_gv_name2):
    # лог
    log = SQLiteLog(log_name)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    # TS fBuilder
    # pool, state function and builder
    pool = AttrListStateIDsPool()
    fnc = PrefixStateFunc(log, pool, PrefixStateFunc.UNLIM_WND_SIZE)
    bldr = TsBuilder(log, fnc, pool)
    ts1 = bldr.build(False)
        
    # записыватель TS в графвиз
    tsdw = EvLogTSWithFreqsDotWriter()
    tsdw.write_ts(ts_gv_name1, ts1, "Before SAS")

    # делаем SAS
    ts1.make_sas()
    tsdw.write_ts(ts_gv_name2, ts1, "After SAS")




# https://google.github.io/styleguide/pyguide.html?showone=Imports#Main
def main():
    
    # выводим префиксное дерево, его в файл, потом ему SAS, и еще раз в файл
    # make_ts_and_sas(LOG_FILE_RTS_05, TS_RES_GV_05_1, TS_RES_GV_05_2)

    # полный цикл синтеза
    #pn_synth1(LOG_FILE_RTS_05, PN_RES_GV_05)

    # evlog_pn_test1()

    # до доработок 29.08.2018 - табуляторный вывод метрик разнопараметровых
    #reduce_experiment1(LOG_FILE_RTS_04)

    # префикс, редукция и метрики
    evlog_ts_reduce_n_metrics_test1()
    #evlog_ts_reducer_test1()

    #evlog_ts_tests1()
    #evlog_ts_fbuiler_tests1()

    #list_test1()
    #sqlite_log_tests2()
    #sqlite_log_tests1()


#------------------------------------------------------------------------------

if __name__ == "__main__":
    main()