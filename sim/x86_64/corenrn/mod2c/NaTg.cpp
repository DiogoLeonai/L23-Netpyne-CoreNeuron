/*********************************************************
Model Name      : NaTg
Filename        : NaTg.mod
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
        "NaTg",
        "gbar_NaTg",
        "vshifth_NaTg",
        "vshiftm_NaTg",
        "slopeh_NaTg",
        "slopem_NaTg",
        0,
        "ina_NaTg",
        "g_NaTg",
        0,
        "m_NaTg",
        "h_NaTg",
        0,
        0
    };


    /** all global variables */
    struct NaTg_Store {
        int na_type{};
        double m0{};
        double h0{};
        int reset{};
        int mech_type{};
        int slist1[2]{7, 8};
        int dlist1[2]{18, 19};
    };
    static_assert(std::is_trivially_copy_constructible_v<NaTg_Store>);
    static_assert(std::is_trivially_move_constructible_v<NaTg_Store>);
    static_assert(std::is_trivially_copy_assignable_v<NaTg_Store>);
    static_assert(std::is_trivially_move_assignable_v<NaTg_Store>);
    static_assert(std::is_trivially_destructible_v<NaTg_Store>);
    static NaTg_Store NaTg_global;


    /** all mechanism instance variables and global variables */
    struct NaTg_Instance  {
        const double* gbar{};
        const double* vshifth{};
        const double* vshiftm{};
        const double* slopeh{};
        const double* slopem{};
        double* ina{};
        double* g{};
        double* m{};
        double* h{};
        double* ena{};
        double* mInf{};
        double* mTau{};
        double* mAlpha{};
        double* mBeta{};
        double* hInf{};
        double* hTau{};
        double* hAlpha{};
        double* hBeta{};
        double* Dm{};
        double* Dh{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_ena{};
        double* ion_ina{};
        double* ion_dinadv{};
        NaTg_Store* global{&NaTg_global};
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
        return 22;
    }


    static inline int int_variables_size() {
        return 3;
    }


    static inline int get_mech_type() {
        return NaTg_global.mech_type;
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
    static void nrn_private_constructor_NaTg(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new NaTg_Instance{};
        assert(inst->global == &NaTg_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(NaTg_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, NaTg_Instance const* inst);
    static inline void delete_instance_from_device(NaTg_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_NaTg(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<NaTg_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &NaTg_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(NaTg_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<NaTg_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &NaTg_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(NaTg_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gbar = ml->data+0*pnodecount;
        inst->vshifth = ml->data+1*pnodecount;
        inst->vshiftm = ml->data+2*pnodecount;
        inst->slopeh = ml->data+3*pnodecount;
        inst->slopem = ml->data+4*pnodecount;
        inst->ina = ml->data+5*pnodecount;
        inst->g = ml->data+6*pnodecount;
        inst->m = ml->data+7*pnodecount;
        inst->h = ml->data+8*pnodecount;
        inst->ena = ml->data+9*pnodecount;
        inst->mInf = ml->data+10*pnodecount;
        inst->mTau = ml->data+11*pnodecount;
        inst->mAlpha = ml->data+12*pnodecount;
        inst->mBeta = ml->data+13*pnodecount;
        inst->hInf = ml->data+14*pnodecount;
        inst->hTau = ml->data+15*pnodecount;
        inst->hAlpha = ml->data+16*pnodecount;
        inst->hBeta = ml->data+17*pnodecount;
        inst->Dm = ml->data+18*pnodecount;
        inst->Dh = ml->data+19*pnodecount;
        inst->v_unused = ml->data+20*pnodecount;
        inst->g_unused = ml->data+21*pnodecount;
        inst->ion_ena = nt->_data;
        inst->ion_ina = nt->_data;
        inst->ion_dinadv = nt->_data;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, NaTg_Instance const* inst) {
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
        tmp.vshifth = cnrn_target_deviceptr(tmp.vshifth);
        tmp.vshiftm = cnrn_target_deviceptr(tmp.vshiftm);
        tmp.slopeh = cnrn_target_deviceptr(tmp.slopeh);
        tmp.slopem = cnrn_target_deviceptr(tmp.slopem);
        tmp.ina = cnrn_target_deviceptr(tmp.ina);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.m = cnrn_target_deviceptr(tmp.m);
        tmp.h = cnrn_target_deviceptr(tmp.h);
        tmp.ena = cnrn_target_deviceptr(tmp.ena);
        tmp.mInf = cnrn_target_deviceptr(tmp.mInf);
        tmp.mTau = cnrn_target_deviceptr(tmp.mTau);
        tmp.mAlpha = cnrn_target_deviceptr(tmp.mAlpha);
        tmp.mBeta = cnrn_target_deviceptr(tmp.mBeta);
        tmp.hInf = cnrn_target_deviceptr(tmp.hInf);
        tmp.hTau = cnrn_target_deviceptr(tmp.hTau);
        tmp.hAlpha = cnrn_target_deviceptr(tmp.hAlpha);
        tmp.hBeta = cnrn_target_deviceptr(tmp.hBeta);
        tmp.Dm = cnrn_target_deviceptr(tmp.Dm);
        tmp.Dh = cnrn_target_deviceptr(tmp.Dh);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_ena = cnrn_target_deviceptr(tmp.ion_ena);
        tmp.ion_ina = cnrn_target_deviceptr(tmp.ion_ina);
        tmp.ion_dinadv = cnrn_target_deviceptr(tmp.ion_dinadv);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(NaTg_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_NaTg(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_NaTg(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<NaTg_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_NaTg(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<NaTg_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_NaTg(int id, int pnodecount, NaTg_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int rates_NaTg(int id, int pnodecount, NaTg_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_rates = 0;
        double qt;
        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
        if (v == ( -38.0 + inst->vshiftm[id])) {
            v = v + 0.0001;
        }
        inst->mAlpha[id] = (0.182 * (v - ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -(v - ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
        inst->mBeta[id] = (0.124 * ( -v + ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -( -v + ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
        inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
        if (v == ( -66.0 + inst->vshifth[id])) {
            v = v + 0.0001;
        }
        inst->hAlpha[id] = ( -0.015 * (v - ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp((v - ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
        inst->hBeta[id] = ( -0.015 * ( -v + ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp(( -v + ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
        inst->hTau[id] = (1.0 / (inst->hAlpha[id] + inst->hBeta[id])) / qt;
        inst->hInf[id] = inst->hAlpha[id] / (inst->hAlpha[id] + inst->hBeta[id]);
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_NaTg(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<NaTg_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (NaTg_global))
                nrn_pragma_omp(target update to(NaTg_global))
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
                    inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                    inst->m[id] = inst->global->m0;
                    inst->h[id] = inst->global->h0;
                    {
                        double qt;
                        qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                        if (v == ( -38.0 + inst->vshiftm[id])) {
                            v = v + 0.0001;
                        }
                        inst->mAlpha[id] = (0.182 * (v - ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -(v - ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
                        inst->mBeta[id] = (0.124 * ( -v + ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -( -v + ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
                        inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
                        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                        if (v == ( -66.0 + inst->vshifth[id])) {
                            v = v + 0.0001;
                        }
                        inst->hAlpha[id] = ( -0.015 * (v - ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp((v - ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
                        inst->hBeta[id] = ( -0.015 * ( -v + ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp(( -v + ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
                        inst->hTau[id] = (1.0 / (inst->hAlpha[id] + inst->hBeta[id])) / qt;
                        inst->hInf[id] = inst->hAlpha[id] / (inst->hAlpha[id] + inst->hBeta[id]);
                    }
                    inst->m[id] = inst->mInf[id];
                    inst->h[id] = inst->hInf[id];
                }
            }
        }
    }


    inline double nrn_current_NaTg(int id, int pnodecount, NaTg_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gbar[id] * inst->m[id] * inst->m[id] * inst->m[id] * inst->h[id];
        inst->ina[id] = inst->g[id] * (v - inst->ena[id]);
        current += inst->ina[id];
        return current;
    }


    /** update current */
    void nrn_cur_NaTg(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<NaTg_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                double g = nrn_current_NaTg(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double dina = inst->ina[id];
                double rhs = nrn_current_NaTg(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                inst->ion_dinadv[indexes[2*pnodecount + id]] += (dina-inst->ina[id])/0.001;
                inst->ion_ina[indexes[1*pnodecount + id]] += inst->ina[id];
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_NaTg(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<NaTg_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ena[id] = inst->ion_ena[indexes[0*pnodecount + id]];
                {
                    double qt;
                    qt = pow(2.3, ((34.0 - 21.0) / 10.0));
                    if (v == ( -38.0 + inst->vshiftm[id])) {
                        v = v + 0.0001;
                    }
                    inst->mAlpha[id] = (0.182 * (v - ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -(v - ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
                    inst->mBeta[id] = (0.124 * ( -v + ( -38.0 + inst->vshiftm[id]))) / (1.0 - (exp( -( -v + ( -38.0 + inst->vshiftm[id])) / inst->slopem[id])));
                    inst->mTau[id] = (1.0 / (inst->mAlpha[id] + inst->mBeta[id])) / qt;
                    inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                    if (v == ( -66.0 + inst->vshifth[id])) {
                        v = v + 0.0001;
                    }
                    inst->hAlpha[id] = ( -0.015 * (v - ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp((v - ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
                    inst->hBeta[id] = ( -0.015 * ( -v + ( -66.0 + inst->vshifth[id]))) / (1.0 - (exp(( -v + ( -66.0 + inst->vshifth[id])) / inst->slopeh[id])));
                    inst->hTau[id] = (1.0 / (inst->hAlpha[id] + inst->hBeta[id])) / qt;
                    inst->hInf[id] = inst->hAlpha[id] / (inst->hAlpha[id] + inst->hBeta[id]);
                }
                inst->m[id] = inst->m[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->mTau[id]))) * ( -(((inst->mInf[id])) / inst->mTau[id]) / (((( -1.0))) / inst->mTau[id]) - inst->m[id]);
                inst->h[id] = inst->h[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->hTau[id]))) * ( -(((inst->hInf[id])) / inst->hTau[id]) / (((( -1.0))) / inst->hTau[id]) - inst->h[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _NaTg_reg() {

        int mech_type = nrn_get_mechtype("NaTg");
        NaTg_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_NaTg, nrn_cur_NaTg, nullptr, nrn_state_NaTg, nrn_init_NaTg, nrn_private_constructor_NaTg, nrn_private_destructor_NaTg, first_pointer_var_index(), 1);
        NaTg_global.na_type = nrn_get_mechtype("na_ion");

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "na_ion");
        hoc_register_dparam_semantics(mech_type, 1, "na_ion");
        hoc_register_dparam_semantics(mech_type, 2, "na_ion");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
