/*********************************************************
Model Name      : Ca_LVA
Filename        : Ca_LVA.mod
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
        "Ca_LVA",
        "gbar_Ca_LVA",
        0,
        "ica_Ca_LVA",
        "g_Ca_LVA",
        0,
        "m_Ca_LVA",
        "h_Ca_LVA",
        0,
        0
    };


    /** all global variables */
    struct Ca_LVA_Store {
        int ca_type{};
        double m0{};
        double h0{};
        int reset{};
        int mech_type{};
        int slist1[2]{3, 4};
        int dlist1[2]{10, 11};
    };
    static_assert(std::is_trivially_copy_constructible_v<Ca_LVA_Store>);
    static_assert(std::is_trivially_move_constructible_v<Ca_LVA_Store>);
    static_assert(std::is_trivially_copy_assignable_v<Ca_LVA_Store>);
    static_assert(std::is_trivially_move_assignable_v<Ca_LVA_Store>);
    static_assert(std::is_trivially_destructible_v<Ca_LVA_Store>);
    static Ca_LVA_Store Ca_LVA_global;


    /** all mechanism instance variables and global variables */
    struct Ca_LVA_Instance  {
        const double* gbar{};
        double* ica{};
        double* g{};
        double* m{};
        double* h{};
        double* eca{};
        double* mInf{};
        double* mTau{};
        double* hInf{};
        double* hTau{};
        double* Dm{};
        double* Dh{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_eca{};
        double* ion_ica{};
        double* ion_dicadv{};
        Ca_LVA_Store* global{&Ca_LVA_global};
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
        return 3;
    }


    static inline int get_mech_type() {
        return Ca_LVA_global.mech_type;
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
    static void nrn_private_constructor_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new Ca_LVA_Instance{};
        assert(inst->global == &Ca_LVA_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(Ca_LVA_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Ca_LVA_Instance const* inst);
    static inline void delete_instance_from_device(Ca_LVA_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Ca_LVA_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Ca_LVA_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Ca_LVA_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Ca_LVA_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gbar = ml->data+0*pnodecount;
        inst->ica = ml->data+1*pnodecount;
        inst->g = ml->data+2*pnodecount;
        inst->m = ml->data+3*pnodecount;
        inst->h = ml->data+4*pnodecount;
        inst->eca = ml->data+5*pnodecount;
        inst->mInf = ml->data+6*pnodecount;
        inst->mTau = ml->data+7*pnodecount;
        inst->hInf = ml->data+8*pnodecount;
        inst->hTau = ml->data+9*pnodecount;
        inst->Dm = ml->data+10*pnodecount;
        inst->Dh = ml->data+11*pnodecount;
        inst->v_unused = ml->data+12*pnodecount;
        inst->g_unused = ml->data+13*pnodecount;
        inst->ion_eca = nt->_data;
        inst->ion_ica = nt->_data;
        inst->ion_dicadv = nt->_data;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Ca_LVA_Instance const* inst) {
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
        tmp.ica = cnrn_target_deviceptr(tmp.ica);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.m = cnrn_target_deviceptr(tmp.m);
        tmp.h = cnrn_target_deviceptr(tmp.h);
        tmp.eca = cnrn_target_deviceptr(tmp.eca);
        tmp.mInf = cnrn_target_deviceptr(tmp.mInf);
        tmp.mTau = cnrn_target_deviceptr(tmp.mTau);
        tmp.hInf = cnrn_target_deviceptr(tmp.hInf);
        tmp.hTau = cnrn_target_deviceptr(tmp.hTau);
        tmp.Dm = cnrn_target_deviceptr(tmp.Dm);
        tmp.Dh = cnrn_target_deviceptr(tmp.Dh);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_eca = cnrn_target_deviceptr(tmp.ion_eca);
        tmp.ion_ica = cnrn_target_deviceptr(tmp.ion_ica);
        tmp.ion_dicadv = cnrn_target_deviceptr(tmp.ion_dicadv);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(Ca_LVA_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_Ca_LVA(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_Ca_LVA(int id, int pnodecount, Ca_LVA_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int rates_Ca_LVA(int id, int pnodecount, Ca_LVA_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_rates = 0;
        double qt;
        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
        v = v + 10.0;
        inst->mInf[id] = 1.0 / (1.0 + exp((v - ( -30.0)) /  -6.0));
        inst->mTau[id] = (5.0 + 20.0 / (1.0 + exp((v - ( -25.0)) / 5.0))) / qt;
        inst->hInf[id] = 1.0 / (1.0 + exp((v - ( -80.0)) / 6.4));
        inst->hTau[id] = (20.0 + 50.0 / (1.0 + exp((v - ( -40.0)) / 7.0))) / qt;
        v = v - 10.0;
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (Ca_LVA_global))
                nrn_pragma_omp(target update to(Ca_LVA_global))
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
                    inst->eca[id] = inst->ion_eca[indexes[0*pnodecount + id]];
                    inst->m[id] = inst->global->m0;
                    inst->h[id] = inst->global->h0;
                    {
                        double qt;
                        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                        v = v + 10.0;
                        inst->mInf[id] = 1.0 / (1.0 + exp((v - ( -30.0)) /  -6.0));
                        inst->mTau[id] = (5.0 + 20.0 / (1.0 + exp((v - ( -25.0)) / 5.0))) / qt;
                        inst->hInf[id] = 1.0 / (1.0 + exp((v - ( -80.0)) / 6.4));
                        inst->hTau[id] = (20.0 + 50.0 / (1.0 + exp((v - ( -40.0)) / 7.0))) / qt;
                        v = v - 10.0;
                    }
                    inst->m[id] = inst->mInf[id];
                    inst->h[id] = inst->hInf[id];
                }
            }
        }
    }


    inline double nrn_current_Ca_LVA(int id, int pnodecount, Ca_LVA_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gbar[id] * inst->m[id] * inst->m[id] * inst->h[id];
        inst->ica[id] = inst->g[id] * (v - inst->eca[id]);
        current += inst->ica[id];
        return current;
    }


    /** update current */
    void nrn_cur_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->eca[id] = inst->ion_eca[indexes[0*pnodecount + id]];
                double g = nrn_current_Ca_LVA(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double dica = inst->ica[id];
                double rhs = nrn_current_Ca_LVA(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                inst->ion_dicadv[indexes[2*pnodecount + id]] += (dica-inst->ica[id])/0.001;
                inst->ion_ica[indexes[1*pnodecount + id]] += inst->ica[id];
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_Ca_LVA(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<Ca_LVA_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->eca[id] = inst->ion_eca[indexes[0*pnodecount + id]];
                {
                    double qt;
                    qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                    v = v + 10.0;
                    inst->mInf[id] = 1.0 / (1.0 + exp((v - ( -30.0)) /  -6.0));
                    inst->mTau[id] = (5.0 + 20.0 / (1.0 + exp((v - ( -25.0)) / 5.0))) / qt;
                    inst->hInf[id] = 1.0 / (1.0 + exp((v - ( -80.0)) / 6.4));
                    inst->hTau[id] = (20.0 + 50.0 / (1.0 + exp((v - ( -40.0)) / 7.0))) / qt;
                    v = v - 10.0;
                }
                inst->m[id] = inst->m[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->mTau[id]))) * ( -(((inst->mInf[id])) / inst->mTau[id]) / (((( -1.0))) / inst->mTau[id]) - inst->m[id]);
                inst->h[id] = inst->h[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->hTau[id]))) * ( -(((inst->hInf[id])) / inst->hTau[id]) / (((( -1.0))) / inst->hTau[id]) - inst->h[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _Ca_LVA_reg() {

        int mech_type = nrn_get_mechtype("Ca_LVA");
        Ca_LVA_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_Ca_LVA, nrn_cur_Ca_LVA, nullptr, nrn_state_Ca_LVA, nrn_init_Ca_LVA, nrn_private_constructor_Ca_LVA, nrn_private_destructor_Ca_LVA, first_pointer_var_index(), 1);
        Ca_LVA_global.ca_type = nrn_get_mechtype("ca_ion");

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 1, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 2, "ca_ion");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
