/*********************************************************
Model Name      : epsp
Filename        : epsp.mod
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
        "epsp",
        "onset",
        "tau0",
        "tau1",
        "imax",
        0,
        "i",
        "myv",
        0,
        0,
        0
    };


    /** all global variables */
    struct epsp_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<epsp_Store>);
    static_assert(std::is_trivially_move_constructible_v<epsp_Store>);
    static_assert(std::is_trivially_copy_assignable_v<epsp_Store>);
    static_assert(std::is_trivially_move_assignable_v<epsp_Store>);
    static_assert(std::is_trivially_destructible_v<epsp_Store>);
    static epsp_Store epsp_global;


    /** all mechanism instance variables and global variables */
    struct epsp_Instance  {
        const double* onset{};
        const double* tau0{};
        const double* tau1{};
        const double* imax{};
        double* i{};
        double* myv{};
        double* v_unused{};
        double* g_unused{};
        const double* node_area{};
        const int* point_process{};
        epsp_Store* global{&epsp_global};
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
        return -1;
    }


    static inline int float_variables_size() {
        return 8;
    }


    static inline int int_variables_size() {
        return 2;
    }


    static inline int get_mech_type() {
        return epsp_global.mech_type;
    }


    static inline Memb_list* get_memb_list(NrnThread* nt) {
        if (!nt->_ml_list) {
            return nullptr;
        }
        return nt->_ml_list[get_mech_type()];
    }


    static inline void* mem_alloc(size_t num, size_t size, size_t alignment = 16) {
        void* ptr;
        cudaMallocManaged(&ptr, num*size);
        cudaMemset(ptr, 0, num*size);
        return ptr;
    }


    static inline void mem_free(void* ptr) {
        cudaFree(ptr);
    }


    static inline void coreneuron_abort() {
        printf("Error : Issue while running OpenACC kernel \n");
        assert(0==1);
    }

    // Allocate instance structure
    static void nrn_private_constructor_epsp(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new epsp_Instance{};
        assert(inst->global == &epsp_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(epsp_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, epsp_Instance const* inst);
    static inline void delete_instance_from_device(epsp_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_epsp(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<epsp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &epsp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(epsp_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<epsp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &epsp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(epsp_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->onset = ml->data+0*pnodecount;
        inst->tau0 = ml->data+1*pnodecount;
        inst->tau1 = ml->data+2*pnodecount;
        inst->imax = ml->data+3*pnodecount;
        inst->i = ml->data+4*pnodecount;
        inst->myv = ml->data+5*pnodecount;
        inst->v_unused = ml->data+6*pnodecount;
        inst->g_unused = ml->data+7*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = ml->pdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, epsp_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.onset = cnrn_target_deviceptr(tmp.onset);
        tmp.tau0 = cnrn_target_deviceptr(tmp.tau0);
        tmp.tau1 = cnrn_target_deviceptr(tmp.tau1);
        tmp.imax = cnrn_target_deviceptr(tmp.imax);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.myv = cnrn_target_deviceptr(tmp.myv);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.node_area = cnrn_target_deviceptr(tmp.node_area);
        tmp.point_process = cnrn_target_deviceptr(tmp.point_process);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(epsp_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_epsp(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_epsp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<epsp_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_epsp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<epsp_Instance*>(ml->instance);

        #endif
    }


    inline static double myexp_epsp(int id, int pnodecount, epsp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx);
    inline static double curr_epsp(int id, int pnodecount, epsp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx);


    inline double myexp_epsp(int id, int pnodecount, epsp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx) {
        double ret_myexp = 0.0;
        if (_lx <  -100.0) {
            ret_myexp = 0.0;
        } else {
            ret_myexp = exp(_lx);
        }
        return ret_myexp;
    }


    inline double curr_epsp(int id, int pnodecount, epsp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lx) {
        double ret_curr = 0.0;
        double tpeak, adjust, amp, myexp_in_0, myexp_in_1;
        tpeak = inst->tau0[id] * inst->tau1[id] * log(inst->tau0[id] / inst->tau1[id]) / (inst->tau0[id] - inst->tau1[id]);
        {
            double _lx_in_0;
            _lx_in_0 =  -tpeak / inst->tau0[id];
            if (_lx_in_0 <  -100.0) {
                myexp_in_0 = 0.0;
            } else {
                myexp_in_0 = exp(_lx_in_0);
            }
        }
        {
            double _lx_in_1;
            _lx_in_1 =  -tpeak / inst->tau1[id];
            if (_lx_in_1 <  -100.0) {
                myexp_in_1 = 0.0;
            } else {
                myexp_in_1 = exp(_lx_in_1);
            }
        }
        adjust = 1.0 / ((1.0 - myexp_in_0) - (1.0 - myexp_in_1));
        amp = adjust * inst->imax[id];
        if (_lx < inst->onset[id]) {
            ret_curr = 0.0;
        } else {
            double myexp_in_2, myexp_in_3;
            {
                double _lx_in_2;
                _lx_in_2 =  -(_lx - inst->onset[id]) / inst->tau0[id];
                if (_lx_in_2 <  -100.0) {
                    myexp_in_2 = 0.0;
                } else {
                    myexp_in_2 = exp(_lx_in_2);
                }
            }
            {
                double _lx_in_3;
                _lx_in_3 =  -(_lx - inst->onset[id]) / inst->tau1[id];
                if (_lx_in_3 <  -100.0) {
                    myexp_in_3 = 0.0;
                } else {
                    myexp_in_3 = exp(_lx_in_3);
                }
            }
            ret_curr =  -amp * ((1.0 - myexp_in_2) - (1.0 - myexp_in_3));
        }
        return ret_curr;
    }


    /** initialize channel */
    void nrn_init_epsp(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;

            setup_instance(nt, ml);
            auto* const inst = static_cast<epsp_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (epsp_global))
                nrn_pragma_omp(target update to(epsp_global))
            }
            if (_nrn_skip_initmodel == 0) {
                nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
                nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
                for (int id = 0; id < nodecount; id++) {
                    int node_id = node_index[id];
                    double v = voltage[node_id];
                    #if NRN_PRCELLSTATE
                    inst->v_unused[id] = v;
                    #endif
                }
            }
        }
    }


    inline double nrn_current_epsp(int id, int pnodecount, epsp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        double curr_in_0;
        inst->myv[id] = v;
        {
            double tpeak, adjust, amp, myexp_in_0, myexp_in_1, _lx_in_4;
            _lx_in_4 = nt->_t;
            tpeak = inst->tau0[id] * inst->tau1[id] * log(inst->tau0[id] / inst->tau1[id]) / (inst->tau0[id] - inst->tau1[id]);
            {
                double _lx_in_0;
                _lx_in_0 =  -tpeak / inst->tau0[id];
                if (_lx_in_0 <  -100.0) {
                    myexp_in_0 = 0.0;
                } else {
                    myexp_in_0 = exp(_lx_in_0);
                }
            }
            {
                double _lx_in_1;
                _lx_in_1 =  -tpeak / inst->tau1[id];
                if (_lx_in_1 <  -100.0) {
                    myexp_in_1 = 0.0;
                } else {
                    myexp_in_1 = exp(_lx_in_1);
                }
            }
            adjust = 1.0 / ((1.0 - myexp_in_0) - (1.0 - myexp_in_1));
            amp = adjust * inst->imax[id];
            if (_lx_in_4 < inst->onset[id]) {
                curr_in_0 = 0.0;
            } else {
                double myexp_in_2, myexp_in_3;
                {
                    double _lx_in_2;
                    _lx_in_2 =  -(_lx_in_4 - inst->onset[id]) / inst->tau0[id];
                    if (_lx_in_2 <  -100.0) {
                        myexp_in_2 = 0.0;
                    } else {
                        myexp_in_2 = exp(_lx_in_2);
                    }
                }
                {
                    double _lx_in_3;
                    _lx_in_3 =  -(_lx_in_4 - inst->onset[id]) / inst->tau1[id];
                    if (_lx_in_3 <  -100.0) {
                        myexp_in_3 = 0.0;
                    } else {
                        myexp_in_3 = exp(_lx_in_3);
                    }
                }
                curr_in_0 =  -amp * ((1.0 - myexp_in_2) - (1.0 - myexp_in_3));
            }
        }
        inst->i[id] = curr_in_0;
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_epsp(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            double* vec_rhs = nt->_actual_rhs;
            double* vec_d = nt->_actual_d;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<epsp_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_epsp(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_epsp(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                double mfactor = 1.e2/inst->node_area[indexes[0*pnodecount + id]];
                g = g*mfactor;
                rhs = rhs*mfactor;
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_rhs[node_id] -= rhs;
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_epsp(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<epsp_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
            }
        }
    }


    /** register channel with the simulator */
    void _epsp_reg() {

        int mech_type = nrn_get_mechtype("epsp");
        epsp_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_epsp, nrn_cur_epsp, nullptr, nrn_state_epsp, nrn_init_epsp, nrn_private_constructor_epsp, nrn_private_destructor_epsp, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
