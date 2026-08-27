/*********************************************************
Model Name      : Im
Filename        : Im.mod
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
        "Im",
        "gbar_Im",
        0,
        "ik_Im",
        "g_Im",
        0,
        "m_Im",
        0,
        0
    };


    /** all global variables */
    struct Im_Store {
        int k_type{};
        double m0{};
        int reset{};
        int mech_type{};
        int slist1[1]{3};
        int dlist1[1]{9};
    };
    static_assert(std::is_trivially_copy_constructible_v<Im_Store>);
    static_assert(std::is_trivially_move_constructible_v<Im_Store>);
    static_assert(std::is_trivially_copy_assignable_v<Im_Store>);
    static_assert(std::is_trivially_move_assignable_v<Im_Store>);
    static_assert(std::is_trivially_destructible_v<Im_Store>);
    static Im_Store Im_global;


    /** all mechanism instance variables and global variables */
    struct Im_Instance  {
        const double* gbar{};
        double* ik{};
        double* g{};
        double* m{};
        double* ek{};
        double* mInf{};
        double* mTau{};
        double* mAlpha{};
        double* mBeta{};
        double* Dm{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_ek{};
        double* ion_ik{};
        double* ion_dikdv{};
        Im_Store* global{&Im_global};
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
        return 12;
    }


    static inline int int_variables_size() {
        return 3;
    }


    static inline int get_mech_type() {
        return Im_global.mech_type;
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
    static void nrn_private_constructor_Im(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new Im_Instance{};
        assert(inst->global == &Im_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(Im_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Im_Instance const* inst);
    static inline void delete_instance_from_device(Im_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_Im(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<Im_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Im_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Im_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<Im_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Im_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Im_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gbar = ml->data+0*pnodecount;
        inst->ik = ml->data+1*pnodecount;
        inst->g = ml->data+2*pnodecount;
        inst->m = ml->data+3*pnodecount;
        inst->ek = ml->data+4*pnodecount;
        inst->mInf = ml->data+5*pnodecount;
        inst->mTau = ml->data+6*pnodecount;
        inst->mAlpha = ml->data+7*pnodecount;
        inst->mBeta = ml->data+8*pnodecount;
        inst->Dm = ml->data+9*pnodecount;
        inst->v_unused = ml->data+10*pnodecount;
        inst->g_unused = ml->data+11*pnodecount;
        inst->ion_ek = nt->_data;
        inst->ion_ik = nt->_data;
        inst->ion_dikdv = nt->_data;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Im_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.gbar = cnrn_target_deviceptr(tmp.gbar);
        tmp.ik = cnrn_target_deviceptr(tmp.ik);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.m = cnrn_target_deviceptr(tmp.m);
        tmp.ek = cnrn_target_deviceptr(tmp.ek);
        tmp.mInf = cnrn_target_deviceptr(tmp.mInf);
        tmp.mTau = cnrn_target_deviceptr(tmp.mTau);
        tmp.mAlpha = cnrn_target_deviceptr(tmp.mAlpha);
        tmp.mBeta = cnrn_target_deviceptr(tmp.mBeta);
        tmp.Dm = cnrn_target_deviceptr(tmp.Dm);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_ek = cnrn_target_deviceptr(tmp.ion_ek);
        tmp.ion_ik = cnrn_target_deviceptr(tmp.ion_ik);
        tmp.ion_dikdv = cnrn_target_deviceptr(tmp.ion_dikdv);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(Im_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_Im(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_Im(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Im_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_Im(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Im_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_Im(int id, int pnodecount, Im_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int rates_Im(int id, int pnodecount, Im_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_rates = 0;
        double qt;
        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
        inst->mAlpha[id] = 3.3e-3 * exp(2.5 * 0.04 * (v - ( -35.0)));
        inst->mBeta[id] = 3.3e-3 * exp( -2.5 * 0.04 * (v - ( -35.0)));
        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
        inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_Im(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Im_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (Im_global))
                nrn_pragma_omp(target update to(Im_global))
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
                    inst->ek[id] = inst->ion_ek[indexes[0*pnodecount + id]];
                    inst->m[id] = inst->global->m0;
                    {
                        double qt;
                        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                        inst->mAlpha[id] = 3.3e-3 * exp(2.5 * 0.04 * (v - ( -35.0)));
                        inst->mBeta[id] = 3.3e-3 * exp( -2.5 * 0.04 * (v - ( -35.0)));
                        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                        inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
                    }
                    inst->m[id] = inst->mInf[id];
                }
            }
        }
    }


    inline double nrn_current_Im(int id, int pnodecount, Im_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gbar[id] * inst->m[id];
        inst->ik[id] = inst->g[id] * (v - inst->ek[id]);
        current += inst->ik[id];
        return current;
    }


    /** update current */
    void nrn_cur_Im(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Im_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ek[id] = inst->ion_ek[indexes[0*pnodecount + id]];
                double g = nrn_current_Im(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double dik = inst->ik[id];
                double rhs = nrn_current_Im(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                inst->ion_dikdv[indexes[2*pnodecount + id]] += (dik-inst->ik[id])/0.001;
                inst->ion_ik[indexes[1*pnodecount + id]] += inst->ik[id];
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_Im(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<Im_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ek[id] = inst->ion_ek[indexes[0*pnodecount + id]];
                {
                    double qt;
                    qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                    inst->mAlpha[id] = 3.3e-3 * exp(2.5 * 0.04 * (v - ( -35.0)));
                    inst->mBeta[id] = 3.3e-3 * exp( -2.5 * 0.04 * (v - ( -35.0)));
                    inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                    inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
                }
                inst->m[id] = inst->m[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->mTau[id]))) * ( -(((inst->mInf[id])) / inst->mTau[id]) / (((( -1.0))) / inst->mTau[id]) - inst->m[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _Im_reg() {

        int mech_type = nrn_get_mechtype("Im");
        Im_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_Im, nrn_cur_Im, nullptr, nrn_state_Im, nrn_init_Im, nrn_private_constructor_Im, nrn_private_destructor_Im, first_pointer_var_index(), 1);
        Im_global.k_type = nrn_get_mechtype("k_ion");

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "k_ion");
        hoc_register_dparam_semantics(mech_type, 1, "k_ion");
        hoc_register_dparam_semantics(mech_type, 2, "k_ion");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
