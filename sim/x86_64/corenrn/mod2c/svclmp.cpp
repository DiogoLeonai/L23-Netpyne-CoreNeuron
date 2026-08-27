/*********************************************************
Model Name      : SEClamp
Filename        : svclmp.mod
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
        "SEClamp",
        "rs",
        "dur1",
        "amp1",
        "dur2",
        "amp2",
        "dur3",
        "amp3",
        0,
        "i",
        "vc",
        0,
        0,
        0
    };


    /** all global variables */
    struct SEClamp_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<SEClamp_Store>);
    static_assert(std::is_trivially_move_constructible_v<SEClamp_Store>);
    static_assert(std::is_trivially_copy_assignable_v<SEClamp_Store>);
    static_assert(std::is_trivially_move_assignable_v<SEClamp_Store>);
    static_assert(std::is_trivially_destructible_v<SEClamp_Store>);
    static SEClamp_Store SEClamp_global;


    /** all mechanism instance variables and global variables */
    struct SEClamp_Instance  {
        const double* rs{};
        const double* dur1{};
        const double* amp1{};
        const double* dur2{};
        const double* amp2{};
        const double* dur3{};
        const double* amp3{};
        double* i{};
        double* vc{};
        double* tc2{};
        double* tc3{};
        double* on{};
        double* v_unused{};
        double* g_unused{};
        const double* node_area{};
        const int* point_process{};
        SEClamp_Store* global{&SEClamp_global};
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
        return 14;
    }


    static inline int int_variables_size() {
        return 2;
    }


    static inline int get_mech_type() {
        return SEClamp_global.mech_type;
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
    static void nrn_private_constructor_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new SEClamp_Instance{};
        assert(inst->global == &SEClamp_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(SEClamp_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, SEClamp_Instance const* inst);
    static inline void delete_instance_from_device(SEClamp_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &SEClamp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(SEClamp_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &SEClamp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(SEClamp_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->rs = ml->data+0*pnodecount;
        inst->dur1 = ml->data+1*pnodecount;
        inst->amp1 = ml->data+2*pnodecount;
        inst->dur2 = ml->data+3*pnodecount;
        inst->amp2 = ml->data+4*pnodecount;
        inst->dur3 = ml->data+5*pnodecount;
        inst->amp3 = ml->data+6*pnodecount;
        inst->i = ml->data+7*pnodecount;
        inst->vc = ml->data+8*pnodecount;
        inst->tc2 = ml->data+9*pnodecount;
        inst->tc3 = ml->data+10*pnodecount;
        inst->on = ml->data+11*pnodecount;
        inst->v_unused = ml->data+12*pnodecount;
        inst->g_unused = ml->data+13*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = ml->pdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, SEClamp_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.rs = cnrn_target_deviceptr(tmp.rs);
        tmp.dur1 = cnrn_target_deviceptr(tmp.dur1);
        tmp.amp1 = cnrn_target_deviceptr(tmp.amp1);
        tmp.dur2 = cnrn_target_deviceptr(tmp.dur2);
        tmp.amp2 = cnrn_target_deviceptr(tmp.amp2);
        tmp.dur3 = cnrn_target_deviceptr(tmp.dur3);
        tmp.amp3 = cnrn_target_deviceptr(tmp.amp3);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.vc = cnrn_target_deviceptr(tmp.vc);
        tmp.tc2 = cnrn_target_deviceptr(tmp.tc2);
        tmp.tc3 = cnrn_target_deviceptr(tmp.tc3);
        tmp.on = cnrn_target_deviceptr(tmp.on);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.node_area = cnrn_target_deviceptr(tmp.node_area);
        tmp.point_process = cnrn_target_deviceptr(tmp.point_process);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(SEClamp_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_SEClamp(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);

        #endif
    }


    inline static int icur_SEClamp(int id, int pnodecount, SEClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);
    inline static int vstim_SEClamp(int id, int pnodecount, SEClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int icur_SEClamp(int id, int pnodecount, SEClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_icur = 0;
        if (inst->on[id]) {
            inst->i[id] = (inst->vc[id] - v) / inst->rs[id];
        } else {
            inst->i[id] = 0.0;
        }
        return ret_icur;
    }


    inline int vstim_SEClamp(int id, int pnodecount, SEClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_vstim = 0;
        inst->on[id] = 1.0;
        if (inst->dur1[id]) {
            at_time(nt, inst->dur1[id]);
        }
        if (inst->dur2[id]) {
            at_time(nt, inst->tc2[id]);
        }
        if (inst->dur3[id]) {
            at_time(nt, inst->tc3[id]);
        }
        if (nt->_t < inst->dur1[id]) {
            inst->vc[id] = inst->amp1[id];
        } else if (nt->_t < inst->tc2[id]) {
            inst->vc[id] = inst->amp2[id];
        } else if (nt->_t < inst->tc3[id]) {
            inst->vc[id] = inst->amp3[id];
        } else {
            inst->vc[id] = 0.0;
            inst->on[id] = 0.0;
        }
        {
            if (inst->on[id]) {
                inst->i[id] = (inst->vc[id] - v) / inst->rs[id];
            } else {
                inst->i[id] = 0.0;
            }
        }
        return ret_vstim;
    }


    /** initialize channel */
    void nrn_init_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (SEClamp_global))
                nrn_pragma_omp(target update to(SEClamp_global))
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
                    inst->tc2[id] = inst->dur1[id] + inst->dur2[id];
                    inst->tc3[id] = inst->tc2[id] + inst->dur3[id];
                    inst->on[id] = 0.0;
                }
            }
        }
    }


    inline double nrn_current_SEClamp(int id, int pnodecount, SEClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        {
            inst->on[id] = 1.0;
            if (inst->dur1[id]) {
                at_time(nt, inst->dur1[id]);
            }
            if (inst->dur2[id]) {
                at_time(nt, inst->tc2[id]);
            }
            if (inst->dur3[id]) {
                at_time(nt, inst->tc3[id]);
            }
            if (nt->_t < inst->dur1[id]) {
                inst->vc[id] = inst->amp1[id];
            } else if (nt->_t < inst->tc2[id]) {
                inst->vc[id] = inst->amp2[id];
            } else if (nt->_t < inst->tc3[id]) {
                inst->vc[id] = inst->amp3[id];
            } else {
                inst->vc[id] = 0.0;
                inst->on[id] = 0.0;
            }
            {
                if (inst->on[id]) {
                    inst->i[id] = (inst->vc[id] - v) / inst->rs[id];
                } else {
                    inst->i[id] = 0.0;
                }
            }
        }
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_SEClamp(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_SEClamp(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                double mfactor = 1.e2/inst->node_area[indexes[0*pnodecount + id]];
                g = g*mfactor;
                rhs = rhs*mfactor;
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_rhs[node_id] += rhs;
                nrn_pragma_acc(atomic update)
                nrn_pragma_omp(atomic update)
                vec_d[node_id] -= g;
                if (nt->nrn_fast_imem) {
                    nrn_pragma_acc(atomic update)
                    nrn_pragma_omp(atomic update)
                    nt->nrn_fast_imem->nrn_sav_rhs[node_id] += rhs;
                    nrn_pragma_acc(atomic update)
                    nrn_pragma_omp(atomic update)
                    nt->nrn_fast_imem->nrn_sav_d[node_id] -= g;
                }
            }
        }
    }


    /** update state */
    void nrn_state_SEClamp(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<SEClamp_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                icur_SEClamp(id, pnodecount, inst, data, indexes, thread, nt, v);
            }
        }
    }


    /** register channel with the simulator */
    void _svclmp_reg() {

        int mech_type = nrn_get_mechtype("SEClamp");
        SEClamp_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_SEClamp, nrn_cur_SEClamp, nullptr, nrn_state_SEClamp, nrn_init_SEClamp, nrn_private_constructor_SEClamp, nrn_private_destructor_SEClamp, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
