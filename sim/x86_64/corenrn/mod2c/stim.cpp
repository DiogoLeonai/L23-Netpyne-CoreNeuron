/*********************************************************
Model Name      : IClamp
Filename        : stim.mod
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
        "IClamp",
        "del",
        "dur",
        "amp",
        0,
        "i",
        0,
        0,
        0
    };


    /** all global variables */
    struct IClamp_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<IClamp_Store>);
    static_assert(std::is_trivially_move_constructible_v<IClamp_Store>);
    static_assert(std::is_trivially_copy_assignable_v<IClamp_Store>);
    static_assert(std::is_trivially_move_assignable_v<IClamp_Store>);
    static_assert(std::is_trivially_destructible_v<IClamp_Store>);
    static IClamp_Store IClamp_global;


    /** all mechanism instance variables and global variables */
    struct IClamp_Instance  {
        const double* del{};
        const double* dur{};
        const double* amp{};
        double* i{};
        double* v_unused{};
        double* g_unused{};
        const double* node_area{};
        const int* point_process{};
        IClamp_Store* global{&IClamp_global};
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
        return 6;
    }


    static inline int int_variables_size() {
        return 2;
    }


    static inline int get_mech_type() {
        return IClamp_global.mech_type;
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
    static void nrn_private_constructor_IClamp(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new IClamp_Instance{};
        assert(inst->global == &IClamp_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(IClamp_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, IClamp_Instance const* inst);
    static inline void delete_instance_from_device(IClamp_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_IClamp(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<IClamp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &IClamp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(IClamp_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<IClamp_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &IClamp_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(IClamp_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->del = ml->data+0*pnodecount;
        inst->dur = ml->data+1*pnodecount;
        inst->amp = ml->data+2*pnodecount;
        inst->i = ml->data+3*pnodecount;
        inst->v_unused = ml->data+4*pnodecount;
        inst->g_unused = ml->data+5*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = ml->pdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, IClamp_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.del = cnrn_target_deviceptr(tmp.del);
        tmp.dur = cnrn_target_deviceptr(tmp.dur);
        tmp.amp = cnrn_target_deviceptr(tmp.amp);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.node_area = cnrn_target_deviceptr(tmp.node_area);
        tmp.point_process = cnrn_target_deviceptr(tmp.point_process);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(IClamp_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_IClamp(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_IClamp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<IClamp_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_IClamp(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<IClamp_Instance*>(ml->instance);

        #endif
    }


    /** initialize channel */
    void nrn_init_IClamp(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<IClamp_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (IClamp_global))
                nrn_pragma_omp(target update to(IClamp_global))
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
                    inst->i[id] = 0.0;
                }
            }
        }
    }


    inline double nrn_current_IClamp(int id, int pnodecount, IClamp_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        if (nt->_t < inst->del[id] + inst->dur[id] && nt->_t >= inst->del[id]) {
            inst->i[id] = inst->amp[id];
        } else {
            inst->i[id] = 0.0;
        }
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_IClamp(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<IClamp_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_IClamp(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_IClamp(id, pnodecount, inst, data, indexes, thread, nt, v);
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
    void nrn_state_IClamp(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<IClamp_Instance*>(ml->instance);

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
    void _stim_reg() {

        int mech_type = nrn_get_mechtype("IClamp");
        IClamp_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_IClamp, nrn_cur_IClamp, nullptr, nrn_state_IClamp, nrn_init_IClamp, nrn_private_constructor_IClamp, nrn_private_destructor_IClamp, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
