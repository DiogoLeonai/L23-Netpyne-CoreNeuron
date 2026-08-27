/*********************************************************
Model Name      : Ih
Filename        : Ih.mod
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
        "Ih",
        "gbar_Ih",
        "shift1_Ih",
        "shift2_Ih",
        "shift3_Ih",
        "shift4_Ih",
        "shift5_Ih",
        "shift6_Ih",
        0,
        "ihcn_Ih",
        "g_Ih",
        0,
        "m_Ih",
        0,
        0
    };


    /** all global variables */
    struct Ih_Store {
        double m0{};
        int reset{};
        int mech_type{};
        double ehcn{-45};
        int slist1[1]{9};
        int dlist1[1]{14};
    };
    static_assert(std::is_trivially_copy_constructible_v<Ih_Store>);
    static_assert(std::is_trivially_move_constructible_v<Ih_Store>);
    static_assert(std::is_trivially_copy_assignable_v<Ih_Store>);
    static_assert(std::is_trivially_move_assignable_v<Ih_Store>);
    static_assert(std::is_trivially_destructible_v<Ih_Store>);
    static Ih_Store Ih_global;


    /** all mechanism instance variables and global variables */
    struct Ih_Instance  {
        const double* gbar{};
        const double* shift1{};
        double* shift2{};
        const double* shift3{};
        double* shift4{};
        const double* shift5{};
        const double* shift6{};
        double* ihcn{};
        double* g{};
        double* m{};
        double* mInf{};
        double* mTau{};
        double* mAlpha{};
        double* mBeta{};
        double* Dm{};
        double* v_unused{};
        double* g_unused{};
        Ih_Store* global{&Ih_global};
    };


    /** connect global (scalar) variables to hoc -- */
    static DoubScal hoc_scalar_double[] = {
        {"ehcn_Ih", &Ih_global.ehcn},
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
        return 17;
    }


    static inline int int_variables_size() {
        return 0;
    }


    static inline int get_mech_type() {
        return Ih_global.mech_type;
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
    static void nrn_private_constructor_Ih(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new Ih_Instance{};
        assert(inst->global == &Ih_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(Ih_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Ih_Instance const* inst);
    static inline void delete_instance_from_device(Ih_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_Ih(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<Ih_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Ih_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Ih_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<Ih_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &Ih_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(Ih_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gbar = ml->data+0*pnodecount;
        inst->shift1 = ml->data+1*pnodecount;
        inst->shift2 = ml->data+2*pnodecount;
        inst->shift3 = ml->data+3*pnodecount;
        inst->shift4 = ml->data+4*pnodecount;
        inst->shift5 = ml->data+5*pnodecount;
        inst->shift6 = ml->data+6*pnodecount;
        inst->ihcn = ml->data+7*pnodecount;
        inst->g = ml->data+8*pnodecount;
        inst->m = ml->data+9*pnodecount;
        inst->mInf = ml->data+10*pnodecount;
        inst->mTau = ml->data+11*pnodecount;
        inst->mAlpha = ml->data+12*pnodecount;
        inst->mBeta = ml->data+13*pnodecount;
        inst->Dm = ml->data+14*pnodecount;
        inst->v_unused = ml->data+15*pnodecount;
        inst->g_unused = ml->data+16*pnodecount;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, Ih_Instance const* inst) {
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
        tmp.shift1 = cnrn_target_deviceptr(tmp.shift1);
        tmp.shift2 = cnrn_target_deviceptr(tmp.shift2);
        tmp.shift3 = cnrn_target_deviceptr(tmp.shift3);
        tmp.shift4 = cnrn_target_deviceptr(tmp.shift4);
        tmp.shift5 = cnrn_target_deviceptr(tmp.shift5);
        tmp.shift6 = cnrn_target_deviceptr(tmp.shift6);
        tmp.ihcn = cnrn_target_deviceptr(tmp.ihcn);
        tmp.g = cnrn_target_deviceptr(tmp.g);
        tmp.m = cnrn_target_deviceptr(tmp.m);
        tmp.mInf = cnrn_target_deviceptr(tmp.mInf);
        tmp.mTau = cnrn_target_deviceptr(tmp.mTau);
        tmp.mAlpha = cnrn_target_deviceptr(tmp.mAlpha);
        tmp.mBeta = cnrn_target_deviceptr(tmp.mBeta);
        tmp.Dm = cnrn_target_deviceptr(tmp.Dm);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(Ih_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_Ih(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_Ih(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Ih_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_Ih(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<Ih_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_Ih(int id, int pnodecount, Ih_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v);


    inline int rates_Ih(int id, int pnodecount, Ih_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        int ret_rates = 0;
        if (fabs(v + inst->shift1[id]) < 1e-9) {
            v = v + 0.0001;
        }
        if (fabs(inst->shift4[id]) < 1e-9) {
            inst->shift4[id] = inst->shift4[id] + 0.0001;
        }
        if (fabs(inst->shift2[id]) < 1e-9) {
            inst->shift2[id] = inst->shift2[id] + 0.0001;
        }
        inst->mAlpha[id] = 0.001 * inst->shift5[id] * (v + inst->shift1[id]) / (exp((v + inst->shift1[id]) / inst->shift2[id]) - 1.0);
        inst->mBeta[id] = 0.001 * inst->shift6[id] * exp((v + inst->shift3[id]) / inst->shift4[id]);
        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
        inst->mTau[id] = 1.0 / (inst->mAlpha[id] + inst->mBeta[id]);
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_Ih(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Ih_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (Ih_global))
                nrn_pragma_omp(target update to(Ih_global))
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
                    inst->m[id] = inst->global->m0;
                    {
                        if (fabs(v + inst->shift1[id]) < 1e-9) {
                            v = v + 0.0001;
                        }
                        if (fabs(inst->shift4[id]) < 1e-9) {
                            inst->shift4[id] = inst->shift4[id] + 0.0001;
                        }
                        if (fabs(inst->shift2[id]) < 1e-9) {
                            inst->shift2[id] = inst->shift2[id] + 0.0001;
                        }
                        inst->mAlpha[id] = 0.001 * inst->shift5[id] * (v + inst->shift1[id]) / (exp((v + inst->shift1[id]) / inst->shift2[id]) - 1.0);
                        inst->mBeta[id] = 0.001 * inst->shift6[id] * exp((v + inst->shift3[id]) / inst->shift4[id]);
                        inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                        inst->mTau[id] = 1.0 / (inst->mAlpha[id] + inst->mBeta[id]);
                    }
                    inst->m[id] = inst->mInf[id];
                }
            }
        }
    }


    inline double nrn_current_Ih(int id, int pnodecount, Ih_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gbar[id] * inst->m[id];
        inst->ihcn[id] = inst->g[id] * (v - inst->global->ehcn);
        current += inst->ihcn[id];
        return current;
    }


    /** update current */
    void nrn_cur_Ih(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<Ih_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                double g = nrn_current_Ih(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double rhs = nrn_current_Ih(id, pnodecount, inst, data, indexes, thread, nt, v);
                g = (g-rhs)/0.001;
                #if NRN_PRCELLSTATE
                inst->g_unused[id] = g;
                #endif
                vec_rhs[node_id] -= rhs;
                vec_d[node_id] += g;
            }
        }
    }


    /** update state */
    void nrn_state_Ih(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<Ih_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                {
                    if (fabs(v + inst->shift1[id]) < 1e-9) {
                        v = v + 0.0001;
                    }
                    if (fabs(inst->shift4[id]) < 1e-9) {
                        inst->shift4[id] = inst->shift4[id] + 0.0001;
                    }
                    if (fabs(inst->shift2[id]) < 1e-9) {
                        inst->shift2[id] = inst->shift2[id] + 0.0001;
                    }
                    inst->mAlpha[id] = 0.001 * inst->shift5[id] * (v + inst->shift1[id]) / (exp((v + inst->shift1[id]) / inst->shift2[id]) - 1.0);
                    inst->mBeta[id] = 0.001 * inst->shift6[id] * exp((v + inst->shift3[id]) / inst->shift4[id]);
                    inst->mInf[id] = inst->mAlpha[id] / (inst->mAlpha[id] + inst->mBeta[id]);
                    inst->mTau[id] = 1.0 / (inst->mAlpha[id] + inst->mBeta[id]);
                }
                inst->m[id] = inst->m[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->mTau[id]))) * ( -(((inst->mInf[id])) / inst->mTau[id]) / (((( -1.0))) / inst->mTau[id]) - inst->m[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _Ih_reg() {

        int mech_type = nrn_get_mechtype("Ih");
        Ih_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_Ih, nrn_cur_Ih, nullptr, nrn_state_Ih, nrn_init_Ih, nrn_private_constructor_Ih, nrn_private_destructor_Ih, first_pointer_var_index(), 1);

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
