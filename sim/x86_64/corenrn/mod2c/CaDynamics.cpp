/*********************************************************
Model Name      : CaDynamics
Filename        : CaDynamics.mod
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


    /** constants used in nmodl from UNITS */
    static const double FARADAY = 0x1.78e555060882cp+16;
    #ifndef NRN_PRCELLSTATE
    #define NRN_PRCELLSTATE 0
    #endif


    /** channel information */
    static const char *mechanism_info[] = {
        "7.7.0",
        "CaDynamics",
        "gamma_CaDynamics",
        "decay_CaDynamics",
        "depth_CaDynamics",
        "minCai_CaDynamics",
        0,
        0,
        0,
        0
    };


    /** all global variables */
    struct CaDynamics_Store {
        int ca_type{};
        double cai0{};
        int reset{};
        int mech_type{};
        int slist1[1]{5};
        int dlist1[1]{6};
    };
    static_assert(std::is_trivially_copy_constructible_v<CaDynamics_Store>);
    static_assert(std::is_trivially_move_constructible_v<CaDynamics_Store>);
    static_assert(std::is_trivially_copy_assignable_v<CaDynamics_Store>);
    static_assert(std::is_trivially_move_assignable_v<CaDynamics_Store>);
    static_assert(std::is_trivially_destructible_v<CaDynamics_Store>);
    static CaDynamics_Store CaDynamics_global;


    /** all mechanism instance variables and global variables */
    struct CaDynamics_Instance  {
        const double* gamma{};
        const double* decay{};
        const double* depth{};
        const double* minCai{};
        double* ica{};
        double* cai{};
        double* Dcai{};
        double* v_unused{};
        double* g_unused{};
        const double* ion_ica{};
        const double* ion_cao{};
        double* ion_cai{};
        double* ion_ca_erev{};
        const int* style_ca{};
        CaDynamics_Store* global{&CaDynamics_global};
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
        return 9;
    }


    static inline int int_variables_size() {
        return 5;
    }


    static inline int get_mech_type() {
        return CaDynamics_global.mech_type;
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
    static void nrn_private_constructor_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
        assert(!ml->instance);
        assert(!ml->global_variables);
        assert(ml->global_variables_size == 0);
        auto* const inst = new CaDynamics_Instance{};
        assert(inst->global == &CaDynamics_global);
        ml->instance = inst;
        ml->global_variables = inst->global;
        ml->global_variables_size = sizeof(CaDynamics_Store);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, CaDynamics_Instance const* inst);
    static inline void delete_instance_from_device(CaDynamics_Instance* inst);
    // Deallocate the instance structure
    static void nrn_private_destructor_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
        auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &CaDynamics_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(CaDynamics_Store));
        delete_instance_from_device(inst);
        delete inst;
        ml->instance = nullptr;
        ml->global_variables = nullptr;
        ml->global_variables_size = 0;
    }

    /** initialize mechanism instance variables */
    static inline void setup_instance(NrnThread* nt, Memb_list* ml) {
        auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);
        assert(inst);
        assert(inst->global);
        assert(inst->global == &CaDynamics_global);
        assert(inst->global == ml->global_variables);
        assert(ml->global_variables_size == sizeof(CaDynamics_Store));
        int pnodecount = ml->_nodecount_padded;
        Datum* indexes = ml->pdata;
        inst->gamma = ml->data+0*pnodecount;
        inst->decay = ml->data+1*pnodecount;
        inst->depth = ml->data+2*pnodecount;
        inst->minCai = ml->data+3*pnodecount;
        inst->ica = ml->data+4*pnodecount;
        inst->cai = ml->data+5*pnodecount;
        inst->Dcai = ml->data+6*pnodecount;
        inst->v_unused = ml->data+7*pnodecount;
        inst->g_unused = ml->data+8*pnodecount;
        inst->ion_ica = nt->_data;
        inst->ion_cao = nt->_data;
        inst->ion_cai = nt->_data;
        inst->ion_ca_erev = nt->_data;
        inst->style_ca = ml->pdata;
        copy_instance_to_device(nt, ml, inst);
    }

    static inline void copy_instance_to_device(NrnThread* nt, Memb_list* ml, CaDynamics_Instance const* inst) {
        if (!nt->compute_gpu) {
            return;
        }
        auto tmp = *inst;
        auto* d_inst = cnrn_target_is_present(inst);
        if (!d_inst) {
            d_inst = cnrn_target_copyin(inst);
        }
        tmp.global = cnrn_target_deviceptr(tmp.global);
        tmp.gamma = cnrn_target_deviceptr(tmp.gamma);
        tmp.decay = cnrn_target_deviceptr(tmp.decay);
        tmp.depth = cnrn_target_deviceptr(tmp.depth);
        tmp.minCai = cnrn_target_deviceptr(tmp.minCai);
        tmp.ica = cnrn_target_deviceptr(tmp.ica);
        tmp.cai = cnrn_target_deviceptr(tmp.cai);
        tmp.Dcai = cnrn_target_deviceptr(tmp.Dcai);
        tmp.v_unused = cnrn_target_deviceptr(tmp.v_unused);
        tmp.g_unused = cnrn_target_deviceptr(tmp.g_unused);
        tmp.ion_ica = cnrn_target_deviceptr(tmp.ion_ica);
        tmp.ion_cao = cnrn_target_deviceptr(tmp.ion_cao);
        tmp.ion_cai = cnrn_target_deviceptr(tmp.ion_cai);
        tmp.ion_ca_erev = cnrn_target_deviceptr(tmp.ion_ca_erev);
        tmp.style_ca = cnrn_target_deviceptr(tmp.style_ca);
        cnrn_target_memcpy_to_device(d_inst, &tmp);
        auto* d_ml = cnrn_target_deviceptr(ml);
        void* d_inst_void = d_inst;
        cnrn_target_memcpy_to_device(&(d_ml->instance), &d_inst_void);
    }

    static inline void delete_instance_from_device(CaDynamics_Instance* inst) {
        if (cnrn_target_is_present(inst)) {
            cnrn_target_delete(inst);
        }
    }



    static void nrn_alloc_CaDynamics(double* data, Datum* indexes, int type) {
        // do nothing
    }


    void nrn_constructor_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);

        #endif
    }


    void nrn_destructor_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
        #ifndef CORENEURON_BUILD
        int nodecount = ml->nodecount;
        int pnodecount = ml->_nodecount_padded;
        const int* node_index = ml->nodeindices;
        double* data = ml->data;
        const double* voltage = nt->_actual_v;
        Datum* indexes = ml->pdata;
        ThreadDatum* thread = ml->_thread;
        auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);

        #endif
    }


    /** initialize channel */
    void nrn_init_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
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
            auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);

            if (nt->compute_gpu) {
                nrn_pragma_acc(update device (CaDynamics_global))
                nrn_pragma_omp(target update to(CaDynamics_global))
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
                    inst->ica[id] = inst->ion_ica[indexes[0*pnodecount + id]];
                    inst->cai[id] = inst->ion_cai[indexes[2*pnodecount + id]];
                    inst->cai[id] = inst->minCai[id];
                    inst->ion_cai[indexes[2*pnodecount + id]] = inst->cai[id];
                    int ca_type = inst->global->ca_type;
                    nrn_wrote_conc(ca_type, &(inst->ion_cai[indexes[2*pnodecount + id]]), 1, inst->style_ca[4], nrn_ion_global_map, celsius, nt->_ml_list[ca_type]->_nodecount_padded);
                }
            }
        }
    }


    /** update state */
    void nrn_state_CaDynamics(NrnThread* nt, Memb_list* ml, int type) {
        nrn_pragma_acc(data present(nt, ml) if(nt->compute_gpu))
        {
            int nodecount = ml->nodecount;
            int pnodecount = ml->_nodecount_padded;
            const int* node_index = ml->nodeindices;
            double* data = ml->data;
            const double* voltage = nt->_actual_v;
            Datum* indexes = ml->pdata;
            ThreadDatum* thread = ml->_thread;
            auto* const inst = static_cast<CaDynamics_Instance*>(ml->instance);

            nrn_pragma_acc(parallel loop present(inst, node_index, data, voltage, indexes, thread) async(nt->stream_id) if(nt->compute_gpu))
            nrn_pragma_omp(target teams distribute parallel for if(nt->compute_gpu))
            for (int id = 0; id < nodecount; id++) {
                int node_id = node_index[id];
                double v = voltage[node_id];
                #if NRN_PRCELLSTATE
                inst->v_unused[id] = v;
                #endif
                inst->ica[id] = inst->ion_ica[indexes[0*pnodecount + id]];
                inst->cai[id] = inst->ion_cai[indexes[2*pnodecount + id]];
                inst->cai[id] = inst->cai[id] + (1.0 - exp(nt->_dt * (( -((1.0)) / inst->decay[id])))) * ( -(( -(10000.0)) * ((((inst->ica[id]) * (inst->gamma[id])) / (2.0 * FARADAY * inst->depth[id]))) - ((( -inst->minCai[id]))) / inst->decay[id]) / (( -((1.0)) / inst->decay[id])) - inst->cai[id]);
                inst->ion_cai[indexes[2*pnodecount + id]] = inst->cai[id];
            }
        }
    }


    /** register channel with the simulator */
    void _CaDynamics_reg() {

        int mech_type = nrn_get_mechtype("CaDynamics");
        CaDynamics_global.mech_type = mech_type;
        if (mech_type == -1) {
            return;
        }

        _nrn_layout_reg(mech_type, 0);
        register_mech(mechanism_info, nrn_alloc_CaDynamics, nullptr, nullptr, nrn_state_CaDynamics, nrn_init_CaDynamics, nrn_private_constructor_CaDynamics, nrn_private_destructor_CaDynamics, first_pointer_var_index(), 1);
        CaDynamics_global.ca_type = nrn_get_mechtype("ca_ion");

        hoc_register_prop_size(mech_type, float_variables_size(), int_variables_size());
        hoc_register_dparam_semantics(mech_type, 0, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 1, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 2, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 3, "ca_ion");
        hoc_register_dparam_semantics(mech_type, 4, "#ca_ion");
        nrn_writes_conc(mech_type, 0);
        hoc_register_var(hoc_scalar_double, hoc_vector_double, NULL);
    }
}
