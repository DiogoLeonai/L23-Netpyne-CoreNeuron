/*********************************************************
Model Name      : Gfluct2
Filename        : Gfluct.mod
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
        "Gfluct2",
        "E_e",
        "E_i",
        "g_e0",
        "g_i0",
        "std_e",
        "std_i",
        "tau_e",
        "tau_i",
        0,
        "i",
        "g_e",
        "g_i",
        "g_e1",
        "g_i1",
        "D_e",
        "D_i",
        0,
        0,
        0
    };


    /** all global variables */
    struct Gfluct2_Store {
        int point_type{};
        int reset{};
        int mech_type{};
    };
    static_assert(std::is_trivially_copy_constructible_v<Gfluct2_Store>);
    static_assert(std::is_trivially_move_constructible_v<Gfluct2_Store>);
    static_assert(std::is_trivially_copy_assignable_v<Gfluct2_Store>);
    static_assert(std::is_trivially_move_assignable_v<Gfluct2_Store>);
    static_assert(std::is_trivially_destructible_v<Gfluct2_Store>);
    static Gfluct2_Store Gfluct2_global;


    /** all mechanism instance variables and global variables */
    struct Gfluct2_Instance  {
        const double* E_e{};
        const double* E_i{};
        const double* g_e0{};
        const double* g_i0{};
        const double* std_e{};
        const double* std_i{};
        const double* tau_e{};
        const double* tau_i{};
        double* i{};
        double* g_e{};
        double* g_i{};
        double* g_e1{};
        double* g_i1{};
        double* D_e{};
        double* D_i{};
        double* exp_e{};
        double* exp_i{};
        double* amp_e{};
        double* amp_i{};
        double* v_unused{};
        double* g_unused{};
        const double* node_area{};
        const int* point_process{};
        void** rng{};
        Gfluct2_Store* global{&Gfluct2_global};
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


    static inline int float_variables_size() {
        return 21;
    }


    static inline int int_variables_size() {
        return 3;
    }


    static inline int get_mech_type() {
        return Gfluct2_global.mech_type;
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
    static void nrn_private_constructor_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new Gfluct2_Instance{};
        assert(inst->global == &Gfluct2_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(Gfluct2_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Gfluct2_Instance const* inst);
    static inline void delete_instance_from_device(Gfluct2_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Gfluct2_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Gfluct2_Store));
        int pnodecount = ml->_nodecount_padded;
        int nodecount = ml->nodecount;
        Datum* indexes = ml->pdata;
        for (int id = 0; id < nodecount; id++) {
            nrnran123_deletestream((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        }
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Gfluct2_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Gfluct2_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->E_e = ml->data+0*pnodecount;
        inst->E_i = ml->data+1*pnodecount;
        inst->g_e0 = ml->data+2*pnodecount;
        inst->g_i0 = ml->data+3*pnodecount;
        inst->std_e = ml->data+4*pnodecount;
        inst->std_i = ml->data+5*pnodecount;
        inst->tau_e = ml->data+6*pnodecount;
        inst->tau_i = ml->data+7*pnodecount;
        inst->i = ml->data+8*pnodecount;
        inst->g_e = ml->data+9*pnodecount;
        inst->g_i = ml->data+10*pnodecount;
        inst->g_e1 = ml->data+11*pnodecount;
        inst->g_i1 = ml->data+12*pnodecount;
        inst->D_e = ml->data+13*pnodecount;
        inst->D_i = ml->data+14*pnodecount;
        inst->exp_e = ml->data+15*pnodecount;
        inst->exp_i = ml->data+16*pnodecount;
        inst->amp_e = ml->data+17*pnodecount;
        inst->amp_i = ml->data+18*pnodecount;
        inst->v_unused = ml->data+19*pnodecount;
        inst->g_unused = ml->data+20*pnodecount;
        inst->node_area = nt->_data;
        inst->point_process = ml->pdata;
        inst->rng = nt->_vdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Gfluct2_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.E_e = cnrn_target_deviceptr(tmp.E_e);
        tmp.E_i = cnrn_target_deviceptr(tmp.E_i);
        tmp.g_e0 = cnrn_target_deviceptr(tmp.g_e0);
        tmp.g_i0 = cnrn_target_deviceptr(tmp.g_i0);
        tmp.std_e = cnrn_target_deviceptr(tmp.std_e);
        tmp.std_i = cnrn_target_deviceptr(tmp.std_i);
        tmp.tau_e = cnrn_target_deviceptr(tmp.tau_e);
        tmp.tau_i = cnrn_target_deviceptr(tmp.tau_i);
        tmp.i = cnrn_target_deviceptr(tmp.i);
        tmp.g_e = cnrn_target_deviceptr(tmp.g_e);
        tmp.g_i = cnrn_target_deviceptr(tmp.g_i);
        tmp.g_e1 = cnrn_target_deviceptr(tmp.g_e1);
        tmp.g_i1 = cnrn_target_deviceptr(tmp.g_i1);
        tmp.D_e = cnrn_target_deviceptr(tmp.D_e);
        tmp.D_i = cnrn_target_deviceptr(tmp.D_i);
        tmp.exp_e = cnrn_target_deviceptr(tmp.exp_e);
        tmp.exp_i = cnrn_target_deviceptr(tmp.exp_i);
        tmp.amp_e = cnrn_target_deviceptr(tmp.amp_e);
        tmp.amp_i = cnrn_target_deviceptr(tmp.amp_i);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.node_area = cnrn_target_deviceptr(tmp.node_area);
        tmp.point_process = cnrn_target_deviceptr(tmp.point_process);
        tmp.rng = cnrn_target_deviceptr(tmp.rng);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(Gfluct2_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_Gfluct2(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);

        #endif
    }


    inline static int oup_Gfluct2(int id, int pnodecount, Gfluct2_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int oup_Gfluct2(int id, int pnodecount, Gfluct2_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_oup = 0;
        if (inst->tau_e[id] > 0.0) {
            inst->g_e1[id] = inst->exp_e[id] * inst->g_e1[id] + inst->amp_e[id] * nrnran123_normal((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        } else {
            inst->g_e1[id] = inst->std_e[id] * nrnran123_normal((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        }
        if (inst->tau_i[id] > 0.0) {
            inst->g_i1[id] = inst->exp_i[id] * inst->g_i1[id] + inst->amp_i[id] * nrnran123_normal((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        } else {
            inst->g_i1[id] = inst->std_i[id] * nrnran123_normal((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]]);
        }
        return ret_oup;
    }


    /** initialize channel */
    void nrn_init_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (Gfluct2_global))
                nrn_pragma_omp(target update to(Gfluct2_global))
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
                    nrnran123_setseq((nrnran123_State*) inst->rng[indexes[2*pnodecount + id]], 0.0);
                    inst->g_e1[id] = 0.0;
                    inst->g_i1[id] = 0.0;
                    inst->g_e[id] = inst->g_e0[id];
                    inst->g_i[id] = inst->g_i0[id];
                    inst->D_e[id] = 0.0;
                    inst->D_i[id] = 0.0;
                    inst->exp_e[id] = 0.0;
                    inst->exp_i[id] = 0.0;
                    inst->amp_e[id] = 0.0;
                    inst->amp_i[id] = 0.0;
                    if (inst->tau_e[id] > 0.0) {
                        inst->D_e[id] = 2.0 * inst->std_e[id] * inst->std_e[id] / inst->tau_e[id];
                        inst->exp_e[id] = exp( -nt->_dt / inst->tau_e[id]);
                        inst->amp_e[id] = inst->std_e[id] * sqrt(1.0 - exp( -2.0 * nt->_dt / inst->tau_e[id]));
                    }
                    if (inst->tau_i[id] > 0.0) {
                        inst->D_i[id] = 2.0 * inst->std_i[id] * inst->std_i[id] / inst->tau_i[id];
                        inst->exp_i[id] = exp( -nt->_dt / inst->tau_i[id]);
                        inst->amp_i[id] = inst->std_i[id] * sqrt(1.0 - exp( -2.0 * nt->_dt / inst->tau_i[id]));
                    }
                }
            }
        }
    }


    inline double nrn_current_Gfluct2(int id, int pnodecount, Gfluct2_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g_e[id] = inst->g_e0[id] + inst->g_e1[id];
        if (inst->g_e[id] < 0.0) {
            inst->g_e[id] = 0.0;
        }
        inst->g_i[id] = inst->g_i0[id] + inst->g_i1[id];
        if (inst->g_i[id] < 0.0) {
            inst->g_i[id] = 0.0;
        }
        inst->i[id] = inst->g_e[id] * (v - inst->E_e[id]) + inst->g_i[id] * (v - inst->E_i[id]);
        current += inst->i[id];
        return current;
    }


    /** update current */
    void nrn_cur_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_Gfluct2(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_Gfluct2(id, pnodecount, inst, data, indexes, thread, nt, v);
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
    void nrn_state_Gfluct2(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<Gfluct2_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                oup_Gfluct2(id, pnodecount, inst, data, indexes, thread, nt, v);
            }
        }
    }


    /** register channel with the simulator */
    void _Gfluct_reg() {

        int mech_type = nrn_get_mechtype("Gfluct2");
        Gfluct2_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        point_register_mech(mechanism_info, nrn_alloc_Gfluct2, nrn_cur_Gfluct2, nullptr, nrn_state_Gfluct2, nrn_init_Gfluct2, nrn_private_constructor_Gfluct2, nrn_private_destructor_Gfluct2, first_pointer_var_index(), nullptr, nullptr, 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "area");
        hoc_register_dparam_semantics(mech_type, 1, "pntproc");
        hoc_register_dparam_semantics(mech_type, 2, "random");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
