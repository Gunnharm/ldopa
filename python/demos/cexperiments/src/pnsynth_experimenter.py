"""
Experimenter components for synthesizing Petri nets.
"""

import xi.ldopa.eventlog as ld
#import sysx
from enum import Enum
import csv
import time
import datetime
import locale

#------------------------------------------------------------------------------

class Experimenter:
    """Performs complex experiments with TS reduction and PN synthesis."""

    #--------------------------------------------------------------------------
    # Constants and enums
    class TsNum(Enum):
        TS1 = 1
        TS2 = 2
        TS3 = 3

    #--------------------------------------------------------------------------
    # constructor
    def __init__(self, logfn):
        self._log_fn = logfn

    #--------------------------------------------------------------------------
    def retrive_log_pa(self):
        """Retrieves log's primary attributes."""
        # автооткрываем лог, если он еще не открыт
        if self._log == None:
            self._open_log()

        self._log_prim_attrs = (
            self._log.get_activities_num(),
            self._log.get_traces_num(),
            self._log.get_events_num()
            )


    #--------------------------------------------------------------------------
    def mine_prefix_tree(self, wnd_sz):
        """Mines a prefix tree. A log must be open prior to call this function."""

        # автооткрываем лог, если он еще не открыт
        if self._log == None:
            self._open_log()

        self._wnd_sz = wnd_sz

        # TS fBuilder. pool, state function and builder
        #self._pool = ld.AttrListStateIDsPool()
        self._pool = ld.AttrListStateIDsPool()
        self._fnc = ld.PrefixStateFunc(self._log, self._pool, wnd_sz)
        self._pfbuilder = ld.TsBuilder(self._log, self._fnc, self._pool)
        self._ts1 = self._pfbuilder.build(False)


    #--------------------------------------------------------------------------
    def mine_prefix_tree_fix_wnd(self, wnd_sz):
        """Mines a prefix tree with fixed wndsize. A log must be open prior to call this function."""

        # автооткрываем лог, если он еще не открыт
        if self._log == None:
            self._open_log()

        self._wnd_sz = wnd_sz

        # TS fBuilder. pool, state function and builder
        #self._pool = ld.AttrListStateIDsPool()
        self._fnc = ld.PrefixStateFunc(self._log, self._pool, wnd_sz)
        self._pfbuilder = ld.TsBuilder(self._log, self._fnc, self._pool)
        self._ts3 = self._pfbuilder.build(False)

    #--------------------------------------------------------------------------
    def reduce_ts(self, thresh, vwsc):
        """Reduces a transition system obtained at the first stage — prefix tree mining."""

        # если ts1 еще нет, создадим
        if self._ts1 == None:
            raise RuntimeError("TS1 must be synthesized in advance.")

        # condenser
        cond = ld.CondensedTsBuilder(self._ts1)
        self._ts2 = cond.build(thresh)

        # restater
        rest = ld.VarWsTsBuilder(self._log, self._ts2, self._fnc)
        self._ts3 = rest.build(vwsc, ld.VarWsTsBuilder.Zsa.spec_state)

        # параметры сохраняем
        self._vwsc = vwsc
        self._thresh = thresh

        self._red_elapsed = cond.elapsed + rest.elapsed

    #--------------------------------------------------------------------------
    def make_sas(self, which_ts):
        """Create single AS for the ts1 or ts3."""
        if which_ts is Experimenter.TsNum.TS1:
            self._ts1.make_sas()
        elif which_ts is Experimenter.TsNum.TS3:
            self._ts3.make_sas()

    #--------------------------------------------------------------------------
    def calc_metrics(self, which_ts):
        """Calculates metrics for a selected TS."""
        if which_ts is Experimenter.TsNum.TS1:
            ts = self._ts1
        elif which_ts is Experimenter.TsNum.TS2:
            ts = self._ts2
        elif which_ts is Experimenter.TsNum.TS3:
            ts = self._ts3
        else:
            raise RuntimeError("Wrong TS")

        # metrics
        mcalc = ld.TsMetricsCalc(self._log, self._ts1)
        simpl = mcalc.calc_simplicity(ts)
        prec = mcalc.calc_precision(ts)
        gener = mcalc.calc_general(ts)

        self._metrics = (simpl, prec, gener)
        return self._metrics

    #--------------------------------------------------------------------------
    def synth_pn(self, which_ts, make_wf, selfloop_pol):
        """Synthesizes a PN for ts1 or ts3."""

        if which_ts is Experimenter.TsNum.TS1:
            ts = self._ts1
        elif which_ts is Experimenter.TsNum.TS3:
            ts = self._ts3
        else:
            raise RuntimeError("Wrong TS")

        # синтезатор регионами
        self._pnsynth = ld.EvLogPNSynth(ts)
        self._pnsynth.make_wf_net = make_wf
        self._pnsynth.self_loop_policy = selfloop_pol

        self._pnsynth.synthesize()


    #--------------------------------------------------------------------------
    def batch1(self, params):
        """Full pipiline of PN synthesis."""


    #--------------------------------------------------------------------------
    @property
    def log(self):
        return self._log

    #--------------------------------------------------------------------------
    @property
    def log_pa(self):
        return self._log_prim_attrs

    #--------------------------------------------------------------------------
    @property
    def fnc(self):
        return self._fnc

    #--------------------------------------------------------------------------
    @property
    def ts1(self):
        return self._ts1

    #--------------------------------------------------------------------------
    @property
    def ts2(self):
        return self._ts2

    #--------------------------------------------------------------------------
    @property
    def ts3(self):
        return self._ts3

    #--------------------------------------------------------------------------
    @property
    def wnd_size(self):
        return self._wnd_sz

    #--------------------------------------------------------------------------
    @property
    def max_wnd_size(self):

        if self._fnc == None:
            raise RuntimeError("State function is not defined yet. Can't get max wnd size.")

        return self._fnc.max_ws


    #--------------------------------------------------------------------------
    @property
    def threshold(self):
        return self._thresh

    #--------------------------------------------------------------------------
    @property
    def vwsc(self):
        return self._vwsc

    #--------------------------------------------------------------------------
    @property
    def red_elapsed(self):
        return self._red_elapsed


    #--------------------------------------------------------------------------
    @property
    def pn(self):
        return self._pnsynth.pn

    #--------------------------------------------------------------------------
    @property
    def pnsynth(self):
        return self._pnsynth

    #--------------------------------------------------------------------------
    @property
    def pfbuilder(self):
        return self._pfbuilder

    #--------------------------------------------------------------------------
    @property
    def init_marking(self):
        return self._pnsynth.init_marking


    #--------------------------------------------------------------------------
    # protected functions
    def _open_log(self):
        self._log = ld.SQLiteLog(self._log_fn)
        self._log.autoload_conf_qry = "SELECT * FROM DefConfig"
        self._log.open()

    #--------------------------------------------------------------------------
    # protected fields
    _log_fn = ""                    # log file name
    _log = None                     # log object
    _log_prim_attrs = None          # log primary attributes
    _fnc = None                     # prefix id function
    _wnd_sz = 0                     # last wnd size for prefix miner
    _thresh = 0                     # last threshold for stage 2
    _vwsc = 0                       # last treshold for stage 3
    _red_elapsed = 0                # elapsed time for stages 2 and 3 - reduction

    # transition systems
    _ts1 = None                     # prefix tree
    _ts2 = None                     # condensed tree
    _ts3 = None                     # reduced ts

    # временно сохраненные, чтобы объекты не выпадали из области видимости!
    # todo: вообще-то тут можно всяческие custodian-модификаторы попробовать использовать,
    # чтобы зафиксировать время жизни по родительским объектам, но лучше с ними
    # не связываться
    _pool = None
    _pfbuilder = None
    _pnsynth = None
    #_metrics_calc = None            # вычислитель метрик
    _metrics = None                 # сами метрики


#----<class Experimenter>------------------------------------------------------


class ExperimentStepper():
    """ Experiment stepper.
    Attributes:
        _params Defines a dictionary of stepping params:
            log_fn      Log file name.
            csv_fn      Output CSV file name.
            thresh_set  Set (list) of threshold values.
            vwsc_set    Set (list) of VWSC values.
    """

    #--------------------------------------------------------------------------
    @staticmethod
    def prepare_comma_locale():
        # https://stackoverflow.com/questions/7751157/python-csv-list-separator-based-on-regional-settings
        #langlocale = locale.getdefaultlocale()[0]
        locale.setlocale(locale.LC_ALL, '')
        #locale.setlocale(locale.LC_NUMERIC, "en_DK.UTF-8")
        #dec_pt_chr = locale.localeconv()['decimal_point']
        #if dec_pt_chr == ",":
        #    list_delimiter = ";"
        #else:
        #    list_delimiter = ","


    #--------------------------------------------------------------------------
    def series1(self, params):
        """Conduct series 1 of experiments: full cycle with PN synthesis"""
        self.full_series1(params, self.build_step1)

    #--------------------------------------------------------------------------
    def series2(self, params):
        """Conduct series 2 of experiments: short cycle w/o PN synthesis"""
        self.full_series1(params, self.build_step2)

    #--------------------------------------------------------------------------
    def build_step1(self, thres, vwsc):
        """Performs one step, including reduction, metrics calc and PN synthesis"""

        self._xp.reduce_ts(thres, vwsc)
        (ms1, mp1, mg1) = self._xp.calc_metrics(Experimenter.TsNum.TS1)
        (ms3, mp3, mg3) = self._xp.calc_metrics(Experimenter.TsNum.TS3)

        # кошмар! дважды синтезировал PN!!!!
        #self._xp.make_sas(Experimenter.TsNum.TS3)
        #self._xp.synth_pn(Experimenter.TsNum.TS3, True, ld.EvLogPNSynth.SelfLoopPolicy.slProcess)

        # синтез PN
        self._xp.make_sas(Experimenter.TsNum.TS3)
        self._xp.synth_pn(Experimenter.TsNum.TS3, True, ld.EvLogPNSynth.SelfLoopPolicy.slProcess)

        pn = self._xp.pn

        st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')

        #log = self._xp.log
        (log_an, log_tn, log_en) = self._xp.log_pa  # основные атрибуты лога

        # записываем CSV-строку очередную
        csv_row = [
            st,
            self._xp._log_fn, log_an, log_tn, log_en,
            self._xp.pfbuilder.elapsed,
            self._xp.ts1.states_num, self._xp.ts1.trans_num,
            ms1, mp1, mg1,                  # метрики TS1
            thres, vwsc,
            self._xp.red_elapsed,
            self._xp.ts3.states_num, self._xp.ts3.trans_num,
            ms3, mp3, mg3,
            self._xp.pnsynth.elapsed,
            pn.poss_num, pn.transs_num, pn.arcs_num,
            ]
        self._csv_writer.writerow(csv_row)

        self.flush_csv()
        1 #

    #--------------------------------------------------------------------------
    def build_step2(self, thres, vwsc):
        """Performs one step, including reduction and metrics calc (no PN synthesis)"""

        self._xp.reduce_ts(thres, vwsc)
        (ms1, mp1, mg1) = self._xp.calc_metrics(Experimenter.TsNum.TS1)
        (ms3, mp3, mg3) = self._xp.calc_metrics(Experimenter.TsNum.TS3)

        ## синтез PN
        #self._xp.make_sas(Experimenter.TsNum.TS3)
        #self._xp.synth_pn(Experimenter.TsNum.TS3, True, ld.EvLogPNSynth.SelfLoopPolicy.slProcess)

        #pn = self._xp.pn

        st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')

        #log = self._xp.log
        (log_an, log_tn, log_en) = self._xp.log_pa  # основные атрибуты лога

        # записываем CSV-строку очередную
        csv_row = [
            st,
            self._xp._log_fn, log_an, log_tn, log_en,
            self._xp.pfbuilder.elapsed,
            self._xp.ts1.states_num, self._xp.ts1.trans_num,
            ms1, mp1, mg1,                  # метрики TS1
            thres, vwsc,
            self._xp.red_elapsed,
            self._xp.ts3.states_num, self._xp.ts3.trans_num,
            ms3, mp3, mg3,
            #self._xp.pnsynth.elapsed,
            #pn.poss_num, pn.transs_num, pn.arcs_num,
            ]
        self._csv_writer.writerow(csv_row)

        self.flush_csv()
        1 #

    #--------------------------------------------------------------------------
    def full_series1(self, params, step_method):
        """Conduct series 1 of experiments with a step method passed."""
        print("Log file: %s" % params["log_fn"])

        ExperimentStepper.prepare_comma_locale()


        # имя файла либо задано явно, либо определяется каталогом и текущим временем
        if "csv_fn" in params:
            fn = params["csv_fn"]
        else:
            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d-%H-%M-%S')
            fn = params["csv_out_dir"] + st + ".txt"


        print("Prepare CSV output: %s" % fn)
        #with self.open_csv(params["csv_fn"]) as csvfile:
        with self.open_csv(fn) as csvfile:

            print("\nOpen log file and build a prefix tree with UNLIM wnd size")
            self._xp = Experimenter(params["log_fn"])

            # префиксное дерево майним сразу
            self._xp.mine_prefix_tree(ld.PrefixStateFunc.UNLIM_WND_SIZE)
            self._xp.retrive_log_pa()       # основные атрибуты лога

            i = 0

            # степпинг по параметрам
            for thres in params["thresh_set"]:
                for vwsc in params["vwsc_set"]:
                    #self.build_step1(thres, vwsc)
                    step_method(thres, vwsc)
                    i = i + 1
                    print("Step %d: (%f, %f)" % (i, thres, vwsc))


            1 #
        print("Well done!")


    #--------------------------------------------------------------------------
    def prefixtree_fixwnd(self, params):
        """Mine prefix tree with fix wnd size."""
        print("Log file: %s" % params["log_fn"])

        ExperimentStepper.prepare_comma_locale()


        # имя файла либо задано явно, либо определяется каталогом и текущим временем
        if "csv_fn" in params:
            fn = params["csv_fn"]
        else:
            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d-%H-%M-%S')
            fn = params["csv_out_dir"] + st + ".txt"

        wndsz = params["wnd_sz"]

        print("Prepare CSV output: %s" % fn)

        with self.open_csv(fn) as csvfile:

            print("\nOpen log file and build a prefix tree with FIX wnd size = %d" % wndsz)
            self._xp = Experimenter(params["log_fn"])

            # префиксное дерево майним сразу
            self._xp.mine_prefix_tree(ld.PrefixStateFunc.UNLIM_WND_SIZE)

            # с фкисрованным окном
            self._xp.mine_prefix_tree_fix_wnd(wndsz)
            self._xp.retrive_log_pa()       # основные атрибуты лога
            (log_an, log_tn, log_en) = self._xp.log_pa  # основные атрибуты лога

            (ms1, mp1, mg1) = self._xp.calc_metrics(Experimenter.TsNum.TS1)
            (ms3, mp3, mg3) = self._xp.calc_metrics(Experimenter.TsNum.TS3)
            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')

                    # записываем CSV-строку очередную
            csv_row = [
                st,
                self._xp._log_fn, log_an, log_tn, log_en,
                self._xp.pfbuilder.elapsed,
                self._xp.ts1.states_num, self._xp.ts1.trans_num,
                ms1, mp1, mg1,                  # метрики TS1
                0, 0, #thres, vwsc,
                0, #self._xp.red_elapsed,
                self._xp.ts3.states_num, self._xp.ts3.trans_num,
                ms3, mp3, mg3,
                #self._xp.pnsynth.elapsed,
                #pn.poss_num, pn.transs_num, pn.arcs_num,
                ]
            self._csv_writer.writerow(csv_row)

            self.flush_csv()

            1 #
        print("Well done!")


    #--------------------------------------------------------------------------
    def synthpn_by_prefixtree(self, params):
        """Mine the prefix tree and synthesize a PN based on it."""

        print("Mine the prefix tree and synthesize a PN based on it.")
        print("Log file: %s" % params["log_fn"])

        ExperimentStepper.prepare_comma_locale()


        # имя файла либо задано явно, либо определяется каталогом и текущим временем
        if "csv_fn" in params:
            fn = params["csv_fn"]
        else:
            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d-%H-%M-%S')
            fn = params["csv_out_dir"] + st + ".txt"

        #wndsz = params["wnd_sz"]

        print("Prepare CSV output: %s" % fn)

        with self.open_csv(fn) as csvfile:

            print("\nOpen log file and build a prefix tree with UNLIM wnd size")
            self._xp = Experimenter(params["log_fn"])

            # префиксное дерево майним сразу
            self._xp.mine_prefix_tree(ld.PrefixStateFunc.UNLIM_WND_SIZE)
            self._xp.retrive_log_pa()       # основные атрибуты лога
            (log_an, log_tn, log_en) = self._xp.log_pa  # основные атрибуты лога
            (ms1, mp1, mg1) = self._xp.calc_metrics(Experimenter.TsNum.TS1)
            #(ms3, mp3, mg3) = self._xp.calc_metrics(Experimenter.TsNum.TS3)

            # синтез PN
            print("\nMake SAS and synth PN")
            self._xp.make_sas(Experimenter.TsNum.TS1)       # здесь мы чуть испортим префиксное дерево
            self._xp.synth_pn(Experimenter.TsNum.TS1, True, ld.EvLogPNSynth.SelfLoopPolicy.slProcess)

            pn = self._xp.pn

            st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')

          # записываем CSV-строку очередную
            csv_row = [
                st,
                self._xp._log_fn, log_an, log_tn, log_en,
                self._xp.pfbuilder.elapsed,
                self._xp.ts1.states_num, self._xp.ts1.trans_num,
                ms1, mp1, mg1,                  # метрики TS1
                0, 0,   #thres, vwsc,
                0,      # self._xp.red_elapsed,
                0, 0,   # self._xp.ts3.states_num, self._xp.ts3.trans_num,
                0, 0, 0,# ms3, mp3, mg3,
                self._xp.pnsynth.elapsed,
                pn.poss_num, pn.transs_num, pn.arcs_num,
            ]

            self._csv_writer.writerow(csv_row)

            self.flush_csv()

            1 #
        print("Well done!")




    #--------------------------------------------------------------------------
    def open_csv(self, csv_fn):
        """Opens a CSV file to output results."""

        self._csv_file = open(csv_fn, "w", newline='')
        self._csv_writer = csv.writer(self._csv_file, dialect='excel', delimiter = '\t')
        cw = self._csv_writer

        # пишем заголовок
        cw.writerow(["Timestamp",
                     "Log file", "Activities #", "Traces #", "Events #",
                     "TS1 build time (ms)",
                     "TS1 states #", "TS1 trans #",
                     "TS1 simpl", "TS1 prec", "TS1 gener",
                     "threshold", "VWSC",
                     "TS2 + TS3 build time (ms)",
                     "TS3 states #", "TS3 trans #",
                     "TS3 simpl", "TS3 prec", "TS3 gener",
                     "PN synth time (ms)",
                     "PN pos #", "PN trans #", "PN acrs #",
                     ])

        self.flush_csv()

        return self._csv_file


    #--------------------------------------------------------------------------
    def flush_csv(self):
        """Flushes CSV file to a file buffer."""
        self._csv_file.flush()

    #--------------------------------------------------------------------------
    #def close_csv(self):
    #    """Closes the opened CSV file."""

    #    if self._csv_file != None:
    #        self._csv_file.close()



    #--------------------------------------------------------------------------


    # dictionary of params:
    _params = None              # исходные параметры
    _xp = None                  # экспериментер
    _csv_file = None            # CSV file
    _csv_writer = None


#----<class ExperimentStepper>-------------------------------------------------



def build_prefixtree_fixwnd(fn, wndsz):
    """Control function, which builds a ts with fixed wnd size."""

    log = ld.SQLiteLog(fn)
    log.autoload_conf_qry = "SELECT * FROM DefConfig"
    log.open()

    pool = ld.AttrListStateIDsPool()
    fnc = ld.PrefixStateFunc(log, pool, -1)
    pfbuilder = ld.TsBuilder(log, fnc, pool)
    ts_full = pfbuilder.build(False)

    fnc = ld.PrefixStateFunc(log, pool, wndsz)
    pfbuilder = ld.TsBuilder(log, fnc, pool)
    ts = pfbuilder.build(False)

    mcalc = ld.TsMetricsCalc(log, ts_full)
    simpl = mcalc.calc_simplicity(ts)
    prec = mcalc.calc_precision(ts)
    gener = mcalc.calc_general(ts)

    1 #





