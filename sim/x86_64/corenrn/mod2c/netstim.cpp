/*********************************************************
Model Name      : NetStim
Filename        : netstim.mod
NMODL Version   : 7.7.0
Vectorized      : true
Threadsafe      : true
Created         : Thu Aug 27 09:07:35 2026
Simulator       : CoreNEURON
Backend         : C++-OpenAcc (api-compatibility)
NMODL Compiler  : 9.0.1 []
*********************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coreneuron/utils/offload.hpp>
#include <cuda_runtime_api.h>

#include <coreneuron/gpu/nrn_acc_manager.hpp>
#include <coreneuron/mechanism/mech/mod2c_core_thread.hpp>
#include <coreneuron/mechanism/register_mech.hpp>
#include <coreneuron/nrnconf.h>
#include <coreneuron/nrniv/nrniv_decl.h>
#include <coreneuron/sim/multicore.hpp>
#include <coreneuron/sim/scopmath/newton_thread.hpp>
#include <coreneuron/utils/ivocvect.hpp>
#include <coreneuron/utils/nrnoc_aux.hpp>
#include <coreneuron/utils/randoms/nrnran123.h>


namespace coreneuron {
    #ifndef NRN_PRCELLSTATE
    #define NRN_PRCELLSTATE 0
    #endif


    /** channel information */
    static const char *mechanism_info[] = {
        "7.7.0",
        "NetStim",
        "interval",
        "number",
        "start",
        "noise",
        0,
        0,
        0,
        0
    };


    /** all global variables */
    struct NetStim_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<NetStim_Store>);
    static_assert(std::is_trivially_move_constructible_v<NetStim_Store>);
    static_assert(std::is_trivially_copy_assignable_v<NetStim_Store>);
    static_assert(std::is_trivially_move_assignable_v<NetStim_Store>);
    static_assert(std::is_trivially_destructible_v<NetStim_Store>);
    static NetStim_Store NetStim_global;


    /** all mechanism instance variables and global variables */
    struct NetStim_Instance  {
        const double* interval{};
        const double* number{};
        const double* start{};
        double* noise{};
        double* event{};
        double* on{};
        double* ispike{};
        double* v_unused{};
        double* tsave{};
        const double* node_area{};
        void** point_process{};
        void** ranvar{};
        void** tqitem{};
        NetStim_Store* global{&NetStim_global};
    };


    /** connect global (scalar) variables to hoc -- */
    static DoubScal hoc_scalar_double[] = {
        {nullptr, nullptr}
    };


    /** connect global (array) variables to hoc -- */
    static DoubVec hoc_vector_double[] = {
        {nullptr, nullptr, 0}
    };


    static inline int first_pointer_var_index() {
        return -1;
    }


    static inline int first_random_var_index() {
        return 2;
    }


    static inline int num_net_receive_args() {
        return 1;
    }


    static inline int float_variables_size() {
        return 9;
    }


    static inline int int_variables_size() {
        return 4;
    }


    static inline int get_mech_type() {
        return NetStim_global.mech_type;
    }


    static inline Memb_list* get_memb_list(NrnThread* nt) {
        if (!nt->_ml_list) {
            return nullptr;
        }
        return nt->_ml_list[get_mech_type()];
    }


    static inline void* mem_alloc(size_t num, size_t size, size_t alignment = 64) {
        size_t aligned_size = ((num*size + alignment - 1) / alignment) * alignment;
        void* ptr = aligned_alloc(alignment, aligned_size);
        memset(ptr, 0, aligned_size);
        return ptr;
    }


    static inline void mem_free(void* ptr) {
        free(ptr);
    }


    static inline void coreneuron_abort() {
        printf("Error : Issue while running OpenACC kernel \n");
        assert(0==1);
    }

    // Allocate instance structure
    static void nrn_private_constructor_NetStim(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new NetStim_Instance{};
        assert(inst->global == &NetStim_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(NetStim_Store);
    }

    // Deallocate the instance structure
    static void nrn_private_destructor_NetStim(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &NetStim_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(NetStim_Store));
        int pnodecount = ml->_nodecount_padded;
        int nodecount = ml->nodecount;
        Datum* indexes = ml->pdata;
        for (int id = 0; id < nodecount; id++) {
            nrnran123_deletestream((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
        }
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &NetStim_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(NetStim_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->interval = ml->data+0*pnodecount;
        inst->number = ml->data+1*pnodecount;
        inst->start = ml->data+2*pnodecount;
        inst->noise = ml->data+3*pnodecount;
        inst->event = ml->data+4*pnodecount;
        inst->on = ml->data+5*pnodecount;
        inst->ispike = ml->data+6*pnodecount;
        inst->v_unused = ml->data+7*pnodecount;
        inst->tsave = ml->data+8*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = nt->_vdata;
        inst->ranvar = nt->_vdata;
        inst->tqitem = nt->_vdata;
    }



    static void nrn_alloc_NetStim(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_NetStim(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_NetStim(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);

        #endif
    }


    inline static double invl_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lmean);
    inline static double erand_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
    inline static int init_sequence_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lt);
    inline static int next_invl_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
    inline static int seed_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx);
    inline static int noiseFromRandom_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
    inline static int noiseFromRandom123_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int init_sequence_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lt) {
        int ret_init_sequence = 0;
        if (inst->number[id] > 0.0) {
            inst->on[id] = 1.0;
            inst->event[id] = 0.0;
            inst->ispike[id] = 0.0;
        }
        return ret_init_sequence;
    }


    inline int next_invl_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_next_invl = 0;
        if (inst->number[id] > 0.0) {
            double invl_in_1;
            {
                double _lmean_in_1;
                _lmean_in_1 = inst->interval[id];
                if (_lmean_in_1 <= 0.0) {
                    _lmean_in_1 = .01;
                }
                if (inst->noise[id] == 0.0) {
                    invl_in_1 = _lmean_in_1;
                } else {
                    double erand_in_0;
                    {
                        erand_in_0 = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
                    }
                    invl_in_1 = (1.0 - inst->noise[id]) * _lmean_in_1 + inst->noise[id] * _lmean_in_1 * erand_in_0;
                }
            }
            inst->event[id] = invl_in_1;
        }
        if (inst->ispike[id] >= inst->number[id]) {
            inst->on[id] = 0.0;
        }
        return ret_next_invl;
    }


    inline int seed_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx) {
        int ret_seed = 0;
        nrnran123_setseq((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], _lx);
        return ret_seed;
    }


    inline int noiseFromRandom_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_noiseFromRandom = 0;
        // VERBATIM
        #if !NRNBBCORE
         {
            if (ifarg(1)) {
                Rand* r = nrn_random_arg(1);
                uint32_t id[3];
                if (!nrn_random_isran123(r, &id[0], &id[1], &id[2])) {
                    hoc_execerr_ext("NetStim: Random.Random123 generator is required.");
                }
                nrnran123_setids((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], id[0], id[1], id[2]);
                char which;
                nrn_random123_getseq(r, &id[0], &which);
                nrnran123_setseq((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], id[0], which);
            }
         }
        #endif
        // ENDVERBATIM

        return ret_noiseFromRandom;
    }


    inline int noiseFromRandom123_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_noiseFromRandom123 = 0;
        // VERBATIM
        #if !NRNBBCORE
            if (ifarg(3)) {
                nrnran123_setids((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], static_cast<uint32_t>(*getarg(1)), static_cast<uint32_t>(*getarg(2)), static_cast<uint32_t>(*getarg(3)));
            } else if (ifarg(2)) {
                nrnran123_setids((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], static_cast<uint32_t>(*getarg(1)), static_cast<uint32_t>(*getarg(2)), 0);
            }
            nrnran123_setseq((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], 0, 0);
        #endif
        // ENDVERBATIM

        return ret_noiseFromRandom123;
    }


    inline double invl_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lmean) {
        double ret_invl = 0.0;
        if (_lmean <= 0.0) {
            _lmean = .01;
        }
        if (inst->noise[id] == 0.0) {
            ret_invl = _lmean;
        } else {
            double erand_in_0;
            {
                erand_in_0 = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
            }
            ret_invl = (1.0 - inst->noise[id]) * _lmean + inst->noise[id] * _lmean * erand_in_0;
        }
        return ret_invl;
    }


    inline double erand_NetStim(int id, int pnodecount, NetStim_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double ret_erand = 0.0;
        ret_erand = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
        return ret_erand;
    }


    static inline void net_receive_NetStim(Point_process* pnt, int weight_index, double flag) {
        int tid = pnt->_tid;
        int id = pnt->_i_instance;
        double v = 0;
        NrnThread* nt = nrn_threads + tid;
        Memb_list* ml = nt->_ml_list[pnt->_type];
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        double* data = ml->data;
        double* weights = nt->weights;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);

        double* w = weights + weight_index + 0;
        double t = nt->_t;
        inst->tsave[id] = t;
        {
            if (flag == 0.0) {
                if ((*w) > 0.0 && inst->on[id] == 0.0) {
                    {
                        double _lt_in_0;
                        _lt_in_0 = t;
                        if (inst->number[id] > 0.0) {
                            inst->on[id] = 1.0;
                            inst->event[id] = 0.0;
                            inst->ispike[id] = 0.0;
                        }
                    }
                    {
                        if (inst->number[id] > 0.0) {
                            double invl_in_1;
                            {
                                double _lmean_in_1;
                                _lmean_in_1 = inst->interval[id];
                                if (_lmean_in_1 <= 0.0) {
                                    _lmean_in_1 = .01;
                                }
                                if (inst->noise[id] == 0.0) {
                                    invl_in_1 = _lmean_in_1;
                                } else {
                                    double erand_in_0;
                                    {
                                        erand_in_0 = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
                                    }
                                    invl_in_1 = (1.0 - inst->noise[id]) * _lmean_in_1 + inst->noise[id] * _lmean_in_1 * erand_in_0;
                                }
                            }
                            inst->event[id] = invl_in_1;
                        }
                        if (inst->ispike[id] >= inst->number[id]) {
                            inst->on[id] = 0.0;
                        }
                    }
                    inst->event[id] = inst->event[id] - inst->interval[id] * (1.0 - inst->noise[id]);
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], weight_index, pnt, nt->_t+inst->event[id], 1.0);
                } else if ((*w) < 0.0) {
                    inst->on[id] = 0.0;
                }
            }
            if (flag == 3.0) {
                if (inst->on[id] == 1.0) {
                    {
                        double _lt_in_1;
                        _lt_in_1 = t;
                        if (inst->number[id] > 0.0) {
                            inst->on[id] = 1.0;
                            inst->event[id] = 0.0;
                            inst->ispike[id] = 0.0;
                        }
                    }
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], weight_index, pnt, nt->_t+0.0, 1.0);
                }
            }
            if (flag == 1.0 && inst->on[id] == 1.0) {
                inst->ispike[id] = inst->ispike[id] + 1.0;
                net_event(pnt, t);
                {
                    if (inst->number[id] > 0.0) {
                        double invl_in_1;
                        {
                            double _lmean_in_1;
                            _lmean_in_1 = inst->interval[id];
                            if (_lmean_in_1 <= 0.0) {
                                _lmean_in_1 = .01;
                            }
                            if (inst->noise[id] == 0.0) {
                                invl_in_1 = _lmean_in_1;
                            } else {
                                double erand_in_0;
                                {
                                    erand_in_0 = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
                                }
                                invl_in_1 = (1.0 - inst->noise[id]) * _lmean_in_1 + inst->noise[id] * _lmean_in_1 * erand_in_0;
                            }
                        }
                        inst->event[id] = invl_in_1;
                    }
                    if (inst->ispike[id] >= inst->number[id]) {
                        inst->on[id] = 0.0;
                    }
                }
                if (inst->on[id] == 1.0) {
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], weight_index, pnt, nt->_t+inst->event[id], 1.0);
                }
            }
        }
    }


    /** initialize channel */
    void nrn_init_NetStim(NrnThread* nt, Memb_list* ml, int type) {
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;

        setup_instance(nt, ml);
        auto* const inst = static_cast<NetStim_Instance*>(ml->instance);

        if (_nrn_skip_initmodel == 0) {
            for (int id = 0; id < nodecount; id++) {
                inst->tsave[id] = -1e20;
                double v = 0.0;
                {
                    double _lx_in_0;
                    _lx_in_0 = 0.0;
                    nrnran123_setseq((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]], _lx_in_0);
                }
                inst->on[id] = 0.0;
                inst->ispike[id] = 0.0;
                if (inst->noise[id] < 0.0) {
                    inst->noise[id] = 0.0;
                }
                if (inst->noise[id] > 1.0) {
                    inst->noise[id] = 1.0;
                }
                if (inst->start[id] >= 0.0 && inst->number[id] > 0.0) {
                    double invl_in_0;
                    inst->on[id] = 1.0;
                    {
                        double _lmean_in_0;
                        _lmean_in_0 = inst->interval[id];
                        if (_lmean_in_0 <= 0.0) {
                            _lmean_in_0 = .01;
                        }
                        if (inst->noise[id] == 0.0) {
                            invl_in_0 = _lmean_in_0;
                        } else {
                            double erand_in_0;
                            {
                                erand_in_0 = nrnran123_negexp((nrnran123_State*) inst->ranvar[indexes[2*pnodecount + id]]);
                            }
                            invl_in_0 = (1.0 - inst->noise[id]) * _lmean_in_0 + inst->noise[id] * _lmean_in_0 * erand_in_0;
                        }
                    }
                    inst->event[id] = inst->start[id] + invl_in_0 - inst->interval[id] * (1.0 - inst->noise[id]);
                    if (inst->event[id] < 0.0) {
                        inst->event[id] = 0.0;
                    }
                    artcell_net_send(&inst->tqitem[indexes[3*pnodecount + id]], 0, (Point_process*)inst->point_process[indexes[1*pnodecount + id]], nt->_t+inst->event[id], 3.0);
                }
            }
        }
    }


    /** register channel with the simulator */
    void _netstim_reg() {

        int mech_type = nrn_get_mechtype("NetStim");
        NetStim_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_NetStim, nullptr, nullptr, nullptr, nrn_init_NetStim, nrn_private_constructor_NetStim, nrn_private_destructor_NetStim, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_dparam_semantics(mech_type, 2, "random");
        hoc_register_dparam_semantics(mech_type, 3, "netsend");
        add_nrn_has_net_event(mech_type);
        add_nrn_artcell(mech_type, 3);
        set_pnt_receive(mech_type, net_receive_NetStim, nullptr, num_net_receive_args());
        hoc_register_net_send_buffering(mech_type);
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
