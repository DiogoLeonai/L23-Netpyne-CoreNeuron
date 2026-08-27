/*********************************************************
Model Name      : SK
Filename        : SK.mod
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
        "SK",
        "gbar_SK",
        0,
        "ik_SK",
        "g_SK",
        0,
        "z_SK",
        0,
        0
    };


    /** all global variables */
    struct SK_Store {
        int k_type{};
        int ca_type{};
        double z0{};
        int reset{};
        int mech_type{};
        double zTau{1};
        int slist1[1]{3};
        int dlist1[1]{7};
    };
    static_assert(std::is_trivially_copy_constructible_v<SK_Store>);
    static_assert(std::is_trivially_move_constructible_v<SK_Store>);
    static_assert(std::is_trivially_copy_assignable_v<SK_Store>);
    static_assert(std::is_trivially_move_assignable_v<SK_Store>);
    static_assert(std::is_trivially_destructible_v<SK_Store>);
    static SK_Store SK_global;


    /** all mechanism instance variables and global variables */
    struct SK_Instance  {
        const double* gbar{};
        double* ik{};
        double* g{};
        double* z{};
        double* ek{};
        double* cai{};
        double* zInf{};
        double* Dz{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_ek{};
        double* ion_ik{};
        double* ion_dikdv{};
        const double* ion_cai{};
        const double* ion_cao{};
        SK_Store* global{&SK_global};
    };


    /** connect global (scalar) variables to hoc -- */
    static DoubScal hoc_scalar_double[] = {
        {"zTau_SK", &SK_global.zTau},
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
        return 10;
    }


    static inline int int_variables_size() {
        return 5;
    }


    static inline int get_mech_type() {
        return SK_global.mech_type;
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
    static void nrn_private_constructor_SK(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new SK_Instance{};
        assert(inst->global == &SK_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(SK_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, SK_Instance const* inst);
    static inline void delete_instance_from_device(SK_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_SK(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<SK_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &SK_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(SK_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<SK_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &SK_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(SK_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gbar = ml->data+0*pnodecount;
        inst->ik = ml->data+1*pnodecount;
        inst->g = ml->data+2*pnodecount;
        inst->z = ml->data+3*pnodecount;
        inst->ek = ml->data+4*pnodecount;
        inst->cai = ml->data+5*pnodecount;
        inst->zInf = ml->data+6*pnodecount;
        inst->Dz = ml->data+7*pnodecount;
        inst->v_unused = ml->data+8*pnodecount;
        inst->g_unused = ml->data+9*pnodecount;
        inst->ion_ek = nt->_data;
        inst->ion_ik = nt->_data;
        inst->ion_dikdv = nt->_data;
        inst->ion_cai = nt->_data;
        inst->ion_cao = nt->_data;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, SK_Instance const* inst) {
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
        tmp.z = cnrn_target_deviceptr(tmp.z);
        tmp.ek = cnrn_target_deviceptr(tmp.ek);
        tmp.cai = cnrn_target_deviceptr(tmp.cai);
        tmp.zInf = cnrn_target_deviceptr(tmp.zInf);
        tmp.Dz = cnrn_target_deviceptr(tmp.Dz);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_ek = cnrn_target_deviceptr(tmp.ion_ek);
        tmp.ion_ik = cnrn_target_deviceptr(tmp.ion_ik);
        tmp.ion_dikdv = cnrn_target_deviceptr(tmp.ion_dikdv);
        tmp.ion_cai = cnrn_target_deviceptr(tmp.ion_cai);
        tmp.ion_cao = cnrn_target_deviceptr(tmp.ion_cao);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(SK_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_SK(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_SK(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<SK_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_SK(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<SK_Instance*>(ml->instance);

        #endif
    }


    inline static int rates_SK(int id, int pnodecount, SK_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lca);


    inline int rates_SK(int id, int pnodecount, SK_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v, double _lca) {
        int ret_rates = 0;
        if (_lca < 1e-4) {
            inst->zInf[id] = 0.0;
        } else {
            inst->zInf[id] = 1.0 / (1.0 + pow((0.00043 / _lca), 4.8));
        }
        return ret_rates;
    }


    /** initialize channel */
    void nrn_init_SK(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<SK_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (SK_global))
                nrn_pragma_omp(target update to(SK_global))
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
                    inst->cai[id] = inst->ion_cai[indexes[3*pnodecount + id]];
                    inst->z[id] = inst->global->z0;
                    {
                        double _lca_in_1;
                        _lca_in_1 = inst->cai[id];
                        if (_lca_in_1 < 1e-4) {
                            inst->zInf[id] = 0.0;
                        } else {
                            inst->zInf[id] = 1.0 / (1.0 + pow((0.00043 / _lca_in_1), 4.8));
                        }
                    }
                    inst->z[id] = inst->zInf[id];
                }
            }
        }
    }


    inline double nrn_current_SK(int id, int pnodecount, SK_Instance* inst, double* data, const Datum* indexes, ThreadDatum* thread, NrnThread* nt, double v) {
        double current = 0.0;
        inst->g[id] = inst->gbar[id] * inst->z[id];
        inst->ik[id] = inst->g[id] * (v - inst->ek[id]);
        current += inst->ik[id];
        return current;
    }


    /** update current */
    void nrn_cur_SK(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<SK_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread, vec_rhs, vec_d) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ek[id] = inst->ion_ek[indexes[0*pnodecount + id]];
                inst->cai[id] = inst->ion_cai[indexes[3*pnodecount + id]];
                double g = nrn_current_SK(id, pnodecount, inst, data, indexes, thread, nt, v+0.001);
                double dik = inst->ik[id];
                double rhs = nrn_current_SK(id, pnodecount, inst, data, indexes, thread, nt, v);
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
    void nrn_state_SK(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<SK_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ek[id] = inst->ion_ek[indexes[0*pnodecount + id]];
                inst->cai[id] = inst->ion_cai[indexes[3*pnodecount + id]];
                {
                    double _lca_in_0;
                    _lca_in_0 = inst->cai[id];
                    if (_lca_in_0 < 1e-4) {
                        inst->zInf[id] = 0.0;
                    } else {
                        inst->zInf[id] = 1.0 / (1.0 + pow((0.00043 / _lca_in_0), 4.8));
                    }
                }
                inst->z[id] = inst->z[id] + (1.0 - exp(nt->_dt * (((( -1.0))) / inst->global->zTau))) * ( -(((inst->zInf[id])) / inst->global->zTau) / (((( -1.0))) / inst->global->zTau) - inst->z[id]);
            }
        }
    }


    /** register channel with the simulator */
    void _SK_reg() {

        int mech_type = nrn_get_mechtype("SK");
        SK_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_SK, nrn_cur_SK, nullptr, nrn_state_SK, nrn_init_SK, nrn_private_constructor_SK, nrn_private_destructor_SK, first_pointer_var_index(), 1);
        SK_global.k_type = nrn_get_mechtype("k_ion");
        SK_global.ca_type = nrn_get_mechtype("ca_ion");

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "k_ion");
        hoc_register_dparam_semantics(mech_type, 1, "k_ion");
        hoc_register_dparam_semantics(mech_type, 2, "k_ion");
        hoc_register_dparam_semantics(mech_type, 3, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 4, "ca_ion");
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
